#include <QtGui>
#include <QDir>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QApplication>
#else
	#include <QApplication>
#endif
#include "mainwindow.h"
#include "work.h"
#ifdef __linux__
	#include <sys/mount.h>
	#include <sys/types.h>
	#include <sys/wait.h>
#endif


int main(int argc, char *argv[])
	{
	QApplication app(argc, argv);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

	app.setApplicationName(QObject::tr("ОКТРОН"));

	if(QSerialPortInfo::availablePorts().count()==0)
		{
		qDebug() << "FATAL ERROR: Serial ports not found!\r\n";
		QMessageBox::critical(0, QObject::tr("Ошибка"), QObject::tr("Нет последовательных портов"));
		return 1;
		}

#if defined(__linux__) && (!defined(__i386__))
	if(QDir::setCurrent("/mnt/localdisk"))
		{
		//Создание подкаталога oscs
		QDir oscs("oscs");
		if(!oscs.exists())
			{
			if(!oscs.mkpath("/mnt/localdisk/oscs")) qDebug() << "ERROR!\r\n";
			}
		}
	else
		{
		qDebug() << "FATAL ERROR: localdisk not found!\r\n";
		QMessageBox::critical(0, QObject::tr("Ошибка"), QObject::tr("Не найден раздел для хранения данных"));
		return 1;
		}
#endif
#ifndef __i386__
	/*
	if(mount("/dev/mmcblk0p4", "/mnt/localdisk", "ext3", MS_NOATIME|MS_SYNCHRONOUS, NULL))
		{
		qDebug() << "FATAL ERROR: localdisk not mounted!\r\n";
		QMessageBox::critical(0, QObject::tr("Ошибка"), QObject::tr("Раздел для хранения данных не удаётся монтировать"));
		return 1;
		}
	*/
#endif
	MainWindow mainWindow;

	app.setStyleSheet("QMainWindow {background-color: white}");

	return app.exec();
	}
