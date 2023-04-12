#!/bin/sh

#pre installation packages
#sudo apt-get install build-essential libgl1-mesa-dev
#sudo apt install libxcb-xinerama0

QMAKE_PATH="${HOME}/Qt/5.15.2/gcc_64/bin"
SOURCE_PATH="$(pwd)"
BUILD_PATH="$(pwd)/../build-WisenetViewer_CI-GCC_64bit-Release"
EXTRA_DEF=""

if [ $# -gt 2 ]; then
    echo "Usage: $0 [qmake directory path]"
    exit -1
fi

if [ $# -gt 2 ]; then
    echo "cannot support more than two arguments about build_linux.sh"
    exit -1
elif [ $# -eq 2 ]; then
	QMAKE_PATH=$1
	echo "Build with QMAKE path : $QMAKE_PATH"
	if [ "$2" = "ec_version" ]; then
		echo "SET EXTRA_DEF to EC"
    		EXTRA_DEF=DEFINES+=EC_VERSION
	elif [ "$2" = "beta_version" ]; then
		echo "SET EXTRA_DEF to BETA"
    		EXTRA_DEF=DEFINES+=BETA_VERSION
    	fi
elif [ $# -eq 1 ]; then
	if [ "$1" = "ec_version" ]; then
		echo "SET EXTRA_DEF to EC"
    		EXTRA_DEF=DEFINES+=EC_VERSION
	elif [ "$1" = "beta_version" ]; then
    		echo "SET EXTRA_DEF to BETA"
    		EXTRA_DEF=DEFINES+=BETA_VERSION
	else	
		QMAKE_PATH=$1
		echo "Set QMAKE PATH $QMAKE_PATH"
	fi
	echo "Build with QMAKE path $QMAKE_PATH and DEF : $EXTRA_DEF"
else
	echo "Didn't get any argument. Run with default QMAKE path. $QMAKE_PATH"
fi


eval QMAKE_PATH=$QMAKE_PATH
eval BUILD_PATH=$BUILD_PATH

QMAKE_BIN=$QMAKE_PATH/qmake
echo "QMAKE PATH : $QMAKE_BIN"
echo "BUILD PATH : $BUILD_PATH"

# Create BUILD DIR and go to there
rm -rf $BUILD_PATH
mkdir -p $BUILD_PATH
cd $BUILD_PATH

# QMake
${QMAKE_BIN} -o Makefile ${SOURCE_PATH}/WisenetViewer_CI.pro -spec linux-g++ CONFIG+=qtquickcompiler ${EXTRA_DEF}

# Make clean
# make clean -j8

make -f Makefile qmake_all

# Make all
make -j8

# go to pwd
cd $SOURCE_PATH
