QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

include($$PWD/../../WisenetDesktopCommon.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MainWindow.cpp \
    ProxyWidget.cpp \
    VideoWidget.cpp \
    ViewingGrid.cpp \
    main.cpp

HEADERS += \
    MainWindow.h \
    ProxyWidget.h \
    VideoWidget.h \
    ViewingGrid.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    shaders.qrc \
    textures.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../CoreService/release/ -lCoreService
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../CoreService/debug/ -lCoreService
else:unix: LIBS += -L$$OUT_PWD/../../CoreService/ -lCoreService

INCLUDEPATH += $$PWD/../../CoreService
DEPENDPATH += $$PWD/../../CoreService

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/libCoreService.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/libCoreService.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/CoreService.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/CoreService.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/libCoreService.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../QCoreServiceManager/release/ -lQCoreServiceManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../QCoreServiceManager/debug/ -lQCoreServiceManager
else:unix: LIBS += -L$$OUT_PWD/../../QCoreServiceManager/ -lQCoreServiceManager

INCLUDEPATH += $$PWD/../../QCoreServiceManager
DEPENDPATH += $$PWD/../../QCoreServiceManager

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QCoreServiceManager/release/libQCoreServiceManager.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QCoreServiceManager/debug/libQCoreServiceManager.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QCoreServiceManager/release/QCoreServiceManager.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QCoreServiceManager/debug/QCoreServiceManager.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../QCoreServiceManager/libQCoreServiceManager.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../QMediaFramework/release/ -lQMediaFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../QMediaFramework/debug/ -lQMediaFramework
else:unix: LIBS += -L$$OUT_PWD/../../QMediaFramework/ -lQMediaFramework

INCLUDEPATH += $$PWD/../../QMediaFramework
DEPENDPATH += $$PWD/../../QMediaFramework

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QMediaFramework/release/libQMediaFramework.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QMediaFramework/debug/libQMediaFramework.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QMediaFramework/release/QMediaFramework.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../QMediaFramework/debug/QMediaFramework.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../QMediaFramework/libQMediaFramework.a

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
