@echo off

:: Load visual studio build environment
if defined VisualStudioVersion (
    echo Visual Studio environment is already loaded
) else (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
)

SET QT_JOM=C:\Qt\Tools\QtCreator\bin\jom\jom.exe
SET QT_DIR=C:\Qt\5.15.2\msvc2019_64\bin
SET QT_QMAKE=%QT_DIR%\qmake.exe
SET SOURCE_PATH=%cd%\..
SET BUILD_PATH=%SOURCE_PATH%\build-WisenetPlayer-MSVC2019_64bit-Release

echo QMAKE PATH : %QT_QMAKE%
echo ZOM PATH : %QT_JOM%
echo SOURCE_PATH : %SOURCE_PATH%
echo BUILD PATH : %BUILD_PATH%

:: Create BUILD DIR and go to there
if exist "%BUILD_PATH%" rd /s /q "%BUILD_PATH%"
mkdir "%BUILD_PATH%"
cd /D "%BUILD_PATH%"

:: Make clean
call "%QT_JOM%" clean

:: QMake
call "%QT_QMAKE%" -o Makefile "%SOURCE_PATH%/WisenetPlayer.pro" -spec win32-msvc CONFIG+=qtquickcompiler DEFINES+=MEDIA_FILE_ONLY
call "%QT_JOM%" -f Makefile qmake_all
call "%QT_JOM%"

:: go to pwd
cd /D "%SOURCE_PATH%"

SET PKG_NAME=package-WisenetPlayer_CI-MSVC2019_64bit-Release
SET PKG_PATH=%cd%\WisenetPlayer\data

:: remove old PACKAGE DIR 
if exist "%PKG_PATH%" rd /s /q "%PKG_PATH%"
:: Create PACKAGE DIR and go to there
mkdir "%PKG_PATH%\plugins"

:: When using Qt Installer, the script QTDIR/bin/qtenv2.bat should be used to set it up.
SET QT_ENV_BAT=%QT_DIR%\qtenv2.bat
call %QT_ENV_BAT%

:: deploy qt components (for test add --dry-run option)
windeployqt --qmldir "%SOURCE_PATH%\WisenetPlayer" --qmldir "%SOURCE_PATH%\WisenetMediaFramework" --qmldir "%SOURCE_PATH%\WisenetStyle" "%BUILD_PATH%\WisenetPlayer\release\WisenetPlayer.exe" --dir %PKG_PATH% --plugindir %PKG_PATH%\plugins --release

:: copy exe and 3rd dlls
xcopy /y /d "%BUILD_PATH%\WisenetPlayer\release\WisenetPlayer.exe" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\ffmpeg-5.1\windows\bin\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\libs\openssl-1.1.1k\windows\lib\*.dll" %PKG_PATH%
:: xcopy /y /d "%SOURCE_PATH%\OpenSource\sipproxy\windows\lib\release\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSource\IMVSDK\windows\x64\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\Microsoft.VC142.CRT\*.dll" %PKG_PATH%
xcopy /y /d "%SOURCE_PATH%\OpenSourceLicense_WisenetPlayer.txt" %PKG_PATH%

:: delete unused files
rd /s /q "%PKG_PATH%\QtWebEngine"
rd /s /q "%PKG_PATH%\resources"
rd /s /q "%PKG_PATH%\translations\qtwebengine_locales"
del /s /q "%PKG_PATH%\Qt5WebChannel.dll
del /s /q "%PKG_PATH%\Qt5WebEngine.dll
del /s /q "%PKG_PATH%\Qt5WebEngineCore.dll
del /s /q "%PKG_PATH%\QtWebEngineProcess.exe
del /s /q "%PKG_PATH%\vc_redist.x64.exe"

:: go to pwd
echo "kkd : %SOURCE_PATH%"
cd /D "%SOURCE_PATH%"

call player_verisign.cmd %PKG_PATH%
