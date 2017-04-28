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
#include "tmainmenu.h"
#include "tgeneralmeasview.h"


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
		TProgSettings *ProgSettings;
		TEventsLog *EventsLog;
		TMainMenu *MainMenu;
		TGeneralMeasView *GeneralMeasView;
		QTimer *SystemTime_QTimer;
		QLabel *DestDiskState_Label;
		bool DestDiskState;
		QMainWindow *Baner_MainWindow;
		QLabel *Baner0_Label;
		QLabel *Baner1_Label;
		QLabel *Baner2_Label;
		int watchdog_fd;

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

		xTabWidget *MainWindow_TabWidget;
		xButton *MainMenu_Button;
		void RegViewSetEnabled(bool, int i);
		bool PrintDataEnabled[2];
		QLabel *SystemTime0_Label, *SystemTime1_Label;
		QLabel *Reg0_Label[2], *Reg1_Label[2];
		QLabel *DestDiskState0_Label, *DestDiskState1_Label;
		static int IdleTimeout;
		bool eventFilter(QObject *obj, QEvent *e);


	public slots:
		void DataProcess(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool);
		void SystemTimeTick();
		void KeysPoll();
		void ChildWindowClose(bool CloseAnyway);
		void CheckGrowKeys(unsigned short mask);
		void DataSender();
		void Retranslate();
		void ChangesCopyBetweenRegsStateChanged();
		void UpdateAll();
		void RegSetup_GetResult(int, TOktServ *);

	private:
		Ui::MainWindow *ui;
		TOktServExt *cur_okt_serv;
		int ErrorsCount[2];
		unsigned short KeysState;
#define OKT_KEYS_NUM	16
		int KeyTimeCnt[OKT_KEYS_NUM];
		QTimer *KeysPoll_QTimer;
		QTimer *DataSender_QTimer;
		qint64 time_scale;
		bool ChangesCopyBetweenRegsState;

	};

#ifndef __i386__
	#define WDT_RST_MAINWINDOW() { if(watchdog_fd) { ioctl(watchdog_fd, WDIOC_KEEPALIVE, 0); } }
#else
	#define WDT_RST_MAINWINDOW() { }
#endif


#endif // MAINWINDOW_H
