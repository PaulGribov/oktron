#ifndef TREGSETUP_H
#define TREGSETUP_H

#include <QMainWindow>
#include "toktservregsetup.h"
#include "toktserv.h"

class TRegSetup : public QMainWindow
	{
		Q_OBJECT
	public:
		explicit TRegSetup(QWidget *parent = 0, TOktServ *OktServ0=0, TOktServ *OktServ1=0);
		TOktServRegSetup *RegSetup0, *RegSetup1;
		QTabWidget *RegSetup_tabWidget;
		QFrame *RegSetup0_Frame;
		QWidget *OktServ0_Tab;
		QFrame *RegSetup1_Frame;
		QWidget *OktServ1_Tab;

	signals:

	public slots:
		void CloseWindow();
	};

#endif // TREGSETUP_H
