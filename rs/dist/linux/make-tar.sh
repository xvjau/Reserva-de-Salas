#!/bin/bash
FILES="Conf/RS.ini libfbclient.so.2.0.0 libicudata.so.30 libicui18n.so.30 libicuuc.so.30 rs.sh \
libfbclient.so libib_util.so libicudata.so.30.0 libicui18n.so.30.0  libicuuc.so.30.0 rs_en.qm \
libfbclient.so.2 libicudata.so libicui18n.so libicuuc.so rs rs_ptBR.qm"

if [ "${1}" == "" ]; then
	echo error: Version string missing
else
	tar cjvf rs-${1}-$(uname -m).tar.bz2 $FILES
fi
