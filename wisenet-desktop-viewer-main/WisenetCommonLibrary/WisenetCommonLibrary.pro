QT -= gui
QT += qml

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include($$PWD/../WisenetDesktopCommon.pri)

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
    CloudHttps.cpp \
    CloudService.cpp \
    LogoManager.cpp \
    QLocaleManager.cpp \
    ResourceMonitor.cpp \
    S1DDNS.cpp \
    S1Sip.cpp \
    SecurityManager.cpp \
    SignatureVerifier.cpp \
    VersionManager.cpp

HEADERS += \
    AppLocalDataLocation.h \
    CloudHttps.h \
    CloudJsonStructure.h \
    CloudService.h \
    FilePathConveter.h \
    LogoManager.h \
    ResourceMonitor.h \
    S1DDNS.h \
    S1SerialConverter.h \
    S1Sip.h \
    SecurityManager.h \
    SignatureVerifier.h \
    LanguageManager.h \
    QLocaleManager.h \
    StorageInfo.h \
    VersionManager.h \
    WisenetViewerDefine.h

# Default rules for deployment.

win32:CONFIG(release, debug|release): {
    LIBS += -lPdh
}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -lPdh
}

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    WisenetLanguage/WisenetLanguage.qrc

QML_IMPORT_PATH += \
        $$PWD \
        $$PWD/WisenetLanguage \
