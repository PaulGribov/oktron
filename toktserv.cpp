#include "toktserv.h"
#include "mainwindow.h"
#ifndef __i386__
	#include <string.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <stdio.h>
#endif

TCommPortSettingsTexts TOktServ::CommPortSettingsTexts;
static const char SYNC_DATA[] = {0x34, 0x12, 0x00};

TOktServ::TOktServ(QGroupBox *PortSettings_GroupBox, QString name0) : QWidget(PortSettings_GroupBox)
	{
	TimeoutCnt=0;
	ErrorFlags=0;
	StateOn=false;
	PktCnt=0;
	DataSender_Prescale=0;

	CommPort = new QSerialPort();
	QWidget::connect(CommPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(ErrorHandler(QSerialPort::SerialPortError)));
	QWidget::connect(CommPort, SIGNAL(readyRead()), this, SLOT(ReadData()));
	//QWidget::connect(CommPort, SIGNAL(bytesWritten(qint64)), this, SLOT(DataWritten(qint64)));

	//Создание списков настроек порта
	if(CommPortSettingsTexts.CommPort.isEmpty())
		{
		CommPortSettingsTexts.CommPort.clear();
		foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
			{
			CommPortSettingsTexts.CommPort.append(info.portName());
			}

		CommPortSettingsTexts.BaudRate.clear();
		CommPortSettingsTexts.BaudRate.append(QObject::tr("1200"));
		CommPortSettingsTexts.BaudRate.append(QObject::tr("2400"));
		CommPortSettingsTexts.BaudRate.append(QObject::tr("4800"));
		CommPortSettingsTexts.BaudRate.append(QObject::tr("9600"));
		CommPortSettingsTexts.BaudRate.append(QObject::tr("19200"));
		CommPortSettingsTexts.BaudRate.append(QObject::tr("38400"));
		CommPortSettingsTexts.BaudRate.append(QObject::tr("57600"));
		CommPortSettingsTexts.BaudRate.append(QObject::tr("115200"));

		CommPortSettingsTexts.DataBits.clear();
		CommPortSettingsTexts.DataBits.append(QObject::tr("5"));
		CommPortSettingsTexts.DataBits.append(QObject::tr("6"));
		CommPortSettingsTexts.DataBits.append(QObject::tr("7"));
		CommPortSettingsTexts.DataBits.append(QObject::tr("8"));

		CommPortSettingsTexts.Parity.clear();
		CommPortSettingsTexts.Parity.append(QObject::tr("Нет"));
		CommPortSettingsTexts.Parity.append(QObject::tr("Чет"));
		CommPortSettingsTexts.Parity.append(QObject::tr("Нечет"));
		CommPortSettingsTexts.Parity.append(QObject::tr("Всегда 1"));
		CommPortSettingsTexts.Parity.append(QObject::tr("Всегда 0"));

		CommPortSettingsTexts.StopBits.clear();
		CommPortSettingsTexts.StopBits.append(QObject::tr("1"));
#ifdef Q_OS_WIN
		CommPortSettingsTexts.StopBits.append(QObject::tr("1.5"));
#endif
		CommPortSettingsTexts.StopBits.append(QObject::tr("2"));

		CommPortSettingsTexts.FlowControl.clear();
		CommPortSettingsTexts.FlowControl.append(QObject::tr("Нет"));
		CommPortSettingsTexts.FlowControl.append(QObject::tr("RTS/CTS"));
		CommPortSettingsTexts.FlowControl.append(QObject::tr("XON/XOFF"));
		}

	//Настройки по-умолчанию
	Settings.CommPort_index=0;
#ifndef __i386__
	foreach(const QString text, CommPortSettingsTexts.CommPort)
		{
		if(text.indexOf(tr("ttySP0"))!=-1) goto quitchkport_loc;
		Settings.CommPort_index++;
		}
	Settings.CommPort_index--;
quitchkport_loc:
#endif
	Settings.BaudRate_index=7;
	Settings.DataBits_index=3;
	Settings.Parity_index=0;
	Settings.StopBits_index=0;
	Settings.FlowControl_index=0;

	//Создание графических элементов настроек
#define CREATE_COMBOBOX_PARAMETER(a, b, c){\
		QHBoxLayout *a##_Layout = new QHBoxLayout();\
		QLabel *a##_Label = new QLabel();\
		a##_Label->setText(tr(b));\
		a##_ComboBox=new xComboBox();\
		a##_Layout->addWidget(a##_Label, 0, Qt::AlignRight);\
		a##_Layout->addWidget(a##_ComboBox);\
		c##_Layout->addLayout(a##_Layout);}

	PortSettings_GroupBox->setTitle(tr("Параметры порта ")+name0);
	QVBoxLayout *PortSettings_Layout = new QVBoxLayout();
	PortSettings_GroupBox->setLayout(PortSettings_Layout);
	CREATE_COMBOBOX_PARAMETER(CommPort, "Порт:", PortSettings)
	QGroupBox *Settings_GroupBox=new QGroupBox();
	Settings_GroupBox->setTitle(tr("Параметры"));
	QVBoxLayout *Settings_Layout = new QVBoxLayout();
	Settings_GroupBox->setLayout(Settings_Layout);
	PortSettings_Layout->addWidget(Settings_GroupBox);
	CREATE_COMBOBOX_PARAMETER(BaudRate, "Скорость:", Settings)
	CREATE_COMBOBOX_PARAMETER(DataBits, "Бит данных:", Settings)
	CREATE_COMBOBOX_PARAMETER(Parity, "Четность:", Settings)
	CREATE_COMBOBOX_PARAMETER(StopBits, "Стоп бит:", Settings)
	CREATE_COMBOBOX_PARAMETER(FlowControl, "Аппарат. контроль:", Settings)

	PortSettings_GroupBox->parentWidget()->setTabOrder(CommPort_ComboBox, BaudRate_ComboBox);
	PortSettings_GroupBox->parentWidget()->setTabOrder(BaudRate_ComboBox, DataBits_ComboBox);
	PortSettings_GroupBox->parentWidget()->setTabOrder(DataBits_ComboBox, Parity_ComboBox);
	PortSettings_GroupBox->parentWidget()->setTabOrder(Parity_ComboBox, StopBits_ComboBox);
	PortSettings_GroupBox->parentWidget()->setTabOrder(StopBits_ComboBox, FlowControl_ComboBox);

	//Заполнить списки и вывод текущих настроек
	PrintPortsParameters();

	DataSender_QTimer = new QTimer(this);
	connect(DataSender_QTimer , SIGNAL(timeout()), this, SLOT(DataSender()));
	DataSender_QTimer->stop();
	PacketToSendQuicklyFlags=0;
	PacketToSendIndex=0;
	}

//===========================================================================
//Применение установленных пользователем насртроек
//===========================================================================
void TOktServ::PortSettingsApply()
	{
	Settings.CommPort_index=CommPort_ComboBox->currentIndex();
	Settings.BaudRate_index=BaudRate_ComboBox->currentIndex();
	Settings.DataBits_index=DataBits_ComboBox->currentIndex();
	Settings.Parity_index=Parity_ComboBox->currentIndex();
	Settings.StopBits_index=StopBits_ComboBox->currentIndex();
	Settings.FlowControl_index=FlowControl_ComboBox->currentIndex();
	}

//===========================================================================
//Вывод текущих настроек порта на форму
//===========================================================================
void TOktServ::PrintPortsParameters()
	{
	BaudRate_ComboBox->clear();
	BaudRate_ComboBox->addItems(CommPortSettingsTexts.BaudRate);
	BaudRate_ComboBox->setCurrentIndex(Settings.BaudRate_index);
	DataBits_ComboBox->clear();
	DataBits_ComboBox->addItems(CommPortSettingsTexts.DataBits);
	DataBits_ComboBox->setCurrentIndex(Settings.DataBits_index);
	Parity_ComboBox->clear();
	Parity_ComboBox->addItems(CommPortSettingsTexts.Parity);
	Parity_ComboBox->setCurrentIndex(Settings.Parity_index);
	StopBits_ComboBox->clear();
	StopBits_ComboBox->addItems(CommPortSettingsTexts.StopBits);
	StopBits_ComboBox->setCurrentIndex(Settings.StopBits_index);
	FlowControl_ComboBox->clear();
	FlowControl_ComboBox->addItems(CommPortSettingsTexts.FlowControl);
	FlowControl_ComboBox->setCurrentIndex(Settings.FlowControl_index);
	CommPort_ComboBox->clear();
	if(CommPortSettingsTexts.CommPort.count())
		{
		CommPort_ComboBox->addItems(CommPortSettingsTexts.CommPort);
		//Main_GroupBox->setEnabled(true);
		if(Settings.CommPort_index<CommPortSettingsTexts.CommPort.count())
			{
			CommPort_ComboBox->setCurrentIndex(Settings.CommPort_index);
			}
		}
	else
		{
		CommPort_ComboBox->addItem(tr("нет доступных портов"));
		}

	}


//===========================================================================
//CRC8
//===========================================================================
unsigned char TOktServ::Crc8Calc(unsigned char *data, int n)
	{
	//Таблица вычисления контрольной суммы CRC8
	//Полином: 0x31    x^8 + x^5 + x^4 + 1
	const unsigned char Crc8Table[] = {
		0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
		0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
		0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
		0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
		0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
		0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
		0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
		0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
		0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
		0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
		0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
		0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
		0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
		0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
		0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
		0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
		0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
		0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
		0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
		0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
		0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
		0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
		0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
		0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
		0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
		0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
		0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
		0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
		0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
		0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
		0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
		0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC};
	unsigned char ks=0xff;
	for(int i=0; i<n; i++) ks = Crc8Table[ks ^ data[i]];
	return ks;
	}

//===========================================================================
//Отправка данных
//===========================================================================
void TOktServ::DataSender()
	{
	++DataSender_Prescale;

	//ReceivedData.UnstructuredPacket[14]=ReceivedData.UnstructuredPacket[OKTSERV_IOPACKET_INDEX];

	//Отправка данных в осциллограммы каждые 20мс
	//Первые 5 пакетов относятся к данным осциллографирования
	if(((DataSender_Prescale&0x1)==0)&&((PacketUpdatedFlags&0x1D)==0x1D))
		{
		TOscDataWithIndic od={
			{
				ReceivedData.Structured.Packet0,
				},
			ReceivedData.Structured.Packet2,
			ReceivedData.Structured.Packet3,
			ReceivedData.Structured.Packet4,
			ReceivedData.Structured.Packet14,
			};
		DataUpdate(od);
		}

	//Отсылка срочных пакетов
	int m,i;
	for(i=0;i<16;i++)
		{
		QMutexLocker locker(&DataToSendLocker);
		m=1<<i;
		if(PacketToSendQuicklyFlags&m)
			{
			static int j;
			j++;
			PacketToSendQuicklyFlags&=~m;
			PutPacket(i);
			return;
			}
		}

	//Отправка данных по последовательному порту для инициализации обмена - каждые 320мс
	if((DataSender_Prescale&0x1F)==0)
		{
		PutPacket(OKTSERV_DIAGPACKET_INDEX);
#ifndef REGSETUPDBG
		//Счётчик таймаута для приёмника
		if(++TimeoutCnt>10)
			{
			if(!(ErrorFlags&OKTSERVERR_TIMEOUT_FLAG))
				{
				ErrorFlags|=OKTSERVERR_TIMEOUT_FLAG;
				PacketUpdatedFlags=0;
				ErrorCallback();
				}
			}
		else
#endif
			ErrorFlags&=~OKTSERVERR_TIMEOUT_FLAG;

		}
	}

void TOktServ::PutPacket(int PacketIndex)
	{
	QByteArray p(OKTSERV_FULL_PACKETSIZE, 0);

	PacketIndex&=0x0F;

	//Маркер пакета
	p.data()[0]=SYNC_DATA[0];
	p.data()[1]=SYNC_DATA[1];
	//Содержимое пакета
	memcpy(&(p.data()[2]), &(DataToSend.UnstructuredPacket[PacketIndex]), OKTSERV_DATA_PACKETSIZE);
	//Целевой номер пакета
	p.data()[OKTSERV_FULL_PACKETSIZE-2]=(unsigned char)PacketIndex;
	//CRC8
	p.data()[OKTSERV_FULL_PACKETSIZE-1]=(unsigned char)Crc8Calc((unsigned char *)&(p.data()[0]), OKTSERV_FULL_PACKETSIZE-1);

	CommPort->write(p);
	}


//===========================================================================
//Обработка полученных данных
//===========================================================================
void TOktServ::ReadData()
	{
	qint64 BytesRead=CommPort->read((char *)&DataBuf[DataBufIndex], DATA_BUF_SIZE-1-DataBufIndex);
	if(BytesRead)
		{
		DataBufIndex+=BytesRead;
		}
	else
		{
		StartStop(false);
		}

	/*
	((MainWindow *)pMW)->TextOut(tr("DataBufIndex=%1").arg(DataBufIndex));
	for(int i=0;i<DataBufIndex;i++)
		{
		((MainWindow *)pMW)->TextOut(tr("0x%1").arg(DataBuf[i],0,16));
		}
	*/

	//Выделение все пакетов из текущих данных
	do
		{
		Indic(0, DataBufIndex);

		//Поиск первого пакета
		int SyncDataFound;
		for(int i=0;i<DataBufIndex-1;i++)
			{
			if((DataBuf[i]==0x34)&&(DataBuf[i+1]==0x12))
				{
				SyncDataFound=i;
				goto NextChk_loc;
				}
			}

		DataBufIndex=0;
		break;

NextChk_loc:

		//Есть маркер и объем данный соответствует размеру пакета
		if((DataBufIndex-SyncDataFound)<OKTSERV_FULL_PACKETSIZE)
			{
			break;
			}

		//Указатель на строку начала пакета
		unsigned char *p=&DataBuf[SyncDataFound];

		//Содержимое пакета верно?
		if(Crc8Calc(p, OKTSERV_FULL_PACKETSIZE-1)==p[OKTSERV_FULL_PACKETSIZE-1])
			{
			TimeoutCnt=0;

			//Целевой номер пакета
			int PacketIndex=p[OKTSERV_FULL_PACKETSIZE-2]&0x0F;

			//ID пакета
			PacketID[PacketIndex]=(p[OKTSERV_FULL_PACKETSIZE-2]>>4)&0x0F;

			PktCnt++;

			//Если данные не обновлены регулятором, то его нет...
			if((PacketIndex==0)&&(p[2]==0x55))
				{
				for(int i=1;i<OKTSERV_DATA_PACKETSIZE;i++)
					{
					if(p[i+2]!=0x55) goto thisisdata_loc;
					}
				if(!(ErrorFlags&OKTSERVERR_NO_REGULATOR_FLAG))
					{
					//ErrorFlags|=OKTSERVERR_NO_REGULATOR_FLAG;
					//ErrorCallback();
					}
				}
			else
				{
thisisdata_loc:
				ErrorFlags&=~OKTSERVERR_NO_REGULATOR_FLAG;
				memcpy(&(ReceivedData.UnstructuredPacket[PacketIndex]), (p+2), OKTSERV_DATA_PACKETSIZE);
				PacketUpdatedFlags|=1<<PacketIndex;
				}
			}

		for(int i=SyncDataFound+OKTSERV_FULL_PACKETSIZE;i<DataBufIndex;i++)
			{
			DataBuf[i-SyncDataFound-OKTSERV_FULL_PACKETSIZE]=DataBuf[i];
			}
		DataBufIndex-=SyncDataFound+OKTSERV_FULL_PACKETSIZE;
		} while(1);
	}


void TOktServ::ErrorHandler(QSerialPort::SerialPortError error)
	{
	if(error == QSerialPort::ResourceError)
		{
#ifndef __i386__
		QMessageBox::critical(this, tr("Ошибка"), CommPort->errorString());
		StartStop(false);
#endif
		}
	}



//===========================================================================
//Старт-стоп сервера
//===========================================================================
bool TOktServ::StartStop(bool StateOnIn)
	{
	if(StateOnIn&&(StateOn!=StateOnIn))
		{
		//QString p=CommPortSettingsTexts.CommPort.at(Settings.CommPort_index);
		//QMessageBox::information(this, QObject::tr("Port"), p);
		DataBufIndex=0;
		{
		QMutexLocker lock(&DataToSendLocker);
		memset(&(DataToSend.UnstructuredPacket[0]), 0x55, OKTSERV_DATA_PACKETSIZE);
		PacketToSendQuicklyFlags=0x01;
		}
		PacketUpdatedFlags=0;
		ErrorFlags=0;
		TimeoutCnt=0;
		DataSender_Prescale=0;

#ifndef __i386__
		//Предварительное открытие порта
		struct termios tio;
		int tty_fd;
		int n;
		char tmp;

		memset(&tio,0,sizeof(tio));
		tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
		tio.c_cc[VMIN]=1;
		tio.c_cc[VTIME]=5;

		QString p="/dev/"+CommPortSettingsTexts.CommPort.at(Settings.CommPort_index);
		tty_fd=::open(p.toAscii(), O_RDWR | O_NONBLOCK);
		if(tty_fd)
			{
			cfsetospeed(&tio,B115200);            // 115200 baud
			cfsetispeed(&tio,B115200);            // 115200 baud
			tcsetattr(tty_fd,TCSANOW,&tio);
			tcflush(tty_fd, TCIOFLUSH);
			do
				{
				n=read(tty_fd, &tmp, 1);
				} while(n>0);
			::close(tty_fd);
			}
		else
			{
			QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Ошибка предварительного открытия порта"));
			StateOnIn=false;
			goto OpenPortQuit_loc;
			}
#endif
		CommPort->setPortName(CommPortSettingsTexts.CommPort.at(Settings.CommPort_index));
		CommPort->setBaudRate(CommPortSettingsTexts.BaudRate.at(Settings.BaudRate_index).toInt());
		CommPort->setDataBits(static_cast<QSerialPort::DataBits>(CommPortSettingsTexts.DataBits.at(Settings.DataBits_index).toInt()));
		CommPort->setParity(static_cast<QSerialPort::Parity>(CommPortSettingsTexts.Parity.at(Settings.Parity_index).toInt()));
		CommPort->setStopBits(static_cast<QSerialPort::StopBits>(CommPortSettingsTexts.StopBits.at(Settings.StopBits_index).toInt()));
		CommPort->setFlowControl(static_cast<QSerialPort::FlowControl>(CommPortSettingsTexts.FlowControl.at(Settings.FlowControl_index).toInt()));
		if(!CommPort->open(QIODevice::ReadWrite))
			{
			QMessageBox::critical(this, QObject::tr("Ошибка при открытии порта"), CommPort->errorString());
			StateOnIn=false;
			}
		}
	else
		{
		CommPort->close();
		}

#ifndef __i386__
OpenPortQuit_loc:
#endif
	if(StateOnIn) DataSender_QTimer->start(10);
	else DataSender_QTimer->stop();
	return StateOn=StateOnIn;
	}
