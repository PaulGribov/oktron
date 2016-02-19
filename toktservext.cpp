#include "toktservext.h"

QMainWindow *TOktServExt::RegSetup_MainWindow=NULL;
xTabWidget *TOktServExt::RegSetup_tabWidget=NULL;
QPixmap *TOktServExt::wait_qp[4]={NULL, NULL, NULL, NULL};
QMainWindow *TOktServExt::GetBlocksID_MainWindow=NULL;
xTabWidget *TOktServExt::GetBlocksID_tabWidget=NULL;;

TOktServExt::TOktServExt(QGroupBox *PortSettings_GroupBox, QString name0, QString name1) : TOktServ(PortSettings_GroupBox, name0)
	{
	Name0=name0;
	Name1=name1;
	connect(this, SIGNAL(DataUpdate(TOscDataWithIndic &)), this, SLOT(GetDataOktServ(TOscDataWithIndic &)));
	connect(this, SIGNAL(ErrorCallback()), this, SLOT(CatchError()));

	OktServStatus0_Label=new QLabel(Name1);
	OktServStatus1_Label=new QLabel();
	OktServIndic_Label=new QLabel();
	PktCnt_Label=new QLabel();

	//Сервисы осциллограмм
	OscList_MainWindow = new QMainWindow();
	OscList_MainWindow->setWindowTitle(tr("Осциллограммы ")+Name0);
	OscList_MainWindow->setWindowIcon(QIcon(":/images/stocks.png"));
	ParametersView_MainWindow = new QMainWindow();
	ParametersView_MainWindow->setWindowTitle(tr("Параметры ")+Name0);
	ParametersView_MainWindow->setWindowIcon(QIcon(":/images/meter3.png"));
	OscService=new TOscService(OscList_MainWindow, ParametersView_MainWindow, true);

	//Иконки для индикатора данных
	for(int i=0;i<4;i++) { wait_qp[i]=new QPixmap(tr(":/images/Wait%1_24.png").arg(i)); }

	//Окно настроек регулятора
	if(RegSetup_MainWindow==NULL)
		{
		RegSetup_MainWindow = new QMainWindow();
		RegSetup_MainWindow->setWindowTitle(tr("Настройки регуляторов"));
		RegSetup_MainWindow->setWindowIcon(QIcon(":/images/advancedsettings.png"));
		}

	//Виджет закладок
	if(RegSetup_tabWidget==NULL)
		{
		RegSetup_tabWidget = new xTabWidget(RegSetup_MainWindow);
		RegSetup_MainWindow->setCentralWidget(RegSetup_tabWidget);
		RegSetup_tabWidget->setVisible(true);
		}
	RegSetup_Tab = new QWidget(RegSetup_MainWindow); //Закладка
	RegSetup_tabWidget->addTab(RegSetup_Tab, QIcon(":/images/advancedsettings.png"), tr("Настройки ")+Name0);

	//Окно списка блоков
	if(GetBlocksID_MainWindow==NULL)
		{
		GetBlocksID_MainWindow = new QMainWindow();
		GetBlocksID_MainWindow->setWindowTitle(tr("Список блоков"));
		GetBlocksID_MainWindow->setWindowIcon(QIcon(":/images/memory.png"));
		}

	//Виджет закладок
	if(GetBlocksID_tabWidget==NULL)
		{
		GetBlocksID_tabWidget = new xTabWidget(GetBlocksID_MainWindow);
		GetBlocksID_MainWindow->setCentralWidget(GetBlocksID_tabWidget);
		}
	GetBlocksID_Tab = new QWidget(GetBlocksID_MainWindow); //Закладка
	GetBlocksID_tabWidget->addTab(GetBlocksID_Tab, QIcon(":/images/memory.png"), tr("Список блоков от ")+Name0);

	//Создание сервиса работы с настройками регулятора и получения списка блоков
	RegSetup_GetBlocksID = new TOktServRegSetup(RegSetup_Tab, GetBlocksID_Tab, this);
	connect(RegSetup_GetBlocksID, SIGNAL(RegSetup_CloseSignal()), this, SLOT(RegSetup_CloseSlotExternal()));
	connect(RegSetup_GetBlocksID, SIGNAL(GetBlocksID_CloseSignal()), this, SLOT(GetBlocksID_CloseSlotExternal()));

	Master=false;
	ForceMaster=false;
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
