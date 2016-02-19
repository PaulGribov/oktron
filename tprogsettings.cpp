#include <QPushButton>
#include "tprogsettings.h"
#include <QSpacerItem>
#include <QXmlStreamReader>
#include "work.h"

TProgSettings::TProgSettings(QWidget *parent, TOktServExt **pOktServExt0, TOktServExt **pOktServExt1) : QMainWindow(parent)
	{
	setWindowIcon(QIcon(":/images/applications_system.png"));
	setWindowTitle("Настройки программы");

	QVBoxLayout *ProgSettings_Layout = new QVBoxLayout();
	QFrame *ProgSettings_Frame = new QFrame();
	ProgSettings_Frame->setLayout(ProgSettings_Layout);
	setCentralWidget(ProgSettings_Frame);

	//Закладки настроек программы
	ProgSettings_tabWidget=new xTabWidget();
	ProgSettings_Layout->addWidget(ProgSettings_tabWidget);

	//------------------------------------------------------------------------------------------------------------
	//Закладка "настройки портов серверов"
	PortSettings_Tab = new QWidget();
	ProgSettings_tabWidget->addTab(PortSettings_Tab, QIcon(":/images/serial_port.png"), tr("Настройки последовательных портов"));

	//Настройки портов
	QHBoxLayout *PortSettings_Layout = new QHBoxLayout();
	PortSettings_GroupBox[0]=new QGroupBox();
	*pOktServExt0=new TOktServExt(PortSettings_GroupBox[0], tr("основного"), tr("Основной"));
	OktServ[0]=*pOktServExt0;
	PortSettings_Layout->addWidget(PortSettings_GroupBox[0], 0, Qt::AlignLeft);
	PortSettings_GroupBox[1]=new QGroupBox();
	PortSettings_Layout->addWidget(PortSettings_GroupBox[1], 0, Qt::AlignRight);
	*pOktServExt1=new TOktServExt(PortSettings_GroupBox[1], tr("резервного"), tr("Резервный"));
	OktServ[1]=*pOktServExt1;

	//PortSettings_Layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	//Кнопка "применить"
	QHBoxLayout *PortSettingsButtons_Layout = new QHBoxLayout();
	PortSettingsButtons_Layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	PortsSettingsApply_Button = new QPushButton(QIcon(":/images/apply.png"), tr("Применить настройки"));
	PortSettingsButtons_Layout->addWidget(PortsSettingsApply_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(PortsSettingsApply_Button, SIGNAL(clicked()), this, SLOT(PortsSettingsApply()));

	//Слой вмещающий в себя Настройки портов и кнопку Применить
	QVBoxLayout *PortSettingsMain_Layout = new QVBoxLayout();
	PortSettingsMain_Layout->addLayout(PortSettings_Layout);
	PortSettingsMain_Layout->addLayout(PortSettingsButtons_Layout);

	//Присвоение слоя закладке
	PortSettings_Tab->setLayout(PortSettingsMain_Layout);

	//------------------------------------------------------------------------------------------------------------
	//Закладка "Общие настройки"
	GeneralSettings_Tab = new QWidget();
	ProgSettings_tabWidget->addTab(GeneralSettings_Tab, QIcon(":/images/advancedsettings.png"), tr("Общие настройки"));

	//Настройки портов
	QVBoxLayout *GeneralSettings_Layout = new QVBoxLayout();

	AutomaticStart_CheckBox = new QCheckBox(tr("Включать сервер при старте"));
	GeneralSettings_Layout->addWidget(AutomaticStart_CheckBox, 0, Qt::AlignHCenter | Qt::AlignVCenter);

	SetDateTime_GroupBox=new QGroupBox(tr("Установка времени"));
	GeneralSettings_Layout->addWidget(SetDateTime_GroupBox, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	QVBoxLayout *SetDateTime_Layout = new QVBoxLayout();
	SetDateTime_GroupBox->setLayout(SetDateTime_Layout);
	SetDateTime=new QDateTimeEdit();
	SetDateTime_Layout->addWidget(SetDateTime);
	//connect(SetDateTime, SIGNAL(timeChanged()), this, SLOT(DateTimeUpdated()));

	//Присвоение слоя закладке
	GeneralSettings_Tab->setLayout(GeneralSettings_Layout);

	//Кнопка "закрыть настройки программы"
	QHBoxLayout *ProgSettingsButtons_Layout = new QHBoxLayout();
	ProgSettingsButtons_Layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton *Save_Button = new QPushButton(QIcon(":/images/document_save.png"), tr("Сохранить"));
	ProgSettingsButtons_Layout->addWidget(Save_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(Save_Button, SIGNAL(clicked()), this, SLOT(Save()));
	QPushButton *Close_Button = new QPushButton(QIcon(":/images/button_cancel.png"), tr("Закрыть"));
	ProgSettingsButtons_Layout->addWidget(Close_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(Close_Button, SIGNAL(clicked()), this, SLOT(Close()));
	ProgSettings_Layout->addLayout(ProgSettingsButtons_Layout);
	}

void TProgSettings::show()
	{
	PresetVal=QDateTime::currentDateTime();
	SetDateTime->setDateTime(PresetVal);
	QMainWindow::show();
	}

void TProgSettings::PortsSettingsApply()
	{
	OktServ[0]->PortSettingsApply();
	OktServ[1]->PortSettingsApply();
	}

void TProgSettings::DateTimeUpdated(QTime &date)
	{
	SetDateTime_GroupBox->setTitle(date.toString(tr("dd.MM.yyyy hh:mm:ss")));
	}

void TProgSettings::Close()
	{
	close();
	}

void TProgSettings::LoadCommSettings(QXmlStreamReader &xml, int server_index)
	{
	TCommPortSettings *settings=&OktServ[server_index]->Settings;
	QXmlStreamAttributes attributes = xml.attributes();

	if(attributes.hasAttribute(tr("CommPortIndex")))
		settings->CommPort_index=attributes.value(tr("CommPortIndex")).toString().toInt();
	if(attributes.hasAttribute(tr("BaudRateIndex")))
		settings->BaudRate_index=attributes.value(tr("BaudRateIndex")).toString().toInt();
	if(attributes.hasAttribute(tr("DataBitsIndex")))
		settings->DataBits_index=attributes.value(tr("DataBitsIndex")).toString().toInt();
	if(attributes.hasAttribute(tr("StopBitsIndex")))
		settings->StopBits_index=attributes.value(tr("StopBitsIndex")).toString().toInt();
	if(attributes.hasAttribute(tr("ParityIndex")))
		settings->Parity_index=attributes.value(tr("ParityIndex")).toString().toInt();
	if(attributes.hasAttribute(tr("FlowControlIndex")))
		settings->FlowControl_index=attributes.value(tr("FlowControlIndex")).toString().toInt();
	}

void TProgSettings::LoadGeneralSettings(QXmlStreamReader &xml)
	{
	QXmlStreamAttributes attributes = xml.attributes();

	if(attributes.hasAttribute(tr("AutomaticStart")))
		GeneralSettings.AutomaticStart = attributes.value(tr("AutomaticStart")).toString().toInt() > 0 ? true : false;
	}

void TProgSettings::Load()
	{
#ifdef __linux__
	QDir::setCurrent("/mnt/localdisk/oscs");
#endif
	QXmlStreamReader xml;

	QFile IniFile(INI_FILENAME);
	if (!IniFile.open(QIODevice::ReadWrite | QIODevice::Text)) qDebug() << tr("File open error!");
	else
		{
		xml.setDevice(&IniFile);
		while(!xml.atEnd() && !xml.hasError())
			{
			QXmlStreamReader::TokenType token = xml.readNext();
			if(token == QXmlStreamReader::StartDocument) continue;
			if(token == QXmlStreamReader::StartElement)
				{
				if(xml.name() == tr("Settings")) continue;
				for(int i=0;i<2;i++)
					{
					if(xml.name() == tr("OktServ%1").arg(i))
						{
						LoadCommSettings(xml, i);
						}
					}
				if(xml.name() == tr("GeneralSettings"))
					{
					LoadGeneralSettings(xml);
					}
				}
			}

		IniFile.close();
		}

	for(int i=0;i<2;i++) OktServ[i]->PrintPortsParameters();
	AutomaticStart_CheckBox->setChecked(GeneralSettings.AutomaticStart);
	}

void TProgSettings::SaveCommSettings(QXmlStreamWriter &xml, int server_index)
	{
	TCommPortSettings *settings=&OktServ[server_index]->Settings;
	QString port_name = TOktServ::CommPortSettingsTexts.CommPort[settings->CommPort_index];
	QString key_prefix = QString(tr("OktServ%1")).arg(server_index);

	PortsSettingsApply();

	xml.writeStartElement(key_prefix);
	xml.writeAttribute(tr("CommPortName"), port_name);
	xml.writeAttribute(tr("CommPortIndex"), QString().setNum(settings->CommPort_index));
	xml.writeAttribute(tr("BaudRateIndex"), QString().setNum(settings->BaudRate_index));
	xml.writeAttribute(tr("DataBitsIndex"), QString().setNum(settings->DataBits_index));
	xml.writeAttribute(tr("StopBitsIndex"), QString().setNum(settings->StopBits_index));
	xml.writeAttribute(tr("ParityIndex"), QString().setNum(settings->Parity_index));
	xml.writeAttribute(tr("FlowControlIndex"), QString().setNum(settings->FlowControl_index));
	xml.writeEndElement();
	}

void TProgSettings::SaveGeneralSettings(QXmlStreamWriter &xml)
	{
	GeneralSettings.AutomaticStart = AutomaticStart_CheckBox->checkState();

	xml.writeStartElement(tr("GeneralSettings"));
	xml.writeAttribute(tr("AutomaticStart"), QString().setNum(GeneralSettings.AutomaticStart));
	xml.writeEndElement();
	}

void TProgSettings::Save()
	{
#ifdef __linux__
	QDir::setCurrent("/mnt/localdisk/oscs");
#endif
	QXmlStreamWriter xml;
	qDebug() << tr("Save Ini-file...");
	QFile IniFile(INI_FILENAME);
	if (!IniFile.open(QIODevice::ReadWrite | QIODevice::Text)) qDebug() << tr("File open error!");
	else
		{
		xml.setDevice(&IniFile);
		xml.setAutoFormatting(true);
		//xml.setCodec("Windows-1251");

		xml.writeStartElement(tr("Settings"));
		for(int i=0;i<2;i++) SaveCommSettings(xml, i);
		SaveGeneralSettings(xml);
		xml.writeEndElement();

		IniFile.close();
		}

	/*
	qDebug() << PresetVal.toString("dd_MM_yyyy__hh_mm_ss");
	qDebug() << SetDateTime->dateTime().toString("dd_MM_yyyy__hh_mm_ss");
	*/
	if(PresetVal!=SetDateTime->dateTime())
		{
		qDebug() << tr("Datetime updated...");
		PresetVal=SetDateTime->dateTime();
#ifdef __linux__
		SetSystemClock(PresetVal);
#endif
		}

	}
