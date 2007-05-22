TEMPLATE = app

CONFIG += warn_on \
debug
SOURCES += teste_udf.cpp

INCLUDEPATH += ../udf \
../ptypes/include/
LIBS += /home/gianni/Projetos/rs/iCalendar/ptypes/lib/libptypes.a

