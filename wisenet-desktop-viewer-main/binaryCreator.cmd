@echo off

:: archive and binary creator
SET QT_INSTALLER_PATH=C:\Qt\Tools\QtInstallerFramework\4.1\bin
SET QT_ARCHIVEGEN=%QT_INSTALLER_PATH%\archivegen.exe
SET QT_BINARYCREATOR=%QT_INSTALLER_PATH%\binarycreator.exe

echo Create binary file::WisenetViewer.exe
%QT_BINARYCREATOR% -c config\config.xml -p packages WisenetViewer.exe
echo Done!!