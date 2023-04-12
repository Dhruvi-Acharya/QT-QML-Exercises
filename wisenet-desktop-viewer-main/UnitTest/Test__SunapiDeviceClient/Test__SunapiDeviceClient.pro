QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

HEADERS += \
    tst_test__sunapideviceclient.h

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

include($$PWD/../../WisenetDesktopCommon.pri)

TEMPLATE = app

SOURCES +=  tst_test__sunapideviceclient.cpp \
            test_AiMetaAttributeSearch.cpp \
            test_AiOcrSearch.cpp \
            test_DeviceAreaZoom.cpp \
            test_DeviceAutoTracking.cpp \
            test_DeviceAuxControl.cpp \
            test_DeviceChangeUserPassword.cpp \
            test_DeviceAlarmOutput.cpp \
            test_DeviceConfigBackupRestore.cpp \
            test_DeviceConnect.cpp \
            test_DeviceDefog.cpp \
            test_DeviceDigitalAutoTracking.cpp \
            test_DevicePTZControl.cpp \
            test_DeviceFirmwareUpdate.cpp \
            test_DevicePreset.cpp \
            test_DeviceSnapshot.cpp \
            test_DeviceTalk.cpp \
            test_EventParse.cpp \
            test_MediaOpenClose.cpp \
            test_RecordingTimeline.cpp \
            test_SmartSearch.cpp


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/ -lSunapiDeviceClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/ -lSunapiDeviceClient
else:unix: LIBS += -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/ -lSunapiDeviceClient

INCLUDEPATH += $$PWD/../../DeviceClient/SunapiDeviceClient $$PWD/../../DeviceClient/include
DEPENDPATH += $$PWD/../../DeviceClient/SunapiDeviceClient

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/libSunapiDeviceClient.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/libSunapiDeviceClient.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/SunapiDeviceClient.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/SunapiDeviceClient.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/libSunapiDeviceClient.a

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

