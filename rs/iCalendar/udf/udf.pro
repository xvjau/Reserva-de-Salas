TEMPLATE = lib

CONFIG += warn_on \
qt \
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

QT += core \
network

INCLUDEPATH += ../ptypes/include/

LIBS += -L../ptypes/lib/ \
-lesmtp \
-lib_util \
-lptypesn
