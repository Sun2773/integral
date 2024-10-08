QT       += core gui serialbus virtualkeyboard

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib\'"

INCLUDEPATH += \
    interface \
    drive

SOURCES += \
    drive/canbus.cpp \
    drive/module.cpp \
    interface/interface_main.cpp \
    interface/interface_module.cpp \
    interface/interface_setting.cpp \
    main.cpp \
    interface/mainwindow.cpp \
    drive/beep.cpp \
    drive/modbus_rtu_can_port.c \
    drive/slave.cpp


HEADERS += \
    drive/canbus.h \
    drive/module.h \
    interface/interface_main.h \
    interface/interface_module.h \
    interface/interface_setting.h \
    interface/mainwindow.h \
    drive/beep.h \
    drive/modbus_rtu_can_port.h \
    drive/slave.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../lib/ -lmodbus

INCLUDEPATH += $$PWD/../include/modbus
DEPENDPATH += $$PWD/../include

RESOURCES += \
    res.qrc

FORMS +=


