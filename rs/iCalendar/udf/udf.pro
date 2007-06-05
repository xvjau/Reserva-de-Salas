TEMPLATE = lib

CONFIG += warn_on \
debug \
dll \
MT
SOURCES += udf.cpp \
utils.cpp \
smtp_config.cpp \
smtp.cpp \
 mailqueue.cpp
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
smtp_config.h \
udf.h \
smtp.h \
 mailqueue.h

MT = 1


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
INCLUDEPATH += ../ptypes/include/

LIBS += -L../ptypes/lib/ \
-lfbclient \
-lesmtp \
-lib_util
