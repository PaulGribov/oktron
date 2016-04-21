#include "toktservext.h"
#include "toktservregsetup.h"
#include "work.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QApplication>
#endif
#include <QPainter>
#include <QStyleOptionViewItemV4>
#include <QSpinBox>
#include <xspinbox.h>
#include <work.h>
#include <QStandardItem>
#include <QTableView>

QStringList TOktServRegSetup::YESNO_sl;


void TGetBlocksIDPar::ButClick()
	{
	//QMessageBox::information(this, QObject::tr("ButClick нажата"), QObject::tr("Кнопка %1").arg(Index));
	((TOktServRegSetup *)OSRS_parent)->PostReq(CMD_PUT_CODE_BLOCK, Index);
	}


void TOktServRegSetup::RegSetupWidgetsCreate(TRegSetupWidgets &Widgets, QWidget *NextFocusChain, TRegSetupPar **pRegSetupPars, TGetBlocksIDPar **pGetBlocksIDPars)
	{
	Widgets.LoadPars.Frame=new QFrame();
	Widgets.LoadPars.Frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	Widgets.LoadPars.Frame->setVisible(false);

	Widgets.LoadPars.Layout=new QVBoxLayout();
	Widgets.LoadPars.Frame->setLayout(Widgets.LoadPars.Layout);

	Widgets.LoadPars.Label = new QLabel();
	Widgets.LoadPars.Label->setStyleSheet("\
		color: rgb(5,116,174);\
		font: 18pt;\
		");
	Widgets.LoadPars.Label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	Widgets.LoadPars.Layout->addWidget(Widgets.LoadPars.Label);

	Widgets.LoadPars.ProgressBar = new QProgressBar();
	Widgets.LoadPars.ProgressBar->setTextVisible(false);
	Widgets.LoadPars.ProgressBar->setVisible(false);
	Widgets.LoadPars.Layout->addWidget(Widgets.LoadPars.ProgressBar);

	Widgets.ButtonsBar.Layout = new QHBoxLayout();
	Widgets.ButtonsBar.Spacer = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum);
	Widgets.ButtonsBar.Layout->addSpacerItem(Widgets.ButtonsBar.Spacer);

	Widgets.ButtonsBar.Reload_Button = new xButton(GenBut, QIcon(":/images/refresh.png"), 32, Qt::ToolButtonTextBesideIcon);
	Widgets.ButtonsBar.Layout->addWidget(Widgets.ButtonsBar.Reload_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	Widgets.ButtonsBar.Close_Button = new xButton(GenBut, QIcon(":/images/button_cancel.png"), 32, Qt::ToolButtonTextBesideIcon);

	Widgets.ButtonsBar.Layout->addWidget(Widgets.ButtonsBar.Close_Button, 0, Qt::AlignRight | Qt::AlignBottom);


	if((pRegSetupPars)||(pGetBlocksIDPars))
		{
		Widgets.TableView = new RegSetupTableView(NextFocusChain?NextFocusChain:Widgets.ButtonsBar.Reload_Button, pRegSetupPars, pGetBlocksIDPars, this);
		Widgets.TableView->setModel(&Widgets.Model);
		Widgets.TableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#ifdef __i386__
		Widgets.TableView->setMinimumHeight(300);
		Widgets.TableView->setMinimumWidth(600);
#endif
		Widgets.TableView->setSelectionMode(QAbstractItemView::SingleSelection);
		Widgets.TableView->setIconSize(QSize(28,28));
		Widgets.TableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		Widgets.TableView->horizontalHeader()->setVisible(false);

		QHeaderView *verticalHeader = Widgets.TableView->verticalHeader();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		verticalHeader->sectionResizeMode(QHeaderView::Fixed);
#else
		verticalHeader->setResizeMode(QHeaderView::Fixed);
#endif
		verticalHeader->setDefaultSectionSize(42);
		}

	Widgets.Layout=new QVBoxLayout();

	Widgets.Layout->addWidget(Widgets.LoadPars.Frame);
	//Widgets.Layout->addWidget(Widgets.LoadPars.Label, 0, Qt::AlignHCenter | Qt::AlignBottom);
	//Widgets.Layout->addWidget(Widgets.LoadPars.ProgressBar, 0, Qt::AlignHCenter | Qt::AlignTop);
	if((pRegSetupPars)||(pGetBlocksIDPars))
		{
		Widgets.Layout->addWidget(Widgets.TableView);
		}

	Widgets.Layout->addLayout(Widgets.ButtonsBar.Layout);

	Widgets.Focused=NULL;
	}


TOktServRegSetup::TOktServRegSetup(QWidget *RegSetupParent, QWidget *GetBlocksIDParent, TOktServ *OktServ) : QWidget(RegSetupParent)
	{
	this->OktServ=OktServ;

	//Создание элементов списка параметров
	pRegSetupPars = new TRegSetupPar *[REG_SETUP_PARS_NUM];
	for(int i=0;i<REG_SETUP_PARS_NUM;i++)
		{
		pRegSetupPars[i] = new TRegSetupPar(this, i);
		}
	Wait4Parameter=false;

	QHBoxLayout *ButtonsBar0_Layout = new QHBoxLayout();
	SettingsApply_Button = new xButton(GenBut, QIcon(":/images/apply.png"), 32, Qt::ToolButtonTextBesideIcon);
	SaveSettings_Button = new xButton(GenBut, QIcon(":/images/document_save.png"), 32, Qt::ToolButtonTextBesideIcon);
	SettingsApply_Button->setEnabled(false);
	SaveSettings_Button->setEnabled(false);
	ButtonsBar0_Layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	ButtonsBar0_Layout->addWidget(SettingsApply_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	ButtonsBar0_Layout->addWidget(SaveSettings_Button, 0, Qt::AlignRight | Qt::AlignBottom);

	RegSetupWidgetsCreate(RegSetup, SettingsApply_Button, pRegSetupPars, NULL);

	RegSetup.Layout->insertLayout(2, ButtonsBar0_Layout);

	RegSetupList_ItemDelegate = new TRegSetupList_ItemDelegate(this);
	RegSetup.TableView->setItemDelegate(RegSetupList_ItemDelegate);

	connect(RegSetup.ButtonsBar.Reload_Button, SIGNAL(clicked()), this, SLOT(ReadSettingsDesc()));
	connect(RegSetup.ButtonsBar.Close_Button, SIGNAL(clicked()), this, SLOT(RegSetup_CloseSlotInternal()));

	connect(SaveSettings_Button, SIGNAL(clicked()), this, SLOT(SaveSettings()));
	//RegSetup.ButtonsBar.Layout->insertWidget(1, SaveSettings_Button, 0, Qt::AlignRight | Qt::AlignBottom);

	connect(SettingsApply_Button, SIGNAL(clicked()), this, SLOT(SettingsApply()));
	//RegSetup.ButtonsBar.Layout->insertWidget(1, SettingsApply_Button, 0, Qt::AlignRight | Qt::AlignBottom);

	RegSetupParent->setLayout(RegSetup.Layout);

	PrevWidget=RegSetupParent->parentWidget();
	RegSetupParent->parentWidget()->parentWidget()->setTabOrder(RegSetupParent->parentWidget(), RegSetup.TableView);
	RegSetupParent->parentWidget()->parentWidget()->setTabOrder(RegSetup.TableView, SettingsApply_Button);
	RegSetupParent->parentWidget()->parentWidget()->setTabOrder(SettingsApply_Button, SaveSettings_Button);
	RegSetupParent->parentWidget()->parentWidget()->setTabOrder(SaveSettings_Button, RegSetup.ButtonsBar.Reload_Button);
	RegSetupParent->parentWidget()->parentWidget()->setTabOrder(RegSetup.ButtonsBar.Reload_Button, RegSetup.ButtonsBar.Close_Button);

	RegSetup_QTimer = new QTimer();
	connect(RegSetup_QTimer, SIGNAL(timeout()), this, SLOT(RegSetupReq()));
	StopService();

	pGetBlocksIDPars = new TGetBlocksIDPar *[GETBLOCKSID_PARS_NUM];
	for(int i=0;i<GETBLOCKSID_PARS_NUM;i++)
		{
		pGetBlocksIDPars[i] = new TGetBlocksIDPar(this, i);
		}

	//Создание виджетов списка блоков, аналогичных настройкам регулятора
	RegSetupWidgetsCreate(GetBlocksID, NULL, NULL, pGetBlocksIDPars);

	GetBlocksIDList_ItemDelegate = new TGetBlocksIDList_ItemDelegate(this);
	GetBlocksID.TableView->setItemDelegate(GetBlocksIDList_ItemDelegate);

	connect(GetBlocksID.ButtonsBar.Reload_Button, SIGNAL(clicked()), this, SLOT(GetBlocksIDSlot()));
	connect(GetBlocksID.ButtonsBar.Close_Button, SIGNAL(clicked()), this, SLOT(GetBlocksID_CloseSlot()));

	GetBlocksIDParent->setLayout(GetBlocksID.Layout);

	Retranslate();
	}

void TOktServRegSetup::Retranslate()
	{
	YESNO_sl.clear();
	YESNO_sl << tr("НЕТ") << tr("ДА");
	RegSetup.ButtonsBar.Reload_Button->setText(tr("ПЕРЕЧИТАТЬ"));
	RegSetup.ButtonsBar.Close_Button->setText(tr("ЗАКРЫТЬ"));
	SettingsApply_Button->setText(tr("ПРИМЕНИТЬ"));
	SaveSettings_Button->setText(tr("СОХРАНИТЬ"));
	GetBlocksID.ButtonsBar.Reload_Button->setText(tr("ПЕРЕЧИТАТЬ"));
	GetBlocksID.ButtonsBar.Close_Button->setText(tr("ЗАКРЫТЬ"));
	RegSetupErrors_StringList.clear();
	RegSetupErrors_StringList  \
		<< tr("Таймаут при получении пакета от блока связи")\
		<< tr("Таймаут при получении пакета от ведущего блока")\
		<< tr("Таймаут при получении ответа от ведущего блока")\
		<< tr("Неверный ответ на команду запроса блока на шине")\
		<< tr("Передача блока кода: Таймаут, устройство занято")\
		<< tr("Передача блока кода: Таймаут, устройство не отвечает")\
		<< tr("Процесс остановлен")\
		<< tr("Ошибка при чтении HEX-файла");
	GetBlocksID_StringList.clear();
	GetBlocksID_StringList  \
		<< tr("0") << tr("Блок регулирования") << tr("T8R-") \
		<< tr("1") << tr("Блок измерения аналоговых параметров") << tr("T8A-")\
		<< tr("2") << tr("Блок основных дискретных сигналов") << tr("T8D-")\
		<< tr("3") << tr("Блок питания") << tr("T8P-")\
		<< tr("4") << tr("Блок дополнительных дискретных входов/выходов") << tr("T8X-")\
		<< tr("5") << tr("Блок дополнительных дискретных входов/выходов") << tr("T8Y-")\
		<< tr("6") << tr("Блок сопряжения с плавным пуском") << tr("T8S-")\
		<< tr("7") << tr("Блок возбуждения тиристорный") << tr("T8T-")\
		<< tr("7") << tr("Блок возбуждения бесщеточный") << tr("T8B-")\
		<< tr("7") << tr("Блок возбуждения IGBT ШИМ щеточный") << tr("T8G-")\
		<< tr("8") << tr("Блок индикаторов (базовый)") << tr("T8K-")\
		<< tr("9") << tr("Блок выдачи сигналов 4/20мА") << tr("T8C-")\
		<< tr("10") << tr("Блок выдачи сигналов RS485") << tr("T8M-")\
		<< tr("11") << tr("Блок связи Ethernet") << tr("T8E-")\
		<< tr("12") << tr("Блок дисплейный (большая индикация)") << tr("T8V-")\
		<< tr("12") << tr("Блок связи с дисплейным процессором") << tr("T8W-");
	}


void TOktServRegSetup::Start_Service()
	{
	TimeoutCnt=0;
	IOPacketDeliveredTimeoutCnt=0;
	ReqSeqIndex=0;
	Cmd=CMD_IDLE;
	prevCmd=CMD_ERROR;
	StartupDelay=100;
	RegSetup_QTimer->start(10);
	}

void TOktServRegSetup::StopService()
	{
	RegSetup_QTimer->stop();
	Stop();
	}

bool TOktServRegSetup::IsBusy()
	{
	return (Cmd!=CMD_IDLE)&&(Cmd!=CMD_ERROR);
	}

void TOktServRegSetup::GetBlocksID_CloseSlot()
	{
	GetBlocksID_CloseSignal();
	}

void TOktServRegSetup::GetBlocksIDSlot()
	{
	if(OktServ->StateOn)
		{
		PostReq(CMD_GET_BLOCK_ID, 0);
		}
	else
		{
		Stop();
		}
	}


void TOktServRegSetup::RegSetup_CloseSlotInternal()
	{
	RegSetup_CloseSignal();
	}

void TOktServRegSetup::SettingsApply()
	{
	PostReqWithStatusClear(CMD_SETTINGS_APPLY, 0);
	}

void TOktServRegSetup::SaveSettings()
	{
	for(int i=0;i<REG_SETUP_PARS_NUM;i++)
		{
		PostReqWithStatusClear(CMD_SAVE_VAL, i);
		}
	}

const float Order[]={1.0, 0.1, 0.01, 0.001};


float TOktServRegSetup::CalcValue(int index)
	{
	return (float)(pRegSetupPars[index]->Value)*Order[pRegSetupPars[index]->Order]/pRegSetupPars[index]->DivBy;
	}

void TOktServRegSetup::StringValue(int index, QString &str)
	{
	switch(pRegSetupPars[index]->ParType)
		{
		case tyName:
		case tyTitle:
		case tyButton:
			break;
		case tyYesNo:
			{
			int val=pRegSetupPars[index]->Value;
			if((val<0)||(val>1)) val=0;
			str=YESNO_sl.at(val);
			}
			break;
		case tyHexChar:
			{
			int val=pRegSetupPars[index]->Value;
			str=QString("%1").arg(val, 2, 16, QLatin1Char('0')).toUpper();
			}
			break;
		case tyHexShort:
			{
			int val=pRegSetupPars[index]->Value;
			str=QString("%1").arg(val, 4, 16, QLatin1Char('0')).toUpper();
			}
			break;
		case tySigned:
		case tyUnsigned:
			{
			float val=pRegSetupPars[index]->Value;
			float coef = Order[pRegSetupPars[index]->Order]/pRegSetupPars[index]->DivBy;
			str=QString("%L1").arg(val*coef, 0, 'f', pRegSetupPars[index]->Order);
			}
			break;
		}
	}

void TOktServRegSetup::RegSetupParSetFlags(int index)
	{
	switch(pRegSetupPars[index]->Affect)
		{
		default:
		//Не влияет на другие параметры
		case afNot:
			break;

		//Влияет на значение параметров в своей группе отображения
		case afOnValInOwnGroup:
			for(int i=0,j=index&(~0xF);i<16;i++)
				{
				PostReq(CMD_GET_VAL, j+i);
				}
			break;

		//Влияет на значения параметров в других группах отображения
		case afOnValInOtherGroups:
			for(int i=0;i<REG_SETUP_PARS_NUM;i++)
				{
				if((i&(~0xF))!=(index&(~0xF)))
					{
					PostReq(CMD_GET_VAL, i);
					}
				}
			break;

		//Влияет только на свое собственное название
		case afOnOwnName:
			PostReq(CMD_GET_NAME, index);
			break;

		//Влияет на значения параметров в своей и других группах отображения
		case afOnValInAllGroups:
			for(int i=0;i<REG_SETUP_PARS_NUM;i++)
				{
				if(i!=index)
					{
					PostReq(CMD_GET_VAL, i);
					}
				}
			break;

		//Влияет на названия параметров в своей группе отображения
		case afOnNamesInOwnGroup:
			for(int i=0,j=index&(~0xF);i<16;i++)
				{
				PostReq(CMD_GET_NAME, j+i);
				}
			break;

		//Влияет на названия параметров только в других группах отображения
		case afOnNamesInOtherGroups:
			for(int i=0;i<REG_SETUP_PARS_NUM;i++)
				{
				if((i&(~0xF))!=(index&(~0xF)))
					{
					PostReq(CMD_GET_NAME, i);
					}
				}
			break;

		case afGlobalPar:
			ReadSettingsDesc();
			break;
		}
	}

//Добавление стоки в таблицу
void TOktServRegSetup::AddRow2Table_RegSetup()
	{
	QList<QStandardItem*> items;
	for(int col=0;col<REGSETUPLIST_COLS_NUM;col++)
		{
		QStandardItem *item=new QStandardItem();
		item->setSelectable(false);
		switch(col)
			{
			case REGSETUPLIST_NAME_COL:
				item->setText(pRegSetupPars[ParameterIndex]->Name);

				switch(pRegSetupPars[ParameterIndex]->ParType)
					{
					case tyButton:
						item->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
						item->setIcon(QIcon(":/images/advancedsettings.png"));
						break;
					case tyTitle:
						item->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
						break;
					default:
						break;
					}

				item->setEditable(pRegSetupPars[ParameterIndex]->ParType==tyButton);
				break;

			case REGSETUPLIST_VAL_COL:
				{
				QString str;
				StringValue(ParameterIndex, str);
				item->setText(str);
				}
				item->setEditable((pRegSetupPars[ParameterIndex]->ParType==tyButton)?false:pRegSetupPars[ParameterIndex]->Writable);
				break;

			default:
			case REGSETUPLIST_PLUSBUT_COL:
			case REGSETUPLIST_MINUSBUT_COL:
			case REGSETUPLIST_READBUT_COL:
			case REGSETUPLIST_WRITEBUT_COL:
				if((pRegSetupPars[ParameterIndex]->ParType!=tyButton)&&
				   (pRegSetupPars[ParameterIndex]->ParType!=tyTitle)&&
				   pRegSetupPars[ParameterIndex]->Writable)
					{
					item->setEditable(true);
#ifndef  REGSETUPLIST_PERSISTENT_EDITORS
					item->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
					switch(col)
						{
						case REGSETUPLIST_PLUSBUT_COL:
							item->setIcon(QIcon(":/images/plus.png"));
							//items.back()->setText(tr("+"));
							break;

						case REGSETUPLIST_MINUSBUT_COL:
							item->setIcon(QIcon(":/images/minus.png"));
							//items.back()->setText(tr("-"));
							break;

						case REGSETUPLIST_READBUT_COL:
							item->setIcon(QIcon(":/images/refresh.png"));
							//items.back()->setText(tr("Обновить"));
							break;

						case REGSETUPLIST_WRITEBUT_COL:
							item->setIcon(QIcon(":/images/document_save.png"));
							//items.back()->setText(tr("Сохранить"));
							break;

						default:
							break;
						}
#endif
					}
				break;

			case REGSETUPLIST_STATUS_COL:
				item->setEditable(false);
				break;
			}
		pRegSetupPars[ParameterIndex]->pCell[col]=item;	//Указатель на ячейку
		items << item;
		}
	RegSetup.Model.appendRow(items);

	for(int i=0;i<REGSETUPLIST_COLS_NUM;i++)
		{
		pRegSetupPars[ParameterIndex]->IsEditable[i]=items[i]->isEditable();
#ifdef  REGSETUPLIST_PERSISTENT_EDITORS
		switch(i)
			{
			case REGSETUPLIST_PLUSBUT_COL:
			case REGSETUPLIST_MINUSBUT_COL:
			case REGSETUPLIST_READBUT_COL:
			case REGSETUPLIST_WRITEBUT_COL:
			case REGSETUPLIST_VAL_COL:
				RegSetup.TableView->openPersistentEditor(RegSetup.Model.index(ParameterIndex, i, QModelIndex()));
				break;
			default:
				break;
			}
#endif
		}

	//Скрыть строку, если её нет
	RegSetup.TableView->setRowHidden(ParameterIndex,!(pRegSetupPars[ParameterIndex]->Enabled));
	}

//Добавление стоки в таблицу
void TOktServRegSetup::AddRow2Table_GetBlocksID(QString id, bool enabled, bool updatable)
	{
	QList<QStandardItem*> items;
	QString BlockName=tr("безымянный");
	int BlockAddr=ParameterIndex;

	//Поиск по ID...
	for(int i=0;i<GetBlocksID_StringList.count()/3;i++)
		{
		if(id.indexOf(GetBlocksID_StringList.at(i*3+2), 0)==0)
			{
			BlockName=GetBlocksID_StringList.at(i*3+1);
			BlockAddr=GetBlocksID_StringList.at(i*3+0).toInt();
			goto BlockFound_loc;
			}
		}
	//Поиск по адресу...
	for(int i=0;i<GetBlocksID_StringList.count()/3;i++)
		{
		if(ParameterIndex==GetBlocksID_StringList.at(i*3+0).toInt())
			{
			BlockName=GetBlocksID_StringList.at(i*3+1);
			BlockAddr=ParameterIndex;
			goto BlockFound_loc;
			}
		}
BlockFound_loc:

	for(int col=0;col<GETBLOCKSID_COLS_NUM;col++)
		{
		QStandardItem *item = new QStandardItem();
		item->setEditable(false);
		item->setSelectable(false);
		switch(col)
			{
			case GETBLOCKSID_NAME_COL:
				item->setText(BlockName);
				break;

			case GETBLOCKSID_N_COL:
				item->setText(QString("%1").arg(BlockAddr));
				break;

			case GETBLOCKSID_ID_COL:
			case GETBLOCKSID_HEXFILE_COL:
				item->setText(id);
				break;

			case GETBLOCKSID_UPDATE_COL:
				item->setEditable(true);
				item->setSelectable(true);
				break;

			case GETBLOCKSID_STATUS_COL:
				break;

			default:
				break;
			}
		pGetBlocksIDPars[ParameterIndex]->pCell[col]=item;	//Указатель на ячейку
		items << item;
		}
	pGetBlocksIDPars[ParameterIndex]->Id=id;
	pGetBlocksIDPars[ParameterIndex]->Addr=BlockAddr;
	pGetBlocksIDPars[ParameterIndex]->Updatable=updatable;
	GetBlocksID.Model.appendRow(items);
	GetBlocksID.TableView->openPersistentEditor(GetBlocksID.Model.index(ParameterIndex, GETBLOCKSID_UPDATE_COL, QModelIndex()));

	//Скрыть строку, если её нет
	GetBlocksID.TableView->setRowHidden(ParameterIndex, !enabled);
	}

void TOktServRegSetup::WidgetsEnabled(bool e)
	{
	//Скрыть кнопку
	if(!e)
		{
		SettingsApply_Button->setEnabled(false);
		SaveSettings_Button->setEnabled(false);
		for(int i=0;i<GETBLOCKSID_PARS_NUM;i++)
			{
			if(pGetBlocksIDPars[i]->UpdBut) pGetBlocksIDPars[i]->UpdBut->setEnabled(false);
			}

		for(int i=0;i<REG_SETUP_PARS_NUM;i++)
			{
			if(pRegSetupPars[i]->Enabled)
				for(int j=0;j<REGSETUPLIST_COLS_NUM;j++)
					{
					if(pRegSetupPars[i]->IsEditable[j])
						{
						pRegSetupPars[i]->pCell[j]->setEditable(false);
						}
					}

			}
		}
	else
		{
		if(RegSetup.TableView->isEnabled())
			{
			SettingsApply_Button->setEnabled(true);
			SaveSettings_Button->setEnabled(true);
			}
		for(int i=0;i<GETBLOCKSID_PARS_NUM;i++)
			{
			if(pGetBlocksIDPars[i]->UpdBut) pGetBlocksIDPars[i]->UpdBut->setEnabled(pGetBlocksIDPars[i]->Updatable);
			}
		for(int i=0;i<REG_SETUP_PARS_NUM;i++)
			{
			if(pRegSetupPars[i]->Enabled)
				for(int j=0;j<REGSETUPLIST_COLS_NUM;j++)
					{
					pRegSetupPars[i]->pCell[j]->setEditable(pRegSetupPars[i]->IsEditable[j]);
					}
			}
		}

	GetBlocksID.ButtonsBar.Reload_Button->setEnabled(e);
	RegSetup.ButtonsBar.Reload_Button->setEnabled(e);

	}


void TOktServRegSetup::RegSetupReq()
	{
	if(StartupDelay) { StartupDelay--; return; }

	TUnstructuredPacket Packet = OktServ->ReceivedData.UnstructuredPacket[OKTSERV_IOPACKET_INDEX];

	//Команда сменилась => обновление графической части
	switch(Cmd)
		{
		case CMD_GET_BLOCK_ID:
			if(prevCmd==Cmd) break;
			GetBlocksID.LoadPars.Label->setText(tr("Загрузка параметров ..."));
			GetBlocksID.LoadPars.Frame->setVisible(true);
			GetBlocksID.LoadPars.ProgressBar->setVisible(true);
			GetBlocksID.LoadPars.ProgressBar->setRange(1,100);
			GetBlocksID.LoadPars.ProgressBar->setValue(1);
			GetBlocksID.TableView->setVisible(false);
			break;

		case CMD_READ_SETTINGS_DESC:
			if(prevCmd==Cmd) break;
			RegSetup.LoadPars.Label->setText(tr("Загрузка параметров ..."));
			RegSetup.LoadPars.Frame->setVisible(true);
			RegSetup.LoadPars.ProgressBar->setVisible(true);
			RegSetup.LoadPars.ProgressBar->setRange(1,100);
			RegSetup.LoadPars.ProgressBar->setValue(1);
			RegSetup.TableView->setVisible(false);
			break;

		case CMD_IDLE:
			if(prevCmd==Cmd) break;
			//Очередь пуста - сделать доступными элементы
			if(ReqSeqIndex==0)
				{
				WidgetsEnabled(true);
				}
			switch(prevCmd)
				{
				//Параметры прочтены - показать список
				case CMD_GET_BLOCK_ID:
					GetBlocksID.LoadPars.Frame->setVisible(false);
					GetBlocksID.LoadPars.ProgressBar->setVisible(false);
					GetBlocksID.TableView->setVisible(true);
					FindHEXs_GetBlocksID();
					break;

				//Параметры прочтены - показать список
				case CMD_READ_SETTINGS_DESC:
					RegSetup.LoadPars.Frame->setVisible(false);
					RegSetup.LoadPars.ProgressBar->setVisible(false);
					RegSetup.TableView->setVisible(true);
					RegSetup.TableView->setEnabled(true);

					//RegSetupList_qtw->resizeColumnsToContents();
					break;

				//Поставить значок OK напротив параметра
				case CMD_GET_VAL:
				case CMD_INC_VAL:
				case CMD_DEC_VAL:
				case CMD_SET_VAL:
				case CMD_SAVE_VAL:
				case CMD_GET_NAME:
					{
					QPixmap pixmap(":/images/apply.png");
					QIcon icon(pixmap);
					pRegSetupPars[ParameterIndex]->pCell[REGSETUPLIST_STATUS_COL]->setIcon(icon);
					//pRegSetupPars[ParameterIndex]->pCell[REGSETUPLIST_STATUS_COL]->setText(tr("Успешно"));
					}
					break;
				case CMD_PUT_CODE_BLOCK:
					{
					}
					break;
				default:
					break;
				}
			break;

		case CMD_ERROR:
			{
			QString ErrMsg = tr("Ошибка загрузки параметров");

			if(prevCmd==Cmd) break;

			if(LastErrorCode<RegSetupErrors_StringList.count())
				{
				ErrMsg=tr("Ошибка: ")+RegSetupErrors_StringList[LastErrorCode];
				}

			//Очередь пуста - сделать доступными элементы
			if(ReqSeqIndex==0)
				{
				WidgetsEnabled(true);
				}

			switch(prevCmd)
				{
				case CMD_GET_BLOCK_ID:
					GetBlocksID.LoadPars.ProgressBar->setVisible(false);
					//Если загружен ID-регулятора, то показать список несмотря на ошибку (режим загрузчика)
					if(ParameterIndex)
						{
						GetBlocksID.LoadPars.Frame->setVisible(false);
						GetBlocksID.TableView->setVisible(true);
						FindHEXs_GetBlocksID();
						}
					else
						{
						GetBlocksID.LoadPars.Label->setText(ErrMsg);
						GetBlocksID.LoadPars.Frame->setVisible(true);
						GetBlocksID.TableView->setVisible(false);
						GetBlocksID.ButtonsBar.Close_Button->setFocus();
						}
					break;

				case CMD_READ_SETTINGS_DESC:
					RegSetup.LoadPars.Label->setText(ErrMsg);
					RegSetup.LoadPars.Frame->setVisible(true);
					RegSetup.LoadPars.ProgressBar->setVisible(false);
					RegSetup.TableView->setVisible(false);
					RegSetup.TableView->setEnabled(false);
					RegSetup.ButtonsBar.Close_Button->setFocus();
					break;

				//Поставить значок ошибки напротив параметра
				case CMD_GET_VAL:
				case CMD_INC_VAL:
				case CMD_DEC_VAL:
				case CMD_SET_VAL:
				case CMD_SAVE_VAL:
				case CMD_GET_NAME:
					{
					QPixmap pixmap(":/images/block_24.png");
					QIcon icon(pixmap);
					pRegSetupPars[ParameterIndex]->pCell[REGSETUPLIST_STATUS_COL]->setIcon(icon);
					//pRegSetupPars[ParameterIndex]->pCell[REGSETUPLIST_STATUS_COL]->setText(tr("Ошибка"));
					}
					break;

				case CMD_PUT_CODE_BLOCK:
					{
					//Если не было таймаута, то вывод ошибки связи
					if(HexUploadTimeout<10)
						{
						pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(102));
						}
					}
					break;

				case CMD_ERROR:
				default:
					break;
				}
#ifdef __i3_86__
			RegSetup_QTimer->stop();
			QMessageBox::critical(0, QObject::tr("Ошибка"), ErrMsg);
			RegSetup_QTimer->start(10);
#endif
			}
			break;

		case CMD_GET_VAL:
		case CMD_INC_VAL:
		case CMD_DEC_VAL:
		case CMD_SET_VAL:
		case CMD_SAVE_VAL:
		case CMD_GET_NAME:
			{
			QIcon icon(*TOktServExt::wait_qp[((wait_qp_cnt++)>>3)&0x03]);
			pRegSetupPars[ParameterIndex]->pCell[REGSETUPLIST_STATUS_COL]->setIcon(icon);
			pRegSetupPars[ParameterIndex]->pCell[REGSETUPLIST_STATUS_COL]->setText(tr(""));
			}

		default:
			break;

		case CMD_PUT_CODE_BLOCK:
			{
			if(prevCmd==Cmd) break;
			pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(0));
			}
			break;
		}
	prevCmd=Cmd;

	//Ожидание ответа?
	if(Wait4Parameter==false)
		{
FormattingReq_loc:
		//Формирование запроса
		switch(Cmd)
			{
			default:
				return;

			case CMD_ERROR:
			case CMD_IDLE:
				{
				//Наличие запроса в очереди...
				if(ReqSeqIndex)
					{
					QMutexLocker locker(&ReqSeqLock);
					WidgetsEnabled(false);
					Cmd=ReqSeq[0].c;
					ParameterIndex=ReqSeq[0].index;
					TimeoutCnt=0;
					IOPacketDeliveredTimeoutCnt=0;
					ReqSeqIndex--;
					for(int i=0;i<ReqSeqIndex;i++) ReqSeq[i]=ReqSeq[i+1];
					switch(Cmd)
						{
						default:
							break;
						//Инициирование: чтение настроек
						case CMD_READ_SETTINGS_DESC:
							RegSetup.Model.clear();
							RegSetup.Model.setHorizontalHeaderLabels(QStringList() << tr("Параметр") << tr("Значение") << tr("") << tr("") << tr("") << tr("") << tr("Статус"));
							RegSetup.TableView->setColumnWidth(REGSETUPLIST_NAME_COL, 260);
							RegSetup.TableView->setColumnWidth(REGSETUPLIST_VAL_COL, 80);
							RegSetup.TableView->setColumnWidth(REGSETUPLIST_PLUSBUT_COL, 42);
							RegSetup.TableView->setColumnWidth(REGSETUPLIST_MINUSBUT_COL, 42);
							RegSetup.TableView->setColumnWidth(REGSETUPLIST_READBUT_COL, 42);
							RegSetup.TableView->setColumnWidth(REGSETUPLIST_WRITEBUT_COL, 42);
							RegSetup.TableView->setColumnWidth(REGSETUPLIST_STATUS_COL, 42);
							break;
						//Инициирование: чтение ID блоков
						case CMD_GET_BLOCK_ID:
							for(int i=0;i<GETBLOCKSID_PARS_NUM;i++)
								{
								pGetBlocksIDPars[i]->UpdBut=NULL;
								}
							GetBlocksID.Model.clear();
							GetBlocksID.Model.setHorizontalHeaderLabels(QStringList() << tr("Блок") << tr("№") << tr("ПО блока") << tr("Доступ.ПО") << tr("Обновлние") << tr("Состояние"));
							GetBlocksID.TableView->setColumnWidth(GETBLOCKSID_NAME_COL, 180);
							GetBlocksID.TableView->setColumnWidth(GETBLOCKSID_N_COL, 25);
							GetBlocksID.TableView->setColumnWidth(GETBLOCKSID_ID_COL, 100);
							GetBlocksID.TableView->setColumnWidth(GETBLOCKSID_HEXFILE_COL, 100);
							GetBlocksID.TableView->setColumnWidth(GETBLOCKSID_UPDATE_COL, 45);
							GetBlocksID.TableView->setColumnWidth(GETBLOCKSID_STATUS_COL, 80);
							break;
						//Инициирование: передача HEX-файла
						case CMD_PUT_CODE_BLOCK:
							{
							QString Filename = pGetBlocksIDPars[ParameterIndex]->HexId + ".hex";
							if(LoadHEX_GetBlocksID(Filename))
								{
								HexLinesNumber=HexFile_StringList.count();
								}
							else
								{
								pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(104));
								LastErrorCode=errHexFileNotLoaded;
								Cmd=CMD_ERROR;
								}
							HexLineIndex=0;
							HexUploadTimeout=0;
							}
							break;

						}
					}
				}
				return;

			case CMD_GET_VAL:
				Packet.data[0]=ParameterIndex;
				Packet.data[1]=0x20;
				break;

			case CMD_INC_VAL:
				Packet.data[0]=ParameterIndex;
				Packet.data[1]=0x21;
				break;

			case CMD_DEC_VAL:
				Packet.data[0]=ParameterIndex;
				Packet.data[1]=0x22;
				break;

			case CMD_SET_VAL:
				Packet.data[0]=ParameterIndex;
				Packet.data[1]=0x23;
				Packet.data_w[1]=pRegSetupPars[ParameterIndex]->Value&0xFF;
				Packet.data[3]=pRegSetupPars[ParameterIndex]->Value>>8;
				break;

			case CMD_SAVE_VAL:
				Packet.data[0]=ParameterIndex;
				Packet.data[1]=0x24;
				break;

			case CMD_SETTINGS_APPLY:
				Packet.data[0]=5;
				Packet.data[1]=0x25;
				break;

			case CMD_GET_NAME:
			case CMD_READ_SETTINGS_DESC:
				Packet.data[0]=ParameterIndex;
				Packet.data[1]=0x26;
				break;

			case CMD_BLOCK_IS_EXISTS:
				Packet.data[0]=0x00;
				Packet.data[1]=0x40;
				break;

			case CMD_GET_BLOCK_ID:
				Packet.data[0]=ParameterIndex;
				Packet.data[1]=0x41;
				break;

			case CMD_PUT_CODE_BLOCK:
				{
				Packet.data[0]=pGetBlocksIDPars[ParameterIndex]->Addr;
				Packet.data[1]=0x42;
				QString HexLine=HexFile_StringList.at(HexLineIndex);
				bool res;
				for(int i=0;i<HexLine.length();i+=2)
					{
					Packet.data[2+(i>>1)]=HexLine.mid(i,2).toInt(&res,16);
					}
				}
				break;
			}

		{
		QMutexLocker locker(&(OktServ->DataToSendLocker));
		OktServ->DataToSend.UnstructuredPacket[OKTSERV_IOPACKET_INDEX]=Packet;
		OktServ->PacketToSendQuicklyFlags|=1<<OKTSERV_IOPACKET_INDEX;
		}
		Wait4Parameter_PacketID=OktServ->PacketID[OKTSERV_IOPACKET_INDEX];
		Wait4Parameter=true;
		//Сброс флага о принятии пакета
		OktServ->PacketUpdatedFlags&=~(1<<OKTSERV_IOPACKET_INDEX);
		IOPacketReceiveTimeoutCnt=0;
		}

	//Проверка состояния ответа...
	else
		{
#ifndef REGSETUPDBG
		//Если пакет не принят - выход
		if(!(OktServ->PacketUpdatedFlags&(1<<OKTSERV_IOPACKET_INDEX)))
			{
			if(++IOPacketReceiveTimeoutCnt>39)
				{
				LastErrorCode=errIOPacketReceiveTimeout;
				goto ErrorCmd_loc;
				}
			else return;
			}

		//Если ID пакета совападет, то значит он не дошёл...
		if( (Wait4Parameter_PacketID==OktServ->PacketID[OKTSERV_IOPACKET_INDEX])
			)
			{
			IOPacketDeliveredTimeoutCnt++;
			if((IOPacketDeliveredTimeoutCnt%10)==0)
				{
				goto FormattingReq_loc;
				}
			if(IOPacketDeliveredTimeoutCnt>39)
				{
				LastErrorCode=errIOPacketDeliveredTimeout;
				goto ErrorCmd_loc;
				}
			return;
			}

		//Ответ получен?
		if( (Packet.data[1]!=0)
			)
			{
			//Таймаут от ведущего: 0.4с в штатном режиме и 5с после посылки первой строки hex-файла
			int Timeout=((Cmd==CMD_PUT_CODE_BLOCK)&&(pGetBlocksIDPars[ParameterIndex]->Addr==0)&&(HexLineIndex==0))?499:39;
			if(++TimeoutCnt>Timeout)
				{
				LastErrorCode=errAnswerTimeout;
				goto ErrorCmd_loc;
				}
			else return;
			}
#endif

		switch(Cmd)
			{
			default:
			case CMD_IDLE:
			case CMD_ERROR:
				return;

			case CMD_GET_VAL:
			case CMD_DEC_VAL:
			case CMD_INC_VAL:
			case CMD_SET_VAL:
			case CMD_SAVE_VAL:
#ifndef REGSETUPDBG
				//Доп.проверка
				if(Packet.data[0]==ParameterIndex)
					{
					if(Cmd!=CMD_SAVE_VAL)
						{
						//Значение
						if(pRegSetupPars[ParameterIndex]->ParType==tySigned)
							{
							pRegSetupPars[ParameterIndex]->Value = *(signed short *)&Packet.data_w[1];
							}
						else
							{
							pRegSetupPars[ParameterIndex]->Value = *(unsigned short *)&Packet.data_w[1];
							}
						RegSetup.TableView->update(RegSetup.Model.index(ParameterIndex, REGSETUPLIST_VAL_COL, QModelIndex()));
						QString str;
						StringValue(ParameterIndex, str);
						RegSetup.Model.setData(RegSetup.Model.index(ParameterIndex, REGSETUPLIST_VAL_COL, QModelIndex()), str);
						}
					if((Cmd!=CMD_GET_VAL)&&(Cmd!=CMD_SAVE_VAL))
						{
						RegSetupParSetFlags(ParameterIndex);
						}

					Cmd=CMD_IDLE;
					}
#else
				//Значение
				pRegSetupPars[ParameterIndex]->Value = 123;
				Cmd=CMD_IDLE;
#endif
				break;

			case CMD_SETTINGS_APPLY:
				Cmd=CMD_IDLE;
				break;

			case CMD_BLOCK_IS_EXISTS:
				if(*(unsigned short *)&Packet.data_w[0]==0)
					{
					Cmd=CMD_IDLE;
					BlocksExistsBits=*(unsigned short *)&Packet.data_w[1];
					}
				else
					{
					LastErrorCode=errBlocksExistsInvalidAnswer;
ErrorCmd_loc:
					Cmd=CMD_ERROR;
					}
				break;

			case CMD_GET_NAME:
			case CMD_READ_SETTINGS_DESC:
#ifndef REGSETUPDBG
				//Доп.проверка
				if(Packet.data[0]==ParameterIndex)
#endif
					{
					int i;
#ifdef REGSETUPDBG
					//Значение
					pRegSetupPars[ParameterIndex]->Value = 123;
					pRegSetupPars[ParameterIndex]->Name=tr("тест");
					pRegSetupPars[ParameterIndex]->Enabled=1;
					pRegSetupPars[ParameterIndex]->Writable=1;//ParameterIndex&1;
					pRegSetupPars[ParameterIndex]->Affect=(TRegSetupParAffect)0;
					pRegSetupPars[ParameterIndex]->TakenImmediately=0;
					pRegSetupPars[ParameterIndex]->ParType=(TRegSetupParType)(1-(ParameterIndex&1));
					float DivBy[]={1.0, 1.28, 2.56, 5.12};
					pRegSetupPars[ParameterIndex]->DivBy=DivBy[0];
					pRegSetupPars[ParameterIndex]->Order=0;
					pRegSetupPars[ParameterIndex]->OnlyDecInc=0;
					if(ParameterIndex==16)
						{
						pRegSetupPars[ParameterIndex]->ParType=tyButton;
						}
					if(ParameterIndex==2)
						{
						pRegSetupPars[ParameterIndex]->Order=2;
						pRegSetupPars[ParameterIndex]->ParType=tySigned;
						}
					if(ParameterIndex==5) pRegSetupPars[ParameterIndex]->ParType=tyYesNo;
					if(ParameterIndex==7) pRegSetupPars[ParameterIndex]->ParType=tyHexChar;

					if(Cmd!=CMD_GET_NAME)
						{
							{
#else


					//Перекодирование названия параметра в UTF-8
					char NameInWin1251[REG_SETUP_NAME_LEN+1];
					for(i=0;i<REG_SETUP_NAME_LEN;i++)
						{
						NameInWin1251[i] = Packet.data[4+i];
						}
					NameInWin1251[i]=0;
					char buf[REG_SETUP_NAME_LEN*4+1];
					cp1251_to_utf8(buf, NameInWin1251);
					pRegSetupPars[ParameterIndex]->Name=buf; //+QString(tr(" (%1)")).arg(*(unsigned short *)&Packet.data[4+REG_SETUP_NAME_LEN], 4, 16, QLatin1Char('0')).toUpper();
					if(Cmd!=CMD_GET_NAME)
						{
						//Выборка параметров
						pRegSetupPars[ParameterIndex]->Enabled=(Packet.data[4+REG_SETUP_NAME_LEN]&0x01);
						//Если первый из каждых 16ти параметров не доступен, то не также недоступны все 16
						if(((ParameterIndex&0xF)==0)&&(pRegSetupPars[ParameterIndex]->Enabled==false))
							{
							for(int i=0;i<16;i++)
								{
								pRegSetupPars[ParameterIndex]->Enabled=false;
								pRegSetupPars[ParameterIndex]->Writable=false;
								pRegSetupPars[ParameterIndex]->Affect=afNot;
								AddRow2Table_RegSetup();
								if(++ParameterIndex>=REG_SETUP_PARS_NUM)
									{
									Cmd=CMD_IDLE;
									}
								}
							}
						else
							{
							pRegSetupPars[ParameterIndex]->Writable=(Packet.data[4+REG_SETUP_NAME_LEN]&0x02);
							//pRegSetupPars[ParameterIndex]->Group=(TRegSetupParGroup)((Packet.data[4+REG_SETUP_NAME_LEN]>>2)&0x3);
							pRegSetupPars[ParameterIndex]->Affect=(TRegSetupParAffect)((Packet.data[4+REG_SETUP_NAME_LEN]>>4)&0x7);
							pRegSetupPars[ParameterIndex]->TakenImmediately=(Packet.data[4+REG_SETUP_NAME_LEN]&0x07);
							pRegSetupPars[ParameterIndex]->ParType=(TRegSetupParType)((Packet.data[4+REG_SETUP_NAME_LEN+1]>>(8-8))&0x7);
							float DivBy[]={1.0, 1.28, 2.56, 5.12};
							pRegSetupPars[ParameterIndex]->DivBy=DivBy[(Packet.data[4+REG_SETUP_NAME_LEN+1]>>(11-8))&0x3];
							pRegSetupPars[ParameterIndex]->Order=(Packet.data[4+REG_SETUP_NAME_LEN+1]>>(13-8))&0x3;
							pRegSetupPars[ParameterIndex]->OnlyDecInc=(Packet.data[4+REG_SETUP_NAME_LEN+1]&(1<<(15-8)));

							//Значение
							if(pRegSetupPars[ParameterIndex]->ParType==tySigned)
								{
								pRegSetupPars[ParameterIndex]->Value = *(signed short *)&Packet.data_w[1];
								}
							else
								{
								pRegSetupPars[ParameterIndex]->Value = *(unsigned short *)&Packet.data_w[1];
								}

#endif

							//Добавление стоки в таблицу
							AddRow2Table_RegSetup();

							//Все настройки прочитаны...
							if(++ParameterIndex>=REG_SETUP_PARS_NUM)
								{
								Cmd=CMD_IDLE;
								}
							}

						//Прогрессбар...
						RegSetup.LoadPars.ProgressBar->setValue((100*ParameterIndex)/REG_SETUP_PARS_NUM);
						RegSetup.LoadPars.ProgressBar->update();

						TimeoutCnt=0;
						IOPacketDeliveredTimeoutCnt=0;

						QApplication::processEvents();
						}
					//Обновить только строку с названием
					else
						{
						pRegSetupPars[ParameterIndex]->pCell[REGSETUPLIST_NAME_COL]->setText(pRegSetupPars[ParameterIndex]->Name);
						Cmd=CMD_IDLE;
						}

					}
				break;

			case CMD_GET_BLOCK_ID:
				{
#ifdef REGSETUPDBG
				char buf[GETBLOCKSID_NAME_LEN*4+1];
				strcpy(buf, "T8K-12345678");
				AddRow2Table_GetBlocksID(buf, true, true);

#else
				char NameInWin1251[GETBLOCKSID_NAME_LEN+1];
				int i;
				for(i=0;i<GETBLOCKSID_NAME_LEN;i++)
					{
					NameInWin1251[i] = Packet.data[2+i];
					}
				NameInWin1251[i]=0;
				char buf[GETBLOCKSID_NAME_LEN*4+1];
				cp1251_to_utf8(buf, NameInWin1251);

				switch(Packet.data[0]>>4)
					{
					case 0:
						AddRow2Table_GetBlocksID(buf, true, true);
						break;
					case 2:
						AddRow2Table_GetBlocksID(tr("нет программы"), true, false);
						break;
					default:
						AddRow2Table_GetBlocksID(tr(""), false, false);
						break;
					}
#endif

				//Все настройки прочитаны или в режиме восстановления ведущего - выход
				if(++ParameterIndex>=13/*GETBLOCKSID_PARS_NUM*/)
					{
					Cmd=CMD_IDLE;
					}

				//Прогрессбар...
				GetBlocksID.LoadPars.ProgressBar->setValue((100*ParameterIndex)/GETBLOCKSID_PARS_NUM);
				GetBlocksID.LoadPars.ProgressBar->update();

				TimeoutCnt=0;
				IOPacketDeliveredTimeoutCnt=0;

				QApplication::processEvents();
				}
				break;

			case CMD_PUT_CODE_BLOCK:
#ifndef REGSETUPDBG
				switch(Packet.data[0])
					{
					default:
					//команда выполнена готов к следующей строке
					case 0:
						HexUploadTimeout=0;
						if(++HexLineIndex<HexLinesNumber)
							{
							pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(HexLineIndex*100/HexLinesNumber));
							}
						break;
					//команда выполнена, запись закончена
					case 1:
						pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(101));
						Cmd=CMD_IDLE;
						break;
					//команда не выполнена, устройство не отвечает
					case 2:
						Cmd=CMD_ERROR;
						LastErrorCode=errPutCodeBlock_DeviceNotExists;
						break;
					//команда не выполнена, запись в текущий момент запрещена
					case 3:
						if(++HexUploadTimeout>10)
							{
							pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(103));
							LastErrorCode=errPutCodeBlock_DeviceBusyTimeout;
							Cmd=CMD_ERROR;
							}
						break;
					}
#else
				if(++HexLineIndex<HexLinesNumber)
					{
					pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(HexLineIndex*100/HexLinesNumber));
					}
				else
					{
					pGetBlocksIDPars[ParameterIndex]->pCell[GETBLOCKSID_STATUS_COL]->setText(QString::number(101));
					Cmd=CMD_IDLE;
					}
#endif
				TimeoutCnt=0;
				IOPacketDeliveredTimeoutCnt=0;
				QApplication::processEvents();

				break;
			}

		Wait4Parameter=false;
		}
	}


void TOktServRegSetup::ReadSettingsDesc()
	{
	if(OktServ->StateOn)
		{
		PostReq(CMD_READ_SETTINGS_DESC, 0);
		}
	else
		{
		Stop();
		}
	}

void TOktServRegSetup::Stop()
	{
	if((Cmd!=CMD_IDLE)||(!OktServ->StateOn))
		{
		LastErrorCode=errProcessAborted;
		Cmd=CMD_ERROR;
		}
	}


void TOktServRegSetup::FindHEXs_GetBlocksID()
	{
#ifdef __linux__
	QDir::setCurrent("/mnt/localdisk");
#endif
	qDebug() << tr("Search for HEXs...");
	QDir dir;
	QFileInfoList listFiles = dir.entryInfoList(QStringList("*.hex"), QDir::Files);
	for(int i=0;i<GETBLOCKSID_PARS_NUM;i++)
		{
		pGetBlocksIDPars[i]->Updatable=false;
		pGetBlocksIDPars[i]->HexId=tr("");
		if(pGetBlocksIDPars[i]->pCell[GETBLOCKSID_HEXFILE_COL]) pGetBlocksIDPars[i]->pCell[GETBLOCKSID_HEXFILE_COL]->setText(tr(""));
		if(pGetBlocksIDPars[i]->UpdBut)
			{
			pGetBlocksIDPars[i]->UpdBut->setEnabled(false);
			//pGetBlocksIDPars[i]->UpdBut->setText(tr("Загрузить"));
			}
		}

	foreach(QFileInfo file, listFiles)
		{
		QString name = QStringList(file.fileName().split(".")).at(0).toUpper();

		for(int i=0;i<GETBLOCKSID_PARS_NUM;i++)
			{
			QString str=pGetBlocksIDPars[i]->Id;
			if(!str.isEmpty())
				{
				str=str.left(3).toUpper();
				if(name.indexOf(str)!=-1)
					{
					pGetBlocksIDPars[i]->Updatable=true;
					pGetBlocksIDPars[i]->HexId=name;
					qDebug() << tr("Hex: ") << name;
					if(pGetBlocksIDPars[i]->pCell[GETBLOCKSID_HEXFILE_COL]) pGetBlocksIDPars[i]->pCell[GETBLOCKSID_HEXFILE_COL]->setText(name);
					if(pGetBlocksIDPars[i]->UpdBut) pGetBlocksIDPars[i]->UpdBut->setEnabled(true);
					}
				}
			}
		}
	}


int TOktServRegSetup::Hex2Byte_LoadHEX_GetBlocksID(QString HexLine, int &p, int &CRC, bool &res)
	{
	int result=HexLine.mid(p,2).toInt(&res,16); p+=2;
	CRC+=result;
	return result;
	}

bool TOktServRegSetup::LoadHEX_GetBlocksID(QString Filename)
	{
#ifdef __linux__
	QDir::setCurrent("/mnt/localdisk");
#endif
	qDebug() << tr("Read HEX-file...");
	bool Result=false;
	QFile HexFile(Filename);
	if (!HexFile.open(QIODevice::ReadOnly | QIODevice::Text)) qDebug() << tr("File open error!");
	else
		{
		QTextStream HexFileStream(&HexFile);
		QString HexLine;
		HexFile_StringList.clear();
		int CRC;
		bool res;
		int ihex_len;
		int seg_offset=0;
		int total_bytes=0;
		int addr;
		int p;

#define MI_LINEBUF_SIZE 43

		while(!HexFileStream.atEnd())
			{
			QApplication::processEvents();

			HexLine = HexFileStream.readLine().toUpper();
			p=0; CRC=0;
			if(HexLine.data()[p++]!=':')
				{
				qDebug() << tr("Bad file format!");
				goto CriticalQuit_loc;
				}
			ihex_len=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res);

			// Check len so don't read past end of line_buf.
			//1 for ':', 1 for cnt, 2 for addr, 1 for type and 1 for checksum.
			if((!res)||((1+(ihex_len+1+2+1+1)*2)!=HexLine.length())||(HexLine.length()> MI_LINEBUF_SIZE))
				{
				qDebug() << tr("Bad record length!");
				goto CriticalQuit_loc;
				}

			// Load address
			addr=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res)<<8;
			if(!res)
				{
				qDebug() << tr("Bad file format!");
				goto CriticalQuit_loc;
				}
			addr+=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res);
			if(!res)
				{
				qDebug() << tr("Bad file format!");
				goto CriticalQuit_loc;
				}
			addr+=seg_offset;					//this allows access to more than 64K

			int rec_type=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res); //read control byte: 00-data, 01-end, 02-seg_offset
			if(!res)
				{
				qDebug() << tr("Bad file format!");
				goto CriticalQuit_loc;
				}

			switch(rec_type)
				{
				case 0x00:
					total_bytes+=ihex_len;
					while((ihex_len--)>0)
						{
						Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res);
						if(!res)
							{
							qDebug() << tr("Bad file format!");
							goto CriticalQuit_loc;
							}
						addr++;
						}

					break;

				case 0x01:  /* end */
				case 0x03:  /* start address record */
				case 0x05:  /* start linear address record */
					    /* don't need to do anything, except calculate the checksum */
					while(ihex_len-- > 0)
						{
						Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res);
						if(!res)
							{
							qDebug() << tr("Bad file format!");
							goto CriticalQuit_loc;
							}
						}
					break;

				case 0x02:
					{
					seg_offset=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res)<<8;
					if(!res)
						{
						qDebug() << tr("Bad file format!");
						goto CriticalQuit_loc;
						}
					seg_offset+=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res);
					if(!res)
						{
						qDebug() << tr("Bad file format!");
						goto CriticalQuit_loc;
						}

					/* seg_offset is bits 4-19 of addr, so shift to that. */
					seg_offset<<=4;
					}
					break;

				case 0x04: /* extended linear address record */
					seg_offset=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res)<<8;
					if(!res)
						{
						qDebug() << tr("Bad file format!");
						goto CriticalQuit_loc;
						}
					seg_offset+=Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res);
					if(!res)
						{
						qDebug() << tr("Bad file format!");
						goto CriticalQuit_loc;
						}
					seg_offset<<= 16;
					break;

				default:
					qDebug() << tr("Bad record type!");
					goto CriticalQuit_loc;
				}


			// Read Check Sum and give a report
			Hex2Byte_LoadHEX_GetBlocksID(HexLine, p, CRC, res);
			CRC&=0xFF;
			if(!res)
				{
				qDebug() << tr("Bad file format!");
				goto CriticalQuit_loc;
				}
			if(CRC)
				{
				qDebug() << tr("Bad CRC!");
				goto CriticalQuit_loc;
				}

			HexLine.remove(0, 1);
			HexFile_StringList.append(HexLine);
			//qDebug() << QString("Hex Line %1 loaded").arg(HexFile_StringList.count());
			}
		if(HexFileStream.status()!=QTextStream::Ok)
			{
			qDebug() << tr("File read error!");
			}
		else
			Result=true;
CriticalQuit_loc:
		HexFile.close();
		}
	return Result;
	}

void TRegSetupPar::ButClick()
	{
	((TOktServRegSetup *)OSRS_parent)->PostReqWithStatusClear(CMD_INC_VAL, Index);
	}
void TRegSetupPar::PlusButClick()
	{
	((TOktServRegSetup *)OSRS_parent)->PostReqWithStatusClear(CMD_INC_VAL, Index);
	}
void TRegSetupPar::MinusButClick()
	{
	((TOktServRegSetup *)OSRS_parent)->PostReqWithStatusClear(CMD_DEC_VAL, Index);
	}
void TRegSetupPar::ReadButClick()
	{
	((TOktServRegSetup *)OSRS_parent)->PostReqWithStatusClear(CMD_GET_VAL, Index);
	}
void TRegSetupPar::WriteButClick()
	{
	((TOktServRegSetup *)OSRS_parent)->PostReqWithStatusClear(CMD_SET_VAL, Index);
	}


#ifndef  REGSETUPLIST_PERSISTENT_EDITORS

void RegSetupTableView::OpenEditor4Index(QModelIndex current)
	{
	if((pRegSetupPars)&&(current.isValid())&&(!pRegSetupPars[current.row()]->EditorIsOpened[current.column()])&&
	  (((current.column()>=REGSETUPLIST_VAL_COL)&&(current.column()<=REGSETUPLIST_WRITEBUT_COL)&&
		(pRegSetupPars[current.row()]->ParType!=tyButton)&&
		(pRegSetupPars[current.row()]->ParType!=tyTitle)&&
		pRegSetupPars[current.row()]->Writable)||
	   ((current.column()==REGSETUPLIST_NAME_COL)&&(pRegSetupPars[current.row()]->ParType==tyButton))))
		{
		if(!((TOktServRegSetup *)OSRS_parent)->IsBusy())
			{
			pRegSetupPars[current.row()]->pCell[current.column()]->setIcon(QIcon());
			edit(current);
			pRegSetupPars[current.row()]->EditorIsOpened[current.column()]=true;
			}
		}
	}

void RegSetupTableView::CloseEditor4Index(QModelIndex previous)
	{
	if((pRegSetupPars)&&(previous.isValid())&&(pRegSetupPars[previous.row()]->EditorIsOpened[previous.column()])&&
	  (((previous.column()>=REGSETUPLIST_VAL_COL)&&(previous.column()<=REGSETUPLIST_WRITEBUT_COL)&&
		(pRegSetupPars[previous.row()]->ParType!=tyButton)&&
		(pRegSetupPars[previous.row()]->ParType!=tyTitle)&&
		pRegSetupPars[previous.row()]->Writable)||
	   ((previous.column()==REGSETUPLIST_NAME_COL)&&(pRegSetupPars[previous.row()]->ParType==tyButton))))
		{
		pRegSetupPars[previous.row()]->EditorIsOpened[previous.column()]=false;
		switch(previous.column())
			{
			case REGSETUPLIST_PLUSBUT_COL:
				pRegSetupPars[previous.row()]->pCell[previous.column()]->setIcon(QIcon(":/images/plus.png"));
				break;

			case REGSETUPLIST_MINUSBUT_COL:
				pRegSetupPars[previous.row()]->pCell[previous.column()]->setIcon(QIcon(":/images/minus.png"));
				break;

			case REGSETUPLIST_READBUT_COL:
				pRegSetupPars[previous.row()]->pCell[previous.column()]->setIcon(QIcon(":/images/refresh.png"));
				break;

			case REGSETUPLIST_WRITEBUT_COL:
				pRegSetupPars[previous.row()]->pCell[previous.column()]->setIcon(QIcon(":/images/document_save.png"));
				break;

			case REGSETUPLIST_NAME_COL:
				pRegSetupPars[previous.row()]->pCell[previous.column()]->setIcon(QIcon(":/images/advancedsettings.png"));
				break;

			default:
				break;
			}
		}
	}


void RegSetupTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
	{
	OpenEditor4Index(current);
	CloseEditor4Index(previous);
	QTableView::currentChanged(current, previous);
	}

bool RegSetupTableView::eventFilter(QObject *obj, QEvent *e)
	{

	QModelIndex current=currentIndex();
	switch(e->type())
		{
		case QEvent::FocusIn:
			OpenEditor4Index(current);
			break;
		case QEvent::FocusOut:
			CloseEditor4Index(current);
			break;
		case QEvent::KeyPress:
			{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
			switch(keyEvent->key())
				{
				case Qt::Key_Escape:
					{
					QWidget *w=this;
					while(w->inherits("QMainWindow")==false)
						{
						w=w->parentWidget();
						}
					w->close();
					return true;
					}
				case Qt::Key_Space:
					e->ignore();
					NextFocusChain->setFocus();
					return true;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	return QObject::eventFilter(obj, e);
	}
#else
bool TRegSetupList_ItemDelegate::eventFilter(QObject *obj, QEvent *e)
	{
	//QModelIndex current=((TOktServRegSetup *)OSRS_parent)->RegSetup.TableView->currentIndex();
	switch(e->type())
		{
		case QEvent::FocusIn:
			//((TOktServRegSetup *)OSRS_parent)->RegSetup.TableView->OpenEditor4Index(current);
			break;
		case QEvent::FocusOut:
			//((TOktServRegSetup *)OSRS_parent)->RegSetup.TableView->CloseEditor4Index(current);
			break;
		case QEvent::KeyPress:
			{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
			if(keyEvent->key()==Qt::Key_Escape)
				{
				QWidget *w=this->OSRS_parent;
				while(w->inherits("QMainWindow")==false)
					{
					w=w->parentWidget();
					}
				w->close();
				return true;
				}
			}
			break;

		default:
			break;
		}
	return QObject::eventFilter(obj, e);
	}
#endif


void TRegSetupList_ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	if((index.isValid())&&
	   (((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType!=tyButton)&&
	   (((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType!=tyTitle)&&
	   (((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable)&&
	   (!((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->EditorIsOpened[index.column()])
	   ) switch(index.column())
		{
		case REGSETUPLIST_PLUSBUT_COL:
		case REGSETUPLIST_MINUSBUT_COL:
		case REGSETUPLIST_READBUT_COL:
		case REGSETUPLIST_WRITEBUT_COL:
			{
			QStyleOptionViewItemV4 opt(option);
			initStyleOption(&opt, index);
			opt.rect.moveLeft(opt.rect.left()+3);
			QStyledItemDelegate::paint(painter, opt, index);
			return;
			}
		default:
			break;
		}

	switch(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType)
		{
		case tyName:
			if(index.column() == REGSETUPLIST_NAME_COL)
				{
				QStyleOptionViewItemV4 opt(option);
				initStyleOption(&opt, index);
				opt.rect.adjusted(1,0,-1,0);
				QStyledItemDelegate::paint(painter, opt, index);
				return;
				}
			break;
		case tyTitle:
			if(index.column() == REGSETUPLIST_NAME_COL)
				{
				QStyleOptionViewItemV4 opt(option);
				initStyleOption(&opt, index);
				opt.font.setBold(true);
				opt.font.setPointSize(opt.font.pointSize()+6);
				opt.rect.adjusted(1, 0, -1, 0);
				QStyledItemDelegate::paint(painter, opt, index);
				return;
				}
			break;
		case tyButton:
			break;

		case tyHexChar:
		case tyHexShort:
		case tyUnsigned:
		case tySigned:
		case tyYesNo:
		default:
			break;
		}

	QStyledItemDelegate::paint(painter, option, index);
	}


QWidget *TRegSetupList_ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	(void)option;
	if((((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType!=tyButton)&&
	   (((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType!=tyTitle)&&
	   (((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable))
		switch(index.column())
			{
			case REGSETUPLIST_PLUSBUT_COL:
				{
				xButton *editor = new xButton(TableBut, QIcon(":/images/plus.png"), 28, Qt::ToolButtonIconOnly, parent);
				connect(editor, SIGNAL(clicked()), ((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()], SLOT(PlusButClick()));
				return editor;
				}

			case REGSETUPLIST_MINUSBUT_COL:
				{
				xButton *editor = new xButton(TableBut, QIcon(":/images/minus.png"), 28, Qt::ToolButtonIconOnly, parent);
				connect(editor, SIGNAL(clicked()), ((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()], SLOT(MinusButClick()));
				return editor;
				}

			case REGSETUPLIST_READBUT_COL:
				{
				xButton *editor = new xButton(TableBut, QIcon(":/images/refresh.png"), 28, Qt::ToolButtonIconOnly, parent);
				connect(editor, SIGNAL(clicked()), ((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()], SLOT(ReadButClick()));
				return editor;
				}

			case REGSETUPLIST_WRITEBUT_COL:
				{
				xButton *editor = new xButton(TableBut, QIcon(":/images/document_save.png"), 28, Qt::ToolButtonIconOnly, parent);
				connect(editor, SIGNAL(clicked()), ((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()], SLOT(WriteButClick()));
				return editor;
				}

			default:
				break;
			}

	switch(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType)
		{
		case tyYesNo:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				QComboBox *editor = new QComboBox(parent);
				editor->setEnabled(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable);
				editor->addItems(TOktServRegSetup::YESNO_sl);
				return editor;
				}
			break;

		case tyButton:
			if(index.column() == REGSETUPLIST_NAME_COL)
				{
				xButton *editor = new xButton(TableBut, QIcon(":/images/advancedsettings.png"), 28, Qt::ToolButtonTextBesideIcon, parent);
				editor->setText(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Name);
				connect(editor, SIGNAL(clicked()), ((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()], SLOT(ButClick()));
				editor->setEnabled(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable);
				return editor;
				}
			break;

		case tyHexChar:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				HexSpinBox *editor = new HexSpinBox(parent, true);
				editor->setEnabled(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable);
				return editor;
				}
			break;

		case tyHexShort:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				HexSpinBox *editor = new HexSpinBox(parent, false);
				editor->setEnabled(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable);
				return editor;
				}
			break;

		case tyUnsigned:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				DoubleSpinBox *editor = new DoubleSpinBox(parent, ((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Order);
				float coef = Order[((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Order]/((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->DivBy;
				editor->setMinimum(0);
				editor->setMaximum(65535*coef);
				editor->setSingleStep(coef);
				editor->setDecimals(10);
				editor->setEnabled(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable);
				return editor;
				}
			break;

		case tySigned:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				DoubleSpinBox *editor = new DoubleSpinBox(parent, ((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Order);
				float coef = Order[((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Order]/((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->DivBy;
				editor->setMinimum(-32767*coef);
				editor->setMaximum(32767*coef);
				editor->setSingleStep(coef);
				editor->setDecimals(10	);
				editor->setEnabled(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable);
				return editor;
				}
			break;

		case tyName:
		case tyTitle:
		default:
			break;

		}
	return 0;
	//return QStyledItemDelegate::createEditor(parent, option, index);
	}


void TRegSetupList_ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
	{
	switch(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType)
		{
		case tyYesNo:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
				Q_ASSERT(comboBox);
				int val=((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Value;
				if((val<0)||(val>1)) val=0;
				comboBox->setCurrentIndex(val);
				return;
				}
			break;

		case tyHexChar:
		case tyHexShort:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				HexSpinBox *spinBox = qobject_cast<HexSpinBox*>(editor);
				Q_ASSERT(spinBox);
				spinBox->setValue(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Value);
				return;
				}
			break;

		case tyUnsigned:
		case tySigned:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				DoubleSpinBox *spinBox = qobject_cast<DoubleSpinBox*>(editor);
				Q_ASSERT(spinBox);
				int order=((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Order;
				float divby=((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->DivBy;
				float coef = Order[order]/divby;
				float val=(float)((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Value * coef;
				spinBox->setValue(val);
				return;
				}
			break;

		case tyButton:
		case tyName:
		case tyTitle:
		default:
			break;
		}

	QStyledItemDelegate::setEditorData(editor, index);
	}


void TRegSetupList_ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
	if(((((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType!=tyButton)&&
	   (((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType!=tyTitle)&&
	   (((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Writable))||
	   ((((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType==tyButton)&&(index.column()==REGSETUPLIST_NAME_COL))
	   )
		switch(index.column())
			{
			case REGSETUPLIST_PLUSBUT_COL:
			case REGSETUPLIST_MINUSBUT_COL:
			case REGSETUPLIST_READBUT_COL:
			case REGSETUPLIST_WRITEBUT_COL:
			case REGSETUPLIST_NAME_COL:
				return;
			default:
				break;
			}

	switch(((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->ParType)
		{
		case tyYesNo:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
				Q_ASSERT(comboBox);
				((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Value=(comboBox->currentIndex()<=0)?0:1;
				QString str;
				((TOktServRegSetup *)OSRS_parent)->StringValue(index.row(), str);
				model->setData(index, str);
				return;
				}
			break;

		case tyHexChar:
		case tyHexShort:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				HexSpinBox *spinBox = qobject_cast<HexSpinBox*>(editor);
				Q_ASSERT(spinBox);
				((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Value=spinBox->value();
				QString str;
				((TOktServRegSetup *)OSRS_parent)->StringValue(index.row(), str);
				model->setData(index, str);
				return;
				}
			break;

		case tyUnsigned:
		case tySigned:
			if(index.column() == REGSETUPLIST_VAL_COL)
				{
				DoubleSpinBox *spinBox = qobject_cast<DoubleSpinBox*>(editor);
				Q_ASSERT(spinBox);
				float coef = Order[((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Order]/((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->DivBy;
				float val = spinBox->value()/coef;
				((TOktServRegSetup *)OSRS_parent)->pRegSetupPars[index.row()]->Value=val;
				QString str;
				((TOktServRegSetup *)OSRS_parent)->StringValue(index.row(), str);
				model->setData(index, str);
				return;
				}
			break;

		case tyButton:
		case tyName:
		case tyTitle:
		default:
			break;
		}

	QStyledItemDelegate::setModelData(editor, model, index);
	}



QWidget *TGetBlocksIDList_ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	switch(index.column())
		{
		case GETBLOCKSID_UPDATE_COL:
			{
			xButton *editor = new xButton(TableBut, QIcon(":/images/advancedsettings.png"), 28, Qt::ToolButtonIconOnly, parent);
			connect(editor, SIGNAL(clicked()), ((TOktServRegSetup *)OSRS_parent)->pGetBlocksIDPars[index.row()], SLOT(ButClick()));
			((TOktServRegSetup *)OSRS_parent)->pGetBlocksIDPars[index.row()]->UpdBut=editor;
			editor->setEnabled(false);
			return editor;
			}

		default:
			break;
		}

	return QStyledItemDelegate::createEditor(parent, option, index);
	}

void TGetBlocksIDList_ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	switch(index.column())
		{
		case GETBLOCKSID_STATUS_COL:
			{
			int progress = index.data(Qt::DisplayRole).toInt();
			QStyleOptionProgressBar progressBarOption;
			progressBarOption.rect = option.rect;
			progressBarOption.rect.setHeight(progressBarOption.rect.height()-1);
			progressBarOption.rect.setTop(progressBarOption.rect.top()+1);
			progressBarOption.rect.setWidth(progressBarOption.rect.width()-1);
			progressBarOption.rect.setLeft(progressBarOption.rect.left()+1);
			progressBarOption.minimum = 1;
			progressBarOption.maximum = 100;
			progressBarOption.progress = progress;
			//Код 0 - ничего нет
			if(progress==0)
				{
				progressBarOption.text = tr("");
				}
			//Код 101 - Сохранена
			else if(progress==101)
				{
				progressBarOption.text = tr("Обновлено");
				}
			//Код 102 - Ошибка
			else if(progress==102)
				{
				progressBarOption.progress = 0;
				progressBarOption.text = tr("Ошибка связи");
				}
			//Код 103 - Таймаут
			else if(progress==103)
				{
				progressBarOption.progress = 0;
				progressBarOption.text = tr("Таймаут,занят");
				}
			//Код 104 - Ошибка файла
			else if(progress==104)
				{
				progressBarOption.progress = 0;
				progressBarOption.text = tr("Ошибка'HEX'");
				}
			else
				{
				progressBarOption.text = QString::number(progress) + "%";
				}
			progressBarOption.textAlignment = Qt::AlignHCenter;
			progressBarOption.textVisible = true;

			QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
			return;
			}

		default:
			break;
		}
	QStyledItemDelegate::paint(painter, option, index);
	}

void TGetBlocksIDList_ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
	switch(index.column())
		{
		case GETBLOCKSID_UPDATE_COL:
			return;
		default:
			break;
		}
	QStyledItemDelegate::setModelData(editor, model, index);
	}
