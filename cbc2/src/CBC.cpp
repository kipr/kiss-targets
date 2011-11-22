/**************************************************************************
 *  Copyright 2007,2008,2009 KISS Institute for Practical Robotics        *
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

#include "CBC.h"
#include "Gdb.h"
#include "Kiss.h"

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
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

CBC::CBC()
{
#ifdef Q_OS_WIN32
	m_gccPath = QDir::currentPath() + "/targets/gcc/mingw/bin/gcc.exe";
	m_gppPath = QDir::currentPath() + "/targets/gcc/mingw/bin/g++.exe";
#else
	m_gccPath="/usr/bin/gcc";
	m_gppPath="/usr/bin/g++";
#endif

	if(!QFileInfo(m_gccPath).exists()) {
		setError(true, "error_locating", QStringList() << "gcc" << m_gccPath);
		return;
	}
	if(!QFileInfo(m_gppPath).exists()) {
		setError(true, "error_locating", QStringList() << "g++" << m_gccPath);
		return;
	}
	
	m_gcc.setReadChannel(QProcess::StandardError);
	
#ifdef Q_OS_WIN32
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	env.insert("PATH", env.value("Path") + ";" + QDir::toNativeSeparators(QDir::currentPath() + "/targets/gcc/mingw/bin"));
	m_gcc.setProcessEnvironment(env);
#endif
	
//FIXME This is ugly
#ifdef Q_OS_MAC
	system(("ranlib " + QDir::currentPath() + "/targets/cbc2/lib/*.a").toLocal8Bit());
#endif
}

CBC::~CBC()
{
	m_gcc.kill();
	m_outputBinary.kill();
}

bool CBC::compile(const QString& filename, const QString& port) { return compile(filename, port, false); }
bool CBC::run(const QString& filename, const QString& port) 
{
	m_serial.setPort(port);
	bool ret = m_serial.sendCommand(KISS_RUN_COMMAND);
	m_serial.close();
	return ret;
}
void CBC::stop(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_STOP_COMMAND); m_serial.close(); }

QStringList CBC::getPaths(const QString& string)
{
	QString str = string;
	QStringList list;

	str.remove(QRegExp("^\\w*\\.o\\:\\s*"));
	str.replace(QRegExp("\\s*\\W\\r?\\n\\s*"), " ");
	str.remove("\n");
	str += " ";

	qWarning("string=\"%s\"", qPrintable(str));

	while(1) {
		int index = str.indexOf(QRegExp("\\w[ ]"));
		if(index == -1) return list;
		list << str.left(index+1).remove("\\").mid(1);
		str.remove(0, index+1);
		while(!str.isEmpty() && string[0] == ' ') str.remove(0,1);
	}
}

bool CBC::download(const QString& filename, const QString& port)
{
	if(!compile(filename, port)) return false;

	qWarning("Calling gcc...");
	m_gcc.reset();

	m_gcc.start(m_gccPath, QStringList() << "-E" << "-Wp,-MM" << filename);
	m_gcc.waitForFinished();

	qWarning("Gcc finished...");

	QString depString = QString::fromLocal8Bit(m_gcc.readAllStandardOutput());

	QStringList deps = getPaths(depString);

	deps.removeFirst();
	qWarning("deps.size()=%d", deps.size());

	qWarning("deps=\"%s\"", qPrintable(deps.join(",")));

	qWarning("Calling sendFile");

	m_serial.setPort(port);
	qWarning() << "Sending file";
	
	QString projectName = QFileInfo(filename).baseName();
	m_serial.sendCommand(KISS_CREATE_PROJECT_COMMAND, projectName.toAscii());
	const QString& common = QString("/mnt/user/code/") + projectName + "/";
	QByteArray dest = (common + QFileInfo(filename).fileName()).toAscii();
	m_serial.sendFile(filename, dest.data());
	foreach(const QString& include, deps) {
		const QString& path = common + QFileInfo(include).filePath().replace(QFileInfo(filename).path(), "").mid(1);
		m_serial.sendCommand(KISS_MKDIR_COMMAND, QFileInfo(path).path().toAscii());
		m_serial.sendFile(include, path.toAscii());
	}
	bool ret = m_serial.sendCommand(KISS_COMPILE_COMMAND, dest);
	m_serial.close();
	return ret;
}

bool CBC::simulate(const QString& filename, const QString& port)
{
	if(!compile(filename, port)) return false;

	QString outputString;
	QFileInfo outputFileInfo(m_outputFileName);
	QFile scriptFile;

#ifdef Q_OS_WIN32
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprBatchFile.cmd"));
	outputString += "@echo off\r\n";
	outputString += "set PATH=%PATH%;" + QDir::toNativeSeparators(QDir::currentPath()) + "\\targets\\gcc\\mingw\\bin\r\n";
	outputString += "\"" + QDir::toNativeSeparators(outputFileInfo.absolutePath()) + "\\" + outputFileInfo.fileName() + "\"\r\n";
	outputString +=  "pause\r\n";
#else
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprScript.sh"));
	outputString += "#!/bin/bash\n";
	outputString += "cd \"" + outputFileInfo.absolutePath() + "\"\n";
	outputString += "clear\n";
	outputString += "\"./" + outputFileInfo.fileName() + "\"\n";
#endif

	if(!scriptFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning("CBC::simulate() Unable to open temp file for writing");
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

bool CBC::debugConsole(const QString& filename, const QString& port, const QList<Location>& bkpts)
{
	if(!compile(filename, port, true))
		return false;

	QString outputString;
	QFileInfo outputFileInfo(m_outputFileName);
	QFile scriptFile;
	
	QString breaks = "";
	if(bkpts.size() > 0) {
		QTemporaryFile temp;
		temp.setAutoRemove(false);
		if(!temp.open()) return false;
		foreach(const Location& bkpt, bkpts) {
			temp.write(("break " + bkpt.file + ":" + QString::number(bkpt.line) + "\n").toAscii());
		}
		breaks = QFileInfo(temp).absoluteFilePath();
		temp.close();
	}

#ifdef Q_OS_WIN32
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprBatchFile.cmd"));
	outputString += "@echo off\r\n";
	outputString += "set PATH=%PATH%;" + QDir::toNativeSeparators(QDir::currentPath()) + "\\targets\\gcc\\mingw\\bin\r\n";
	outputString += "gdb " + (breaks.isEmpty() ? "" : ("-x \"" + QDir::toNativeSeparators(breaks) + "\" ")) + "\"" +
		QDir::toNativeSeparators(outputFileInfo.absolutePath()) + "\\" + outputFileInfo.fileName() + "\"\r\n";
	outputString += "pause\r\n";
#else
	scriptFile.setFileName(QDir::temp().absoluteFilePath("kiprScript.sh"));
	outputString += "#!/bin/bash\n";
	outputString += "cd \"" + outputFileInfo.absolutePath() + "\"\n";
	outputString += "clear\n";
	outputString += "gdb " + (breaks.isEmpty() ? "" : ("-x \"" + breaks + "\" ")) + "\"./" + outputFileInfo.fileName() + "\"\n";
#endif

	if(!scriptFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning("Cbc::debugConsole() Unable to open temp file for writing");
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

DebuggerInterface* CBC::debug(const QString& filename, const QString& port) 
{
	return compile(filename, port, true) ? new Gdb(m_outputFileName) : 0;
}

Tab* CBC::ui(const QString& port) { return new Controller(this, &m_serial, port); }

QByteArray CBC::screenGrab(const QString& port)
{
	m_serial.setPort(port);
	QString filename = "/mnt/user/screenCapture.jpg";
	m_serial.sendCommand(KISS_EXECUTE_COMMAND, (QString("imgtool --mode=cap --fmt=jpg --quality=100 ") + filename).toAscii());
	m_serial.sendCommand(KISS_REQUEST_FILE_COMMAND, filename.toAscii());
	QByteArray ret;
	m_serial.waitForResult(CBC_REQUEST_FILE_RESULT, ret);
	m_serial.close();
	return ret;
}

QStringList CBC::requestDir(const QString& filename, const QString& port)
{
	m_serial.setPort(port);
	m_serial.sendCommand(KISS_LS_COMMAND, filename.toAscii());
	QByteArray data;
	m_serial.waitForResult(CBC_LS_RESULT, data);
	QDataStream stream(&data, QIODevice::ReadOnly);
	QStringList ret;
	stream >> ret;
	m_serial.close();
	return ret;
}

QByteArray CBC::requestFile(const QString& filename, const QString& port)
{
	m_serial.setPort(port);
	m_serial.sendCommand(KISS_REQUEST_FILE_COMMAND, filename.toAscii());
	QByteArray ret;
	m_serial.waitForResult(CBC_REQUEST_FILE_RESULT, ret);
	m_serial.close();
	return ret;
}

void CBC::processCompilerOutput()
{
	bool foundError = false, foundWarning = false;
	m_errorMessages.clear();
	m_warningMessages.clear();
	m_verboseMessages.clear();

	while(m_gcc.canReadLine()) {
		QString inputLine = QString::fromLocal8Bit(m_gcc.readLine());
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
		}
		else
			outputLine += inputLine.section(':', 1);

		if(outputLine.section(':',2,2) == "error") {
			m_errorMessages << outputLine;
			foundError=true;
		}
		else if(outputLine.section(':', 2,2) == "warning") {
			m_warningMessages << outputLine;
			foundWarning=true;
		}
		else {
			m_errorMessages << inputLine;
			m_warningMessages << inputLine;
		}
	}
	if(!foundError && !foundWarning) m_warningMessages.clear();
	else if(!foundError) m_errorMessages.clear();
	else if(!foundWarning) m_warningMessages.clear();
}

void CBC::processLinkerOutput()
{
	m_linkerMessages.clear();
	while(m_gcc.canReadLine()) {
		QString line = QString::fromLocal8Bit(m_gcc.readLine()).remove(QRegExp("\\r*\\n$"));
		m_verboseMessages << line;
		m_linkerMessages << line;
	}
}

void CBC::refreshSettings()
{
	QStringList include_dirs;
	QStringList lib_dirs;
	QSettings settings(m_targetFile, QSettings::IniFormat);

	m_cflags.clear();
	m_lflags.clear();

	include_dirs = settings.value(QString(OS_NAME) + "/include_dirs").toString().split(' ', QString::SkipEmptyParts);
	lib_dirs = settings.value(QString(OS_NAME) + "/lib_dirs").toString().split(' ', QString::SkipEmptyParts);

	QStringListIterator i(include_dirs);
	while(i.hasNext()) {
		QDir includePath(i.next());
		m_cflags << "-I" + (!includePath.isAbsolute() ? (QDir::currentPath() + "/") : "") + includePath.path();
	}

	i = lib_dirs;
	while(i.hasNext()) {
		QDir libPath(i.next());
		m_lflags << "-L" + (!libPath.isAbsolute() ? (QDir::currentPath() + "/") : "") + libPath.path();
	}

	m_cflags << settings.value(QString(OS_NAME) + "/cflags").toString().split(' ', QString::SkipEmptyParts);
	m_lflags << settings.value(QString(OS_NAME) + "/lflags").toString().split(' ', QString::SkipEmptyParts);

#ifdef Q_OS_MAC
	if(QSysInfo::MacintoshVersion == QSysInfo::MV_TIGER) {
		m_cflags << "-isysroot" << "/Developer/SDKs/MacOSX10.4u.sdk";
		m_lflags << "-isysroot" << "/Developer/SDKs/MacOSX10.4u.sdk";
	}
#endif
}

void CBC::aPressed(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_PRESS_A_COMMAND); }
void CBC::bPressed(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_PRESS_B_COMMAND); }
void CBC::leftPressed(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_PRESS_LEFT_COMMAND); }
void CBC::rightPressed(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_PRESS_RIGHT_COMMAND); }
void CBC::upPressed(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_PRESS_UP_COMMAND); }
void CBC::downPressed(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_PRESS_DOWN_COMMAND); }
void CBC::aReleased(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_RELEASE_A_COMMAND); }
void CBC::bReleased(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_RELEASE_B_COMMAND); }
void CBC::leftReleased(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_RELEASE_LEFT_COMMAND); }
void CBC::rightReleased(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_RELEASE_RIGHT_COMMAND); }
void CBC::upReleased(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_RELEASE_UP_COMMAND); }
void CBC::downReleased(const QString& port) { m_serial.setPort(port); m_serial.sendCommand(KISS_RELEASE_DOWN_COMMAND); }

bool CBC::compile(const QString& filename, const QString& port, bool debug)
{
	QString p = port;
	
	QFileInfo sourceInfo(filename);
	QStringList args;

	refreshSettings();

#ifdef Q_OS_WIN32
	m_outputFileName = sourceInfo.dir().absoluteFilePath(sourceInfo.baseName() + ".exe");
#else
	m_outputFileName = sourceInfo.dir().absoluteFilePath(sourceInfo.baseName());
#endif
	QString objectName = sourceInfo.dir().absoluteFilePath(sourceInfo.baseName() + ".o");

	QFileInfo outputInfo(m_outputFileName);
	if(sourceInfo.lastModified() < outputInfo.lastModified())
		return true;

	args = m_cflags;
	p.replace("\\", "\\\\");
	args << "-DDEFAULT_SERIAL_PORT=\"" + p + "\"";
	args << "-c" << filename << "-o" << objectName;
	if(debug) {
		args << "-g";
	#ifndef Q_OS_WIN
		args << "-pg";
	#endif
	}
	
	qWarning() << "Object Args:" << args;
	m_gcc.start(m_gccPath, args);
	m_gcc.waitForFinished();
	processCompilerOutput();
	m_linkerMessages.clear();

	if(m_gcc.exitCode() != 0) return false;

	args.clear();
	args << "-o" << m_outputFileName << objectName;
	args << m_lflags;
	qWarning() << "Linker Args:" << args;
	m_gcc.start((sourceInfo.completeSuffix() == "cpp") ? m_gppPath : m_gccPath, args);
	m_gcc.waitForFinished();
	processLinkerOutput();

	if(!debug) QFile(objectName).remove();
	
	return m_gcc.exitCode() == 0;
}

Q_EXPORT_PLUGIN2(cbc_plugin, CBC);
