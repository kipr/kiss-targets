#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "ui_Controller.h"

#include <QWidget>
#include "Tab.h"
#include "SerialClient.h"

class CBC;

class Controller : public QWidget, public Tab, public Ui::Controller
{
	Q_OBJECT
public:
	Controller(CBC* cbc, SerialClient* client, const QString& port);
	
	void activate();

	void addActionsFile(QMenu* file);
	void addActionsEdit(QMenu* edit);
	void addActionsHelp(QMenu* help);
	void addOtherActions(QMenuBar* menuBar);
	void addToolbarActions(QToolBar* toolbar);

	bool beginSetup();
	void completeSetup();

	bool close();

	void refreshSettings();

private slots:
	void update();
	void on_ui_refresh_clicked();
	void on_ui_stop_clicked();

	void on_ui_a_pressed();
	void on_ui_b_pressed();
	void on_ui_left_pressed();
	void on_ui_right_pressed();
	void on_ui_up_pressed();
	void on_ui_down_pressed();
	
	void on_ui_a_released();
	void on_ui_b_released();
	void on_ui_left_released();
	void on_ui_right_released();
	void on_ui_up_released();
	void on_ui_down_released();
private:
	void delayedUpdate();
	
	CBC* m_cbc;
	SerialClient* m_client;
	QString m_port;
};

#endif