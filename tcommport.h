#ifndef TCOMMPORT_H
#define TCOMMPORT_H

#include <QRunnable>

#ifdef __MINGW32__
	#include <windows.h>
#endif
#ifdef __linux
	#include <sys/types.h>
	#include <fcntl.h>
	#include <termios.h>
#endif
#include <stdio.h>


class TCommPort : public QRunnable
	{
		Q_OBJECT
	public:
		explicit TCommPort(QObject *parent = 0);


	signals:


	public slots:

	};

#endif // TCOMMPORT_H
