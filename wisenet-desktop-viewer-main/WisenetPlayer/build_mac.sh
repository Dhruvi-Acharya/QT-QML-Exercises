QMAKE_PATH="${HOME}/Qt/5.15.2/clang_64/bin"
SOURCE_PATH="$(pwd)/.."
BUILD_PATH="$(pwd)/../build-WisenetPlayer-clang_64bit-Release"

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
${QMAKE_BIN} -o Makefile ${SOURCE_PATH}/WisenetPlayer.pro -spec macx-clang CONFIG+=x86_64 CONFIG+=qtquickcompiler DEFINES+=MEDIA_FILE_ONLY
make -f Makefile qmake_all

# Make all
make -j8


# go to pwd
cd $SOURCE_PATH

SOURCE_PATH="$(pwd)"
PACKAGE_PATH="$(pwd)/package-WisenetPlayer-clang_64bit-Release"

mkdir -p $PACKAGE_PATH
rm -rf $PACKAGE_PATH/WisenetPlayer.app
cp -R $BUILD_PATH/WisenetPlayer/WisenetPlayer.app $PACKAGE_PATH/

mkdir $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks
cp $SOURCE_PATH/OpenSource/libs/ffmpeg-5.1/macos/lib/*.dylib $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSource/libs/openssl-1.1.1k/macos/lib/*.dylib $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks/
cp -R $SOURCE_PATH/OpenSource/IMVSDK/macos/Library/IMV1.framework $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSource/libs/libusb-1.0.26/macos_10.13/lib/*.dylib $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSourceLicense_WisenetPlayer.txt $PACKAGE_PATH/WisenetPlayer.app/Contents/MacOS

cd $PACKAGE_PATH

# macdeployqt
$QMAKE_PATH/macdeployqt WisenetPlayer.app -qmldir=$SOURCE_PATH/WisenetPlayer -qmldir=$SOURCE_PATH/WisenetMediaFramework -qmldir=$SOURCE_PATH/WisenetStyle

# Remove unused files (QtWebEngine)
rm -rf $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks/QtWebChannel.framework
rm -rf $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks/QtWebEngine.framework
rm -rf $PACKAGE_PATH/WisenetPlayer.app/Contents/Frameworks/QtWebEngineCore.framework

# codesign
codesign -d --deep -f --timestamp --options=runtime -s "Developer ID Application: Hanwha Techwin Company Limited (7NS4AYQAP6)" WisenetPlayer.app
codesign -dv WisenetPlayer.app

# need notarize manually
#xcrun altool --notarize-app --primary-bundle-id "com.HanwhaTechwin.WisenetPlayer" --username "dihe.park@hanwha.com" --password "tzqw-qzwo-zdip-soer" --asc-provider "SamsungTechwin" --file "WisenetPlayer.zip"
#xcrun altool --notarization-info 6947abbf-8242-49d8-a27f-64d408b1d65e --username "dihe.park@hanwha.com" --password "tzqw-qzwo-zdip-soer"
#sleep 300
#xcrun stapler staple "WisenetPlayer.app"

