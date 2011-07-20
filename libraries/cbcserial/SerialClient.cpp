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

#include "SerialClient.h"

#include <QBuffer>
#include <QFileInfo>
#include <QVector>
#include <QProcess>
#include <QDebug>
#include <QTime>
#include <QThread>

#define TIMEOUT 500

SerialClient::SerialClient(QString port, QObject *parent) : QObject(parent), 
                                                            m_serialPort(port), 
                                                            m_stream(&m_serialPort)
{ open(); }

SerialClient::~SerialClient() { close(); }

bool SerialClient::open()
{
	if(m_serialPort.isOpen()) return true;
	m_serialPort.open();
	return true;
}

void SerialClient::close() { if(m_serialPort.isOpen()) m_serialPort.close(); }

bool SerialClient::sendCommand(quint16 command, const QByteArray& data)
{
	QByteArray compressedData = qCompress(data, 9);

	QList<QByteArray> dataChunks;
	while(compressedData.size()) {
		dataChunks.push_front(compressedData.right(64));
		compressedData.chop(64);
	}

	compressedData.squeeze();

	QByteArray header;
	QDataStream stream(&header, QIODevice::WriteOnly);
	stream << SERIAL_START;
	stream << (quint16)dataChunks.size();
	stream << command;

	dataChunks.push_front(header);

	for(int i = 0; i < dataChunks.size(); ++i) 
		if(!writePacket(dataChunks[i])) return false;
	
	return true;
}

bool SerialClient::waitForResult(quint16 command, QByteArray& data)
{
	
	QTime timer;
	timer.start();
	QByteArray header;
	while(timer.elapsed() < TIMEOUT) {
		header.clear();
		if(readPacket(&header)) break;
		QThread::yieldCurrentThread();
	}
	
	if(timer.elapsed() >= TIMEOUT) return false;
	
	QDataStream headerStream(&header, QIODevice::ReadOnly);

	quint16 startWord;
	quint16 packetCount;
	quint16 recCommand;

	headerStream >> startWord;
	headerStream >> packetCount;
	headerStream >> recCommand;
	
	qWarning() << recCommand;

	if(startWord != SERIAL_START) return false;
	qWarning("StartWord: %x", startWord);

	qWarning() << "Reading Header for command" << recCommand;
	QByteArray compressedData;
	
	for(quint16 i = 0;i < packetCount;i++) {
		QByteArray data;
		if(!readPacket(&data)) return false;
		compressedData += data;
	}
	qWarning() << "Uncompressing...";

	data = qUncompress(compressedData);
	compressedData.clear();
	compressedData.squeeze();
	
	return true;
}

bool SerialClient::sendFile(const QString& name, const QString& destination)
{
	QByteArray data;
	QDataStream dataStream(&data, QIODevice::WriteOnly);

	QFileInfo info(name);
	QFile fin(name);
	fin.open(QIODevice::ReadOnly);
	dataStream << destination;
	dataStream << fin.readAll();
	fin.close();

	sendCommand(KISS_SEND_FILE_COMMAND, data);
	
	return true;
}

bool SerialClient::checkOk()
{
	quint8 ret = 0;
	m_stream >> ret;
	return ret == SERIAL_MESSAGE_OK;
}

bool SerialClient::writePacket(QByteArray& data)
{
	for(int i = 0;i < SERIAL_MAX_RETRY;i++) {
		m_stream << SERIAL_KEY;
		m_stream << data;
		m_stream << qChecksum(data.constData(), data.size());
		if(checkOk()) return true;
	}
	return false;
}

/* Packets look like this:
        quint32     key = 4 bytes
        QByteArray  data = n bytes
        quint16     checksum = 2 bytes */
bool SerialClient::readPacket(QByteArray *packetData)
{
	for(int i = 0; i < 5; ++i) {
		QByteArray data;
		quint32 key = 0;
		quint16 checksum = 0xFFFF;

		m_stream >> key;
		if (key == SERIAL_KEY) {
		    m_stream >> data;
		    m_stream >> checksum;
		    qWarning("data.size()=%d", data.size());
		    if(checksum == qChecksum(data, data.size())) {
		        *packetData = data;
		        sendOk();
		        return true;
		    }
		}
		m_stream.skipRawData(1024);
		m_stream.resetStatus();
		qWarning("Retry...");
	}
	return false;
}

void SerialClient::sendOk() { m_stream << SERIAL_MESSAGE_OK; }
void SerialClient::setPort(const QString& port) { close(); m_serialPort.setPort(port); open(); }
