#include "tmainmenu.h"
#include "mainwindow.h"

TMainMenu::TMainMenu(QWidget *obj_MainWindow) : QMainWindow(obj_MainWindow)
	{
	this->obj_MainWindow=(MainWindow *)obj_MainWindow;
	this->OktServExt=((MainWindow *)obj_MainWindow)->OktServExt;
	MenuCreate();
	Retranslate();
	}

void TMainMenu::MenuCreate()
	{
	QHBoxLayout *Line1_Layout = new QHBoxLayout();

	ParsOfReg_Button[0] = new xButton(GenBut, QIcon(":/images/meter3.png"));
	connect(ParsOfReg_Button[0], SIGNAL(clicked()), this, SLOT(ParsOfBase_Button_OnClick()));
	Line1_Layout->addWidget(ParsOfReg_Button[0]);

	ParsOfReg_Button[1] = new xButton(GenBut, QIcon(":/images/meter3.png"));
	connect(ParsOfReg_Button[1], SIGNAL(clicked()), this, SLOT(ParsOfReserv_Button_OnClick()));
	Line1_Layout->addWidget(ParsOfReg_Button[1]);

	RegsSetup_Button = new xButton(GenBut, QIcon(":/images/advancedsettings.png"));
	connect(RegsSetup_Button, SIGNAL(clicked()), this, SLOT(RegsSetup_Button_OnClick()));
	Line1_Layout->addWidget(RegsSetup_Button);

	QHBoxLayout *Line2_Layout = new QHBoxLayout();

	GetBlocksID_Button = new xButton(GenBut, QIcon(":/images/memory.png"));
	connect(GetBlocksID_Button, SIGNAL(clicked()), this, SLOT(GetBlocksID_Button_OnClick()));
	Line2_Layout->addWidget(GetBlocksID_Button);

	OscList_Button[0] = new xButton(GenBut, QIcon(":/images/stocks.png"));
	connect(OscList_Button[0], SIGNAL(clicked()), this, SLOT(OscListOfBase_Button_OnClick()));
	Line2_Layout->addWidget(OscList_Button[0]);

	OscList_Button[1] = new xButton(GenBut, QIcon(":/images/stocks.png"));
	connect(OscList_Button[1], SIGNAL(clicked()), this, SLOT(OscListOfReserv_Button_OnClick()));
	Line2_Layout->addWidget(OscList_Button[1]);

	ProgSettings_Button = new xButton(GenBut, QIcon(":/images/applications_system.png"));
	ProgSettings_Button->setIcon(QIcon(":/images/applications_system.png"));
	connect(ProgSettings_Button, SIGNAL(clicked()), this, SLOT(ProgSettings_Button_OnClick()));
	Line2_Layout->addWidget(ProgSettings_Button);

	QWidget *Main_Widget=new QWidget();
	QVBoxLayout *Col_Layout = new QVBoxLayout();
	Col_Layout->addLayout(Line1_Layout);
	Col_Layout->addLayout(Line2_Layout);
	Main_Widget->setLayout(Col_Layout);
	setCentralWidget(Main_Widget);
	}

void TMainMenu::Retranslate()
	{
	ParsOfReg_Button[0]->setText(tr("ПАРАМЕТРЫ\nОСНОВНОГО"));
	ParsOfReg_Button[1]->setText(tr("ПАРАМЕТРЫ\nРЕЗЕРВНОГО"));
	RegsSetup_Button->setText(tr("НАСТРОЙКИ\nРЕГУЛЯТОРА"));
	GetBlocksID_Button->setText(tr("СПИСОК\nБЛОКОВ"));
	OscList_Button[0]->setText(tr("ОСЦ.\nОСНОВ."));
	OscList_Button[1]->setText(tr("ОСЦ.\nРЕЗЕРВ."));
	ProgSettings_Button->setText(tr("НАСТРОЙК.\nПРОГРАМ."));
	}

void TMainMenu::ParsOfBase_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->ParametersView_MainWindow->showFullScreen();
#else
	OktServExt[0]->ParametersView_MainWindow->showNormal();
#endif
	}

void TMainMenu::ParsOfReserv_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[1]->ParametersView_MainWindow->showFullScreen();
#else
	OktServExt[1]->ParametersView_MainWindow->showNormal();
#endif
	}

void TMainMenu::EventsLog_Button_OnClick()
	{
	}

void TMainMenu::MakeOsc_Button_OnClick()
	{
	//TEvent Event={QDateTime::currentDateTime(), tr("Старт")};
	//OscService0->OscStart(Event);
	//OscService1->OscStart(Event);
	close();
	/*
	QApplication::postEvent(QApplication::focusWidget(),
		new QKeyEvent(QEvent::KeyPress,
		Qt::Key_Up,
		Qt::NoModifier));
	*/
	}

void TMainMenu::RegsSetup_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->RegSetup_MainWindow->showFullScreen();
#else
	OktServExt[0]->RegSetup_MainWindow->showNormal();
#endif
	OktServExt[0]->RegSetup_Tab->setFocus();
	//Сброс отображения ошибок
	for(int j=0;j<2;j++)
		{
		OktServExt[j]->RegSetup_tabWidget->setTabText(j, OktServExt[j]->Name);
		OktServExt[j]->RegSetup_tabWidget->setTabIcon(j, QIcon(""));
		}
	}

void TMainMenu::ProgSettings_Button_OnClick()
	{
#ifndef __i386__
	((MainWindow *)obj_MainWindow)->ProgSettings->showFullScreen();
#else
	((MainWindow *)obj_MainWindow)->ProgSettings->show();
#endif
	}

void TMainMenu::OscListOfBase_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->OscList_MainWindow->showFullScreen();
#else
	OktServExt[0]->OscList_MainWindow->showNormal();
#endif
	}

void TMainMenu::OscListOfReserv_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[1]->OscList_MainWindow->showFullScreen();
#else
	OktServExt[1]->OscList_MainWindow->showNormal();
#endif
	}

void TMainMenu::GetBlocksID_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->GetBlocksID_MainWindow->showFullScreen();
#else
	OktServExt[0]->GetBlocksID_MainWindow->showNormal();
#endif
	//Сброс отображения ошибок
	for(int j=0;j<2;j++)
		{
		OktServExt[j]->GetBlocksID_tabWidget->setTabText(j, OktServExt[j]->Name);
		OktServExt[j]->GetBlocksID_tabWidget->setTabIcon(j, QIcon(""));
		}
	}
