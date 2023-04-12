CONFIG -= qt

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

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RtpSession/BackChannelRtpSink.cpp \
    RtpSession/RtpSession.cpp \
    RtpSession/RtpUdpSocket.cpp \
    RtpSource/AacRtpSource.cpp \
    RtpSource/BitStream.cpp \
    RtpSource/H264RtpSource.cpp \
    RtpSource/H265RtpSource.cpp \
    RtpSource/MjpegRtpSource.cpp \
    RtpSource/RtpAudioSource.cpp \
    RtpSource/RtpMetaDataSource.cpp \
    RtpSource/RtpMetaImageSource.cpp \
    RtpSource/RtpSource.cpp \
    RtpSource/RtpVideoSource.cpp \
    RtspClient.cpp \
    RtspSession/RtspHttpSocket.cpp \
    RtspSession/RtspHttpSslSocket.cpp \
    RtspSession/RtspSession.Command.cpp \
    RtspSession/RtspSession.cpp \
    RtspSession/RtspSocket.cpp \
    SdpParser.cpp \
    SecurityUtil.cpp

HEADERS += \
    ../Common/include/Media/MediaSourceFrame.h \
    RtpSession/BackChannelRtpSink.h \
    RtpSession/RtpSession.h \
    RtpSession/RtpUdpSocket.h \
    RtpSource/AacRtpSource.hpp \
    RtpSource/BitStream.hpp \
    RtpSource/H264RtpSource.hpp \
    RtpSource/H265RtpSource.hpp \
    RtpSource/MjpegRtpSource.hpp \
    RtpSource/RtpAudioSource.hpp \
    RtpSource/RtpMetaDataSource.hpp \
    RtpSource/RtpMetaImageSource.hpp \
    RtpSource/RtpSource.h \
    RtpSource/RtpVideoSource.hpp \
    RtspClient.h \
    RtspRequestResponse.h \
    RtspSession/RtspHttpSocket.h \
    RtspSession/RtspHttpSslSocket.h \
    RtspSession/RtspSession.h \
    RtspSession/RtspSocket.h \
    SdpParser.h \
    SecurityUtil.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
