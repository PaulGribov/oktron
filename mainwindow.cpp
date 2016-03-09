#include <QtCore/QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "work.h"
#include "OscService.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
	{
	ui->setupUi(this);

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
		ui->statusBar->addWidget(OktServExt[i]->OktServIndic_Label);
		ui->statusBar->addWidget(OktServExt[i]->OktServStatus0_Label);
		ui->statusBar->addWidget(OktServExt[i]->OktServStatus1_Label);
		ui->statusBar->addWidget(OktServExt[i]->PktCnt_Label);
		ui->statusBar->addWidget(new QLabel(tr("")));
		}

	ProgSettings->GeneralSettings.AutomaticStart=true;
	ProgSettings->Load();

	//Таймер
	SystemTime_Label=new QLabel();
	ui->statusBar->addWidget(SystemTime_Label);
	SystemTime_QTimer = new QTimer(this);
	connect(SystemTime_QTimer, SIGNAL(timeout()), this, SLOT(SystemTimeTick()));
	SystemTime_QTimer->start(1000);


	DestDiskState_Label=new QLabel();
	DestDiskState_Label->setText(tr("Диска нет"));
	ui->statusBar->addWidget(DestDiskState_Label);

	//Журнал событий
	EventsLog_MainWindow = new QMainWindow();
	EventsLog_MainWindow->setWindowTitle(tr("Журнал событий"));
	EventsLog_MainWindow->setWindowIcon(QIcon(":/images/clipboard_new.png"));
	EventsLog = new TEventsLog(EventsLog_MainWindow);

	//Текущие измерения
	QVBoxLayout *LeftCol_Layout = new QVBoxLayout();
	QVBoxLayout *RightCol_Layout = new QVBoxLayout();

	QBrush brush(Qt::blue);
	brush.setStyle(Qt::SolidPattern);
	QPalette palette0;
	palette0.setBrush(QPalette::Active, QPalette::WindowText, brush);
	palette0.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
	brush.setColor(QColor(230, 88, 42));
	QPalette palette1;
	palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
	palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);

	QFont font0;
	font0.setPointSize(20);
	font0.setBold(false);
	QFont font1;
	font1.setPointSize(58);
	font1.setBold(true);
	QFont font2;
	font2.setPointSize(16);
	font2.setBold(false);
	QFont font3;
	font3.setPointSize(28);
	font3.setBold(true);
	QFont font4;
	font4.setPointSize(12);
	font4.setBold(false);
	//Ustat1_Label->setAlignment(Qt::AlignRight);

#define CREATE_LABEL_PARAMETER(a, b, c, d, e, f, g)\
		QHBoxLayout *a##_Layout = new QHBoxLayout();\
		a##0_Label = new QLabel(tr(b));\
		a##0_Label->setPalette(palette0);\
		a##0_Label->setFont(font##d);\
		a##_Layout->addWidget(a##0_Label, 0, Qt::AlignRight | Qt::AlignVCenter);\
		a##1_Label = new QLabel();\
		a##1_Label->setPalette(palette1);\
		a##1_Label->setFont(font##e);\
		a##1_Label->setFixedWidth(g);\
		a##1_Label->setAlignment(Qt::AlignRight);\
		a##_Layout->addWidget(a##1_Label, 0, Qt::AlignCenter | Qt::AlignVCenter);\
		a##2_Label = new QLabel(tr(f));\
		a##2_Label->setPalette(palette0);\
		a##2_Label->setFont(font##d);\
		a##_Layout->addWidget(a##2_Label, 0, Qt::AlignLeft | Qt::AlignVCenter);\
		c##_Layout->addLayout(a##_Layout);


#ifndef __i386__
	#define W0 180
	#define W1 105
#else
	#define W0 250
	#define W1 120
#endif
	CREATE_LABEL_PARAMETER(Reg, "Регулятор:", LeftCol, 2, 3, "", W0+20)
	CREATE_LABEL_PARAMETER(Ustat, "U стат:", LeftCol, 0, 1, "кВ", W0)
	CREATE_LABEL_PARAMETER(Istat, "I стат:", LeftCol, 0, 1, "А", W0)
	CREATE_LABEL_PARAMETER(Istat_react, "I стат.реакт:", LeftCol, 2, 3, "А.реакт.", W1)
	CREATE_LABEL_PARAMETER(Mode, "Режим:", RightCol, 2, 3, "", W0+50)
	CREATE_LABEL_PARAMETER(Uvozb, "U возб:", RightCol, 0, 1, "В", W0)
	CREATE_LABEL_PARAMETER(Ivozb, "I возб:", RightCol, 0, 1, "А", W0)
	CREATE_LABEL_PARAMETER(Istat_act, "I стат.акт:", RightCol, 2, 3, "А.реакт.", W1)

	QHBoxLayout *Meas_Layout = new QHBoxLayout();
	Meas_Layout->addLayout(LeftCol_Layout);
	Meas_Layout->addLayout(RightCol_Layout);
	ui->Meas_GroupBox->setLayout(Meas_Layout);

	QVBoxLayout *LastEventExt_Layout = new QVBoxLayout();
	CREATE_LABEL_PARAMETER(LastEvent, "", LastEventExt, 4, 4, "", 350)
	ui->LastEvent_GroupBox->setLayout(LastEventExt_Layout);

	PrintDataEnabled=false;
	ui->ParsOfBase_Button->setEnabled(false);
	ui->ParsOfReserv_Button->setEnabled(false);

	for(int i=0;i<OKT_KEYS_NUM;i++) KeyTimeCnt[i]=0;
	KeysPoll_QTimer = new QTimer(this);
	connect(KeysPoll_QTimer , SIGNAL(timeout()), this, SLOT(KeysPoll()));
	KeysPoll_QTimer->start(1);
	KeysState=0;

	Connect_Disconnect(ProgSettings->GeneralSettings.AutomaticStart);

	PrintEvent(EventsLog->MakeEvent(tr("Старт программы"), false));
	}

void MainWindow::Connect_Disconnect(bool state)
	{
	for(int i=0;i<2;i++)
		{
		if(state!=OktServExt[i]->StateOn) OktServExt[i]->StartStop(state);

		OktServExt[i]->OktServStatus1_Label->setText(OktServExt[i]->StateOn?tr("включен"):tr("выключен"));
		if(!OktServExt[i]->StateOn)
			{
			OktServExt[i]->OktServIndic_Label->setPixmap(QPixmap(":/images/stop_24.png"));
			OktServExt[i]->PktCnt_Label->setText(tr(""));
			OktServExt[i]->ParametersView_MainWindow->close();
			}
		else
			{
			OktServExt[i]->PktCnt_Label->setText(tr("Нет данных"));
			}
		}

	state=(OktServExt[0]->StateOn||OktServExt[1]->StateOn);
	QPixmap pixmap(state?":/images/red_switch_256.png":":/images/green_switch_256.png");
	QIcon ButtonIcon(pixmap);
	ui->OktServOnOff_Button->setIcon(ButtonIcon);
	ui->OktServOnOff_Button->setText(state?tr("Выключить сервер"):tr("Включить сервер"));
	if(!state)
		{
		PrintDataDisabled();
		ProgSettings->PortsSettingsApply_Button->setEnabled(true);
		for(int i=0;i<2;i++)
			{
			OktServExt[i]->RegSetup_GetBlocksID->Stop();
			}
		}
	else
		{
		ProgSettings->PortsSettingsApply_Button->setEnabled(false);

		for(int i=0;i<2;i++)
			{
			OktServExt[i]->RegSetup_GetBlocksID->ReadSettingsDesc();
			OktServExt[i]->RegSetup_GetBlocksID->GetBlocksIDSlot();
			}

		}

	//Read_qpb->setIconSize(pixmap.rect().size());
	//OktServStatus0_Label->setPixmap(QPixmap(OktServ0->StateOn?":/images/connect_24.png":":/images/disconnect_24.png"));
	}


//Слот обработки таймера
void MainWindow::SystemTimeTick()
	{
	SystemTime_Label->setText(QDateTime::currentDateTime().toString(tr("dd.MM.yyyy hh:mm:ss")));

	//Выбор регулятора для принудительного осц-ия
	OktServExt[1]->ForceMaster=OktServExt[0]->ErrorFlags || !OktServExt[0]->StateOn;
	OktServExt[0]->ForceMaster=OktServExt[1]->ErrorFlags || !OktServExt[1]->StateOn;

	/*
	for(int i=0;i<2;i++)
		//Попытка восстановить связь при таймауте
		if((OktServExt[i]->ErrorFlags & OKTSERVERR_TIMEOUT_FLAG))
			{
			//Выкл-Вкл сервера (флаги ошибок при вкл.сбрасываются)
			OktServExt[i]->StartStop(!OktServExt[i]->StateOn);
			}
	*/
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
		//okt_serv->OscService->AddOscRecord(od, print);

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
				PrintData(od, okt_serv->Name1);
				}
			}

		//Перезапись предыдущих данных
		previous_od=od;
		}
	else
		{
		if((okt_serv->Master)||(okt_serv->ForceMaster)) PrintDataDisabled();
		}
	}

//Печать данных на экран
void MainWindow::PrintData(TOscDataWithIndic &od, QString name)
	{
	Reg1_Label->setText(name);
	Mode1_Label->setText(((od.OscData.Packet0.ModeFlags1 & SW_ON_MODEFALGS1_BIT)?tr("В работе"):tr("Остановлен")));
	Ustat1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Ustat_1V/1000, 5, 'f', 2, ' '));
	Istat1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Istat_0_1A/10, 5, 'f', 0, ' '));
	Uvozb1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Uvozb_0_1V/10, 5, 'f', 1, ' '));
	Ivozb1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Ivozb_0_1A/10, 5, 'f', 1, ' '));
	Istat_react1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Istat_react_0_1A/10, 5, 'f', 0, ' '));
	Istat_act1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Istat_act_0_1A/10, 5, 'f', 0, ' '));
	if(!PrintDataEnabled)
		{
		PrintDataEnabled=true;
		ui->ParsOfBase_Button->setEnabled(OktServExt[0]->StateOn);
		ui->ParsOfReserv_Button->setEnabled(OktServExt[1]->StateOn);
		ui->RegsSetup_Button->setEnabled(true);
		ui->GetBlocksID_Button->setEnabled(true);
		}
	}

//Печать данных на экран
void MainWindow::PrintDataDisabled()
	{
	Reg1_Label->setText(tr("---"));
	Mode1_Label->setText(tr("---"));
	Ustat1_Label->setText(tr("---"));
	Istat1_Label->setText(tr("---"));
	Uvozb1_Label->setText(tr("---"));
	Ivozb1_Label->setText(tr("---"));
	Istat_react1_Label->setText(tr("---"));
	Istat_act1_Label->setText(tr("---"));

	OktServExt[0]->ParametersView_MainWindow->close();
	OktServExt[0]->OscService->ResetOscProcess();
	ui->ParsOfBase_Button->setEnabled(false);
	OktServExt[1]->ParametersView_MainWindow->close();
	OktServExt[1]->OscService->ResetOscProcess();
	ui->ParsOfReserv_Button->setEnabled(false);
	OktServExt[0]->RegSetup_CloseSlotExternal();
	ui->RegsSetup_Button->setEnabled(false);
	ui->GetBlocksID_Button->setEnabled(false);
	OktServExt[0]->GetBlocksID_CloseSlotExternal();

	PrintDataEnabled=false;
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


void MainWindow::on_ParsOfBase_Button_clicked()
	{
#ifndef __i386__
	OktServExt[0]->ParametersView_MainWindow->showFullScreen();
#else
	OktServExt[0]->ParametersView_MainWindow->showNormal();
#endif
	}

void MainWindow::on_ParsOfReserv_Button_clicked()
	{
#ifndef __i386__
	OktServExt[1]->ParametersView_MainWindow->showFullScreen();
#else
	OktServExt[1]->ParametersView_MainWindow->showNormal();
#endif
	}


void MainWindow::on_EventsLog_Button_clicked()
	{
#ifndef __i386__
	EventsLog_MainWindow->showFullScreen();
#else
	EventsLog_MainWindow->showNormal();
#endif
	}

void MainWindow::on_MakeOsc_Button_clicked()
	{
	//TEvent Event={QDateTime::currentDateTime(), tr("Старт")};
	//OscService0->OscStart(Event);
	//OscService1->OscStart(Event);
	close();
	}

void MainWindow::on_RegsSetup_Button_clicked()
	{
#ifndef __i386__
	OktServExt[0]->RegSetup_MainWindow->showFullScreen();
#else
	OktServExt[0]->RegSetup_MainWindow->showNormal();
#endif
	OktServExt[0]->RegSetup_Tab->setFocus();
	}

void MainWindow::on_ProgSettings_Button_clicked()
	{
#ifndef __i386__
	ProgSettings->showFullScreen();
#else
	ProgSettings->show();
#endif
	}

void MainWindow::on_OscListOfBase_Button_clicked()
	{
#ifndef __i386__
	OktServExt[0]->OscList_MainWindow->showFullScreen();
#else
	OktServExt[0]->OscList_MainWindow->showNormal();
#endif
	}

void MainWindow::on_OscListOfReserv_Button_clicked()
	{
#ifndef __i386__
	OktServExt[1]->OscList_MainWindow->showFullScreen();
#else
	OktServExt[1]->OscList_MainWindow->showNormal();
#endif
	}

void MainWindow::on_OktServOnOff_Button_clicked()
	{
	Connect_Disconnect(!(OktServExt[0]->StateOn|OktServExt[1]->StateOn));
	}

void MainWindow::on_GetBlocksID_Button_clicked()
	{
#ifndef __i386__
	OktServExt[0]->GetBlocksID_MainWindow->showFullScreen();
#else
	OktServExt[0]->GetBlocksID_MainWindow->showNormal();
#endif
	}
