TEMPLATE = lib

CONFIG += warn_on \
dll \
 build_all \
 debug
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
 thread \
 release

HEADERS += utils.h \
udf.h \
mailqueue.h \
 globals.h \
 icalmessage.h \
 smtpconfig.h \
 smtpsend.h

DEFINES += DEBUG

INCLUDEPATH += /opt/firebird/include \
../include/
LIBS += -L../lib/ \
-lgsasl \
-lgnutls \
-lvmime \
-lfbclient
