QT += quick multimedia svg

CONFIG += c++11

include($$PWD/../WisenetDesktopCommon.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        MainViewModel.cpp \
        Resource/BaseTreeItemInfo.cpp \
        Resource/BaseTreeModel.cpp \
        Resource/ResourceTreeModel.cpp \
        Resource/ResourceViewModel.cpp \
        Resource/TreeItemModel.cpp \
        Resource/TreeProxyModel.cpp \
        main.cpp

RESOURCES += \
        qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += \
        $$PWD \
        $$PWD/../WisenetStyle \
        $$PWD/../WisenetMediaFramework \
        $$PWD/../WisenetCommonLibrary/WisenetLanguage \

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =


VERSION = 1.00.01
QMAKE_TARGET_COMPANY = Hanwha Techwin Co., Ltd.
QMAKE_TARGET_COPYRIGHT = Copyright. 2021. Hanwha Techwin Co., Ltd. All rights Reserved.
QMAKE_TARGET_PRODUCT = Wisenet Player

win32: {
    RC_ICONS = $$PWD/../Installer/WisenetPlayer.ico
}

macx: {
    QMAKE_TARGET_BUNDLE_PREFIX = com.HanwhaTechwin
    ICON = $$PWD/../Installer/WisenetPlayer.icns
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../WisenetMediaFramework/release/ -lWisenetMediaFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../WisenetMediaFramework/debug/ -lWisenetMediaFramework
else:unix: LIBS += -L$$OUT_PWD/../WisenetMediaFramework/ -lWisenetMediaFramework

INCLUDEPATH += $$PWD/../WisenetMediaFramework
DEPENDPATH += $$PWD/../WisenetMediaFramework

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetMediaFramework/release/libWisenetMediaFramework.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetMediaFramework/debug/libWisenetMediaFramework.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetMediaFramework/release/WisenetMediaFramework.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetMediaFramework/debug/WisenetMediaFramework.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../WisenetMediaFramework/libWisenetMediaFramework.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../WisenetCommonLibrary/release/ -lWisenetCommonLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../WisenetCommonLibrary/debug/ -lWisenetCommonLibrary
else:unix: LIBS += -L$$OUT_PWD/../WisenetCommonLibrary/ -lWisenetCommonLibrary

INCLUDEPATH += $$PWD/../WisenetCommonLibrary
DEPENDPATH += $$PWD/../WisenetCommonLibrary

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetCommonLibrary/release/libWisenetCommonLibrary.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetCommonLibrary/debug/libWisenetCommonLibrary.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetCommonLibrary/release/WisenetCommonLibrary.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetCommonLibrary/debug/WisenetCommonLibrary.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../WisenetCommonLibrary/libWisenetCommonLibrary.a

HEADERS += \
    DummyLoginViewModel.h \
    DummyUserGroupModel.h \
    MainViewModel.h \
    Resource/BaseTreeItemInfo.h \
    Resource/BaseTreeModel.h \
    Resource/ResourceTreeModel.h \
    Resource/ResourceViewModel.h \
    Resource/TreeItemModel.h \
    Resource/TreeProxyModel.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../WisenetStyle/release/ -lWisenetStyle
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../WisenetStyle/debug/ -lWisenetStyle
else:unix: LIBS += -L$$OUT_PWD/../WisenetStyle/ -lWisenetStyle

INCLUDEPATH += $$PWD/../WisenetStyle
DEPENDPATH += $$PWD/../WisenetStyle

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetStyle/release/libWisenetStyle.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetStyle/debug/libWisenetStyle.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetStyle/release/WisenetStyle.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../WisenetStyle/debug/WisenetStyle.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../WisenetStyle/libWisenetStyle.a

win32:CONFIG(release, debug|release): LIBS += -lPdh
else:win32:CONFIG(debug, debug|release): LIBS += -lPdh


WISENET_OPENSOURCE_PATH = $$PWD/../OpenSource
WISENET_CRYPTO_PATH = $$WISENET_OPENSOURCE_PATH/cryptoadapter

unix: !macx {
    LIBS += -L$$WISENET_CRYPTO_PATH/linux -lCryptoAdapter
}
