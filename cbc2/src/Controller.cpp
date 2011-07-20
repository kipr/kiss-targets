#include "Controller.h"
#include "CBC.h"
#include "TargetUtils.h"

#include <QTimer>
#include <QDebug>
#include <QTableWidgetItem>
#include <QThread>

Controller::Controller(CBC* cbc, SerialClient* client, const QString& port) : QWidget(0), m_cbc(cbc), m_client(client), m_port(port) { 
	setupUi(this); 
	on_ui_refresh_clicked();
}

void Controller::activate() {}
void Controller::addActionsFile(QMenu* file) { Q_UNUSED(file); }
void Controller::addActionsEdit(QMenu* edit) { Q_UNUSED(edit); }
void Controller::addActionsHelp(QMenu* help) { Q_UNUSED(help); }
void Controller::addOtherActions(QMenuBar* menuBar) { Q_UNUSED(menuBar); }
void Controller::addToolbarActions(QToolBar* toolbar) { Q_UNUSED(toolbar); }
bool Controller::beginSetup() { return true; }
void Controller::completeSetup() {}
bool Controller::close() { return true; }
void Controller::refreshSettings() {}
void Controller::moveTo(int line, int pos) { Q_UNUSED(line); Q_UNUSED(pos); }

void Controller::update()
{
	m_client->setPort(m_port);
	m_client->sendCommand(KISS_GET_STDOUT_COMMAND);
	QByteArray data;
	m_client->waitForResult(CBC_STDOUT_RESULT, data);
	if(!data.isEmpty()) ui_console->append(data.data());
}

void Controller::on_ui_refresh_clicked()
{	  
	update();
	QByteArray data;
	m_client->sendCommand(KISS_GET_STATE_COMMAND);
	m_client->waitForResult(CBC_STATE_RESULT, data);
	QMap<QString, QString> state;
	
	QDataStream stream(&data, QIODevice::ReadOnly);
	stream >> state;
	ui_table->clearContents();
	ui_table->setRowCount(state.size());
	int i = 0;
	foreach(const QString& key, state.keys()) {
		ui_table->setItem(i, 0, new QTableWidgetItem(key));
		ui_table->setItem(i++, 1, new QTableWidgetItem(state[key]));
	}
}

void Controller::on_ui_stop_clicked()
{
	m_client->setPort(m_port);
	m_client->sendCommand(KISS_STOP_COMMAND);
	delayedUpdate();
}

void Controller::on_ui_a_pressed() 	{ m_cbc->aPressed(m_port); }
void Controller::on_ui_b_pressed() 	{ m_cbc->bPressed(m_port); }
void Controller::on_ui_left_pressed() 	{ m_cbc->leftPressed(m_port); }
void Controller::on_ui_right_pressed() 	{ m_cbc->rightPressed(m_port); }
void Controller::on_ui_up_pressed() 	{ m_cbc->upPressed(m_port); }
void Controller::on_ui_down_pressed() 	{ m_cbc->downPressed(m_port); }

void Controller::on_ui_a_released() 	{ m_cbc->aReleased(m_port); delayedUpdate(); }
void Controller::on_ui_b_released() 	{ m_cbc->bReleased(m_port); delayedUpdate(); }
void Controller::on_ui_left_released() 	{ m_cbc->leftReleased(m_port); delayedUpdate(); }
void Controller::on_ui_right_released() { m_cbc->rightReleased(m_port); delayedUpdate(); }
void Controller::on_ui_up_released() 	{ m_cbc->upReleased(m_port); delayedUpdate(); }
void Controller::on_ui_down_released() 	{ m_cbc->downReleased(m_port); delayedUpdate(); }

void Controller::delayedUpdate() { QTimer::singleShot(100, this, SLOT(update())); }
