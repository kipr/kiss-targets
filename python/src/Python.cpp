/**************************************************************************
 *  Copyright 2007-2011 KISS Institute for Practical Robotics             *
 *                                                                        *
 *  This file is part of KISS (Kipr's Instructional Software System).     *
 *                                                                        *
 *  KISS is free software: you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  KISS is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with KISS.  Check the LICENSE file in the project root.         *
 *  If not, see <http://www.gnu.org/licenses/>.                           *
 **************************************************************************/
 
#include "Python.h"
#include "QSerialPort.h"
#include <QProcess>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QtGlobal>
#include <QBuffer>
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

Python::Python()
{
#ifdef Q_OS_WIN
	// This code finds the python directory, which doesn't really have a standard naming convention
	QDir pythonInstall("C:/");
	QStringList pythons = pythonInstall.entryList(QStringList() << "Python*", QDir::NoDotAndDotDot | QDir::Dirs);
	if(pythons.size() > 0) pythonInstall.cd(pythons[0]);
	m_pythonPath = pythonInstall.filePath("python.exe");
#else
	m_pythonPath="/usr/bin/python";
#endif

	QFileInfo pythonExecutable(m_pythonPath);
	if(!pythonExecutable.exists()) QMessageBox::critical(0, "Error", "Could not find Python Executable!");

	m_python.setReadChannel(QProcess::StandardError);

}

Python::~Python()
{
	m_python.kill();
	m_outputBinary.kill();
}

bool Python::run(const QString& filename, const QString& port)
{
	QString outputString;
	QFileInfo outputFileInfo(filename);
	QFile scriptFile;

#ifdef Q_OS_WIN32
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprBatchFile.cmd"));
	outputString += "@echo off\n";
	outputString += "cd \"" + QDir::toNativeSeparators(outputFileInfo.absolutePath()) + "\"\n";
	outputString += m_pythonPath + " \"" + outputFileInfo.fileName() + "\"\n";
	outputString +=  "pause\n";
#else
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprScript.sh"));
	outputString += "#!/bin/bash\n";
	outputString += "cd \"" + outputFileInfo.absolutePath() + "\"\n";
	outputString += "clear\n";
	outputString += m_pythonPath + " \"" + outputFileInfo.fileName() + "\"\n";
#endif

	qWarning() << outputString;

	if(!scriptFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning("Python::run() Unable to open temp file for writing");
		return false;
	}

	scriptFile.setPermissions(scriptFile.permissions() | QFile::ExeOwner);
	scriptFile.write(outputString.toLocal8Bit());
	scriptFile.close();

	QStringList args;
	QFileInfo scriptInfo(scriptFile);

	m_outputBinary.setWorkingDirectory(outputFileInfo.absolutePath());

#ifdef Q_OS_WIN32
	QString startLine = "start \"" + m_outputFileName + "\" \"cmd /c " +
						scriptInfo.absoluteFilePath() + "\"\n";
	args << "/k";
	m_outputBinary.start("cmd", args);
	m_outputBinary.write(startLine.toLocal8Bit());
	m_outputBinary.write("exit\n");
#elif defined(Q_OS_MAC)
	args << "-a" << "/Applications/Utilities/Terminal.app" << scriptInfo.absoluteFilePath();
	m_outputBinary.start("open", args);
#else
	args << "-e" << scriptInfo.absoluteFilePath() + " && echo \"\nQuitting in 5 secs...\" && sleep 5";
	m_outputBinary.start("xterm", args);
#endif

	return true;
}

void Python::refreshSettings()
{
	QStringList lib_dirs;
	QSettings settings(m_targetFile, QSettings::IniFormat);

	lib_dirs = settings.value("Target/lib_dirs").toString().split(' ', QString::SkipEmptyParts);

	QStringListIterator i = lib_dirs;
	while(i.hasNext()) {
		QDir libPath(i.next());
		if(libPath.isAbsolute())
			m_lflags << "-L" + libPath.path();
		else
			m_lflags << "-L" + QDir::currentPath() + "/" + libPath.path();
	}
}

Q_EXPORT_PLUGIN2(python_plugin, Python);
