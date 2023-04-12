QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle


TEMPLATE = app

HEADERS += \
    Test__CommonLibrary.h

SOURCES +=  \
    test_createUserToCloud.cpp \
    tst_test__commonlibrary.cpp

include($$PWD/../../WisenetDesktopCommon.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../WisenetCommonLibrary/release/ -lWisenetCommonLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../WisenetCommonLibrary/debug/ -lWisenetCommonLibrary
else:unix: LIBS += -L$$OUT_PWD/../../WisenetCommonLibrary/ -lWisenetCommonLibrary

INCLUDEPATH += $$PWD/../../WisenetCommonLibrary
DEPENDPATH += $$PWD/../../WisenetCommonLibrary

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetCommonLibrary/release/libWisenetCommonLibrary.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetCommonLibrary/debug/libWisenetCommonLibrary.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetCommonLibrary/release/WisenetCommonLibrary.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetCommonLibrary/debug/WisenetCommonLibrary.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../WisenetCommonLibrary/libWisenetCommonLibrary.a
