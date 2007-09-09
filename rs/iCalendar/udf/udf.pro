TEMPLATE = lib

CONFIG += warn_on \
debug \
dll \
MT
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
 release

HEADERS += utils.h \
udf.h \
mailqueue.h \
 globals.h \
 icalmessage.h \
 smtpconfig.h \
 smtpsend.h



contains( MT,1 ){
    DEFINES += MT
    target.path = /opt/firebird/UDF

    LIBS += -lptypes

    OBJECTS_DIR = ./obj

}
!contains( MT,1 ){    target.path = /opt/firebird/UDF

    LIBS += -lptypesn

    OBJECTS_DIR = ./obj

}

INCLUDEPATH += /opt/firebird/include \
../include/ \
../ptypes/include/
LIBS += -L../lib/ \
-L../ptypes/lib/ \
-lgnutls \
-lgsasl \
-lvmime \
-lib_util \
-lfbclient
