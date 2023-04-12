#!/bin/sh
QMAKE_PATH=""

if [ $# -eq 1 ]; then
	QMAKE_PATH=$1
	./pkg_linux.sh $QMAKE_PATH ec_version 
else
	./pkg_linux.sh ec_version
fi

