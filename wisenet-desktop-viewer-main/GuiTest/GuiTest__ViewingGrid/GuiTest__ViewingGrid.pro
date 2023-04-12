QT += core quick multimedia quickcontrols2 qml webengine

CONFIG += c++11
#CONFIG += qmltypes
#QML_IMPORT_NAME = Wisenet.MediaController
#QML_IMPORT_MAJOR_VERSION = 1
#QML_IMPORT_MINOR_VERSION = 0

include($$PWD/../../WisenetDesktopCommon.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        MainModel.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH +=  $$PWD/../WisenetMediaFramework \

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    MainModel.h


win32:CONFIG(release, debug|release): {
    LIBS += -L$$OUT_PWD/../../CoreService/release/ -lCoreService \
            -L$$OUT_PWD/../../QCoreServiceManager/release/ -lQCoreServiceManager \
            -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../../RtspClient/release/ -lRtspClient \
            -L$$OUT_PWD/../../WisenetMediaFramework/release/ -lWisenetMediaFramework \
            -L$$OUT_PWD/../../WisenetCommonLibrary/release/ -lWisenetCommonLibrary \
            -lPdh
}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -L$$OUT_PWD/../../CoreService/debug/ -lCoreService \
            -L$$OUT_PWD/../../QCoreServiceManager/debug/ -lQCoreServiceManager \
            -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../../RtspClient/debug/ -lRtspClient \
            -L$$OUT_PWD/../../WisenetMediaFramework/debug/ -lWisenetMediaFramework \
            -L$$OUT_PWD/../../WisenetCommonLibrary/debug/ -lWisenetCommonLibrary \
            -lPdh
}
else:unix: {
    # link order is important for g++
    LIBS += -L$$OUT_PWD/../../WisenetMediaFramework/ -lWisenetMediaFramework \
            -L$$OUT_PWD/../../QCoreServiceManager/ -lQCoreServiceManager \
            -L$$OUT_PWD/../../CoreService/ -lCoreService \
            -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../../RtspClient/ -lRtspClient \
            -L$$OUT_PWD/../../WisenetCommonLibrary/ -lWisenetCommonLibrary \
            -lPdh
}

DEPENDPATH += $$PWD/../../CoreService \
              $$PWD/../../QCoreServiceManager \
              $$PWD/../../DeviceClient/SunapiDeviceClient \
              $$PWD/../../RtspClient \
              $$PWD/../../WisenetMediaFramework \
              $$PWD/../../WisenetCommonLibrary


win32-g++:CONFIG(release, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/libCoreService.a \
                      $$OUT_PWD/../../QCoreServiceManager/release/libQCoreServiceManager.a \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/libSunapiDeviceClient.a \
                      $$OUT_PWD/../../RtspClient/release/libRtspClient.a \
                      $$OUT_PWD/../../WisenetMediaFramework/release/libWisenetMediaFramework.a \
                      $$OUT_PWD/../../WisenetCommonLibrary/release/ libWisenetCommonLibrary.a
}
else:win32-g++:CONFIG(debug, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/libCoreService.a \
                      $$OUT_PWD/../../QCoreServiceManager/debug/libQCoreServiceManager.a \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/libSunapiDeviceClient.a \
                      $$OUT_PWD/../../RtspClient/debug/libRtspClient.a \
                      $$OUT_PWD/../../WisenetMediaFramework/debug/libWisenetMediaFramework.a \
                      $$OUT_PWD/../../WisenetCommonLibrary/debug/ libWisenetCommonLibrary.a
}
else:win32:!win32-g++:CONFIG(release, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/CoreService.lib \
                      $$OUT_PWD/../../QCoreServiceManager/release/QCoreServiceManager.lib \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/SunapiDeviceClient.lib \
                      $$OUT_PWD/../../RtspClient/release/RtspClient.lib \
                      $$OUT_PWD/../../WisenetMediaFramework/release/WisenetMediaFramework.lib \
                      $$OUT_PWD/../../WisenetCommonLibrary/release/WisenetCommonLibrary.lib
}
else:win32:!win32-g++:CONFIG(debug, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/CoreService.lib \
                      $$OUT_PWD/../../QCoreServiceManager/debug/QCoreServiceManager.lib \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/SunapiDeviceClient.lib \
                      $$OUT_PWD/../../RtspClient/debug/RtspClient.lib \
                      $$OUT_PWD/../../WisenetMediaFramework/debug/WisenetMediaFramework.lib \
                      $$OUT_PWD/../../WisenetCommonLibrary/debug/WisenetCommonLibrary.lib
}
else:unix: {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/libCoreService.a \
                      $$OUT_PWD/../../QCoreServiceManager/libQCoreServiceManager.a \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/libSunapiDeviceClient.a \
                      $$OUT_PWD/../../RtspClient/libRtspClient.a \
                      $$OUT_PWD/../../WisenetMediaFramework/libWisenetMediaFramework.a \
                      $$OUT_PWD/../../WisenetCommonLibrary/libWisenetCommonLibrary.a
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../WisenetStyle/release/ -lWisenetStyle
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../WisenetStyle/debug/ -lWisenetStyle
else:unix: LIBS += -L$$OUT_PWD/../../WisenetStyle/ -lWisenetStyle

INCLUDEPATH += $$PWD/../../WisenetStyle
DEPENDPATH += $$PWD/../../WisenetStyle

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetStyle/release/libWisenetStyle.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetStyle/debug/libWisenetStyle.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetStyle/release/WisenetStyle.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../WisenetStyle/debug/WisenetStyle.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../WisenetStyle/libWisenetStyle.a
