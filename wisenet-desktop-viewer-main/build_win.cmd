@echo off

if "%1" == "beta_version" goto beta
if "%1" == "ec_version" goto ec
if "%1" == "s1_version" goto s1

goto commercial

:dev
SET EXTRA_DEF=DEFINES+=DEVELOP_VERSION
goto continue

:beta
SET EXTRA_DEF=DEFINES+=BETA_VERSION
goto continue

:ec
SET EXTRA_DEF=DEFINES+=EC_VERSION
goto continue

:s1
SET EXTRA_DEF=DEFINES+=WISENET_S1_VERSION
goto continue

:commercial
SET EXTRA_DEF=
goto continue

:continue
echo extradef is %EXTRA_DEF%

:: Load visual studio build environment
if defined VisualStudioVersion (
    echo Visual Studio environment is already loaded
) else (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64
)

SET QT_JOM=C:\Qt\Tools\QtCreator\bin\jom\jom.exe
set QT_DIR=C:\Qt\5.15.2\msvc2019_64\bin
SET QT_QMAKE=%QT_DIR%\qmake.exe
SET SOURCE_PATH=%cd%

::TEST with QT Creator
::SET BUILD_PATH=%cd%\..\build-WisenetViewer_CI-Desktop_Qt_5_15_2_MSVC2019_64bit-Release
SET BUILD_PATH=%cd%\build-WisenetViewer_CI-MSVC2019_64bit-Release

echo QMAKE PATH : %QT_QMAKE%
echo ZOM PATH : %QT_JOM%
echo BUILD PATH : %BUILD_PATH%

:: Create BUILD DIR and go to there
if exist "%BUILD_PATH%" rd /s /q "%BUILD_PATH%"
mkdir "%BUILD_PATH%"
cd /D "%BUILD_PATH%"

:: Comment below lines to test packaging only
:: -------------------------------------->
:: Make clean
call "%QT_JOM%" clean

:: QMake
call "%QT_QMAKE%" -o Makefile "%SOURCE_PATH%/WisenetViewer_CI.pro" -spec win32-msvc CONFIG+=qtquickcompiler %EXTRA_DEF%
call "%QT_JOM%" -f Makefile qmake_all
call "%QT_JOM%"
:: <--------------------------------------

:: go to pwd
cd /D "%SOURCE_PATH%"