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
 
#include "Shell.h"
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

Shell::Shell() {}
Shell::~Shell() { m_outputBinary.kill(); }

bool Shell::simulate(const QString& filename, const QString& port)
{
	QString outputString;
	QFileInfo outputFileInfo(filename);
	QFile scriptFile;
	
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprScript.sh"));
	outputString += "#!/bin/bash\n";
	outputString += "cd \"" + outputFileInfo.absolutePath() + "\"\n";
	outputString += "clear\n";
	outputString += "sh " + outputFileInfo.fileName() + "\n";

	qWarning() << outputString;

	if(!scriptFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning("Shell::run() Unable to open temp file for writing");
		return false;
	}

	scriptFile.setPermissions(scriptFile.permissions() | QFile::ExeOwner);
	scriptFile.write(outputString.toLocal8Bit());
	scriptFile.close();

	QStringList args;
	QFileInfo scriptInfo(scriptFile);

	m_outputBinary.setWorkingDirectory(outputFileInfo.absolutePath());

#ifdef Q_OS_MAC
	args << "-a" << "/Applications/Utilities/Terminal.app" << scriptInfo.absoluteFilePath();
	m_outputBinary.start("open", args);
#else
	args << "-e" << scriptInfo.absoluteFilePath() + " && echo \"\nQuitting in 5 secs...\" && sleep 5";
	m_outputBinary.start("xterm", args);
#endif

	return true;
}

bool Shell::download(const QString& filename, const QString& port)
{
	m_serial.setPort(port);
	QString projectName = QFileInfo(filename).baseName();
	m_serial.sendCommand(KISS_CREATE_PROJECT_COMMAND, projectName.toAscii());
	QByteArray dest = (QString("/mnt/user/code/") + projectName + "/" + QFileInfo(filename).fileName()).toAscii();
	QFileInfo fileInfo(filename);
	m_serial.sendFile(fileInfo.absolutePath() + "/" + fileInfo.fileName(), dest.data());
	bool ret = m_serial.sendCommand(KISS_COMPILE_COMMAND, dest);
	m_serial.close();
	return ret;
}

void Shell::refreshSettings() {}

Q_EXPORT_PLUGIN2(shell_plugin, Shell);
