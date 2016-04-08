lessThan(QT_MAJOR_VERSION, 5) {
	QT += core gui
	win32 {
	CONFIG += serialport
	}
} else {
	win32 {
	QT += widgets serialport
	}
	unix {
	QT += widgets
	}
}


SOURCES += main.cpp mainwindow.cpp \
    work.cpp \
    OscService.cpp \
    toktserv.cpp \
    toktservregsetup.cpp \
    xspinbox.cpp \
    tprogsettings.cpp \
    teventslog.cpp \
    toktservext.cpp \
    tgeneralmeasview.cpp \
    xWidgets.cpp
HEADERS += mainwindow.h \
    work.h \
    OscService.h \
    toktserv.h \
    toktservregsetup.h \
    xspinbox.h \
    tprogsettings.h \
    teventslog.h \
    toktservext.h \
    tgeneralmeasview.h \
    xWidgets.h
FORMS += mainwindow.ui

RESOURCES += \
    crvd.qrc
