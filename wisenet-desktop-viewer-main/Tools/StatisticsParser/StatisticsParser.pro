QT += quick

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

OPEN_SOURCE_PATH = $$PWD/../../OpenSource

include($$PWD/../../OpenSource/qt/qtxlsxwriter/src/xlsx/qtxlsx.pri)

SOURCES += \
    Cpp/MainViewModel.cpp \
    StatisticsParser.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Cpp/FilePathConveter.h \
    Cpp/MainViewModel.h

INCLUDEPATH += $$OPEN_SOURCE_PATH/cryptoadapter/include \
            += $$OPEN_SOURCE_PATH/libs/openssl-1.1.1k/windows/include
DEPENDPATH += $$OPEN_SOURCE_PATH/cryptoadapter/include

win32{
    CONFIG(release, debug|release){
        LIBS += -L$$OPEN_SOURCE_PATH/cryptoadapter/windows/release/ \
                -L$$OPEN_SOURCE_PATH/libs/openssl-1.1.1k/windows/lib \
                -lCryptoAdapter -llibcrypto -llibssl
    }

    CONFIG(debug, debug|release){
        LIBS += -L$$OPEN_SOURCE_PATH/cryptoadapter/windows/debug/ \
                -L$$OPEN_SOURCE_PATH/libs/openssl-1.1.1k/windows/lib \
                -lCryptoAdapter -llibcrypto -llibssl
    }
}
