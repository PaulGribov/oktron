#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QMainWindow>
#else
	#include <QtGui/QMainWindow>
#endif
#include <QStringListModel>
#include <QStandardItemModel>

#include "tprogsettings.h"
#include "tregsetup.h"
#include "teventslog.h"
#include "toktservext.h"
#include "xWidgets.h"


namespace Ui {
    class MainWindow;
}



class MainWindow : public QMainWindow
	{
	Q_OBJECT
	public:

		explicit MainWindow(QWidget *parent = 0);
		virtual ~MainWindow();
		void showExpanded();

		TOktServExt *OktServExt[2];
		QLabel *SystemTime_Label;
		TProgSettings *ProgSettings;
		TEventsLog *EventsLog;
		QTimer *SystemTime_QTimer;
		QLabel *DestDiskState_Label;
		bool DestDiskState;

		void Connect_Disconnect(bool state);
		void PrintEvent(TEventExt *);

#define OKT_KEY_UP_MASK			0x0001
#define OKT_KEY_ENTER_MASK		0x0002
#define OKT_KEY_DOWN_MASK		0x0004
#define OKT_KEY_MORE_MASK		0x0008
#define OKT_KEY_LEFT_MASK		0x0010
#define OKT_KEY_LESS_MASK		0x0020
#define OKT_KEY_RIGHT_MASK		0x0040
#define OKT_KEY_UNLOCKPROTECT_MASK	0x0080
#define OKT_KEY_AUTOMANUAL_MASK		0x0100
#define OKT_KEY_STOP_MASK		0x0200
#define OKT_KEY_TRY_MASK		0x0400
#define OKT_KEY_ESC_MASK		0x0800



	public slots:
		void DataProcess(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool);

		void SystemTimeTick();
		void KeysPoll();
		void CheckGrowKeys(unsigned short mask);
		void DataSender();
		void MenuCreate();
		void Retranslate();

		void ParsOfReserv_Button_OnClick();
		void ParsOfBase_Button_OnClick();
		void EventsLog_Button_OnClick();
		void MakeOsc_Button_OnClick();
		void RegsSetup_Button_OnClick();
		void ProgSettings_Button_OnClick();
		void OscListOfBase_Button_OnClick();
		void OscListOfReserv_Button_OnClick();
		void OktServOnOff_Button_OnClick();
		void GetBlocksID_Button_OnClick();

	private:
		Ui::MainWindow *ui;

		xButton *ParsOfReserv_Button;
		xButton *ParsOfBase_Button;
		xButton *EventsLog_Button;
		xButton *RegsSetup_Button;
		xButton *GetBlocksID_Button;
		xButton *MakeOsc_Button;
		xButton *ProgSettings_Button;
		/*
		void OscListOfBase_Button_OnClick();
		void OscListOfReserv_Button_OnClick();
		void OktServOnOff_Button_OnClick();
		*/


		unsigned short KeysState;
#define OKT_KEYS_NUM	16
		int KeyTimeCnt[OKT_KEYS_NUM];
		QTimer *KeysPoll_QTimer;
		QTimer *DataSender_QTimer;



	};

#endif // MAINWINDOW_H
