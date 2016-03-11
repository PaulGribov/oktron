#ifndef TOKTSERVEXT_H
#define TOKTSERVEXT_H

#include "toktservregsetup.h"

class TOktServExt : public TOktServ
	{
		Q_OBJECT
	public:
		TOktServExt(QGroupBox *PortSettings_GroupBox = 0, QString name0=0, QString name1="");

		QMainWindow *OscList_MainWindow;
		QMainWindow *ParametersView_MainWindow;
		TOscService *OscService;
		QLabel *OktServStatus0_Label, *OktServStatus1_Label, *PktCnt_Label, *OktServIndic_Label;
		static QPixmap *wait_qp[4];
		QString Name0, Name1;


		static QMainWindow *RegSetup_MainWindow;
		static xTabWidget *RegSetup_tabWidget;
		QWidget *RegSetup_Tab;

		static QMainWindow *GetBlocksID_MainWindow;
		static xTabWidget *GetBlocksID_tabWidget;
		QWidget *GetBlocksID_Tab;

		TOktServRegSetup *RegSetup_GetBlocksID;
		bool Master;//Ведущий
		bool ForceMaster;
		void StartStop(bool);

	Q_SIGNALS:
		void DataProcessLocal(TOscDataWithIndic &, TOscDataWithIndic &, TOktServExt *, int, bool);

	private slots:
		//Слот обработки данных
		void GetDataOktServ(TOscDataWithIndic &od)
			{
			bool print=false;
			if((++PktCntPrescale&0x0F)==0)
				{
				/*
				if(ForceMaster)
					{
					OktServIndic_Label->setPixmap(QPixmap(tr(":/images/warning_24.png")));
					}
				else
					{
					OktServIndic_Label->setPixmap(*wait_qp[(PktCntPrescale>>3) & 0x03]);
					}
				*/
				OktServIndic_Label->setPixmap(*wait_qp[(PktCntPrescale>>3) & 0x03]);
				OktServIndic_Label->clear();
				//PktCnt_Label->setText(tr("Пакеты: %1").arg(PktCnt));
				PktCnt_Label->clear();
				print=true;
				}
			DataProcessLocal(od, previous_od, this, 0, print);
			}

		void CatchError()
			{
			if(ErrorFlags & OKTSERVERR_TIMEOUT_FLAG)
				{
				PktCnt_Label->setText(tr("Таймаут"));
				}
			else if(ErrorFlags & OKTSERVERR_NO_REGULATOR_FLAG)
				{
				PktCnt_Label->setText(tr("Нет регулятора"));
				}
			else
				{
				PktCnt_Label->setText(QString("ErrFlags: 0x%1").arg(ErrorFlags, 4, 16, QLatin1Char('0')).toUpper());
				}

			OktServIndic_Label->setPixmap(QPixmap(tr(":/images/block_24.png")));
			DataProcessLocal(previous_od, previous_od, this, ErrorFlags, true);
			}

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
