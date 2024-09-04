QT       += core gui serialport
QT +=  printsupport

QMAKE_CFLAGS += -Wa,-mbig-obj
QMAKE_CXXFLAGS += -Wa,-mbig-obj
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mydial.cpp

HEADERS += \
    mainwindow.h \
    mydial.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/muulu/ -lqcustomplot2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/muulu/ -lqcustomplotd2

INCLUDEPATH += $$PWD/muulu
DEPENDPATH += $$PWD/muulu
