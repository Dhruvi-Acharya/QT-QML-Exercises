#!/bin/sh

QMAKE_PATH="${HOME}/Qt/5.15.2/gcc_64/bin"
SOURCE_PATH="$(pwd)"
BUILD_PATH="$(pwd)/../build-WisenetViewer_CI-GCC_64bit-Release"
PACKAGE_PATH="${BUILD_PATH}/LinuxPackage"
DEPLOYKIT_PATH="${PACKAGE_PATH}/opt/HanwhaVision/WisenetViewer"
SHARED_LIB_ARCHIVE_PATH="${BUILD_PATH}/shared-lib-archive"
PLAYER_BUILD_PATH="$(pwd)/../build-WisenetPlayer_CI-GCC_64bit-Release"
PACKAGING_VERSION=""
CURRENT_VERSION=1.04.00
TODAY=$(date "+%Y%m%d")

# set packaging version following argument
if [ $# -gt 2 ]; then
    echo "cannot support more than two arguments about pkg_linux.sh"
    exit -1
elif [ $# -eq 2 ]; then
	QMAKE_PATH=$1
	PACKAGING_VERSION=$2
	if [ "$2" = "ec_version" ]; then
		CURRENT_VERSION="${CURRENT_VERSION}ec_${TODAY}"
	elif [ "$2" = "beta_version" ]; then
		CURRENT_VERSION="${CURRENT_VERSION}b_${TODAY}"
	fi
	echo "Run with QMAKE path and packaging version. $QMAKE_PATH $PACKAGING_VERSION"
elif [ $# -eq 1 ]; then
	if [ "$1" = "ec_version" ]; then
		echo "Set current version to ec_version"
		CURRENT_VERSION="${CURRENT_VERSION}ec_${TODAY}"
	elif [ "$1" = "beta_version" ]; then
		echo "Set current version to beta_version"
		CURRENT_VERSION="${CURRENT_VERSION}b_${TODAY}"
	else	
		echo "Got QMAKE path"
		QMAKE_PATH=$1
		CURRENT_VERSION="${CURRENT_VERSION}_${TODAY}"
	fi
else
	echo "Didn't get any argument. Run with default QMAKE path. $QMAKE_PATH"
	CURRENT_VERSION="${CURRENT_VERSION}_${TODAY}"
fi

echo "CURRENT_VERSION : $CURRENT_VERSION"
echo "QMAKE_PATH : $QMAKE_PATH"

# make WisenetPlayer package
cd WisenetPlayer
echo "build Wisenet Player with at QMAKE path : $QMAKE_PATH"
sh ./build_linux.sh $QMAKE_PATH
cd ..

# build WNV codes
if [ -d ${BUILD_PATH} ]; then
    echo "build-WisenetViewer_CI-GCC_64bit-Release already exists"
    rm -rf ${BUILD_PATH}
fi

echo "build Wisenet Viewer with $PACKAGING_VERSION at QMAKE path $QMAKE_PATH"
sh ./build_linux.sh $QMAKE_PATH $PACKAGING_VERSION

# copy DEBIAN config files
if [ -d ${PACKAGE_PATH} ]; then
    rm -rf ${PACKAGE_PATH}
fi
cp -r ${SOURCE_PATH}/Installer/LinuxPackage ${BUILD_PATH}

# extract all libraries of WNV app
if [ -d ${DEPLOYKIT_PATH} ]; then
    rm -rf ${DEPLOYKIT_PATH}
fi
cqtdeployer -bin ${BUILD_PATH}/WisenetViewer/WisenetViewer -qmlDir ${SOURCE_PATH} -qmake ${QMAKE_PATH}/qmake -targetDir ${DEPLOYKIT_PATH} clear

# set Qt WebEngine Invironment Variable
sed -i '$ d' ${DEPLOYKIT_PATH}/WisenetViewer.sh
echo 'export QTWEBENGINE_DISABLE_SANDBOX=1' >> ${DEPLOYKIT_PATH}/WisenetViewer.sh
echo '"$BASE_DIR/bin/WisenetViewer" "$@"' >> ${DEPLOYKIT_PATH}/WisenetViewer.sh

# copy shared object(*.so) files to Deploy directory
if [ -d ${SHARED_LIB_ARCHIVE_PATH} ]; then
    rm -rf ${SHARED_LIB_ARCHIVE_PATH}
fi
mkdir ${SHARED_LIB_ARCHIVE_PATH}
find ${SOURCE_PATH}/OpenSource -name "*.so*" -exec cp {} ${SHARED_LIB_ARCHIVE_PATH} \;
cp ${SHARED_LIB_ARCHIVE_PATH}/*.so* ${DEPLOYKIT_PATH}/lib

# copy extra files
cp ${PLAYER_BUILD_PATH}/WisenetPlayer*.deb ${DEPLOYKIT_PATH}/bin
cp ${SOURCE_PATH}/OpenSourceLicense.txt ${DEPLOYKIT_PATH}/bin
cp -r ${SOURCE_PATH}/RootCA ${DEPLOYKIT_PATH}/bin
cp -r ${SOURCE_PATH}/Manual ${DEPLOYKIT_PATH}/bin
cp -r ${SOURCE_PATH}/Cert ${DEPLOYKIT_PATH}/bin

# make deb
echo "Make deb : WisenetViewer_$CURRENT_VERSION"
dpkg-deb --build ${PACKAGE_PATH} WisenetViewer_$CURRENT_VERSION.deb
