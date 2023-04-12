QT += testlib
QT -= gui
QT += multimedia
QT += concurrent

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_test__rtspclient.cpp

include($$PWD/../../WisenetDesktopCommon.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../RtspClient/release/ -lRtspClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../RtspClient/debug/ -lRtspClient
else:unix: LIBS += -L$$OUT_PWD/../../RtspClient/ -lRtspClient

INCLUDEPATH += $$PWD/../../RtspClient
DEPENDPATH += $$PWD/../../RtspClient

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RtspClient/release/libRtspClient.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RtspClient/debug/libRtspClient.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RtspClient/release/RtspClient.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../RtspClient/debug/RtspClient.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../RtspClient/libRtspClient.a
