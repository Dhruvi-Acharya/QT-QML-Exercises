@echo off

:: build WisenetPlayer
cd /D %cd%\WisenetPlayer
call build.cmd

:: no parameter
if "%1" == "" (goto noparam) else (goto hasparam)

:noparam
::echo "no parameter"
call build_win.cmd
goto package

:hasparam
::echo "has parameter %1"
call build_win.cmd %1
goto package

:package

SET PKG_NAME=package-WisenetViewer_CI-MSVC2019_64bit-Release
SET PKG_PATH=%cd%\data

:: remove old PACKAGE DIR 
if exist "%PKG_PATH%" rd /s /q "%PKG_PATH%"
:: Create PACKAGE DIR and go to there
mkdir "%PKG_PATH%\plugins"

:: When using Qt Installer, the script QTDIR/bin/qtenv2.bat should be used to set it up.
SET QT_ENV_BAT=%QT_DIR%\qtenv2.bat
call %QT_ENV_BAT%

::deploy qt components (for test add --dry-run option)
windeployqt --qmldir "%SOURCE_PATH%\WisenetViewer" --qmldir "%SOURCE_PATH%\WisenetMediaFramework" "%BUILD_PATH%\WisenetViewer\release\WisenetViewer.exe" --dir %PKG_PATH% --plugindir %PKG_PATH%\plugins --release

::copy exe and 3rd dlls
xcopy /y /d "%BUILD_PATH%\WisenetViewer\release\WisenetViewer.exe" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\ffmpeg-5.1\windows\bin\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\openssl-1.1.1k\windows\lib\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\libssh2-1.10.0\windows\lib\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\libusb-1.0.26\VS2015-x64\dll\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\openldap-2.5.13\windows\lib\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\smtpclient-for_qt\windows\release\SMTPEmail.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\opencv-4.7.0\windows\lib\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\sipproxy\windows\lib\release\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\IMVSDK\windows\x64\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\Microsoft.VC142.CRT\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\Installer\auto_uninstall.qs" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\Installer\updateUninstallPath.bat" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\Installer\WisenetViewer_new.ico" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\Installer\deleteOldRegistry.bat" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\Installer\WisenetViewerInstaller.conf" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\WisenetPlayer\WisenetPlayer.exe" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSourceLicense.txt" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\restart.bat" %PKG_PATH%
mkdir "%PKG_PATH%\RootCA"
xcopy /y /d "%SOURCE_PATH%\RootCA\*.*" %PKG_PATH%\RootCA
mkdir "%PKG_PATH%\Manual"
xcopy /y /d /e "%SOURCE_PATH%\Manual\*.*" %PKG_PATH%\Manual
mkdir "%PKG_PATH%\Cert"
xcopy /y /d "%SOURCE_PATH%\CERT\*.*" %PKG_PATH%\Cert

:: go to pwd
cd /D "%SOURCE_PATH%"

call viewer_verisign.cmd %PKG_PATH%

:finish
echo "Exit..."
