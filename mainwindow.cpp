#include <QtCore/QCoreApplication>
#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "work.h"
#include "OscService.h"

#ifndef __i386__
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#endif

int MainWindow::IdleTimeout=0;

#define CREATE_STATUS_LABEL(a,b) {\
	QVBoxLayout *a = new QVBoxLayout();\
	a##0##b=new QLabel();\
	a##0##b->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);\
	a##0##b->setStyleSheet("\
color: rgb(154,154,154);\
font: 18pt;\
");\
a->addWidget(a##0##b);\
a##1##b=new QLabel();\
a##1##b->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);\
a##1##b->setStyleSheet("\
color: rgb(154,154,154);\
font: 21pt;\
");\
a->addWidget(a##1##b);\
Status_Layout->addLayout(a); }

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui(new Ui::MainWindow)
	{
	watchdog_fd=NULL;
	ui->setupUi(this);
	setStyleSheet("background-color: rgb(235,236,236);");

#ifndef __i386__
	showFullScreen();
#else
	show();
#endif
	setWindowTitle(QApplication::applicationName());

	QHBoxLayout *Status_Layout = new QHBoxLayout();

	CREATE_STATUS_LABEL(Reg, _Label[0])
	CREATE_STATUS_LABEL(Reg, _Label[1])
	CREATE_STATUS_LABEL(SystemTime, _Label)
	CREATE_STATUS_LABEL(DestDiskState, _Label)

	ChangesCopyBetweenRegsState=false;

	//Создание серверов
	ProgSettings=new TProgSettings(this);
	for(int i=0;i<2;i++)
		{
		connect(OktServExt[i], SIGNAL(DataProcessLocal(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool)), this, SLOT(DataProcess(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool)));
		PrintDataEnabled[i]=true;
		}
	//Подключить кнопки дублирования настроек
	connect(OktServExt[0]->RegSetup_GetBlocksID->ChangesCopyBetweenRegs_Button, SIGNAL(clicked()), this, SLOT(ChangesCopyBetweenRegsStateChanged()));
	connect(OktServExt[1]->RegSetup_GetBlocksID->ChangesCopyBetweenRegs_Button, SIGNAL(clicked()), this, SLOT(ChangesCopyBetweenRegsStateChanged()));

	ProgSettings->GeneralSettings.AutomaticStart=true;
	ProgSettings->Load();

	time_scale=0;
	SystemTime_QTimer = new QTimer(this);
	connect(SystemTime_QTimer, SIGNAL(timeout()), this, SLOT(SystemTimeTick()));
	SystemTime_QTimer->start(1000);

	DataSender_QTimer = new QTimer(this);
	connect(DataSender_QTimer , SIGNAL(timeout()), this, SLOT(DataSender()));
	DataSender_QTimer->stop();

	QWidget *MainWindow_CentralWidget=new QWidget();
	setCentralWidget(MainWindow_CentralWidget);

	QVBoxLayout *MainWindow_ExtLayout = new QVBoxLayout();
	MainWindow_CentralWidget->setLayout(MainWindow_ExtLayout);

	MainWindow_TabWidget=new xTabWidget();
	MainWindow_ExtLayout->addWidget(MainWindow_TabWidget);
	MainWindow_TabWidget->setStyleSheet(xTabWidgetStyleSheet.arg(19).arg(180).arg(36));
	MainWindow_TabWidget->setUsesScrollButtons(false);
	MainWindow_TabWidget->setIconSize(QSize(32,32));

	EventsLog = new TEventsLog(this); //Журнал событий
	MainWindow_TabWidget->addTab(EventsLog, "");
	MainWindow_TabWidget->setTabIcon(0, QIcon(":/images/clipboard_new.png"));

	GeneralMeasView = new TGeneralMeasView(this); //Текущие показания
	MainWindow_TabWidget->addTab(GeneralMeasView, "");
	MainWindow_TabWidget->setTabIcon(1, QIcon(":/images/meter3.png"));

	MainMenu = new TMainMenu(this); //Главное меню
	MainWindow_TabWidget->addTab(MainMenu, "");
	MainWindow_TabWidget->setTabIcon(2, QIcon(":/images/advancedsettings.png"));

	MainWindow_TabWidget->setCurrentIndex(1); //Текущая закладка - "ПОКАЗАНИЯ"

	MainWindow_ExtLayout->addLayout(Status_Layout);

	Baner_MainWindow = new QMainWindow();
	QWidget *Baner_CentralWidget=new QWidget(Baner_MainWindow);
	Baner_MainWindow->setCentralWidget(Baner_CentralWidget);
	QVBoxLayout *Baner_Layout = new QVBoxLayout();
	Baner_CentralWidget->setLayout(Baner_Layout);
	Baner0_Label = new QLabel(Baner_MainWindow);
	Baner0_Label->setStyleSheet("\
	color: rgb(154,154,154);\
	font: 32pt;\
	");\
	Baner_Layout->addWidget(Baner0_Label, 0, Qt::AlignHCenter | Qt::AlignBottom);
	Baner1_Label = new QLabel(Baner_MainWindow);
	Baner1_Label->setStyleSheet("\
	color: rgb(154,154,154);\
	font: 32pt;\
	");\
	Baner_Layout->addWidget(Baner1_Label, 0, Qt::AlignHCenter | Qt::AlignTop);

	for(int i=0;i<OKT_KEYS_NUM;i++) KeyTimeCnt[i]=0;
	KeysPoll_QTimer = new QTimer(this);
	connect(KeysPoll_QTimer , SIGNAL(timeout()), this, SLOT(KeysPoll()));
	KeysPoll_QTimer->start(1);
	KeysState=0;

	Connect_Disconnect(true);

	PrintEvent(EventsLog->MakeEvent(tr("Старт программы"), false));

	installEventFilter(this);

	Retranslate();

#ifndef __i386__
	//Установка Wdt - 30c таймаут
	int timeout=30;
	watchdog_fd = open("/dev/watchdog", O_RDWR);
	if(watchdog_fd)
		{
		ioctl(watchdog_fd, WDIOC_SETTIMEOUT, &timeout);
		}
#endif
	}

void MainWindow::ChangesCopyBetweenRegsStateChanged()
	{
	bool state=!ChangesCopyBetweenRegsState;
	ChangesCopyBetweenRegsState=state;
	QIcon icon;
	if(state)
		{
		icon=QIcon(":/images/but_on_enabled.png");
		//Установить свзяь между осн. и резерв. при настройке
		connect(OktServExt[0]->RegSetup_GetBlocksID, SIGNAL(RegSetupReqEvent_Signal(TRegSetupCmd,int)), OktServExt[1]->RegSetup_GetBlocksID, SLOT(RegSetupReqEvent_Slot(TRegSetupCmd,int)));
		connect(OktServExt[0]->RegSetup_GetBlocksID, SIGNAL(ChangeValEvent_Signal(int,int)), OktServExt[1]->RegSetup_GetBlocksID, SLOT(ChangeValEvent_Slot(int,int)));
		connect(OktServExt[1]->RegSetup_GetBlocksID, SIGNAL(RegSetupReqEvent_Signal(TRegSetupCmd,int)), OktServExt[0]->RegSetup_GetBlocksID, SLOT(RegSetupReqEvent_Slot(TRegSetupCmd,int)));
		connect(OktServExt[1]->RegSetup_GetBlocksID, SIGNAL(ChangeValEvent_Signal(int,int)), OktServExt[0]->RegSetup_GetBlocksID, SLOT(ChangeValEvent_Slot(int,int)));
		}
	else
		{
		icon=QIcon(":/images/but_off_enabled.png");
		//Разорвать свзяь между осн. и резерв. при настройке
		disconnect(OktServExt[0]->RegSetup_GetBlocksID, SIGNAL(RegSetupReqEvent_Signal(TRegSetupCmd,int)), OktServExt[1]->RegSetup_GetBlocksID, SLOT(RegSetupReqEvent_Slot(TRegSetupCmd,int)));
		disconnect(OktServExt[0]->RegSetup_GetBlocksID, SIGNAL(ChangeValEvent_Signal(int,int)), OktServExt[1]->RegSetup_GetBlocksID, SLOT(ChangeValEvent_Slot(int,int)));
		disconnect(OktServExt[1]->RegSetup_GetBlocksID, SIGNAL(RegSetupReqEvent_Signal(TRegSetupCmd,int)), OktServExt[0]->RegSetup_GetBlocksID, SLOT(RegSetupReqEvent_Slot(TRegSetupCmd,int)));
		disconnect(OktServExt[1]->RegSetup_GetBlocksID, SIGNAL(ChangeValEvent_Signal(int,int)), OktServExt[0]->RegSetup_GetBlocksID, SLOT(ChangeValEvent_Slot(int,int)));
		}
	OktServExt[0]->RegSetup_GetBlocksID->ChangesCopyBetweenRegs_Button->setIcon(icon);
	OktServExt[1]->RegSetup_GetBlocksID->ChangesCopyBetweenRegs_Button->setIcon(icon);
	}

void MainWindow::Retranslate()
	{
	MainWindow_TabWidget->setTabText(0, tr("ЖУРНАЛ"));
	MainWindow_TabWidget->setTabText(1, tr("ПОКАЗАНИЯ"));
	MainWindow_TabWidget->setTabText(2, tr("МЕНЮ"));
	Reg0_Label[0]->setText(tr("Основной:"));
	Reg0_Label[1]->setText(tr("Резервный:"));
	SystemTime0_Label->setText(tr("Системное время:"));
	DestDiskState0_Label->setText(tr("Накопитель:"));
	}

void MainWindow::Connect_Disconnect(bool state)
	{
	for(int i=0;i<2;i++)
		{
#ifndef __linux__
		if(i==0) {
#endif
			if(state!=OktServExt[i]->StateOn) OktServExt[i]->StartStop(state);
#ifndef __linux__
			} else OktServExt[i]->StartStop(false);
#endif
		if(!OktServExt[i]->StateOn)
			{
			RegViewSetEnabled(false, i);
			}
		else
			{
			Reg1_Label[i]->setText(tr("Нет данных"));
			}
		}

	state=(OktServExt[0]->StateOn||OktServExt[1]->StateOn);

	if(!state)
		{
		GeneralMeasView->PrintDataDisabled();
#ifndef __linux__
		ProgSettings->PortsSettingsApply_Button->setEnabled(true);
#endif
		for(int i=0;i<2;i++)
			{
			RegViewSetEnabled(false, i);
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
			RegViewSetEnabled(OktServExt[i]->StateOn, i);
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
	QDateTime dt=QDateTime::currentDateTime();
	SystemTime1_Label->setText(dt.toString(tr("dd.MM.yyyy hh:mm:ss")));
	/*
	if(++IdleTimeout==30)
		{
		ChildWindowClose(true);
		MainWindow_TabWidget->setCurrentIndex(0);
		EventsLog->GotoLastEvent();
		EventsLog->EventsList_TableView->setFocus();
		}
	*/
	//SystemTime1_Label->setText(QString("%1").arg(MainWindow::IdleTimeout));

	time_scale++;

#ifdef __linux__
	if((time_scale & 0x3)==0x3)
		{
		WDT_RST_MAINWINDOW() //Сброс Wdt

		SystemTime_QTimer->stop();
		char *dev_name = LastConnectedDiskFind();
		bool DestDiskStateNew = strlen(dev_name)>0;
		if(DestDiskState != DestDiskStateNew)
			{
			DestDiskState = DestDiskStateNew;
			if(DestDiskStateNew)
				{
				DestDiskState1_Label->setText(dev_name);
				Baner0_Label->setText(tr("Обнаружен внешний"));
				Baner1_Label->setText(tr("USB-накопитель, ожидайте..."));
#ifndef __i386__
				Baner_MainWindow->showFullScreen();
#else
				Baner_MainWindow->showNormal();
#endif
				PrintEvent(EventsLog->MakeEvent(tr("Обнаружен внешний USB-накопитель..."), false));
				Connect_Disconnect(false); //Остановка сервера для нормальной работы с USB
				QApplication::processEvents();
				sleep(3);

				WDT_RST_MAINWINDOW() //Сброс Wdt

				//Повторное определение его имени диска
				dev_name = LastConnectedDiskFind();
				DestDiskState1_Label->setText(dev_name);
				bool is_hex, result=CopyToDestDisk(dev_name, is_hex);
				QString res0_txt, res1_txt;
				if(is_hex)
					{
					if(result)
						{
						res0_txt=tr("Файлы ПО успешно выгружены");
						res1_txt=tr("со съёмного диска");
						}
					else
						{
						res0_txt=tr("Ошибка при выгрузке ПО");
						res1_txt=tr("со съёмного диска");
						}
					}
				else
					{
					if(result)
						{
						res0_txt=tr("Информация успешно выгружена");
						res1_txt=tr("на съёмный диск");
						}
					else
						{
						res0_txt=tr("Ошибка при выгрузке информации");
						res1_txt=tr("на съёмный диск");
						}
					}
				PrintEvent(EventsLog->MakeEvent(res0_txt+" "+res1_txt, false));
				Connect_Disconnect(true); //Пуск сервера
				WDT_RST_MAINWINDOW() //Сброс Wdt
				Baner0_Label->setText(res0_txt);
				Baner1_Label->setText(res1_txt);
				QApplication::processEvents();
				sleep(5);
				WDT_RST_MAINWINDOW() //Сброс Wdt
				Baner_MainWindow->close();
				}
			}
		if(!DestDiskStateNew)
			{
			DestDiskState1_Label->setText(tr("НЕТ диска"));
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


void MainWindow::ChildWindowClose(bool CloseAnyway)
	{
	QWidget *w=QApplication::focusWidget();
	//Нет фокуса - закрыть все окна
	if(w==NULL)
		{
		OktServExt[0]->ParametersView_MainWindow->close();
		OktServExt[1]->ParametersView_MainWindow->close();
		OktServExt[0]->RegSetup_MainWindow->close();
		OktServExt[1]->RegSetup_MainWindow->close();
		OktServExt[0]->GetBlocksID_MainWindow->close();
		OktServExt[1]->GetBlocksID_MainWindow->close();
		ProgSettings->close();
		return;
		}
	//Если кнопка "Выход"
	else if(!CloseAnyway)
		{
		//.. и есть родитель из закладок, то переход на закладки
		if(	w->inherits("xButton") ||
		w->inherits("xTableView") ||
		w->inherits("RegSetupTableView") ||
		w->inherits("QTableView") ||
		w->inherits("QDateTimeEdit")
		)
			{
			while(w->inherits("QMainWindow")==false)
				{
				if((w->inherits("QTabBar"))||(w->inherits("xTabWidget")))
					{
					w->setFocus();
					return;
					}
				w=w->parentWidget();
				}
			return;
			}
		// Настройки регулятора - редакторы - просто закрыть виджет
		else if(w->inherits("QComboBox") ||
		w->inherits("HexSpinBox") ||
		w->inherits("DoubleSpinBox")
		)
			{
			w->close();
			return;
			}
		}
	//Поиск и закрытие главного окна
	while(w->inherits("QMainWindow")==false)
		{
		w=w->parentWidget();
		}
	if((w!=this)&&(w!=MainMenu)) w->close();
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
				QWidget *w=QApplication::focusWidget();
				if(w==NULL)
					{
					//Нет фокуса - ничего не делаем
					}
				else if(mask==OKT_KEY_LEFT_MASK)
					{
					QApplication::postEvent(w,
					new QKeyEvent(QEvent::KeyPress,
					Qt::Key_Left,
					Qt::NoModifier));
					}
				else if(mask==OKT_KEY_RIGHT_MASK)
					{
					QApplication::postEvent(w,
					new QKeyEvent(QEvent::KeyPress,
					Qt::Key_Right,
					Qt::NoModifier));
					}
				else if(mask==OKT_KEY_ENTER_MASK)
					{
					//Выход из списка по нажатию ввода
					if(w->inherits("xTableView"))
						{
						ChildWindowClose(false);
						}
					else
						{
						QApplication::postEvent(w,
						new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Space,
						Qt::NoModifier));
						QApplication::postEvent(w,
						new QKeyEvent(QEvent::KeyRelease,
						Qt::Key_Space,
						Qt::NoModifier));
						}
					}
				else if(mask==OKT_KEY_ESC_MASK)
					{
					ChildWindowClose(false);
					}
				else
					{
					CheckGrowKeys(mask);
					}
				}
			else if(((KeyTimeCnt[i]>1000)&&(KeyTimeCnt[i]%50==0)) ||
			((KeyTimeCnt[i]>3000)&&(KeyTimeCnt[i]%5==0)) ||
			((KeyTimeCnt[i]>6000))
			)
				{
				CheckGrowKeys(mask);
				}

			if(++KeyTimeCnt[i]<1000) IdleTimeout=0;
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
	static TOktServExt *cur_okt_serv;

	if(error_flags==0)
		{
		okt_serv->Master = (od.OscData.Packet0.ModeFlags1 & REG_SELECTED_MODEFALGS1_BIT)?true:false;

		//Осциллографирование данных
		okt_serv->OscService->AddOscRecord(od, print);

		//Если регулятор в работе или принудительно используется, то обработка событий...
		if(okt_serv->Master)
			{
			if(cur_okt_serv!=okt_serv)
				{
				cur_okt_serv=okt_serv;
				if(okt_serv->Master) PrintEvent(EventsLog->MakeEvent(tr("Переход на ")+okt_serv->Name, false));
				}
data_acquire_loc:
			KeysState=od.Packet3.KeysState;

			if(okt_serv->FirstDataAsquired)
				{
				//Вычисление события и его обработка
				TEventExt *e=EventsLog->CheckDataEvent(od, previous_od);
				if(e) PrintEvent(e);
				}
			else
				{
				okt_serv->FirstDataAsquired=true;
				}

			//Вывод данных на форму
			if(print)
				{
				GeneralMeasView->PrintData(od, cur_okt_serv->Name);
				RegViewSetEnabled(true, okt_serv->server_index);
				}

			//Перезапись предыдущих данных
			previous_od=od;
			}
		else if(((TOktServExt *)(okt_serv->okt_serv_other))->ErrorFlags)
			{
			//Проверка перехода на др.регулятор в "пассивном" режиме - отслеживания нуля в бите выбора
			// -- для случая отсутствия связи с ведущим
			if(cur_okt_serv==okt_serv)
				{
				cur_okt_serv=((TOktServExt *)(okt_serv->okt_serv_other));
				PrintEvent(EventsLog->MakeEvent(tr("Переход на ")+((TOktServExt *)(okt_serv->okt_serv_other))->Name+" (нет связи)", false));
				}
			goto data_acquire_loc;
			}
		}
	else
		{
		PrintEvent(EventsLog->MakeEvent(tr("Нет связи с блоком \"")+okt_serv->Name+"\"", false));

		RegViewSetEnabled(false, okt_serv->server_index);

		if(((TOktServExt *)(okt_serv->okt_serv_other))->ErrorFlags)
			{
			GeneralMeasView->PrintDataDisabled();
			}
		}
	}

void MainWindow::RegViewSetEnabled(bool e, int i)
	{
	if(e)
		{
		if(!PrintDataEnabled[i])
			{
			PrintDataEnabled[i]=true;

			MainMenu->ParsOfReg_Button[i]->setEnabled(OktServExt[i]->StateOn);
			}
		}
	else
		{
		if(PrintDataEnabled[i])
			{
			OktServExt[i]->ParametersView_MainWindow->close();
			OktServExt[i]->OscService->ResetOscProcess();
			MainMenu->ParsOfReg_Button[i]->setEnabled(false);

			PrintDataEnabled[i]=false;
			}
		}
	}


bool MainWindow::eventFilter(QObject *obj, QEvent *e)
	{
	switch(e->type())
		{
		case QEvent::KeyPress:
		case QEvent::MouseMove:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonDblClick:
			{
			IdleTimeout=0;
			}
			break;

		default:
			break;
		}

	return QObject::eventFilter(obj, e);
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


