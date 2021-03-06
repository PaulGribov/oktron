#ifndef TPROGSETTINGS_H
#define TPROGSETTINGS_H

#include <QtCore/QCoreApplication>
#include <QMainWindow>
#include <QFrame>
#include <QLayout>
#include "toktservext.h"
#include <QDateTimeEdit>
#include <QCheckBox>
#if defined(Q_OS_WIN)
	#include <QtSerialPort/QSerialPort>
	#include <QtSerialPort/QSerialPortInfo>
#endif

class TProgSettings : public QMainWindow
	{
		Q_OBJECT
	public:
		explicit TProgSettings(QWidget *obj_MainWindow);

		QDateTimeEdit *SetDateTime;
		QGroupBox *PortSettings_GroupBox[2], *GeneralSettings_GroupBox, *SetDateTime_GroupBox;
		xTabWidget *ProgSettings_tabWidget;
		QWidget *PortSettings_Tab, *GeneralSettings_Tab;
		xButton *PortsSettingsApply_Button;
		QCheckBox *AutomaticStart_CheckBox;
		struct {
			bool AutomaticStart;
			} GeneralSettings;

		void show();
		QDateTime PresetVal;
		xButton *Save_Button;
		xButton *Close_Button;
		void Retranslate();
		QWidget *obj_MainWindow;
		xButton *TSC_Button;

	private:
		void SaveCommSettings(QXmlStreamWriter &xml, int server_index);
		void SaveGeneralSettings(QXmlStreamWriter &xml);
		void LoadCommSettings(QXmlStreamReader &xml, int server_index);
		void LoadGeneralSettings(QXmlStreamReader &xml);

	signals:

	public slots:
#if defined(Q_OS_WIN)
		void PortsSettingsApply();
#endif
		void Close();
		void Save();
		void Load();
		void TSC_Exe();
	};

#define INI_FILENAME	"ini.xml"

#endif // TPROGSETTINGS_H
