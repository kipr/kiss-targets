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

#ifndef __SERIAL_CLIENT_H__
#define __SERIAL_CLIENT_H__

#include "QSerialPort.h"
#include <QDataStream>
#include <QStringList>

#define SERIAL_MAX_RETRY 5
#define SERIAL_TIMEOUT 500
#define SERIAL_START (quint16)(0xCBC)
#define SERIAL2_START (quint16)(0xCBC2)
#define SERIAL_KEY (quint32)(0xB07BA11)
#define HEADER_KEY (quint32)(0xB07BA11)

#define SERIAL_MESSAGE_OK   ((quint8)1)
#define SERIAL_MESSAGE_FAIL ((quint8)2)

#define KISS_SEND_FILE_COMMAND 		1
#define KISS_REQUEST_FILE_COMMAND 	2
#define KISS_LS_COMMAND 		3
#define KISS_RUN_COMMAND 		4
#define KISS_STOP_COMMAND 		5
#define KISS_EXECUTE_COMMAND 		6
#define KISS_COMPILE_COMMAND 		7
#define KISS_CREATE_PROJECT_COMMAND 	8
#define KISS_PRESS_A_COMMAND 		9
#define KISS_PRESS_B_COMMAND 		10
#define KISS_PRESS_LEFT_COMMAND 	11
#define KISS_PRESS_RIGHT_COMMAND 	12
#define KISS_PRESS_UP_COMMAND 		13
#define KISS_PRESS_DOWN_COMMAND 	14
#define KISS_RELEASE_A_COMMAND 		15
#define KISS_RELEASE_B_COMMAND 		16
#define KISS_RELEASE_LEFT_COMMAND 	17
#define KISS_RELEASE_RIGHT_COMMAND 	18
#define KISS_RELEASE_UP_COMMAND 	19
#define KISS_RELEASE_DOWN_COMMAND 	20
#define KISS_GET_STATE_COMMAND 		21
#define KISS_GET_STDOUT_COMMAND 	22

#define CBC_REQUEST_FILE_RESULT 	127
#define CBC_LS_RESULT 			128
#define CBC_EXECUTE_RESULT 		129
#define CBC_COMPILE_SUCCESS_RESULT 	130
#define CBC_STATE_RESULT 		131
#define CBC_STDOUT_RESULT 		132

class SerialClient : public QObject
{
Q_OBJECT

public:
	SerialClient(QString port = QString(), QObject *parent = 0);
	~SerialClient();

	bool open();
	void close();
	bool sendCommand(quint16 command, const QByteArray& data = "");
	bool waitForResult(quint16 command, QByteArray& data);
	bool sendFile(const QString& name, const QString& destination);
	void setPort(const QString& port);
	
private:
	QSerialPort m_serialPort;
	QDataStream m_stream;
	QString m_cbcName;
	
	bool checkOk();
	void sendOk();
	bool writePacket(QByteArray& data);
	bool readPacket(QByteArray* packetData);
    
};

#endif
