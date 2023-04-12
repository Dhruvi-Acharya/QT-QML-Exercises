QT += testlib network
QT -= gui
QT += sql

HEADERS += \
    tst_test__coreservice.h


CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

include($$PWD/../../WisenetDesktopCommon.pri)

TEMPLATE = app

SOURCES +=  tst_test__coreservice.cpp \
    test_AudtiLog.cpp \
    test_Bookmark.cpp \
    test_Device.cpp \
    test_DiscoverDevice.cpp \
    test_Layout.cpp \
    test_Login.cpp \
    test_SequenceLayout.cpp \
    test_ServiceStatus.cpp \
    test_User.cpp \
    test_UserRole.cpp \
    test_Webpage.cpp


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../CoreService/release/ -lCoreService
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../CoreService/debug/ -lCoreService
else:unix: LIBS += -L$$OUT_PWD/../../CoreService/ -lCoreService

INCLUDEPATH += $$PWD/../../CoreService $$PWD/../../DeviceClient/include
DEPENDPATH += $$PWD/../../CoreService

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/libCoreService.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/libCoreService.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/CoreService.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/CoreService.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/libCoreService.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/ -lSunapiDeviceClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/ -lSunapiDeviceClient
else:unix: LIBS += -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/ -lSunapiDeviceClient

INCLUDEPATH += $$PWD/../../DeviceClient/SunapiDeviceClient
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
