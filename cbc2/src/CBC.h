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

#ifndef __CBC_H__
#define __CBC_H__

#include <qplugin.h>
#include <QProcess>
#include <QString>
#include <QSettings>

#include "TargetInterface.h"
#include "SerialClient.h"
#include "Controller.h"

class CBC : public QObject, public TargetInterface
{
	Q_OBJECT
	Q_INTERFACES(TargetInterface)
	
public:
	CBC();
	~CBC();

	bool compile(const QString& filename, const QString& port);
	bool run(const QString&, const QString&);
	void stop(const QString&);
	int download(const QString& filename, const QString& port);
	bool rawDownload(const QString& filename, const QString& port);
	bool simulate(const QString& filename, const QString& port);
	bool debugConsole(const QString& filename, const QString& port, const QList<Location>& bkpts);
	DebuggerInterface* debug(const QString&, const QString&);
	Tab* ui(const QString& port);

	bool hasCompile() 	{ return true; }
	bool hasRun() 		{ return true; }
	bool hasDownload() 	{ return true; }
	bool hasRawDownload() 	{ return true; }
	bool hasStop() 		{ return true; }
	bool hasSimulate() 	{ return true; }
	bool hasDebug() 	{ return true; }
	bool hasUi() 		{ return true; }
	bool hasScreenGrab()	{ return true; }
	bool hasFileRequest() 	{ return true; }
	
	QByteArray screenGrab(const QString& port);
	QStringList requestDir(const QString& filename, const QString& port);
	QByteArray requestFile(const QString& filename, const QString& port);

	void aPressed(const QString& port);
	void bPressed(const QString& port);
	void leftPressed(const QString& port);
	void rightPressed(const QString& port);
	void upPressed(const QString& port);
	void downPressed(const QString& port);

	void aReleased(const QString& port);
	void bReleased(const QString& port);
	void leftReleased(const QString& port);
	void rightReleased(const QString& port);
	void upReleased(const QString& port);
	void downReleased(const QString& port);

private:
	bool compile(const QString& filename, const QString& port, bool debug);
	
	QProcess m_gcc;
	QProcess m_outputBinary;
	QString m_gccPath;
	QString m_gppPath;
	QString m_outputFileName;
	QStringList m_cflags,m_lflags;
  	SerialClient m_serial;
	
	void processCompilerOutput();
	void processLinkerOutput();
	void refreshSettings();
	
  	QStringList getPaths(const QString& string);
};

#endif
