QT += quick
QT += quickcontrols2
QT += svg
QT += xml
QT += sql

CONFIG += c++11

include($$PWD/../../WisenetDesktopCommon.pri)

include($$PWD/../../OpenSource/qt/bppgrid/BppTable.pri)
include($$PWD/../../OpenSource/qt/bppgrid/BppFa.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
    BOOST_DATE_TIME_NO_LIB \
    BOOST_REGEX_NO_LIB

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        MediaController/DateAreaModel.cpp \
        MediaController/GraphAreaModel.cpp \
        MediaController/TimeAreaModel.cpp \
        alarm/AlarmModel.cpp \
        MediaController/CalendarTimelineModel.cpp \
        MediaController/MediaControlModel.cpp \
        layoutNavigator/layouttabbehaviors.cpp \
        layoutNavigator/layouttabmodel.cpp \
        login/LoginModel.cpp \
        main.cpp \
        setup/AutoDiscoveryModel.cpp \
        setup/ManualDiscoveryModel.cpp \
        tree/MainTreeItemBase.cpp \
        tree/MainTreeModel.cpp \
        tree/MainTreeSortFilterProxyModel.cpp \
        windowHandler/multiwindow.cpp \
        windowHandler/multiwindowhandler.cpp

RESOURCES += qml.qrc \
    shaders.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    MediaController/DateAreaModel.h \
    MediaController/GraphAreaModel.h \
    MediaController/TimeAreaModel.h \
    alarm/AlarmModel.h \
    MediaController/CalendarTimelineModel.h \
    MediaController/MediaControlModel.h \
    layoutNavigator/layouttabbehaviors.h \
    layoutNavigator/layouttabmodel.h \
    login/LoginModel.h \
    setup/AutoDiscoveryModel.h \
    setup/ManualDiscoveryModel.h \
    tree/MainTreeItemBase.h \
    tree/MainTreeModel.h \
    tree/MainTreeSortFilterProxyModel.h \
    windowHandler/multiwindow.h \
    windowHandler/multiwindowhandler.h


win32:CONFIG(release, debug|release): {
    LIBS += -L$$OUT_PWD/../../CoreService/release/ -lCoreService \
            -L$$OUT_PWD/../../QCoreServiceManager/release/ -lQCoreServiceManager \
            -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../../RtspClient/release/ -lRtspClient \
            -L$$OUT_PWD/../../WisenetMediaFramework/release/ -lWisenetMediaFramework
}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -L$$OUT_PWD/../../CoreService/debug/ -lCoreService \
            -L$$OUT_PWD/../../QCoreServiceManager/debug/ -lQCoreServiceManager \
            -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../../RtspClient/debug/ -lRtspClient \
            -L$$OUT_PWD/../../WisenetMediaFramework/debug/ -lWisenetMediaFramework
}
else:unix: {
    # link order is important for g++
    LIBS += -L$$OUT_PWD/../../WisenetMediaFramework/ -lWisenetMediaFramework \
            -L$$OUT_PWD/../../QCoreServiceManager/ -lQCoreServiceManager \
            -L$$OUT_PWD/../../CoreService/ -lCoreService \
            -L$$OUT_PWD/../../DeviceClient/SunapiDeviceClient/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../../RtspClient/ -lRtspClient \

}

DEPENDPATH += $$PWD/../../CoreService \
              $$PWD/../../QCoreServiceManager \
              $$PWD/../../DeviceClient/SunapiDeviceClient \
              $$PWD/../../RtspClient \
              $$PWD/../../WisenetMediaFramework


win32-g++:CONFIG(release, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/libCoreService.a \
                      $$OUT_PWD/../../QCoreServiceManager/release/libQCoreServiceManager.a \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/libSunapiDeviceClient.a \
                      $$OUT_PWD/../../RtspClient/release/libRtspClient.a \
                      $$OUT_PWD/../../WisenetMediaFramework/release/libWisenetMediaFramework.a
}
else:win32-g++:CONFIG(debug, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/libCoreService.a \
                      $$OUT_PWD/../../QCoreServiceManager/debug/libQCoreServiceManager.a \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/libSunapiDeviceClient.a \
                      $$OUT_PWD/../../RtspClient/debug/libRtspClient.a \
                      $$OUT_PWD/../../WisenetMediaFramework/debug/libWisenetMediaFramework.a
}
else:win32:!win32-g++:CONFIG(release, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/release/CoreService.lib \
                      $$OUT_PWD/../../QCoreServiceManager/release/QCoreServiceManager.lib \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/release/SunapiDeviceClient.lib \
                      $$OUT_PWD/../../RtspClient/release/RtspClient.lib \
                      $$OUT_PWD/../../WisenetMediaFramework/release/WisenetMediaFramework.lib
}
else:win32:!win32-g++:CONFIG(debug, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/debug/CoreService.lib \
                      $$OUT_PWD/../../QCoreServiceManager/debug/QCoreServiceManager.lib \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/debug/SunapiDeviceClient.lib \
                      $$OUT_PWD/../../RtspClient/debug/RtspClient.lib \
                      $$OUT_PWD/../../WisenetMediaFramework/debug/WisenetMediaFramework.lib
}
else:unix: {
    PRE_TARGETDEPS += $$OUT_PWD/../../CoreService/libCoreService.a \
                      $$OUT_PWD/../../QCoreServiceManager/libQCoreServiceManager.a \
                      $$OUT_PWD/../../DeviceClient/SunapiDeviceClient/libSunapiDeviceClient.a \
                      $$OUT_PWD/../../RtspClient/libRtspClient.a \
                      $$OUT_PWD/../../WisenetMediaFramework/libWisenetMediaFramework.a
}
