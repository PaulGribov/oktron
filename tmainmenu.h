#ifndef TMAINMENU_H
#define TMAINMENU_H

#include <QMainWindow>
#include <QtGui>
#include <QWidget>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QPushButton>
#endif
#include "xWidgets.h"
#include "toktservext.h"


class TMainMenu : public QMainWindow
	{
		Q_OBJECT
	public:
		explicit TMainMenu(QWidget *obj_MainWindow);

		QWidget *obj_MainWindow;
		TOktServExt **OktServExt;
		xButton *ParsOfReg_Button[2];
		xButton *EventsLog_Button;
		xButton *RegsSetup_Button;
		xButton *GetBlocksID_Button;
		xButton *MakeOsc_Button;
		xButton *ProgSettings_Button;
		xButton *OscList_Button[2];

		/*
		void OscListOfBase_Button_OnClick();
		void OscListOfReserv_Button_OnClick();
		*/


	signals:


	public slots:
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
		void GetBlocksID_Button_OnClick();

	};

#endif // TMAINMENU_H
