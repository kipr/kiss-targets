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

SerialClient::SerialClient(QString port, QObject *parent) : QObject(parent), 
                                                            m_serialPort(port), 
                                                            m_stream(&m_serialPort)
{}

SerialClient::~SerialClient() {}

bool SerialClient::sendFile(QString name, QStringList deps)
{
	m_serialPort.open();

	QByteArray data;
	QDataStream dataStream(&data, QIODevice::WriteOnly);

	QFileInfo info(name);
	QFile fin(name);
	fin.open(QIODevice::ReadOnly);
	dataStream << info.fileName();
	dataStream << fin.readAll();
	fin.close();

	for(int i = 0;i < deps.size();i++) {
		info.setFile(deps[i]);
		fin.setFileName(deps[i]);
		fin.open(QIODevice::ReadOnly);
		dataStream << info.fileName();
		dataStream << fin.readAll();
		fin.close();
	}

	QByteArray compressedData = qCompress(data, 9);
	data.clear();
	data.squeeze();

	QList<QByteArray> dataChunks;
	while(compressedData.size()) {
		dataChunks.push_front(compressedData.right(32));
		compressedData.chop(32);
	}

	compressedData.squeeze();

	QByteArray header;
	QDataStream stream(&header, QIODevice::WriteOnly);
	stream << SERIAL_START;
	stream << (quint16)dataChunks.size();

	dataChunks.push_front(header);

	for(int i = 0; i < dataChunks.size(); ++i) {
		if(!writePacket(dataChunks[i])) return false;
	}

	m_serialPort.close();

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
	quint16 checksum;

	checksum = qChecksum(data, data.size());

	for(int i = 0;i < SERIAL_MAX_RETRY;i++) {
		m_stream << SERIAL_KEY;
		m_stream << data;
		m_stream << qChecksum(data.constData(), data.size());
		if(checkOk()) return true;
	}
	return false;
}

void SerialClient::setPort(QString port)
{
	m_serialPort.setPort(port);
}
