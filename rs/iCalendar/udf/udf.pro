TEMPLATE = lib

CONFIG += warn_on \
debug \
dll
SOURCES += udf.cpp

DESTDIR = ../lib

OBJECTS_DIR = ./obj

linux-g++{
    INCLUDEPATH += /usr/include/ \
			/opt/firebird/include

    LIBS += -L/opt/firebird/lib/
}
TARGET = icalendar

INSTALLS += target

target.path = /opt/firebird/UDF

CONFIG -= qt
INCLUDEPATH += ../ptypes/include/

LIBS += -L../ptypes/lib/ \
-lfbclient \
-lesmtp \
-lib_util \
-lptypes
