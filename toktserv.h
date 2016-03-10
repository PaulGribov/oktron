#ifndef TOKTSERV_H
#define TOKTSERV_H

#include <QtCore/QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QTimer>
#include "OscService.h"
#ifdef __linux__
	#include <string.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <stdio.h>
#endif

typedef struct {
	int CommPort_index;
	int BaudRate_index;
	int DataBits_index;
	int Parity_index;
	int StopBits_index;
	int FlowControl_index;
	} TCommPortSettings;

typedef struct {
	QStringList CommPort;
	QStringList BaudRate;
	QStringList DataBits;
	QStringList Parity;
	QStringList StopBits;
	QStringList FlowControl;
	} TCommPortSettingsTexts;


#define DATA_BUF_SIZE 1024

class TOktServ : public QWidget
	{
		Q_OBJECT
	public:
		explicit TOktServ(QGroupBox *PortSettings_GroupBox = 0, QString name0=tr("основного"));
		unsigned char Crc8Calc(unsigned char *, int);
		int PktCnt;
		bool StartStop(bool);
		void PortSettingsApply();
		void PrintPortsParameters();
		bool StateOn;
		TOktServNativeData ReceivedData;
		int PacketUpdatedFlags;
		TOktServNativeData DataToSend;
		QMutex DataToSendLocker;
		int PacketToSendQuicklyFlags;
		int PacketToSendIndex;
		int PacketID[OKTSERV_PACKETS_NUM];
		int ErrorFlags;
#define OKTSERVERR_TIMEOUT_FLAG			0x01
#define OKTSERVERR_NO_REGULATOR_FLAG		0x02
		TCommPortSettings Settings;
		static TCommPortSettingsTexts CommPortSettingsTexts;
		void DataSender();

	Q_SIGNALS:
		void DataUpdate(TOscDataWithIndic &);
		void Indic(int, int);
		void ErrorCallback();

	signals:

	public slots:
		void ErrorHandler(QSerialPort::SerialPortError error);

	private:
		xComboBox *CommPort_ComboBox;
		xComboBox *BaudRate_ComboBox;
		xComboBox *DataBits_ComboBox;
		xComboBox *Parity_ComboBox;
		xComboBox *StopBits_ComboBox;
		xComboBox *FlowControl_ComboBox;

		int PutPacket(int);

		unsigned char DataBuf[DATA_BUF_SIZE];
		int DataBufIndex;
		int TimeoutCnt;
		int DataSender_Prescale;
#ifdef __linux__
		int CommPortFD;
#else
		QSerialPort *CommPort;
#endif
	};

#define OKTSERV_BOX_MINWIDTH    200


#endif // TOKTSERV_H
