TEMPLATE = app

CONFIG += warn_on \
debug
SOURCES += teste_udf.cpp

CONFIG -= release

DESTDIR = .

INCLUDEPATH += ../udf \
/opt/firebird/include
LIBS += -L../lib \
-licalendar
