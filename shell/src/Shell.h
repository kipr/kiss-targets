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
 
#ifndef __SHELL_H__
#define __SHELL_H__

#include <qplugin.h>
#include <QProcess>
#include <QString>
#include <QSettings>

#include "TargetInterface.h"
#include "SerialClient.h"

class Shell : public QObject, public TargetInterface
{
	Q_OBJECT
	Q_INTERFACES(TargetInterface)
	
public:
	Shell();
	~Shell();

	int download(const QString&, const QString&);
	bool simulate(const QString& filename, const QString& port);

	bool hasDownload() 	{ return true; }
	bool hasSimulate() 	
	{
		#ifndef Q_OS_WIN
		return true;
		#else
		return false;
		#endif
	}

private:
	QProcess m_outputBinary;
	QString m_outputFileName;
	QStringList m_cflags,m_lflags;
	SerialClient m_serial;
	
	void refreshSettings();
};

#endif
