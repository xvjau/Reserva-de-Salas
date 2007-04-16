#!/bin/bash
RS_FILES="Conf/RS.ini rs rs_ptBR.qm rs_en.qm"

FB_FILES="libicuuc.so libicudata.so libicui18n.so libfbclient.so libib_util.so"
 
QT_FILES="libQtCore.so libQtGui.so"

STATIC_QT=0

FB_DIR="/opt/firebird"
QT_DIR=$(qmake -v | tail -1 | cut -c 27-)

if [ "${1}" == "" ]; then
	echo error: Version string missing
else
	echo "Cheking rs dir"
	if [ ! -d ./rs ]; then
		mkdir rs
	fi

	echo "Copy RS binary:"
	cp ../../bin/rs ./rs

	echo "Copy Translations"
	cp ../../bin/*.qm ./rs
	
	echo "Upx"
	upx -9 ./rs/rs
	
	echo "Checking firebird files"
	for FILE in $FB_FILES; do
		if [ ! -f ./rs/$FILE ]; then
			echo "Copying $FB_DIR/lib/$FILE"
			cp $FB_DIR/lib/$FILE ./rs/$FILE
		fi
	done
	
	echo "Checking Qt files"
	for FILE in $QT_FILES; do
		if [ ! -f ./rs/$FILE ]; then
			echo "Checking $QT_DIR/lib/$FILE"
			if [ -f $QT_DIR/lib/$FILE ]; then
				echo "Copying $FB_DIR/lib/$FILE"
				cp $FB_DIR/lib/$FILE ./rs/$FILE
				STATIC_QT=1
			fi
			echo "Checking $QT_DIR/$FILE"
			if [ -f $QT_DIR/$FILE ]; then
				echo "Copying $QT_DIR/$FILE"
				cp $QT_DIR/$FILE ./rs/$FILE
				STATIC_QT=1
			fi
		fi
	done

	if (( $STATIC_QT )); then
		tar cjvf rs-${1}-$(uname -m)-static_qt.tar.bz2 -C ./rs $RS_FILES $FB_FILES
	else
		tar cjvf rs-${1}-$(uname -m)-no_qt.tar.bz2 -C ./rs $RS_FILES $FB_FILES
		tar cjvf rs-${1}-$(uname -m).tar.bz2 -C ./rs $RS_FILES $FB_FILES $QT_FILES
	fi
fi

