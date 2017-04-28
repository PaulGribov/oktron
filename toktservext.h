#ifndef TOKTSERVEXT_H
#define TOKTSERVEXT_H

#include "toktservregsetup.h"

class TOktServExt : public TOktServ
	{
		Q_OBJECT
	public:
		TOktServExt(QGroupBox *PortSettings_GroupBox = 0
#if !defined(Q_OS_WIN)
			, const char *port_name="ttySP1"
#endif
			, QLabel *RegState_Label=NULL
			);

		QMainWindow *OscList_MainWindow;
		QMainWindow *ParametersView_MainWindow;
		TOscService *OscService;
		static QPixmap *wait_qp[4];

		static QMainWindow *RegSetup_MainWindow;
		static xTabWidget *RegSetup_tabWidget;
		QWidget *RegSetup_Tab;

		static QMainWindow *GetBlocksID_MainWindow;
		static xTabWidget *GetBlocksID_tabWidget;
		QWidget *GetBlocksID_Tab;

		TOktServRegSetup *RegSetup_GetBlocksID;

		QString Name;
		bool Master;//Ведущий
		void StartStop(bool);
		void Retranslate();
		QLabel *RegState_Label;
		void *okt_serv_other;
		bool FirstDataAsquired;

	Q_SIGNALS:
		void DataProcessLocal(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool);

	private slots:
		void GetDataOktServ(TOscDataWithIndic &od);
		void CatchError();

	public slots:
		void RegSetup_CloseSlotExternal()
			{
			RegSetup_MainWindow->close();
			}
		void GetBlocksID_CloseSlotExternal()
			{
			GetBlocksID_MainWindow->close();
			}

	private:
		TOscDataWithIndic previous_od;
		int PktCntPrescale;
	};


#endif // TOKTSERVEXT_H
