#!/bin/bash
RS_FILES="Conf/RS.ini rs rs.sh rs_ptBR.qm rs_en.qm"

FB_FILES="libicuuc.so* libicudata.so* libicui18n.so* libfbclient.so* libib_util.so*"
 
QT_FILES="libQtCore.so* libQtGui.so*"

STATIC_QT=1

FB_DIR="/opt/firebird"
QT_DIR=$(qmake -v | tail -1 | cut -c 27-)

if [ "${1}" == "" ]; then
	echo error: Version string missing
else
	echo "Cheking rs dir"
	if [ ! -d ./rs ]; then
		mkdir rs
	fi
	rm ./rs/lib*

	echo "Upx"
	upx -9 ../../bin/rs

	echo "Copy RS binary:"
	cp ../../bin/rs ./rs

	echo "Copy Translations"
	cp ../../bin/*.qm ./rs
	
	echo "Checking firebird files"	
	for FILE in $FB_FILES; do
		TFB_FILES+=$(find $FB_DIR/lib/$FILE -printf "%f ")
	done 
	
	FB_FILES=$TFB_FILES
	TFB_FILES=""
	
	for FILE in $FB_FILES; do
		if [ ! -f ./rs/$FILE ]; then
			echo "Copying $FB_DIR/lib/$FILE"
			cp -a $FB_DIR/lib/$FILE ./rs/$FILE
		fi
	done
	
	echo "Checking Qt files"
	for FILE in $QT_FILES; do
		TQT_FILES+=$(find $QT_DIR/lib/$FILE -printf "%f ")
		TQT_FILES+=$(find $QT_DIR/$FILE -printf "%f ")
	done
	
	QT_FILES=$TQT_FILES
	TQT_FILES=""	
	
	
	for FILE in $QT_FILES; do
		if [ ! -f ./rs/$FILE ]; then
			echo "Checking $QT_DIR/lib/$FILE"
			if [ -f $QT_DIR/lib/$FILE ]; then
				echo "Copying $QT_DIR/lib/$FILE"
				cp -a $QT_DIR/lib/$FILE ./rs/$FILE
				STATIC_QT=0
			fi
			echo "Checking $QT_DIR/$FILE"
			if [ -f $QT_DIR/$FILE ]; then
				echo "Copying $QT_DIR/$FILE"
				cp -a $QT_DIR/$FILE ./rs/$FILE
				STATIC_QT=0
			fi
		fi
	done

	if (( STATIC_QT )); then
		if [ -f rs-${1}-$(uname -m)-static_qt.tar* ]; then
			echo "Removing rs-${1}-$(uname -m)-static_qt"
			rm -f rs-${1}-$(uname -m)-static_qt.tar*
		fi
		
		for FILE in $RS_FILES $FB_FILES; do
			tar rvf rs-${1}-$(uname -m)-static_qt.tar ./rs/$FILE
		done
		
		echo "Compressing"
		bzip2 rs-${1}-$(uname -m)-static_qt.tar
	else
		if [ -f rs-${1}-$(uname -m)-no_qt.tar* ]; then 
			echo "Removing rm -f rs-${1}-$(uname -m)-no_qt"
			rm -f rs-${1}-$(uname -m)-no_qt.tar*
		fi
		if [ -f rs-${1}-$(uname -m).tar* ]; then
			echo "Removing rs-${1}-$(uname -m)"
			rm -f rs-${1}-$(uname -m).tar*
		fi
		
		for FILE in $RS_FILES $FB_FILES; do
			tar rvf rs-${1}-$(uname -m)-no_qt.tar ./rs/$FILE
			tar rvf rs-${1}-$(uname -m).tar ./rs/$FILE
		done
		
		for FILE in  $QT_FILES; do
			tar rvf rs-${1}-$(uname -m).tar ./rs/$FILE
		done
		
		echo "Compressing"
		bzip2 rs-${1}-$(uname -m)-no_qt.tar
		bzip2 rs-${1}-$(uname -m).tar		                        
	fi
fi

