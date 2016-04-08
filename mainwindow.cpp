#include <QtCore/QCoreApplication>
#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "work.h"
#include "OscService.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
	{
	ui->setupUi(this);
	setStyleSheet("background-color: rgb(235,236,236);");

#ifndef __i386__
	showFullScreen();
#else
	show();
#endif
	setWindowTitle(QApplication::applicationName());

	//Создание серверов
	ProgSettings=new TProgSettings(this, &OktServExt[0], &OktServExt[1]);
	for(int i=0;i<2;i++)
		{
		connect(OktServExt[i], SIGNAL(DataProcessLocal(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool)), this, SLOT(DataProcess(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool)));
		}

	ProgSettings->GeneralSettings.AutomaticStart=true;
	ProgSettings->Load();

	//Таймер
	SystemTime_Label=new QLabel();
	//ui->statusBar->addWidget(SystemTime_Label);
	SystemTime_QTimer = new QTimer(this);
	connect(SystemTime_QTimer, SIGNAL(timeout()), this, SLOT(SystemTimeTick()));
	SystemTime_QTimer->start(1000);

	DataSender_QTimer = new QTimer(this);
	connect(DataSender_QTimer , SIGNAL(timeout()), this, SLOT(DataSender()));
	DataSender_QTimer->stop();


	DestDiskState_Label=new QLabel();
	DestDiskState_Label->setText(tr("Диска нет"));
	//ui->statusBar->addWidget(DestDiskState_Label);

	//Журнал событий
	EventsLog = new TEventsLog();

	for(int i=0;i<OKT_KEYS_NUM;i++) KeyTimeCnt[i]=0;
	KeysPoll_QTimer = new QTimer(this);
	connect(KeysPoll_QTimer , SIGNAL(timeout()), this, SLOT(KeysPoll()));
	KeysPoll_QTimer->start(1);
	KeysState=0;

	Connect_Disconnect(ProgSettings->GeneralSettings.AutomaticStart);

	PrintEvent(EventsLog->MakeEvent(tr("Старт программы"), false));

	EventsLog->showNormal();

	MenuCreate();
	Retranslate();
	}

void MainWindow::MenuCreate()
	{
	QHBoxLayout *Line1_Layout = new QHBoxLayout();

	ParsOfBase_Button = new xButton(QIcon(":/images/meter3.png"));
	connect(ParsOfBase_Button, SIGNAL(clicked()), this, SLOT(ParsOfBase_Button_OnClick()));
	Line1_Layout->addWidget(ParsOfBase_Button);

	ParsOfReserv_Button = new xButton(QIcon(":/images/meter3.png"));
	connect(ParsOfReserv_Button, SIGNAL(clicked()), this, SLOT(ParsOfReserv_Button_OnClick()));
	Line1_Layout->addWidget(ParsOfReserv_Button);

	RegsSetup_Button = new xButton(QIcon(":/images/advancedsettings.png"));
	connect(RegsSetup_Button, SIGNAL(clicked()), this, SLOT(RegsSetup_Button_OnClick()));
	Line1_Layout->addWidget(RegsSetup_Button);

	QHBoxLayout *Line2_Layout = new QHBoxLayout();

	GetBlocksID_Button = new xButton(QIcon(":/images/memory.png"));
	connect(GetBlocksID_Button, SIGNAL(clicked()), this, SLOT(GetBlocksID_Button_OnClick()));
	Line2_Layout->addWidget(GetBlocksID_Button);

	EventsLog_Button = new xButton(QIcon(":/images/clipboard_new.png"));
	connect(EventsLog_Button, SIGNAL(clicked()), this, SLOT(EventsLog_Button_OnClick()));
	Line2_Layout->addWidget(EventsLog_Button);

	ProgSettings_Button = new xButton(QIcon(":/images/applications_system.png"));
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

void MainWindow::Retranslate()
	{
	ParsOfBase_Button->setText(tr("ПАРАМЕТРЫ\nОСНОВНОГО"));
	ParsOfReserv_Button->setText(tr("ПАРАМЕТРЫ\nРЕЗЕРВНОГО"));
	RegsSetup_Button->setText(tr("НАСТРОЙКИ\nРЕГУЛЯТОРА"));
	GetBlocksID_Button->setText(tr("СПИСОК\nБЛОКОВ"));
	EventsLog_Button->setText(tr("ЖУРНАЛ\nСОБЫТИЙ"));
	ProgSettings_Button->setText(tr("НАСТРОЙКИ\nПРОГРАММЫ"));
	}


void MainWindow::Connect_Disconnect(bool state)
	{
	for(int i=0;i<2;i++)
		{
		if(i==0) { if(state!=OktServExt[i]->StateOn) OktServExt[i]->StartStop(state); }
		else OktServExt[i]->StartStop(false);

		if(!OktServExt[i]->StateOn)
			{
			//OktServExt[i]->OktServIndic_Label->setPixmap(QPixmap(":/images/stop_24.png"));
			OktServExt[i]->ParametersView_MainWindow->close();
			}
		else
			{
			//OktServExt[i]->PktCnt_Label->setText(tr("Нет данных"));
			}
		}

	state=(OktServExt[0]->StateOn||OktServExt[1]->StateOn);
	QPixmap pixmap(state?":/images/red_switch_256.png":":/images/green_switch_256.png");
	QIcon ButtonIcon(pixmap);
	//ui->OktServOnOff_Button->setIcon(ButtonIcon);
	//ui->OktServOnOff_Button->setText(state?tr("Выключить сервер"):tr("Включить сервер"));
	if(!state)
		{
		//PrintDataDisabled();
		ProgSettings->PortsSettingsApply_Button->setEnabled(true);
		for(int i=0;i<2;i++)
			{
			OktServExt[i]->RegSetup_GetBlocksID->Stop();
			}
		DataSender_QTimer->stop();
		}
	else
		{
		ProgSettings->PortsSettingsApply_Button->setEnabled(false);

		for(int i=0;i<2;i++)
			{
			OktServExt[i]->RegSetup_GetBlocksID->ReadSettingsDesc();
			OktServExt[i]->RegSetup_GetBlocksID->GetBlocksIDSlot();
			}
		DataSender_QTimer->start(20);
		}

	}

void MainWindow::DataSender()
	{
	if(OktServExt[0]->StateOn) OktServExt[0]->DataSender();
	if(OktServExt[1]->StateOn) OktServExt[1]->DataSender();
	}


//Слот обработки таймера
void MainWindow::SystemTimeTick()
	{
	SystemTime_Label->setText(QDateTime::currentDateTime().toString(tr("dd.MM.yyyy hh:mm:ss")));

	//Выбор регулятора для принудительного осц-ия
	OktServExt[1]->ForceMaster=OktServExt[0]->ErrorFlags || !OktServExt[0]->StateOn;
	OktServExt[0]->ForceMaster=OktServExt[1]->ErrorFlags || !OktServExt[1]->StateOn;

#ifdef __linux__

	static int scale;
	if((scale++ & 0x3)==0x3)
		{
		SystemTime_QTimer->stop();
		char *dev_name = LastConnectedDiskFind();
		bool DestDiskStateNew = strlen(dev_name)>0;
		if(DestDiskState != DestDiskStateNew)
			{
			DestDiskState = DestDiskStateNew;
			DestDiskState_Label->setText(DestDiskStateNew?dev_name:tr("НЕТ диска"));
			if(DestDiskStateNew)
				{
				PrintEvent(EventsLog->MakeEvent(CopyToDestDisk(dev_name)?tr("Информация успешно выгружена на съёмный диск"):tr("Ошибка при выгрузке информации на съёмный диск"), false));
				}
			}
		SystemTime_QTimer->start(1000);
		}
#endif
	}

//Вывод события
void MainWindow::PrintEvent(TEventExt *e)
	{
	if(e)
		{
		/*
		//Надпись на экране
		LastEvent0_Label->setText(e->Event.DateTime.toString(tr("dd.MM.yyyy hh:mm:ss")));
		LastEvent1_Label->setText(e->Event.Text);

		//Если присвоен номер осц., то выполнить осциллографиование
		if(e->OscIndex>=0)
			{
			LastEvent2_Label->setText(tr("Осц.#%1").arg(e->OscIndex));
			for(int i=0;i<2;i++) if(OktServExt[i]->StateOn) OktServExt[i]->OscService->OscStart(e);
			}
		else
			{
			LastEvent2_Label->setText(tr(""));
			}
		*/
		//Если присвоен номер осц., то выполнить осциллографиование
		if(e->OscIndex>=0) for(int i=0;i<2;i++) if(OktServExt[i]->StateOn) OktServExt[i]->OscService->OscStart(e);

		//Удаление события созданного в MakeEvent
		delete e;
		}
	}


void MainWindow::CheckGrowKeys(unsigned short mask)
	{
	if(mask==OKT_KEY_UP_MASK)
		{
		QApplication::postEvent(QApplication::focusWidget(),
			new QKeyEvent(QEvent::KeyPress,
			Qt::Key_Up,
			Qt::NoModifier));
		}
	else if(mask==OKT_KEY_DOWN_MASK)
		{
		QApplication::postEvent(QApplication::focusWidget(),
			new QKeyEvent(QEvent::KeyPress,
			Qt::Key_Down,
			Qt::NoModifier));
		}
	}

void MainWindow::KeysPoll()
	{
	static int DecPrescale;
	for(int i=0;i<OKT_KEYS_NUM;i++)
		{
		unsigned short mask = (1<<i);
		if(KeysState & mask)
			{
			if(KeyTimeCnt[i]==0)
				{
				if(mask==OKT_KEY_LEFT_MASK)
					{
					QApplication::postEvent(QApplication::focusWidget(),
						new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Left,
						Qt::NoModifier));
					}
				else if(mask==OKT_KEY_RIGHT_MASK)
					{
					QApplication::postEvent(QApplication::focusWidget(),
						new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Right,
						Qt::NoModifier));
					}
				else if((mask==OKT_KEY_ENTER_MASK)&&(QApplication::focusWidget()->objectName()!="OktServOnOff_Button"))
					{
					QApplication::postEvent(QApplication::focusWidget(),
								new QKeyEvent(QEvent::KeyPress,
								Qt::Key_Space,
								Qt::NoModifier));
					QApplication::postEvent(QApplication::focusWidget(),
								new QKeyEvent(QEvent::KeyRelease,
								Qt::Key_Space,
								Qt::NoModifier));
					}
				else if(mask==OKT_KEY_ESC_MASK)
					{
					QApplication::postEvent(QApplication::focusWidget(),
						new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Escape,
						Qt::NoModifier));
					}
				else
					CheckGrowKeys(mask);

				}
			else if(((KeyTimeCnt[i]>1000)&&(KeyTimeCnt[i]%50==0)) ||
				((KeyTimeCnt[i]>3000)&&(KeyTimeCnt[i]%5==0)) ||
				((KeyTimeCnt[i]>6000))
				)
				{
				CheckGrowKeys(mask);
				}


			KeyTimeCnt[i]++;
			}
		else
			{
			if(DecPrescale%50==0) KeyTimeCnt[i]=0;
			}
		}
	DecPrescale++;
	}


//Обработка данных от сервера
void MainWindow::DataProcess(TOscDataWithIndic &od, TOscDataWithIndic &previous_od, TOktServExt *okt_serv, int error_flags, bool print)
	{
	if((error_flags&(~OKTSERVERR_NO_REGULATOR_FLAG))==0)
		{
		okt_serv->Master = od.OscData.Packet0.ModeFlags1 & REG_SELECTED_MODEFALGS1_BIT;

		//Осциллографирование данных
		okt_serv->OscService->AddOscRecord(od, print);

		//SystemTime_Label->setText(QString("0x%1").arg(okt_serv->PacketUpdatedFlags, 4, 16, QLatin1Char('0')).toUpper());

		//Если регулятор в работе или принудительно используется, то обработка событий...
		if((okt_serv->Master)||(okt_serv->ForceMaster))
			{
			KeysState=od.Packet3.KeysState;

			//Вычисление события и его обработка
			TEventExt *e=EventsLog->CheckDataEvent(od, previous_od);
			if(e) PrintEvent(e);

			//Вывод данных на форму
			if(print)
				{
				//PrintData(od, okt_serv->Name1);
				}
			}

		//Перезапись предыдущих данных
		previous_od=od;
		}
	else
		{
		//if((okt_serv->Master)||(okt_serv->ForceMaster)) PrintDataDisabled();
		}
	}



MainWindow::~MainWindow()
	{
	delete ui;
	}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}


void MainWindow::ParsOfBase_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->ParametersView_MainWindow->showFullScreen();
#else
	OktServExt[0]->ParametersView_MainWindow->showNormal();
#endif
	}

void MainWindow::ParsOfReserv_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[1]->ParametersView_MainWindow->showFullScreen();
#else
	OktServExt[1]->ParametersView_MainWindow->showNormal();
#endif
	}


void MainWindow::EventsLog_Button_OnClick()
	{
#ifndef __i386__
	EventsLog->showFullScreen();
#else
	EventsLog->showNormal();
#endif
	}

void MainWindow::MakeOsc_Button_OnClick()
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

void MainWindow::RegsSetup_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->RegSetup_MainWindow->showFullScreen();
#else
	OktServExt[0]->RegSetup_MainWindow->showNormal();
#endif
	OktServExt[0]->RegSetup_Tab->setFocus();
	}

void MainWindow::ProgSettings_Button_OnClick()
	{
#ifndef __i386__
	ProgSettings->showFullScreen();
#else
	ProgSettings->show();
#endif
	}

void MainWindow::OscListOfBase_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->OscList_MainWindow->showFullScreen();
#else
	OktServExt[0]->OscList_MainWindow->showNormal();
#endif
	}

void MainWindow::OscListOfReserv_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[1]->OscList_MainWindow->showFullScreen();
#else
	OktServExt[1]->OscList_MainWindow->showNormal();
#endif
	}

void MainWindow::OktServOnOff_Button_OnClick()
	{
	Connect_Disconnect(!(OktServExt[0]->StateOn|OktServExt[1]->StateOn));
	}

void MainWindow::GetBlocksID_Button_OnClick()
	{
#ifndef __i386__
	OktServExt[0]->GetBlocksID_MainWindow->showFullScreen();
#else
	OktServExt[0]->GetBlocksID_MainWindow->showNormal();
#endif
	}
