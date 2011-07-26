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
 
#include "Haskell.h"
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

Haskell::Haskell()
{
#ifdef Q_OS_WIN
	// This code finds the haskell directory, which doesn't really have a standard naming convention
	QDir haskellInstall("C:/Program Files/Haskell Platform/");
	const QStringList& installed = haskellInstall.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	if(!installed.isEmpty()) haskellInstall.cd(jdks[0]);
	haskellInstall.cd("bin");
	m_haskellPath = haskellInstall.filePath("ghc.exe");
#else
	m_haskellPath="/usr/bin/ghc";
#endif

	QFileInfo haskellExecutable(m_haskellPath);
	if(!haskellExecutable.exists()) QMessageBox::critical(0, "Error", "Could not find GHC Executable!");

	m_haskell.setReadChannel(QProcess::StandardError);

}

Haskell::~Haskell()
{
	m_haskell.kill();
	m_outputBinary.kill();
}

bool Haskell::compile(const QString& filename, const QString& port)
{
	QFileInfo sourceInfo(filename);
	QStringList args;

	refreshSettings();

	m_outputFileName = sourceInfo.dir().absoluteFilePath(sourceInfo.baseName());

	QFileInfo outputInfo(m_outputFileName);
	if(sourceInfo.lastModified() < outputInfo.lastModified())
		return true;

	args << filename;
	qWarning() << args;
	m_haskell.start(m_haskellPath, args);
	m_haskell.waitForFinished();
	processCompilerOutput();
	m_linkerMessages.clear();

	return m_haskell.exitCode() == 0;
}

bool Haskell::run(const QString& filename, const QString& port)
{
	if(!compile(filename, port)) return false;

	QString outputString;
	QFileInfo outputFileInfo(m_outputFileName);
	QFile scriptFile;

#ifdef Q_OS_WIN32
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprBatchFile.cmd"));
	outputString += "@echo off\n";
	outputString += "cd \"" + QDir::toNativeSeparators(outputFileInfo.absolutePath()) + "\"\n";
	outputString += "\"" + outputFileInfo.baseName() + "\"\n";
	outputString +=  "pause\n";
#else
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprScript.sh"));
	outputString += "#!/bin/bash\n";
	outputString += "cd \"" + outputFileInfo.absolutePath() + "\"\n";
	outputString += "clear\n";
	outputString += "\"./" + outputFileInfo.baseName() + "\"\n";
#endif

	qWarning() << outputString;

	if(!scriptFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning("Haskell::run() Unable to open temp file for writing");
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

void Haskell::processCompilerOutput()
{
	bool foundError = false, foundWarning = false;
	m_errorMessages.clear();
	m_warningMessages.clear();
	m_verboseMessages.clear();

	while(m_haskell.canReadLine()) {
		QString inputLine = QString::fromLocal8Bit(m_haskell.readLine());
		QString outputLine;

		inputLine.remove(QRegExp("\\r*\\n$"));

		m_verboseMessages << inputLine;

		inputLine.remove(QRegExp("^C\\:"));
		inputLine.remove(QRegExp("^/.*/(?=\\S*\\:)"));

		outputLine += inputLine.section(':',0,0) + ":";

		if(inputLine.section(':',2,2).length() > 0) {
			outputLine += inputLine.section(':',1,1) + ":";
			outputLine += inputLine.section(':',2,2).remove(' ') + ":";
			outputLine += inputLine.section(':', 3);
		} else outputLine += inputLine.section(':', 1);

		if(outputLine.section(':',2,2) == "error") {
			m_errorMessages << outputLine;
			foundError=true;
		} else if(outputLine.section(':', 2,2) == "warning") {
			m_warningMessages << outputLine;
			foundWarning=true;
		} else {
			m_errorMessages << inputLine;
			m_warningMessages << inputLine;
		}
	}
	if(!foundError && !foundWarning) m_warningMessages.clear();
	else if(!foundError) m_errorMessages.clear();
	else if(!foundWarning) m_warningMessages.clear();
}

void Haskell::refreshSettings()
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

Q_EXPORT_PLUGIN2(python_plugin, Haskell);
