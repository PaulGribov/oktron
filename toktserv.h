#ifndef TOKTSERV_H
#define TOKTSERV_H

#include <QtCore/QCoreApplication>
#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QTimer>
#include "OscService.h"
#if defined(Q_OS_WIN)
	#include <QtSerialPort/QSerialPort>
	#include <QtSerialPort/QSerialPortInfo>
#else
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
	} TCommPortSettings;

typedef struct {
	QStringList CommPort;
	} TCommPortSettingsTexts;


#define DATA_BUF_SIZE 1024

class TOktServ : public QWidget
	{
		Q_OBJECT
	public:
		explicit TOktServ(QGroupBox *PortSettings_GroupBox = 0
#if !defined(Q_OS_WIN)
			, const char *port_name="ttySP1"
#endif
			);
		unsigned char Crc8Calc(unsigned char *, int);
		int PktCnt;
		bool StartStop(bool);
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
		void DataSender();

#if defined(Q_OS_WIN)
		void PortSettingsApply();
		void PrintPortsParameters();
		TCommPortSettings Settings;
		static TCommPortSettingsTexts CommPortSettingsTexts;

	public slots:
		void ErrorHandler(QSerialPort::SerialPortError error);
#endif

	Q_SIGNALS:
		void DataUpdate(TOscDataWithIndic &);
		void Indic(int, int);
		void ErrorCallback();


	private:
		xComboBox *CommPort_ComboBox;
		xComboBox *BaudRate_ComboBox;
		xComboBox *DataBits_ComboBox;
		xComboBox *Parity_ComboBox;
		xComboBox *StopBits_ComboBox;
		xComboBox *FlowControl_ComboBox;
		QString PortName;
		int PutPacket(int);

		unsigned char DataBuf[DATA_BUF_SIZE];
		int DataBufIndex;
		int TimeoutCnt;
		int DataSender_Prescale;
#if defined(Q_OS_WIN)
		QSerialPort *CommPort;
#else
		int CommPortFD;
#endif
	};

#define OKTSERV_BOX_MINWIDTH    200


#endif // TOKTSERV_H
