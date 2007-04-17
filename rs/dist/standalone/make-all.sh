#!/bin/bash

BASE_FILES="Conf/RS.ini IDPLicense.txt IPLicense.txt RS.FDB aliases.conf \
		firebird.conf fbclient.dll firebird.msg ib_udf.dll ib_util.dll
		icudt30.dll icuin30.dll icuuc30.dll msvcp71.dll msvcr71.dll"

windows()
{
	FILES="QtCore4.dll QtGui4.dll mingwm10.dll \
		rs.exe rs_en.qm rs_ptBR.qm"
	
	mkdir -p rs-standalone/Conf
	
	for FILE in $BASE_FILES; do
		cp ./base/$FILE rs-standalone/$FILE
	done
	
	for FILE in $FILES; do
		cp ../windows/$FILE rs-standalone/$FILE
	done
	
	zip -9r rs-${1}-standalone.zip rs-standalone
	rm -rf rs-standalone
}

if [ "${1}" == "" ]; then
	echo error: Version string missing
else
	windows ${1}
fi