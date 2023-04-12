#!/bin/sh

EXTRA_DEF=""
echo "arg 1 is $1"
if [ "$1" = "beta_version" ]; then EXTRA_DEF=DEFINES+=BETA_VERSION; fi
if [ "$1" = "ec_version" ]; then EXTRA_DEF=DEFINES+=EC_VERSION; fi
echo "EXTRA_DEF is $EXTRA_DEF"

QMAKE_PATH="${HOME}/Qt/5.15.2/clang_64/bin"
SOURCE_PATH="$(pwd)"
BUILD_PATH="$(pwd)/build-WisenetViewer_CI-clang_64bit-Release"

eval QMAKE_PATH=$QMAKE_PATH
eval BUILD_PATH=$BUILD_PATH

QMAKE_BIN=$QMAKE_PATH/qmake
echo "QMAKE PATH : $QMAKE_BIN"
echo "BUILD PATH : $BUILD_PATH"

# Create BUILD DIR and go to there
rm -rf $BUILD_PATH
mkdir -p $BUILD_PATH
cd $BUILD_PATH

# Make clean
make clean -j8

# QMake
${QMAKE_BIN} -o Makefile ${SOURCE_PATH}/WisenetViewer_CI.pro -spec macx-clang CONFIG+=x86_64 CONFIG+=qtquickcompiler ${EXTRA_DEF}
make -f Makefile qmake_all

# Make all
make -j8

# go to pwd
cd $SOURCE_PATH
