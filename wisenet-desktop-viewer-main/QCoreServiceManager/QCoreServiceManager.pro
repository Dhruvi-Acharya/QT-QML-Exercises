QT -= gui
QT += sql

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11
#CONFIG += c++17

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
    QCoreServiceDatabase.cpp \
    QCoreServiceManager.cpp \
    QLocalSettings.cpp \
    QUpdateManager.cpp

HEADERS += \
    QCoreServiceDatabase.h \
    QCoreServiceManager.h \
    QCoreServiceReply.h \
    QLocalSettings.h \
    QUpdateManager.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
