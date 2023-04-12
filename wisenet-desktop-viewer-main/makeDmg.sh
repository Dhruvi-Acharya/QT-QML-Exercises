#!/bin/sh

QMAKE_PATH="${HOME}/Qt/5.15.2/clang_64/bin"
SOURCE_PATH="$(pwd)"

# just for testing with qt creator
#BUILD_PATH="$(pwd)/../build-WisenetViewer_CI-Desktop_Qt_5_15_2_clang_64bit-Release"
BUILD_PATH="$(pwd)/build-WisenetViewer_CI-clang_64bit-Release"
PACKAGE_PATH="$(pwd)/package-WisenetViewer_CI-clang_64bit-Release"

mkdir -p $PACKAGE_PATH
rm -rf $PACKAGE_PATH/WisenetViewer.app

cp -R $BUILD_PATH/WisenetViewer/WisenetViewer.app $PACKAGE_PATH/
defaults write com.apple.finder AppleShowAllFiles YES && killall Finder
cp -f $SOURCE_PATH/Installer/.background.png $PACKAGE_PATH/
cp -f $SOURCE_PATH/Installer/WisenetPlayer.zip $PACKAGE_PATH/WisenetViewer.app/Contents/MacOS

mkdir $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks
cp $SOURCE_PATH/OpenSource/libs/ffmpeg-5.1/macos/lib/*.dylib $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSource/libs/openssl-1.1.1k/macos/lib/*.dylib $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSource/sipproxy/macos/lib/*.dylib $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks/
cp -R $SOURCE_PATH/OpenSource/IMVSDK/macos/Library/IMV1.framework $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSource/libs/smtpclient-for_qt/macos/release/*.dylib $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSource/libs/openldap-2.5.13/macos/lib/*.dylib $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks/
cp $SOURCE_PATH/OpenSource/libs/libusb-1.0.26/macos_10.13/lib/*.dylib $PACKAGE_PATH/WisenetViewer.app/Contents/Frameworks/

mkdir $PACKAGE_PATH/WisenetViewer.app/Contents/MacOS/RootCA
cp $SOURCE_PATH/RootCA/*.* $PACKAGE_PATH/WisenetViewer.app/Contents/MacOS/RootCA
cp $SOURCE_PATH/OpenSourceLicense.txt $PACKAGE_PATH/WisenetViewer.app/Contents/MacOS
cp -R $SOURCE_PATH/Manual $PACKAGE_PATH/WisenetViewer.app/Contents/MacOS/

mkdir $PACKAGE_PATH/WisenetViewer.app/Contents/MacOS/Cert
cp -R $SOURCE_PATH/Cert/*.* $PACKAGE_PATH/WisenetViewer.app/Contents/MacOS/Cert

cd $PACKAGE_PATH

# macdeployqt
$QMAKE_PATH/macdeployqt WisenetViewer.app -qmldir=$SOURCE_PATH/WisenetViewer -qmldir=$SOURCE_PATH/WisenetMediaFramework -qmldir=$SOURCE_PATH/WisenetStyle
# -dmg make
#$QMAKE_PATH/macdeployqt WisenetViewer.app -qmldir=$SOURCE_PATH/WisenetViewer -qmldir=$SOURCE_PATH/WisenetMediaFramework -dmg

# codesign
codesign -d --deep -f --timestamp --options=runtime -s "Developer ID Application: Hanwha Techwin Company Limited (7NS4AYQAP6)"  --entitlements ../entitlements.plist  WisenetViewer.app
codesign -dv WisenetViewer.app
codesign -f --timestamp -s "Developer ID Application: Hanwha Techwin Company Limited (7NS4AYQAP6)" --entitlements ../QtWebEngineProcess.entitlements --options runtime WisenetViewer.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess
codesign -dv WisenetViewer.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess
codesign -f --timestamp -s "Developer ID Application: Hanwha Techwin Company Limited (7NS4AYQAP6)" --options runtime --entitlements ../entitlements.plist WisenetViewer.app/Contents/MacOS/WisenetViewer
codesign -dv WisenetViewer.app/Contents/MacOS/WisenetViewer

cd $SOURCE_PATH


# bash variables
applicationName="WisenetViewer"
title="Wisenet Viewer 1.04.00"
size="800000"
source="$(pwd)/package-WisenetViewer_CI-clang_64bit-Release"
name="WisenetViewer_1.04.00"
today=$(date "+%Y%m%d")
finalDMGName="${name}_${today}.dmg"

# Initialize (unmount and delete old dmg file)
hdiutil detach "${HOME}/../../Volumes/${title}" -force
rm -f pack.temp.dmg
rm -f ${finalDMGName}
defaults write com.apple.finder AppleShowAllFiles YES && killall Finder

# Create a temp R/W DMG. It must be larger than the result will be.
hdiutil create -srcfolder "${source}" -volname "${title}" -fs HFS+ \
      -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${size}k pack.temp.dmg

# Mount the disk image, and store the device name.
# you might want to use sleep for a few seconds after this operation.
device=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 3

# Use AppleScript to set the visual styles.
# name of .app must be in bash variable "applicationName"
echo '
tell application "Finder"
tell disk "'${title}'"
	open
	set current view of container window to icon view
	set toolbar visible of container window to false
	set statusbar visible of container window to false
	set the bounds of container window to {400, 400, 1000, 800}
	set theViewOptions to the icon view options of container window
	set arrangement of theViewOptions to not arranged
	set icon size of theViewOptions to 72
	set background picture of theViewOptions to file ".background.png"
	make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
	set position of item "'${applicationName}'" of container window to {175, 240}
    set name of item "'${applicationName}'" to "Wisenet Viewer.app"
	set position of item "Applications" of container window to {425, 240}
	update without registering applications
	delay 3
	close
end tell
end tell
' | osascript

# Finialize the DMG by setting permissions properly, compressing and releasing it.
chmod -Rf go-w /Volumes/"${title}"
sync
sync
hdiutil detach ${device}
hdiutil convert "pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o "${finalDMGName}"
rm -f pack.temp.dmg

# codesign
codesign -f --timestamp --options=runtime -s "Developer ID Application: Hanwha Techwin Company Limited (7NS4AYQAP6)" "${finalDMGName}"
codesign -dv "${finalDMGName}"

# need notarize manually
#xcrun altool --notarize-app --primary-bundle-id "com.HanwhaTechwin.WisenetViewer" --username "dihe.park@hanwha.com" --password "tzqw-qzwo-zdip-soer" --asc-provider "SamsungTechwin" --file "${finalDMGName}"
#xcrun altool --notarization-info 6947abbf-8242-49d8-a27f-64d408b1d65e --username "dihe.park@hanwha.com" --password "tzqw-qzwo-zdip-soer"
#sleep 300
#xcrun stapler staple "${finalDMGName}"
