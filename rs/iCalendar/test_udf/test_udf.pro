TEMPLATE = app

CONFIG += warn_on \
debug
SOURCES += teste_udf.cpp

CONFIG -= release

DESTDIR = .

linux-g++ {
	INCLUDEPATH += /opt/firebird/include
}
macx {
	INCLUDEPATH += /Library/Frameworks/Firebird.framework/Headers
}
	
INCLUDEPATH += ../udf

LIBS += -L../lib \
-licalendar
