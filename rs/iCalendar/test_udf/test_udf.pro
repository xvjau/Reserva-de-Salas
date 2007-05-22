TEMPLATE = app

CONFIG += warn_on \
debug
SOURCES += teste_udf.cpp

INCLUDEPATH += ../udf \
../ptypes/include/
LIBS += -L/opt/firebird/UDF \
-licalendar
