#include "toktservext.h"

int TOktServExt::server_count=0;
QMainWindow *TOktServExt::RegSetup_MainWindow=NULL;
xTabWidget *TOktServExt::RegSetup_tabWidget=NULL;
QPixmap *TOktServExt::wait_qp[4]={NULL, NULL, NULL, NULL};
QMainWindow *TOktServExt::GetBlocksID_MainWindow=NULL;
xTabWidget *TOktServExt::GetBlocksID_tabWidget=NULL;;

TOktServExt::TOktServExt(QGroupBox *PortSettings_GroupBox
#ifdef __linux__
		, const char *port_name) : TOktServ(PortSettings_GroupBox)
#else
		) : TOktServ(PortSettings_GroupBox)
#endif
	{
	server_index=server_count++;

	connect(this, SIGNAL(DataUpdate(TOscDataWithIndic &)), this, SLOT(GetDataOktServ(TOscDataWithIndic &)));
	connect(this, SIGNAL(ErrorCallback()), this, SLOT(CatchError()));

	//Сервисы осциллограмм
	OscList_MainWindow = new QMainWindow();
	OscList_MainWindow->setWindowIcon(QIcon(":/images/stocks.png"));
	ParametersView_MainWindow = new QMainWindow();
	ParametersView_MainWindow->setWindowIcon(QIcon(":/images/meter3.png"));
	OscService=new TOscService(OscList_MainWindow, ParametersView_MainWindow, true);

	//Иконки для индикатора данных
	for(int i=0;i<4;i++) { wait_qp[i]=new QPixmap(tr(":/images/Wait%1_24.png").arg(i)); }

	//Окно настроек регулятора
	if(RegSetup_MainWindow==NULL)
		{
		RegSetup_MainWindow = new QMainWindow();
		RegSetup_MainWindow->setWindowIcon(QIcon(":/images/advancedsettings.png"));
		}

	//Виджет закладок
	if(RegSetup_tabWidget==NULL)
		{
		RegSetup_tabWidget = new xTabWidget(RegSetup_MainWindow);
		RegSetup_tabWidget->setStyleSheet(xTabWidgetStyleSheet.arg(150).arg(36));
		RegSetup_MainWindow->setCentralWidget(RegSetup_tabWidget);
		RegSetup_tabWidget->setVisible(true);
		}
	RegSetup_Tab = new QWidget(RegSetup_MainWindow); //Закладка
	RegSetup_tabWidget->addTab(RegSetup_Tab/*, QIcon(":/images/advancedsettings.png")*/, "");

	//Окно списка блоков
	if(GetBlocksID_MainWindow==NULL)
		{
		GetBlocksID_MainWindow = new QMainWindow();
		GetBlocksID_MainWindow->setWindowIcon(QIcon(":/images/memory.png"));
		}

	//Виджет закладок
	if(GetBlocksID_tabWidget==NULL)
		{
		GetBlocksID_tabWidget = new xTabWidget(GetBlocksID_MainWindow);
		GetBlocksID_tabWidget->setStyleSheet(xTabWidgetStyleSheet.arg(150).arg(36));
		GetBlocksID_MainWindow->setCentralWidget(GetBlocksID_tabWidget);
		}
	GetBlocksID_Tab = new QWidget(GetBlocksID_MainWindow); //Закладка
	GetBlocksID_tabWidget->addTab(GetBlocksID_Tab/*, QIcon(":/images/memory.png")*/, "");

	//Создание сервиса работы с настройками регулятора и получения списка блоков
	RegSetup_GetBlocksID = new TOktServRegSetup(RegSetup_Tab, GetBlocksID_Tab, this);
	connect(RegSetup_GetBlocksID, SIGNAL(RegSetup_CloseSignal()), this, SLOT(RegSetup_CloseSlotExternal()));
	connect(RegSetup_GetBlocksID, SIGNAL(GetBlocksID_CloseSignal()), this, SLOT(GetBlocksID_CloseSlotExternal()));

	Master=false;
	ForceMaster=false;
	Retranslate();
	}

void TOktServExt::Retranslate()
	{
	QString Name0=(server_index==0)?tr("ОСНОВНОЙ"):tr("РЕЗЕРВНЫЙ");
	OscList_MainWindow->setWindowTitle(tr("ОСЦИЛЛОГРАММЫ -")+Name0);
	ParametersView_MainWindow->setWindowTitle(tr("ПАРАМЕТРЫ -")+Name0);
	RegSetup_MainWindow->setWindowTitle(tr("НАСТРОЙКИ РЕГУЛЯТОРА"));
	RegSetup_tabWidget->setTabText(server_index, Name0);
	GetBlocksID_MainWindow->setWindowTitle(tr("СПИСОК БЛОКОВ"));
	GetBlocksID_tabWidget->setTabText(server_index, Name0);
	}

//Слот обработки данных
void TOktServExt::GetDataOktServ(TOscDataWithIndic &od)
	{
	bool print=false;
	if((++PktCntPrescale&0x07)==0)
		{
		/*
		if(ForceMaster)
			{
			OktServIndic_Label->setPixmap(QPixmap(tr(":/images/warning_24.png")));
			}
		else
			{
			OktServIndic_Label->setPixmap(*wait_qp[(PktCntPrescale>>3) & 0x03]);
			}
		OktServIndic_Label->setPixmap(*wait_qp[(PktCntPrescale>>3) & 0x03]);
		OktServIndic_Label->clear();
		*/
		//PktCnt_Label->setText(tr("Пакеты: %1").arg(PktCnt));
		print=true;
		}
	DataProcessLocal(od, previous_od, this, 0, print);
	}

void TOktServExt::CatchError()
	{
	/*
	if(ErrorFlags & OKTSERVERR_TIMEOUT_FLAG)
		{
		PktCnt_Label->setText(tr("Таймаут"));
		}
	else if(ErrorFlags & OKTSERVERR_NO_REGULATOR_FLAG)
		{
		PktCnt_Label->setText(tr("Нет регулятора"));
		}
	else
		{
		PktCnt_Label->setText(QString("ErrFlags: 0x%1").arg(ErrorFlags, 4, 16, QLatin1Char('0')).toUpper());
		}
	OktServIndic_Label->setPixmap(QPixmap(tr(":/images/block_24.png")));
	*/
	DataProcessLocal(previous_od, previous_od, this, ErrorFlags, true);
	}

void TOktServExt::StartStop(bool StateOn)
	{
	TOktServ::StartStop(StateOn);
	if(StateOn)
		{
		RegSetup_GetBlocksID->Start_Service();
		}
	else
		{
		RegSetup_GetBlocksID->StopService();
		}
	}
