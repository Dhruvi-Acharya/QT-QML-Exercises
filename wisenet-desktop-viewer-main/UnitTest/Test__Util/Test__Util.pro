QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

include($$PWD/../../WisenetDesktopCommon.pri)

TEMPLATE = app

SOURCES +=  tst_test__util.cpp \
    test_TimeUtil.cpp

HEADERS += \
    Test__Util.h
