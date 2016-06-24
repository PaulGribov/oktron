#include <QPushButton>
#include "tprogsettings.h"
#include <QSpacerItem>
#include <QXmlStreamReader>
#include "work.h"
#include "mainwindow.h"

TProgSettings::TProgSettings(QWidget *obj_MainWindow) : QMainWindow(obj_MainWindow)
	{
	this->obj_MainWindow=obj_MainWindow;

	setWindowIcon(QIcon(":/images/applications_system.png"));

	QVBoxLayout *ProgSettings_Layout = new QVBoxLayout();
	QFrame *ProgSettings_Frame = new QFrame();
	ProgSettings_Frame->setLayout(ProgSettings_Layout);
	setCentralWidget(ProgSettings_Frame);

	//Закладки настроек программы
	ProgSettings_tabWidget=new xTabWidget();
	ProgSettings_Layout->addWidget(ProgSettings_tabWidget);
	ProgSettings_tabWidget->setStyleSheet(xTabWidgetStyleSheet.arg(24).arg(300).arg(36));
	ProgSettings_tabWidget->setIconSize(QSize(32,32));
	ProgSettings_tabWidget->setUsesScrollButtons(false);

	//------------------------------------------------------------------------------------------------------------
	//Закладка "Общие настройки"
	GeneralSettings_Tab = new QWidget();
	ProgSettings_tabWidget->addTab(GeneralSettings_Tab, QIcon(":/images/advancedsettings.png"), "");

	//Настройки портов
	QVBoxLayout *GeneralSettings_Layout = new QVBoxLayout();

	AutomaticStart_CheckBox = new QCheckBox();
	//GeneralSettings_Layout->addWidget(AutomaticStart_CheckBox, 0, Qt::AlignHCenter | Qt::AlignVCenter);

	SetDateTime_GroupBox=new QGroupBox();
	GeneralSettings_Layout->addWidget(SetDateTime_GroupBox, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	QVBoxLayout *SetDateTime_Layout = new QVBoxLayout();
	SetDateTime_GroupBox->setLayout(SetDateTime_Layout);
	SetDateTime=new QDateTimeEdit();
	SetDateTime->setStyleSheet("font: 20pt;");
	SetDateTime_Layout->addWidget(SetDateTime);

	//Присвоение слоя закладке
	GeneralSettings_Tab->setLayout(GeneralSettings_Layout);

	//------------------------------------------------------------------------------------------------------------
	//Закладка "настройки портов серверов"
#if defined(Q_OS_WIN)
	PortSettings_Tab = new QWidget();
	ProgSettings_tabWidget->addTab(PortSettings_Tab, QIcon(":/images/serial_port.png"), "");
	//Настройки портов
	QHBoxLayout *PortSettings_Layout = new QHBoxLayout();
	PortSettings_GroupBox[0]=new QGroupBox();
	PortSettings_Layout->addWidget(PortSettings_GroupBox[0], 0, Qt::AlignLeft);
	PortSettings_GroupBox[1]=new QGroupBox();
	PortSettings_Layout->addWidget(PortSettings_GroupBox[1], 0, Qt::AlignRight);
#else
	PortSettings_GroupBox[0]=NULL;
	PortSettings_GroupBox[1]=NULL;
#endif

	((MainWindow *)obj_MainWindow)->OktServExt[0]=new TOktServExt(PortSettings_GroupBox[0]
#ifdef __linux__
	#ifndef __i386__
		, "ttySP1"
	#else
		, "ttyUSB0"
	#endif
#endif
		, (((MainWindow *)obj_MainWindow)->Reg1_Label)[0]
		);

	((MainWindow *)obj_MainWindow)->OktServExt[1]=new TOktServExt(PortSettings_GroupBox[1]
#ifdef __linux__
	#ifndef __i386__
		, "ttySP2"
	#else
		, "ttyS0"
	#endif
#endif
		, (((MainWindow *)obj_MainWindow)->Reg1_Label)[1]
		);

	((MainWindow *)obj_MainWindow)->OktServExt[0]->okt_serv_other=((MainWindow *)obj_MainWindow)->OktServExt[1];
	((MainWindow *)obj_MainWindow)->OktServExt[1]->okt_serv_other=((MainWindow *)obj_MainWindow)->OktServExt[0];

#if defined(Q_OS_WIN)
	//PortSettings_Layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	//Кнопка "применить"
	QHBoxLayout *PortSettingsButtons_Layout = new QHBoxLayout();
	PortSettingsButtons_Layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	PortsSettingsApply_Button = new xButton(GenBut, QIcon(":/images/apply.png"), 32, Qt::ToolButtonTextBesideIcon);
	PortSettingsButtons_Layout->addWidget(PortsSettingsApply_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(PortsSettingsApply_Button, SIGNAL(clicked()), this, SLOT(PortsSettingsApply()));

	//Слой вмещающий в себя Настройки портов и кнопку Применить
	QVBoxLayout *PortSettingsMain_Layout = new QVBoxLayout();
	PortSettingsMain_Layout->addLayout(PortSettings_Layout);
	PortSettingsMain_Layout->addLayout(PortSettingsButtons_Layout);

	//Присвоение слоя закладке
	PortSettings_Tab->setLayout(PortSettingsMain_Layout);
#endif
	//Кнопка "закрыть настройки программы"
	QHBoxLayout *ProgSettingsButtons_Layout = new QHBoxLayout();
	ProgSettingsButtons_Layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	Save_Button = new xButton(GenBut, QIcon(":/images/document_save.png"), 32, Qt::ToolButtonTextBesideIcon);
	ProgSettingsButtons_Layout->addWidget(Save_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(Save_Button, SIGNAL(clicked()), this, SLOT(Save()));
	Close_Button = new xButton(GenBut, QIcon(":/images/button_cancel.png"), 32, Qt::ToolButtonTextBesideIcon);
	ProgSettingsButtons_Layout->addWidget(Close_Button, 0, Qt::AlignRight | Qt::AlignBottom);
	connect(Close_Button, SIGNAL(clicked()), this, SLOT(Close()));
	ProgSettings_Layout->addLayout(ProgSettingsButtons_Layout);
	Retranslate();
	}

void TProgSettings::Retranslate()
	{
	setWindowTitle("НАСТРОЙКИ ПРОГРАММЫ");
	ProgSettings_tabWidget->setTabText(0, tr("ОБЩИЕ НАСТРОЙКИ"));
#if defined(Q_OS_WIN)
	ProgSettings_tabWidget->setTabText(1, tr("НАСТРОЙКИ ПОРТОВ"));
	PortsSettingsApply_Button->setText(tr("ПРИМЕНИТЬ"));
#endif
	AutomaticStart_CheckBox->setText(tr("ВКЛЮЧИТЬ СЕРВЕР ПРИ СТАРТЕ"));
	SetDateTime_GroupBox->setTitle(tr("УСТАНОВКА ВРЕМЕНИ"));
	Save_Button->setText(tr("СОХРАНИТЬ"));
	Close_Button->setText(tr("ЗАКРЫТЬ"));
	}

void TProgSettings::show()
	{
	PresetVal=QDateTime::currentDateTime();
	SetDateTime->setDateTime(PresetVal);
	QMainWindow::show();
	}

#if defined(Q_OS_WIN)
void TProgSettings::PortsSettingsApply()
	{
	((MainWindow *)obj_MainWindow)->OktServExt[0]->PortSettingsApply();
	((MainWindow *)obj_MainWindow)->OktServExt[1]->PortSettingsApply();
	}

#endif

void TProgSettings::Close()
	{
	close();
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
#if defined(Q_OS_WIN)
				for(int i=0;i<2;i++)
					{
					if(xml.name() == tr("OktServ%1").arg(i))
						{
						LoadCommSettings(xml, i);
						}
					}
#endif
				if(xml.name() == tr("GeneralSettings"))
					{
					LoadGeneralSettings(xml);
					}
				}
			}

		IniFile.close();
		}
#if defined(Q_OS_WIN)
	for(int i=0;i<2;i++) ((MainWindow *)obj_MainWindow)->OktServExt[i]->PrintPortsParameters();
#endif
	AutomaticStart_CheckBox->setChecked(GeneralSettings.AutomaticStart);
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
#if defined(Q_OS_WIN)
		for(int i=0;i<2;i++) SaveCommSettings(xml, i);
#endif
		SaveGeneralSettings(xml);
		xml.writeEndElement();

		IniFile.close();
		}
	if(PresetVal!=SetDateTime->dateTime())
		{
		qDebug() << tr("Datetime updated...");
		PresetVal=SetDateTime->dateTime();
#ifdef __linux__
		SetSystemClock(PresetVal);
#endif
		}
	}

void TProgSettings::LoadGeneralSettings(QXmlStreamReader &xml)
	{
	QXmlStreamAttributes attributes = xml.attributes();

	if(attributes.hasAttribute(tr("AutomaticStart")))
		GeneralSettings.AutomaticStart = attributes.value(tr("AutomaticStart")).toString().toInt() > 0 ? true : false;
	}

void TProgSettings::SaveGeneralSettings(QXmlStreamWriter &xml)
	{
	GeneralSettings.AutomaticStart = AutomaticStart_CheckBox->checkState();

	xml.writeStartElement(tr("GeneralSettings"));
	xml.writeAttribute(tr("AutomaticStart"), QString().setNum(GeneralSettings.AutomaticStart));
	xml.writeEndElement();
	}

#if defined(Q_OS_WIN)
void TProgSettings::LoadCommSettings(QXmlStreamReader &xml, int server_index)
	{
	TCommPortSettings *settings=&(((MainWindow *)obj_MainWindow)->OktServExt[server_index]->Settings);
	QXmlStreamAttributes attributes = xml.attributes();

	if(attributes.hasAttribute(tr("CommPortIndex")))
		settings->CommPort_index=attributes.value(tr("CommPortIndex")).toString().toInt();
	}

void TProgSettings::SaveCommSettings(QXmlStreamWriter &xml, int server_index)
	{
	TCommPortSettings *settings=&(((MainWindow *)obj_MainWindow)->OktServExt[server_index]->Settings);
	QString port_name = TOktServ::CommPortSettingsTexts.CommPort[settings->CommPort_index];
	QString key_prefix = QString(tr("OktServ%1")).arg(server_index);

	PortsSettingsApply();

	xml.writeStartElement(key_prefix);
	xml.writeAttribute(tr("CommPortName"), port_name);
	xml.writeEndElement();
	}
#endif
