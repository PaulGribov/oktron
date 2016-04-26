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
	ProgSettings=new TProgSettings(this);
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

	QWidget *MainWindow_CentralWidget=new QWidget();
	setCentralWidget(MainWindow_CentralWidget);

	QVBoxLayout *MainWindow_ExtLayout = new QVBoxLayout();
	MainWindow_CentralWidget->setLayout(MainWindow_ExtLayout);

	MainWindow_TabWidget=new xTabWidget();
	MainWindow_ExtLayout->addWidget(MainWindow_TabWidget);
	MainWindow_TabWidget->setStyleSheet(xTabWidgetStyleSheet.arg(19).arg(180).arg(36));
	MainWindow_TabWidget->setUsesScrollButtons(false);

	EventsLog = new TEventsLog(); //Журнал событий
	MainWindow_TabWidget->addTab(EventsLog, "");

	GeneralMeasView = new TGeneralMeasView(this); //Текущие показания
	MainWindow_TabWidget->addTab(GeneralMeasView, "");

	MainMenu = new TMainMenu(this); //Главное меню
	MainWindow_TabWidget->addTab(MainMenu, "");

	//MainMenu_Button = new xButton(GenBut, QIcon(":/images/button_cancel.png"), 32, Qt::ToolButtonTextBesideIcon);
	//MainWindow_ExtLayout->addWidget(MainMenu_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	//connect(EventsList_CloseButton, SIGNAL(clicked()), this, SLOT(Close()));

	for(int i=0;i<OKT_KEYS_NUM;i++) KeyTimeCnt[i]=0;
	KeysPoll_QTimer = new QTimer(this);
	connect(KeysPoll_QTimer , SIGNAL(timeout()), this, SLOT(KeysPoll()));
	KeysPoll_QTimer->start(1);
	KeysState=0;

	Connect_Disconnect(true);

	PrintEvent(EventsLog->MakeEvent(tr("Старт программы"), false));

	Retranslate();
	}


void MainWindow::Retranslate()
	{
	MainWindow_TabWidget->setTabText(0, tr("ЖУРНАЛ"));
	MainWindow_TabWidget->setTabText(1, tr("ПОКАЗАНИЯ"));
	MainWindow_TabWidget->setTabText(2, tr("ГЛАВНОЕ МЕНЮ"));
	}


void MainWindow::Connect_Disconnect(bool state)
	{
	for(int i=0;i<2;i++)
		{
		/*if(i==0) { */if(state!=OktServExt[i]->StateOn) OktServExt[i]->StartStop(state); /*}
		else OktServExt[i]->StartStop(false);*/

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
		GeneralMeasView->PrintDataDisabled();

#ifndef __linux__
		ProgSettings->PortsSettingsApply_Button->setEnabled(true);
#endif
		for(int i=0;i<2;i++)
			{
			OktServExt[i]->RegSetup_GetBlocksID->Stop();
			}
		DataSender_QTimer->stop();
		}
	else
		{
#ifndef __linux__
		ProgSettings->PortsSettingsApply_Button->setEnabled(false);
#endif
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
					/*
					QApplication::postEvent(QApplication::focusWidget(),
						new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Escape,
						Qt::NoModifier));
					*/
					QWidget *w=QApplication::focusWidget();
					while((w)&&(w->inherits("QMainWindow")==false))
						{
						w=w->parentWidget();
						}
					if((w)&&(w!=this)) w->close();
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
				GeneralMeasView->PrintData(od, ""/*okt_serv->Name1*/);
				}
			}

		//Перезапись предыдущих данных
		previous_od=od;
		}
	else
		{
		if((okt_serv->Master)||(okt_serv->ForceMaster)) GeneralMeasView->PrintDataDisabled();
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


