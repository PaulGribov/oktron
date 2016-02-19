lessThan(QT_MAJOR_VERSION, 5) {
QT += core gui
CONFIG += serialport
} else {
QT += widgets serialport
}


SOURCES += main.cpp mainwindow.cpp \
    work.cpp \
    OscService.cpp \
    toktserv.cpp \
    toktservregsetup.cpp \
    xspinbox.cpp \
    tprogsettings.cpp \
    teventslog.cpp \
    toktservext.cpp
HEADERS += mainwindow.h \
    work.h \
    OscService.h \
    toktserv.h \
    toktservregsetup.h \
    xspinbox.h \
    tprogsettings.h \
    teventslog.h \
    toktservext.h
FORMS += mainwindow.ui

RESOURCES += \
    crvd.qrc
