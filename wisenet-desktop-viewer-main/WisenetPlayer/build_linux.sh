#!/bin/sh

#pre installation packages
#sudo apt-get install build-essential libgl1-mesa-dev
#sudo apt install libxcb-xinerama0

QMAKE_PATH="${HOME}/Qt/5.15.2/gcc_64/bin"
SOURCE_PATH="$(pwd)/.."
BUILD_PATH="${SOURCE_PATH}/../build-WisenetPlayer_CI-GCC_64bit-Release"
PACKAGE_PATH="${BUILD_PATH}/LinuxPackage"
DEPLOYKIT_PATH="${PACKAGE_PATH}/opt/HanwhaTechwin/WisenetPlayer"
SHARED_LIB_ARCHIVE_PATH="${BUILD_PATH}/shared-lib-archive"

if [ $# -gt 1 ]; then
    echo "Usage: $0 [qmake directory path]"
    exit -1
fi

if [ $# -eq 1 ]; then
    QMAKE_PATH=$1
fi
eval QMAKE_PATH=$QMAKE_PATH
eval BUILD_PATH=$BUILD_PATH

QMAKE_BIN=$QMAKE_PATH/qmake
echo "QMAKE PATH : $QMAKE_BIN"
echo "BUILD PATH : $BUILD_PATH"

# Create BUILD DIR and go to there
if [ -d ${BUILD_PATH} ]; then
    rm -rf ${BUILD_PATH}
fi
mkdir -p $BUILD_PATH
cd $BUILD_PATH

# Make clean
make clean -j8

# QMake
${QMAKE_BIN} -o Makefile ${SOURCE_PATH}/WisenetPlayer.pro -spec linux-g++ CONFIG+=qtquickcompiler DEFINES+=MEDIA_FILE_ONLY
make -f Makefile qmake_all

# Make all
make -j8

# go to pwd
cd $SOURCE_PATH

# copy DEBIAN config files
if [ -d ${PACKAGE_PATH} ]; then
    rm -rf ${PACKAGE_PATH}
fi
cp -r ${SOURCE_PATH}/WisenetPlayer/LinuxPackage ${BUILD_PATH}

# extract all libraries
if [ -d ${DEPLOYKIT_PATH} ]; then
    rm -rf ${DEPLOYKIT_PATH}
fi
cqtdeployer -bin ${BUILD_PATH}/WisenetPlayer/WisenetPlayer -qmlDir ${SOURCE_PATH} -qmake ${QMAKE_PATH}/qmake -targetDir ${DEPLOYKIT_PATH} clear

# copy extra files
cp ${SOURCE_PATH}/OpenSource/libs/ffmpeg-4.4/linux/lib/*.so* ${DEPLOYKIT_PATH}/lib
cp ${SOURCE_PATH}/OpenSource/libs/openssl-1.1.1k/linux/lib/*.so* ${DEPLOYKIT_PATH}/lib
cp ${SOURCE_PATH}/OpenSource/IMVSDK/linux/64bits/*.so* ${DEPLOYKIT_PATH}/lib
cp ${SOURCE_PATH}/OpenSourceLicense_WisenetPlayer.txt ${DEPLOYKIT_PATH}/bin

# delete unused files
rm -f ${DEPLOYKIT_PATH}/bin/QtWebEngineProcess
rm -f ${DEPLOYKIT_PATH}/lib/libQt5WebEngine.so.5
rm -f ${DEPLOYKIT_PATH}/lib/libQt5WebChannel.so.5
rm -f ${DEPLOYKIT_PATH}/lib/libQt5WebEngineCore.so.5
rm -rf ${DEPLOYKIT_PATH}/resources
rm -rf ${DEPLOYKIT_PATH}/translations/qtwebengine_locales

# make deb
dpkg-deb --build ${PACKAGE_PATH} ${BUILD_PATH}/WisenetPlayer_1.01.00.deb

