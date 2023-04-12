TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++11

include($$PWD/../../WisenetDesktopCommon.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DeviceAddProfile.cpp \
    DeviceAreaZoom.cpp \
    DeviceAutoTracking.cpp \
    DeviceCertificate.cpp \
    DeviceChangeConfigUri.cpp \
    DeviceChangeUserPassword.cpp \
    DeviceAlarmOutput.cpp \
    DeviceChannelFirmwareUpdate.cpp \
    DeviceConfigBackupRestore.cpp \
    DeviceControlDigitalAutoTracking.cpp \
    DeviceDefog.cpp \
    DeviceExtendPasswordExpiration.cpp \
    DeviceFirmwareUpdate.cpp \
    DeviceGetPasswordExpiry.cpp \
    DeviceMediaOpenClose.cpp \
    DeviceMediaTalk.cpp \
    DeviceMetaAttributeSearch.cpp \
    DeviceOcrSearch.cpp \
    DevicePTZControl.cpp \
    DeviceObjectComposer.cpp \
    DevicePTZGroup.cpp \
    DevicePTZSwing.cpp \
    DevicePTZTour.cpp \
    DevicePTZTrace.cpp \
    DevicePreset.cpp \
    DeviceSearch.cpp \
    DeviceSmartSearch.cpp \
    DeviceSnapshot.cpp \
    DeviceStorageInfo.cpp \
    DeviceTextSearch.cpp \
    DeviceThumbnail.cpp \
    DeviceUrlSnapShot.cpp \
    EasyConnection.cpp \
    EasyConnectionLogger.cpp \
    EventStatusDispatcher.cpp \
    HttpSession.cpp \
    MediaAgent.cpp \
    PosDataDispatcher.cpp \
    SunapiDeviceClient.cpp \
    DeviceConnect.cpp \
    DeviceDisconnect.cpp \
    SunapiDeviceClientImpl.cpp \
    cgis/AttributesCgi.cpp \
    cgis/BaseCommand.cpp \
    cgis/NameValueText.cpp \
    iPolisDDNS.cpp

HEADERS += \
    ConfigUriComposer.h \
    DeviceCertificate.h \
    DeviceObjectComposer.h \
    EasyConnection.h \
    EasyConnectionLogger.h \
    EventStatusDispatcher.h \
    HttpSession.h \
    HttpStream.h \
    MediaAgent.h \
    PosDataDispatcher.h \
    SunapiDeviceClient.h \
    SunapiDeviceClientImpl.h \
    SunapiDeviceClientLogSettings.h \
    SunapiDeviceClientUtil.h \
    cgis/AttributesCgi.h \
    cgis/BaseCommand.h \
    cgis/EventSourcesCgi.h \
    cgis/EventStatusCgi.h \
    cgis/IoCgi.h \
    cgis/MediaCgi.h \
    cgis/NameValueText.h \
    cgis/NetworkCgi.h \
    cgis/PwInitCgi.h \
    cgis/RecordingCgi.h \
    cgis/SecurityCgi.h \
    cgis/SunapiSyntaxBuilder.h \
    cgis/SystemCgi.h \
    iPolisDDNS.h

#INCLUDEPATH += $$PWD/../../RtspClient

win32:QMAKE_CXXFLAGS += /bigobj

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
