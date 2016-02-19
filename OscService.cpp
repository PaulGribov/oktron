#include "mainwindow.h"
#include "work.h"
#include "OscService.h"
#include <QFile>
#include <QTextStream>

/*
#include <QComboBox>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItemV4>


void Test(TEvent &Event)
	{
	QList<QStandardItem*> items;
	QString OscIndexText;
	OscIndexText.setNum(1);
	foreach(const QString &text, QStringList() << OscIndexText << Event.DateTime.toString(Qt::SystemLocaleLongDate) << Event.Text << QObject::tr("Запись...")  )
		{
		items << new QStandardItem(text);
		items.back()->setEditable(false);
		}

	items[OSC_STATE_COL]->setForeground(Qt::red);

	OscList_tableView = new QTableView(OscService_tab);
	OscList_stim.appendRow(items);
	}
*/

void TOscService::BitsDescFillToModel(struct PacketDescStruct &packet, struct BitsDescStruct &bits)
	{
	for(int k=0;k<bits.StringList.count();k++)
		{
		bits.Items[k] << new QStandardItem() << new QStandardItem(bits.StringList[k]) << new QStandardItem("text");
		bits.Items[k][0]->setEditable(false);
		bits.Items[k][1]->setEditable(false);
		bits.Items[k][2]->setEditable(false);
		packet.Model.appendRow(bits.Items[k]);
		}

	}

//===========================================================================
//Конструктор
//===========================================================================
TOscService::TOscService(QWidget *OscList_parent, QWidget *ParametersView_parent, bool Base_Reserv) : QWidget(OscList_parent)
	{
	this->OscList_parent=OscList_parent;
	this->ParametersView_parent=ParametersView_parent;
	this->Base_Reserv=Base_Reserv;
	OscHalfSize=1000;


	QWidget *OscList_CentralWidget=new QWidget();
	((QMainWindow *)OscList_parent)->setCentralWidget(OscList_CentralWidget);

	QVBoxLayout *OscList_ExtLayout = new QVBoxLayout();
	OscList_CentralWidget->setLayout(OscList_ExtLayout);

	xTabWidget *OscList_TabWidget=new xTabWidget();
	OscList_ExtLayout->addWidget(OscList_TabWidget);

	QWidget *OscList_Tab=new QWidget();
	OscList_TabWidget->addTab(OscList_Tab, QIcon(":/images/stocks.png"), ((QMainWindow *)OscList_parent)->windowTitle());

	QVBoxLayout *OscList_IntLayout = new QVBoxLayout();
	OscList_Tab->setLayout(OscList_IntLayout);

	OscList_TableView=new QTableView();
	OscList_IntLayout->addWidget(OscList_TableView);
	OscList_TableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QPushButton *OscList_CloseButton = new QPushButton(QIcon(":/images/button_cancel.png"), tr("Закрыть"), OscList_parent);
	OscList_ExtLayout->addWidget(OscList_CloseButton, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(OscList_CloseButton, SIGNAL(clicked()), this, SLOT(OscList_CloseWindow()));

	OscList_TableView->setModel(&OscList_Model);
	OscList_TableView->setItemDelegate(new OscList_ItemDelegate(this));
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
	OscList_TableView->verticalHeader()->setDefaultAlignment(Qt::AlignVCenter|Qt::AlignRight);
#endif
#ifdef __i386__
	OscList_TableView->setMinimumHeight(300);
	OscList_TableView->setMinimumWidth(600);
#endif
	OscList_Model.setHorizontalHeaderLabels(QStringList() << tr("Номер осц.") << tr("Дата и время") << tr("Событие") << tr("Состояние"));

	//Инициализация элементов граф.интерфейса MainWindow для сервиса осциллограмм
	OscList_TableView->setColumnWidth(OSC_INDEX_COL, 80);
	OscList_TableView->setColumnWidth(OSC_DATETIME_COL, 160);
	OscList_TableView->setColumnWidth(OSC_EVENT_COL, 250);
	OscList_TableView->setColumnWidth(OSC_STATE_COL, 100);

	QWidget *ParametersView_CentralWidget=new QWidget();
	((QMainWindow *)ParametersView_parent)->setCentralWidget(ParametersView_CentralWidget);

	QVBoxLayout *ParametersView_Layout = new QVBoxLayout();
	ParametersView_CentralWidget->setLayout(ParametersView_Layout);

	ParametersView_TabWidget=new xTabWidget();
	ParametersView_Layout->addWidget(ParametersView_TabWidget);

	QPushButton *ParametersView_CloseButton = new QPushButton(QIcon(":/images/button_cancel.png"), tr("Закрыть"), ParametersView_parent);
	ParametersView_Layout->addWidget(ParametersView_CloseButton, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(ParametersView_CloseButton, SIGNAL(clicked()), this, SLOT(ParametersView_CloseWindow()));

	TabNames_StringList  << tr("Данные с регулятора") << tr("Не используется") << tr("Данные с блока питания") << tr("Данные с индикаторов") << tr("Аналоговые параметры");

	Packet[0].StringList\
		<< tr("Действующее напряжение статора")	<< tr("Uстат")\
		<< tr("Действующий ток статора")	<< tr("Iстат")\
		<< tr("Напряжение возбуждения")		<< tr("Uвозб")\
		<< tr("Ток возбуждения")		<< tr("Iвозб")\
		<< tr("Реактивный ток статора")		<< tr("Iстат.реакт")\
		<< tr("Активный ток статора")		<< tr("Iстат.акт")\
		<< tr("Выходной код управления")	<< tr("OutCode")\
		<< tr("Флаги режима работы #1")		<< tr("ModeFlags1")\
		<< tr("Флаги режима работы #2")		<< tr("ModeFlags2")\
		<< tr("Частота сети электродвигателя")	<< tr("Fсети_эл.дв.");

	Packet[2].StringList\
		<< tr("Напряжение первого источника")	<< tr("Uип1")\
		<< tr("Напряжение второго источника")	<< tr("Uип2")\
		<< tr("Напряжение накопителя")		<< tr("Uнакопит")\
		<< tr("Выходное напряжение +5В")	<< tr("U5V")\
		<< tr("Выходное напряжение +24В")	<< tr("U24V")\
		<< tr("Ток входного напряжения +24В")	<< tr("I24V")\
		<< tr("Температура блока")		<< tr("T")\
		<< tr("Слово конфигурации")		<< tr("Config");

	Packet[3].StringList\
		<< tr("Состояние кнопок и переключателей")	<< tr("KeysState")\
		<< tr("Напряжение 24В, входное #1")		<< tr("U24V1")\
		<< tr("Напряжение 24В, входное #2")		<< tr("U24V2")\
		<< tr("Напряжение +5В, основное питание")	<< tr("U5V")\
		<< tr("Напряжение питания блока V")		<< tr("UблокV")\
		<< tr("Ток потребления от ИП")			<< tr("Iип")\
		<< tr("Ток потребления блока V")		<< tr("IблокV");

	Packet[4].StringList\
		<< tr("Код действующего напряжения статора")		<< tr("Ustat_code")\
		<< tr("Код действующего тока статора")			<< tr("Istat_code")\
		<< tr("Код напряжения возбуждения")			<< tr("Uvozb_code")\
		<< tr("Код тока возбуждения")				<< tr("Ivozb_code")\
		<< tr("Код активного тока статора")			<< tr("Istat_act_code")\
		<< tr("Код реактивного тока статора")			<< tr("Istat_react_code")\
		<< tr("Частота сети")					<< tr("Fсети")\
		<< tr("Переменная 150-300Гц составляющая тока ротора")	<< tr("Iрот.f_150-300")\
		<< tr("Переменная 50Гц составляющая напряжения ротора")	<< tr("Uрот.f_50");

	ModeFlags1.StringList\
		<< tr("Двигатель включен")\
		<< tr("Блок в работе")\
		<< tr("Возбуждение подано")\
		<< tr("Работа под управлением систем ПП или ЧРП")\
		<< tr("Геркон пускового сопротивления")\
		<< tr("Команда включить R пусковое")\
		<< tr("Схема выпрямления - мост)")\
		<< tr("---")\
		<< tr("---")\
		<< tr("---")\
		<< tr("---")\
		<< tr("---")\
		<< tr("Больше")\
		<< tr("Меньше")\
		<< tr("Форсировка")\
		<< tr("Работа ОМВ");

	ModeFlags2.StringList\
		<< tr("Защита \"Затянувшийся пуск\"")\
		<< tr("Защита \"Потеря возбуждения\"")\
		<< tr("Защита \"Асинхронный ход по Rпусковому\"")\
		<< tr("Защита \"Асинхронный ход по реактивному току\"")\
		<< tr("Защита \"МТЗ ротора\"")\
		<< tr("Защита \"КЗ ротора по частоте 150/300Гц\"")\
		<< tr("Защита \"Перегрев возбудителя\"")\
		<< tr("Защита \"Обрыв внешнего управления\"")\
		<< tr("Нагрев ротора (ток более 110%)")\
		<< tr("Режим остывания")\
		<< tr("---")\
		<< tr("---")\
		<< tr("---")\
		<< tr("---")\
		<< tr("Реле готовности (1-готов)")\
		<< tr("Реле защит (срабатывание отключающей защиты)");

	Config.StringList\
		<< tr("Блок исправен")\
		<< tr("Тест 1-го ИП")\
		<< tr("Тест 2-го ИП")\
		<< tr("Наличие накопителя")\
		<< tr("Исправность накопителя")\
		<< tr("Накопитель требует замены")\
		<< tr("Работа от накопителя")\
		<< tr("Снижение напряжения +24В менее +20В")\
		<< tr("Наличие напряжения +48В на выходе")\
		<< tr("КЗ +5В внешних цепей")\
		<< tr("КЗ +24В внешних цепей")\
		<< tr("Исправность источника +48В");

	KeysState.StringList\
		<< tr("Кнопка \"Вверх\"")\
		<< tr("Кнопка \"Ввод\"")\
		<< tr("Кнопка \"Вниз\"")\
		<< tr("Кнопка \"Больше\"")\
		<< tr("Кнопка \"Влево\"")\
		<< tr("Кнопка \"Меньше\"")\
		<< tr("Кнопка \"Вправо\"")\
		<< tr("Кнопка \"Деблокирование защит\"")\
		<< tr("Тумблер \"Автомат/ручное\"")\
		<< tr("Кнопка \"СТОП\"")\
		<< tr("Кнопка \"Опробование\"");

	Packet[14].StringList\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16")\
		<< tr("") << tr("int16");

	for(int i=5;i<OKTSERV_PACKETS_NUM;i++)
		{
		if(i==14)
			{
			TabNames_StringList.append(tr("Диагностика"));
			/*
			for(int j=0;j<16;j++)
				{
				Packet[i].StringList.append(tr("int16"));
				Packet[i].StringList.append(tr("int16"));
				}
			*/
			}
		else
			{
			TabNames_StringList.append(tr(""));
			}
		}

	//Создание закладок "Параметры регулятора"
	int pi_cnt=0;
	for(int i=0;i<OKTSERV_PACKETS_NUM;i++)
		{
		Packet[i].Tab = new QWidget();
		if(((i!=1)&&(i<5))||(i==14)) ParametersView_TabWidget->addTab(Packet[i].Tab, QIcon(":/images/meter3.png"), TabNames_StringList.at(i));
		Packet[i].TableView = new QTableView(Packet[i].Tab);
#ifdef __i386__
		Packet[i].TableView->setMinimumHeight(300);
		Packet[i].TableView->setMinimumWidth(600);
#endif
		QVBoxLayout *vbl = new QVBoxLayout();
		vbl->addWidget(Packet[i].TableView);
		Packet[i].Tab->setLayout(vbl);
		Packet[i].TableView->setModel(&Packet[i].Model);
		Packet[i].Model.setHorizontalHeaderLabels(QStringList() << tr("Адрес") << tr("Параметр") << tr("Значение"));

		for(int j=0;j<Packet[i].StringList.count();j+=2)
			{

			Packet[i].Items[j>>1] << new QStandardItem(QString("%1h").arg(i*0x10+j/2, 4, 16, QLatin1Char('0'))) << new QStandardItem(Packet[i].StringList[j]) << new QStandardItem("text");
			Packet[i].Items[j>>1][0]->setEditable(false);
			Packet[i].Items[j>>1][1]->setEditable(false);
			Packet[i].Items[j>>1][2]->setEditable(false);
			Packet[i].Model.appendRow(Packet[i].Items[j>>1]);

			switch(pi_cnt+(j>>1))
				{
				//Флаги режима работы #1
				case 7:
					BitsDescFillToModel(Packet[i], ModeFlags1);
					break;
				//Флаги режима работы #2
				case 8:
					BitsDescFillToModel(Packet[i], ModeFlags2);
					break;
				//Слово конфигурации
				case PACKET0_WORDS_COUNT+7:
					BitsDescFillToModel(Packet[i], Config);
					break;
				//Состояние кнопок и переключателей
				case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+0:
					BitsDescFillToModel(Packet[i], KeysState);
					break;
				default:
					break;
				}
			}

		Packet[i].TableView->setColumnWidth(PARAMETERSVIEWTAB_ADDR_COL, 50);
		Packet[i].TableView->setColumnWidth(PARAMETERSVIEWTAB_NAME_COL, 300);
		Packet[i].TableView->setColumnWidth(PARAMETERSVIEWTAB_VAL_COL, 100);
		//Packet[i].TableView->resizeColumnsToContents();
		//Packet[i].TableView->resizeRowsToContents();

		pi_cnt+=Packet[i].StringList.count()>>1;
		}

	}

void TOscService::ParametersView_CloseWindow()
	{
	ParametersView_parent->close();
	}

void TOscService::OscList_CloseWindow()
	{
	OscList_parent->close();
	}

void TOscService::BitsStateFillToModel(struct BitsDescStruct &bits, unsigned short state)
	{
	int mask=0x0001;
	for(int i=0;i<bits.StringList.count();i++)
		{
		bits.Items[i][PARAMETERSVIEWTAB_VAL_COL]->setText((state&mask)?tr("ДА"):tr("НЕТ"));
		mask<<=1;
		}
	}

//===========================================================================
//Сброс активных процессов
//===========================================================================
void TOscService::ResetOscProcess()
	{
	foreach(TOscProcess op, OscProcess)
		{
		op.pOscStateCell->setText(QString::number(102));
		}
	OscProcess.clear();
	OscData.clear();
	}

//===========================================================================
//Добавление записи данных для осциллограммы
//===========================================================================
void TOscService::AddOscRecord(TOscDataWithIndic &data, bool print)
	{
	//Добавление записи
	OscData << data.OscData;

	//Если активных процессов записи осциллограмм нет, то в буфере половина осциллограммы
	if(OscProcess.count()==0)
		{
		if(OscData.count()>OscHalfSize) OscData.removeFirst();
		//if(tester_QTimer->isActive()) tester_QTimer->stop();
		}
	//Есть активные процессы...
	else
		{
		//Осциллограмма записана?
		if(OscData.count()>=OscHalfSize*2)
			{
			int offset_osc_data;

			if(SaveOscillogram(OscProcess.first()))
				{
				//Код 101 - осц.сохранена
				OscProcess.first().pOscStateCell->setText(QObject::tr("101"));
				}
			else
				{
				//Код 102 - ошибка
				OscProcess.first().pOscStateCell->setText(QObject::tr("102"));
				}


			//Удаляем первый процесс
			OscProcess.removeFirst();

			//Если процессы ещё есть
			if(OscProcess.count())
				{
				//Смещение массива данных, на то кол-во, которое более не понадобится
				offset_osc_data=OscProcess.first().DataIndex;

				//Смещение индексов для всех процессов
				for(int i=0; i<OscProcess.count(); i++)
					{
					OscProcess[i].DataIndex-=offset_osc_data;
					}
				}
			else
				{
				offset_osc_data=OscData.count()-OscHalfSize;
				}

			//Освобождаем память
			for(int i=0;i<offset_osc_data;i++) OscData.removeFirst();
			}

		//Обновление процента записи осциллограммы
		foreach(TOscProcess op, OscProcess)
			{
			int i=((float)(OscData.count()-op.DataIndex)*100)/(OscHalfSize*2);
			op.pOscStateCell->setText(QString::number(i));
			}
		}

	if(print)
		{
		//Обновление содержимого таблицы
		Packet[0].Items[0][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.OscData.Packet0.Ustat_1V, 0, 'f', 0));
		Packet[0].Items[1][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.OscData.Packet0.Istat_0_1A/10, 0, 'f', 1));
		Packet[0].Items[2][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.OscData.Packet0.Uvozb_0_1V/10, 0, 'f', 1));
		Packet[0].Items[3][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.OscData.Packet0.Ivozb_0_1A/10, 0, 'f', 1));
		Packet[0].Items[4][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.OscData.Packet0.Istat_react_0_1A/10, 0, 'f', 1));
		Packet[0].Items[5][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.OscData.Packet0.Istat_act_0_1A/10, 0, 'f', 1));
		Packet[0].Items[6][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.OscData.Packet0.OutCode));
		Packet[0].Items[7][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.OscData.Packet0.ModeFlags1, 4, 16, QLatin1Char('0')).toUpper());
		BitsStateFillToModel(ModeFlags1, data.OscData.Packet0.ModeFlags1);
		Packet[0].Items[8][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.OscData.Packet0.ModeFlags2, 4, 16, QLatin1Char('0')).toUpper());
		BitsStateFillToModel(ModeFlags2, data.OscData.Packet0.ModeFlags2);
		Packet[0].Items[9][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.OscData.Packet0.Freq_0_01Hz/100, 0, 'f', 2));

		Packet[2].Items[0][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet2.Uip1_0_01V/100, 0, 'f', 2));
		Packet[2].Items[1][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet2.Uip2_0_01V/100, 0, 'f', 2));
		Packet[2].Items[2][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet2.Unakopit_0_01V/100, 0, 'f', 2));
		Packet[2].Items[3][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet2.U5V_0_01V/100, 0, 'f', 2));
		Packet[2].Items[4][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet2.U24V_0_01V/100, 0, 'f', 2));
		Packet[2].Items[5][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet2.I24V_0_001A/1000, 0, 'f', 3));
		Packet[2].Items[6][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet2.T_0_1C/10, 0, 'f', 1));
		Packet[2].Items[7][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet2.Config, 4, 16, QLatin1Char('0')).toUpper());
		BitsStateFillToModel(Config, data.Packet2.Config);

		Packet[3].Items[0][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet3.KeysState, 4, 16, QLatin1Char('0')).toUpper());
		BitsStateFillToModel(KeysState, data.Packet3.KeysState);
		Packet[3].Items[1][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet3.U24V1_0_01V/100, 0, 'f', 2));
		Packet[3].Items[2][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet3.U24V2_0_01V/100, 0, 'f', 2));
		Packet[3].Items[3][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet3.U5V_0_01V/100, 0, 'f', 2));
		Packet[3].Items[4][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet3.Ublokv_0_01V/100, 0, 'f', 2));
		Packet[3].Items[5][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet3.Iip_0_001A/1000, 0, 'f', 3));
		Packet[3].Items[6][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%L1").arg((float)data.Packet3.Iblokv_0_001A/1000, 0, 'f', 3));

		Packet[4].Items[0][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Ustat_code));
		Packet[4].Items[1][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Istat_code));
		Packet[4].Items[2][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Uvozb_code));
		Packet[4].Items[3][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Ivozb_code));
		Packet[4].Items[4][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Istat_react_code));
		Packet[4].Items[5][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Istat_act_code));
		Packet[4].Items[6][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Freq));
		Packet[4].Items[7][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Irot_f150_300));
		Packet[4].Items[8][PARAMETERSVIEWTAB_VAL_COL]->setText(QString("%1").arg(data.Packet4.Urot_f50));

		for(int i=0;i<16;i++)
			{
			QString txt=QString("0x%1, %L2, %L3").arg(data.Packet14.vals[i].uint16, 4, 16, QLatin1Char('0')).arg((float)data.Packet14.vals[i].uint16, 0, 'f', 0).arg((float)data.Packet14.vals[i].int16, 0, 'f', 0);
			Packet[14].Items[i][PARAMETERSVIEWTAB_VAL_COL]->setText(txt);
			}

		}
	}

void TOscService::BitsDescWriteToOscFile(QFile &OscFile, struct BitsDescStruct &bits)
	{
	OscFile.write("<DATA>{<TP>{bul}");
	for(int k=0;k<bits.StringList.count();k++)
		{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		OscFile.write(QString(tr("<B%1>{")).arg(k, 1, 16, QLatin1Char('0')).toUpper().toLatin1().data());
#else
		OscFile.write(QString(tr("<B%1>{")).arg(k, 1, 16, QLatin1Char('0')).toUpper().toAscii().data());
#endif
		char *buf=utf8_to_cp1251_qstr(bits.StringList[k]);
		OscFile.write(buf);
		delete buf;
		OscFile.write("}");
		}
	OscFile.write("}\r\n");
	}

//===========================================================================
//Старт осциллограммы для события
//===========================================================================
bool TOscService::SaveOscillogram(TOscProcess &op)
	{
	QStringList OsdHeaderBuf;
	QString OscFileName = QString(Base_Reserv?tr("OscBase%1.OSD"):tr("OscReserv%1.OSD")).arg(op.EventExt.OscIndex);
	QFile OscFile(OscFileName);
#ifdef __linux__
	QDir::setCurrent("/mnt/localdisk/oscs");
#endif
	if(OscFile.open(QFile::WriteOnly | QFile::Text))
		{
		char *buf;

		//Формирование заголовка в памяти
		OsdHeaderBuf	<< tr("<VERSION>{1.0}\r\n")
				<< tr("<RUS CODE>{WIN}\r\n")
				<< tr("<RATE>{0.001}\r\n")
				<< tr("<NUMBER>{%1}\r\n").arg(OscData.count())
				<< tr("<FILENAME>{") << OscFileName << tr("}\r\n")
				<< tr("<EVENT TIME>") << op.EventExt.Event.DateTime.toString(tr("{dd.MM.yyyy hh:mm:ss}\r\n"))
				<< tr("<EVENT>{") << op.EventExt.Event.Text << tr("}\r\n")
				<< tr("<TIME>") << op.EventExt.Event.DateTime.toString(tr("{dd.MM.yyyy hh:mm:ss}\r\n"))
				<< tr("<DEVICE NAME>{ОКТРОН}\r\n");

		for(int i=0;i<OsdHeaderBuf.count();i++)
			{
			buf=utf8_to_cp1251_qstr(OsdHeaderBuf.at(i)); //Перекодирование названия параметра в Win 1251
			OscFile.write(buf);
			delete buf;
			}
		int pi_cnt=0;
		for(int i=0;i<OKTSERV_PACKETS_TO_OSC_NUM;i++)
			{
			if((i!=1)&&(i<5)) for(int j=0;j<Packet[i].StringList.count();j+=2)
				{
				int pi=pi_cnt+(j>>1);
				switch(pi)
					{
					//Флаги режима работы #1
					case 7:
						BitsDescWriteToOscFile(OscFile, ModeFlags1);
						break;
					//Флаги режима работы #2
					case 8:
						BitsDescWriteToOscFile(OscFile, ModeFlags2);
						break;
					//Слово конфигурации
					case PACKET0_WORDS_COUNT+7:
						BitsDescWriteToOscFile(OscFile, Config);
						break;
					//Состояние кнопок и переключателей
					case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+0:
						BitsDescWriteToOscFile(OscFile, KeysState);
						break;

					default:
						OscFile.write("<DATA>{<TP>{int}<LN>{");
						buf=utf8_to_cp1251_qstr(Packet[i].StringList[j]);
						OscFile.write(buf);
						delete buf;
						OscFile.write("}<SN>{");
						buf=utf8_to_cp1251_qstr(Packet[i].StringList[j+1]);
						OscFile.write(buf);
						delete buf;
						OscFile.write("}<km>{");
						switch(pi)
							{
							//Действующее напряжение статора
							case 0:
							//Выходной код управления
							case 6:

							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+PACKET3_WORDS_COUNT+0:
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+PACKET3_WORDS_COUNT+1:
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+PACKET3_WORDS_COUNT+2:
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+PACKET3_WORDS_COUNT+3:
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+PACKET3_WORDS_COUNT+4:
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+PACKET3_WORDS_COUNT+5:
							default:
								OscFile.write("1.0");
								break;

							//Действующий ток статора
							case 1:
							//Напряжение возбуждения
							case 2:
							//Ток возбуждения
							case 3:
							//Реактивный ток статора
							case 4:
							//Активный ток статора
							case 5:
							//Температура блока
							case PACKET0_WORDS_COUNT+6:
								OscFile.write("0.1");
								break;

							//Частота сети электродвигателя
							case 9:
							//Напряжение первого источника
							case PACKET0_WORDS_COUNT+0:
							//Напряжение второго источника
							case PACKET0_WORDS_COUNT+1:
							//Напряжение накопителя
							case PACKET0_WORDS_COUNT+2:
							//Выходное напряжение +5В
							case PACKET0_WORDS_COUNT+3:
							//Выходное напряжение +24В
							case PACKET0_WORDS_COUNT+4:
							//Напряжение 24В, входное #1
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+1:
							//Напряжение 24В, входное #2
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+2:
							//Напряжение +5В, основное питание
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+3:
							//Напряжение питания блока V
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+4:
								OscFile.write("0.01");
								break;

							//Ток входного напряжения +24В
							case PACKET0_WORDS_COUNT+5:
							//Ток потребления от ИП
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+5:
							//Ток потребления блока V
							case PACKET0_WORDS_COUNT+PACKET2_WORDS_COUNT+6:
								OscFile.write("0.001");
								break;
							}
						OscFile.write("}}\r\n");
						break;
					}
				}

			pi_cnt+=Packet[i].StringList.count()>>1;
			}
		OscFile.write("<END TITLE>\r\n");


		for(int k=0;k<OscData.count();k++)
			{
			int ofs=0, ofs_new;
			for(int i=0;i<OKTSERV_PACKETS_TO_OSC_NUM;i++)
				{
				if((i!=1)&&(i<5))
					{
					ofs_new=Packet[i].StringList.count();
					OscFile.write((char *)&OscData[k].Packet0+ofs, ofs_new);
					ofs=ofs_new;
					}
				}
			OscFile.write("\r\n");
			}

		OscFile.close();
		}
	else
		{
		return false;
		}

	return true;
	}


//===========================================================================
//Старт осциллограммы для события
//===========================================================================
int TOscService::OscStart(TEventExt *e)
	{
	if(OscProcess.count()<OSC_PROCESS_NUM_MAX)
		{
		TOscProcess op;

		//Присвоение величин процесса
		if(OscData.count()>OscHalfSize) op.DataIndex=OscData.count()-OscHalfSize;
		else op.DataIndex=0;
		op.EventOffset=OscData.count()-1-op.DataIndex;
		op.EventExt=*e;

		//Добавление строки в таблицу осциллограмм
		QList<QStandardItem*> items;
		//                                            OSC_INDEX_COL                             OSC_DATETIME_COL                                     OSC_EVENT_COL OSC_STATE_COL
		foreach(const QString &text, QStringList() << QString().setNum(op.EventExt.OscIndex) << e->Event.DateTime.toString(tr("dd.MM.yyyy hh:mm:ss")) << e->Event.Text << QObject::tr("0")  )
			{
			items << new QStandardItem(text);
			items.back()->setEditable(false);
			}
		op.pOscStateCell=items[OSC_STATE_COL]; //ссылка на ячейку
		OscList_Model.appendRow(items);
		if(OscList_Model.rowCount()>OSC_NUM_MAX)
			{
			OscList_Model.removeRow(0);
			}

/*
			{
			//Изменение содержимого строки в таблице
			int c=0;
			foreach(QStandardItem *item,  items)
				{
				OscList_Model.setItem(op.EventExt.OscIndex, c++, item);
				}
			}
		OscList_TableView->openPersistentEditor(op.pOscStateCell->index());
*/


		//Добавление процесса
		OscProcess.append(op);

		return op.EventExt.OscIndex;

		/*if(!(tester_QTimer->isActive()))
			{*/
			//tester_QTimer->start(20);
			/*}*/

		}
	else
		return -1;
	}


void OscList_ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	if((index.column() == OSC_STATE_COL))
		{
		int progress = index.data(Qt::DisplayRole).toInt();
		QStyleOptionProgressBar progressBarOption;
		progressBarOption.rect = option.rect;
		progressBarOption.minimum = 1;
		progressBarOption.maximum = 100;
		progressBarOption.progress = progress;
		//Код 0 - Начало записи
		if(progress==0)
			{
			progressBarOption.text = tr("Запись...");
			}
		//Код 101 - Сохранена
		else if(progress==101)
			{
			progressBarOption.text = tr("Сохранена");
			}
		//Код 102 - Ошибка
		else if(progress==102)
			{
			progressBarOption.progress = 0;
			progressBarOption.text = tr("Ошибка");
			}
		else
			{
			progressBarOption.text = QString::number(progress) + "%";
			}
		progressBarOption.textVisible = true;
		progressBarOption.textAlignment = Qt::AlignHCenter;

		QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

		}
	else
		QStyledItemDelegate::paint(painter, option, index);
	}


QWidget *OscList_ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	if(index.column() == OSC_STATE_COL)
		{
		}

	return QStyledItemDelegate::createEditor(parent, option, index);
	}


void OscList_ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
	{
	if(index.column() == OSC_STATE_COL)
		{
		}
	else
		QStyledItemDelegate::setEditorData(editor, index);
	}


void OscList_ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
	if(index.column() == OSC_STATE_COL)
		{
		}
	else
		{
		QStyledItemDelegate::setModelData(editor, model, index);
		}
	}


