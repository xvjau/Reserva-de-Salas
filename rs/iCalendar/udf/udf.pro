TEMPLATE = lib

CONFIG += warn_on \
dll \
 build_all
SOURCES += udf.cpp \
utils.cpp \
mailqueue.cpp \
 globals.cpp \
 icalmessage.cpp \
 smtpconfig.cpp \
 smtpsend.cpp
DESTDIR = ../lib

OBJECTS_DIR = ./obj

linux-g++ {
    INCLUDEPATH += /usr/include/ \
			/opt/firebird/include

    LIBS += -L/opt/firebird/lib/
}
TARGET = icalendar

INSTALLS += target

target.path = /opt/firebird/UDF

CONFIG -= qt \
 stl \
 thread

HEADERS += utils.h \
udf.h \
mailqueue.h \
 globals.h \
 icalmessage.h \
 smtpconfig.h \
 smtpsend.h

DEFINES += DEBUG \
 MT

INCLUDEPATH += /opt/firebird/include \
../include/ \
/usr/local/include
LIBS += -L../lib/ \
-lboost_thread \
-lgsasl \
-lgnutls \
-lvmime \
-lfbclient
