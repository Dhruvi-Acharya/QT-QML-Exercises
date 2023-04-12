@echo off

SET PKG_NAME=package-WisenetViewer_CI-MSVC2019_64bit-Release
SET PKG_PATH=%cd%\..\data

:: archive and binary creator
SET QT_INSTALLER_PATH=C:\Qt\Tools\QtInstallerFramework\4.1\bin
SET QT_ARCHIVEGEN=%QT_INSTALLER_PATH%\archivegen.exe
SET QT_BINARYCREATOR=%QT_INSTALLER_PATH%\binarycreator.exe

setlocal
SET HOUR=%TIME:~0,2%
IF "%HOUR:~0,1%" == " " SET HOUR=0%HOUR:~1,1%
SET T=%HOUR%%TIME:~3,2%%TIME:~6,2%
SET D=%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%
SET DATETIME=%D%_%T%

:: remove old PKG_PATH 
if exist packages\Hanwha.Techwin.WisenetViewer\data rd /s /q packages\Hanwha.Techwin.WisenetViewer\data

echo "Create archive file::packages\Hanwha.Techwin.WisenetViewer\data\%PKG_NAME%_%DATETIME%.7z
%QT_ARCHIVEGEN% packages\Hanwha.Techwin.WisenetViewer\data\release.7z %PKG_PATH%\
echo "Done!!"