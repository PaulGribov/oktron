#include "tregsetup.h"

TRegSetup::TRegSetup(QWidget *parent, TOktServ *OktServ0, TOktServ *OktServ1) :
	QMainWindow(parent)
	{
	RegSetup_tabWidget=new QTabWidget(this);

	//*************************************************
	//Закладка: настройки основного
	//*************************************************
	OktServ0_Tab = new QWidget();
	RegSetup_tabWidget->addTab(OktServ0_Tab, tr("Настройки основного"));
	RegSetup_tabWidget->setTabIcon(0, QIcon(":/images/advancedsettings.png"));

	RegSetup0_Frame=new QFrame();
	QVBoxLayout *RegSetup0_qvl = new QVBoxLayout();
	RegSetup0 = new TOktServRegSetup(RegSetup0_Frame, OktServ0);
	RegSetup0_qvl->addWidget(RegSetup0_Frame);
	OktServ0_Tab->setLayout(RegSetup0_qvl);
	connect(RegSetup0, SIGNAL(Close()), this, SLOT(CloseWindow()));

	//*************************************************
	//Закладка: настройки резервного
	//*************************************************
	OktServ1_Tab = new QWidget();
	RegSetup_tabWidget->addTab(OktServ1_Tab, tr("Настройки резервного"));
	RegSetup_tabWidget->setTabIcon(1, QIcon(":/images/advancedsettings.png"));

	RegSetup1_Frame=new QFrame();
	QVBoxLayout *RegSetup1_qvl = new QVBoxLayout();
	RegSetup1 = new TOktServRegSetup(RegSetup1_Frame, OktServ1);
	RegSetup1_qvl->addWidget(RegSetup1_Frame);
	OktServ1_Tab->setLayout(RegSetup1_qvl);
	connect(RegSetup1, SIGNAL(Close()), this, SLOT(CloseWindow()));

	setCentralWidget(RegSetup_tabWidget);
	RegSetup_tabWidget->setVisible(true);


	}

void TRegSetup::CloseWindow()
	{
	close();
	}
