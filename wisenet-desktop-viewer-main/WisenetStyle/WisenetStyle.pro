QT -= gui
QT += qml

TEMPLATE = lib
CONFIG += staticlib

# Input
SOURCES +=

HEADERS +=

RESOURCES += \
    WisenetStyle.qrc

QML_IMPORT_PATH += \
        $$PWD \

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
