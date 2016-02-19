#ifndef TPROGSETTINGS_H
#define TPROGSETTINGS_H

#include <QtCore/QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMainWindow>
#include <QFrame>
#include <QLayout>
#include "toktservext.h"
#include <QDateTimeEdit>
#include <QCheckBox>

class TProgSettings : public QMainWindow
	{
		Q_OBJECT
	public:
		explicit TProgSettings(QWidget *parent=NULL, TOktServExt **pOktServExt0=NULL, TOktServExt **pOktServExt1=NULL);

		QDateTimeEdit *SetDateTime;
		QGroupBox *PortSettings_GroupBox[2], *GeneralSettings_GroupBox, *SetDateTime_GroupBox;
		TOktServ *OktServ[2];
		xTabWidget *ProgSettings_tabWidget;
		QWidget *PortSettings_Tab, *GeneralSettings_Tab;
		QPushButton *PortsSettingsApply_Button;
		QCheckBox *AutomaticStart_CheckBox;
		struct {
			bool AutomaticStart;
			} GeneralSettings;

		void SetOktServers(TOktServ *OktServ0, TOktServ *OktServ1)
			{
			this->OktServ[0]=OktServ0;
			this->OktServ[1]=OktServ1;
			}
		void show();
		QDateTime PresetVal;

	private:
		void SaveCommSettings(QXmlStreamWriter &xml, int server_index);
		void SaveGeneralSettings(QXmlStreamWriter &xml);
		void LoadCommSettings(QXmlStreamReader &xml, int server_index);
		void LoadGeneralSettings(QXmlStreamReader &xml);

	signals:

	public slots:
		void PortsSettingsApply();
		void Close();
		void Save();
		void Load();
		void DateTimeUpdated(QTime &date);

	};

#define INI_FILENAME	"ini.xml"

#endif // TPROGSETTINGS_H
