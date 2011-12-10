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

#ifndef __GDB_H__
#define __GDB_H__

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QList>
#include <QDir>
#include "DebuggerInterface.h"

class Gdb : public QObject, public DebuggerInterface 
{
	Q_OBJECT
public:
	Gdb(QString filename);
	~Gdb();

	void run();
	void stop();
	void step();
	void pause();

	void send(const QString& str);
	void addBreakpoint(const QString& filename, const int lineNum);
	void breakpoints();
	void backtrace();
	void value(const QString& variable);
	void variables();
 	void threads();

	QStringList libs();
	
	void free();
signals:
	void error(QString error);
	
private slots:
	void gdbError(QProcess::ProcessError err);
	void readyRead();
	
private:
	void parse(const QByteArray& input);
	QString shlibsAdded(const QString& data);
	void stopped(const QString& data);
	void stack(const QString& data);
	void stackArgs(const QString& data);
	void locals(const QString& data);
	void bkpt(const QString& data);
	void breakpointTable(const QString& data);
	
	QString cString(const QString& data, int starting);
	int after(const QString& data, const QString& key);
	
	QString m_filename;
	QProcess m_process;
	bool m_active;
	bool m_run;
	QStringList m_libs;
	
	QList<Frame> m_frames;
};

#endif