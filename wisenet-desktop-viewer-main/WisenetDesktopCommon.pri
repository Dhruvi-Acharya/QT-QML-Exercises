# 테스트용 DEFINE 정의
include($$PWD/DevelopDefine.pri)

QT += core network sql multimedia

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
	BOOST_DATE_TIME_NO_LIB \
    BOOST_REGEX_NO_LIB

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

WISENET_COMMON_PATH = $$PWD/Common
WISENET_OPENSOURCE_PATH = $$PWD/OpenSource
WISENET_CRYPTO_PATH = $$WISENET_OPENSOURCE_PATH/cryptoadapter

# disable c4819 warning on visual studio
win32 : QMAKE_CXXFLAGS_WARN_ON += -wd4819
win32 : DEFINES += WIN32_LEAN_AND_MEAN

# qt5webenginecore.so .dynsym link error
unix:!macx {
QMAKE_CXX +=  -fuse-ld=gold
QMAKE_LFLAGS += -fuse-ld=gold
}

INCLUDEPATH +=  $$WISENET_OPENSOURCE_PATH/headeronly/boost_1_75_0 \
                $$WISENET_OPENSOURCE_PATH/headeronly/spdlog-1.8.2/include \
                $$WISENET_OPENSOURCE_PATH/headeronly/rapidjson-1.1.0/include \
                $$WISENET_OPENSOURCE_PATH/headeronly/toml11-3.6.0 \
                $$WISENET_OPENSOURCE_PATH/headeronly/pugixml-1.11.1/src \
                $$WISENET_OPENSOURCE_PATH/libs/smtpclient-for_qt/include \
                $$WISENET_OPENSOURCE_PATH/StatisticsClient/include \
                $$WISENET_COMMON_PATH/include \
                $$WISENET_CRYPTO_PATH/include \
                $$PWD/DeviceClient/SunapiDeviceClient \
                $$PWD/CoreService \
                $$PWD/QCoreServiceManager \
                $$PWD/WisenetMediaFramework \
                $$PWD/WisenetMediaFramework/Decoder \
                $$PWD/WisenetMediaFramework/MediaSource \
                $$PWD/WisenetMediaFramework/MediaControl \
                $$PWD/WisenetMediaFramework/MediaPlayer \
                $$PWD/WisenetMediaFramework/ViewModel \
                $$PWD/WisenetMediaFramework/MediaWriter \
                $$PWD/WisenetMediaFramework/FisheyeDewarp \
                $$PWD/RtspClient \
                $$PWD/WisenetCommonLibrary


win32 {
    INCLUDEPATH +=  $$WISENET_OPENSOURCE_PATH/NVIDIA/CUDA/v11.2/include \
                    $$WISENET_OPENSOURCE_PATH/NVIDIA/Video_Codec_SDK_11.0.10/ffnvcodec \
                    $$WISENET_OPENSOURCE_PATH/libs/openssl-1.1.1k/windows/include \
                    $$WISENET_OPENSOURCE_PATH/libs/ffmpeg-5.1/windows/include \
                    $$WISENET_OPENSOURCE_PATH/libs/libusb-1.0.26/VS2015-x64/include/ \
                    $$WISENET_OPENSOURCE_PATH/sipproxy/include \
                    $$WISENET_OPENSOURCE_PATH/IMVSDK/windows \
                    $$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13\windows\include \
                    $$WISENET_OPENSOURCE_PATH/libs/libssh2-1.10.0\windows\include \
                    $$WISENET_OPENSOURCE_PATH/libs/opencv-4.7.0/windows/include


    CONFIG(debug, debug|release){
        LIBS += -L$$WISENET_OPENSOURCE_PATH/libs/smtpclient-for_qt\windows\debug \
                -L$$WISENET_OPENSOURCE_PATH/libs/openssl-1.1.1k/windows/lib \
                -L$$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13/windows/lib \
                -L$$WISENET_OPENSOURCE_PATH/libs/ffmpeg-5.1/windows/bin \
                -L$$WISENET_OPENSOURCE_PATH/sipproxy/windows/lib/debug \
                -L$$WISENET_OPENSOURCE_PATH/IMVSDK/windows/x64 \
                -L$$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13/windows/lib \
                -L$$WISENET_OPENSOURCE_PATH/libs/libusb-1.0.26/VS2015-x64/lib \
                -L$$WISENET_OPENSOURCE_PATH/libs/libssh2-1.10.0/windows/lib \
                -L$$WISENET_OPENSOURCE_PATH/StatisticsClient/windows/debug \
                -L$$WISENET_OPENSOURCE_PATH/libs/opencv-4.7.0/windows/lib \
                -lSMTPEmail -llibcrypto -llibssl -lws2_32 -lgdi32 -ladvapi32 -lcrypt32 -luser32 \
                -lavcodec -lavdevice -lavutil -lavfilter -lavformat -lswresample -lswscale \
                -lsipproxy -lIMV1 -lldap -llber -llibusb-1.0 -llibssh2 -lStatisticsClient \
                -lopencv_core470d -lopencv_video470d -lopencv_tracking470d -lopencv_imgproc470d
    } else {
    LIBS += -L$$WISENET_OPENSOURCE_PATH/libs/smtpclient-for_qt\windows\release \
            -L$$WISENET_OPENSOURCE_PATH/libs/openssl-1.1.1k/windows/lib \
            -L$$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13/windows/lib \
            -L$$WISENET_OPENSOURCE_PATH/IMVSDK/windows/x64 \
            -L$$WISENET_OPENSOURCE_PATH/libs/ffmpeg-5.1/windows/bin \
            -L$$WISENET_OPENSOURCE_PATH/libs/libusb-1.0.26/VS2015-x64/lib \
            -L$$WISENET_OPENSOURCE_PATH/sipproxy/windows/lib/release \
            -L$$WISENET_OPENSOURCE_PATH/libs/libssh2-1.10.0/windows/lib \
            -L$$WISENET_OPENSOURCE_PATH/StatisticsClient/windows/release \
            -L$$WISENET_OPENSOURCE_PATH/libs/opencv-4.7.0/windows/lib \
            -lSMTPEmail -llibcrypto -llibssl -lws2_32 -lgdi32 -ladvapi32 -lcrypt32 -luser32 \
            -lavcodec -lavdevice -lavutil -lavfilter -lavformat -lswresample -lswscale \
            -lsipproxy -lIMV1 -lldap -llber -llibusb-1.0 -llibssh2 -lStatisticsClient \
            -lopencv_core470 -lopencv_video470 -lopencv_tracking470 -lopencv_imgproc470

    }
}
macx {
    #QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
    INCLUDEPATH +=  $$WISENET_OPENSOURCE_PATH/libs/openssl-1.1.1k/macos/include \
                    $$WISENET_OPENSOURCE_PATH/libs/ffmpeg-5.1/macos/include \
                    $$WISENET_OPENSOURCE_PATH/sipproxy/include \
                    $$WISENET_OPENSOURCE_PATH/IMVSDK/macos/Library/IMV1.framework/Headers \
                    $$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13/include \
                    $$WISENET_OPENSOURCE_PATH/libs/libusb-1.0.26/macos_10.13/include \



    LIBS += -L$$WISENET_OPENSOURCE_PATH/libs/openssl-1.1.1k/macos/lib \
            -L$$WISENET_OPENSOURCE_PATH/libs/ffmpeg-5.1/macos/lib \
            -L$$WISENET_OPENSOURCE_PATH/IMVSDK/macos/Library \
            -L$$WISENET_OPENSOURCE_PATH/libs/libusb-1.0.26/macos_10.13/lib \
            -L$$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13/macos/lib \
            -L$$WISENET_OPENSOURCE_PATH/StatisticsClient/macos \
            -lcrypto -lssl \
            -lavcodec -lavdevice -lavutil -lavfilter -lavformat -lswresample -lswscale \
            -lusb -lStatisticsClient
    LIBS += -F$$WISENET_OPENSOURCE_PATH/IMVSDK/macos/Library
    LIBS += -framework IMV1

    !contains(DEFINES, MEDIA_FILE_ONLY) {
        LIBS += -L$$WISENET_OPENSOURCE_PATH/libs/smtpclient-for_qt/macos/release \
                -L$$WISENET_OPENSOURCE_PATH/sipproxy/macos/lib \
                -lSMTPEmail \
                -lsipproxy \
                -llber -lldap
    }
}

unix:!macx {
    INCLUDEPATH +=  $$WISENET_OPENSOURCE_PATH/libs/openssl-1.1.1k/linux/include \
                    $$WISENET_OPENSOURCE_PATH/libs/ffmpeg-4.4/linux/include \
                    $$WISENET_OPENSOURCE_PATH/sipproxy/include \
                    $$WISENET_OPENSOURCE_PATH/libs/libusb-1.0.26/ubuntu_20.04/include \
                    $$WISENET_OPENSOURCE_PATH/IMVSDK/linux \
                    $$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13/linux/include

    LIBS += -L$$WISENET_OPENSOURCE_PATH/libs/openssl-1.1.1k/linux/lib \
            -L$$WISENET_OPENSOURCE_PATH/libs/ffmpeg-4.4/linux/lib \
            -L$$WISENET_OPENSOURCE_PATH/sipproxy/linux/lib \
            -L$$WISENET_OPENSOURCE_PATH/IMVSDK/linux/64bits \
            -L$$WISENET_OPENSOURCE_PATH/libs/libusb-1.0.26/ubuntu_20.04/lib \
            -L$$WISENET_OPENSOURCE_PATH/libs/smtpclient-for_qt/ubuntu_20.04/release \
            -L$$WISENET_OPENSOURCE_PATH/libs/openldap-2.5.13/linux/lib \
            -L$$WISENET_OPENSOURCE_PATH/StatisticsClient/linux \
            -lcrypto -lssl  \
            -lavcodec -lavdevice -lavutil -lavfilter -lavformat -lswresample -lswscale \
            -lsipproxy -lIMV1 -lusb -lSMTPEmail \
            -llber -lldap -lStatisticsClient
}


DEPENDPATH += $$WISENET_CRYPTO_PATH/include

win32{
    CONFIG(debug, debug|release) {
        PRE_TARGETDEPS += $$WISENET_CRYPTO_PATH/windows/debug/CryptoAdapter.lib
        LIBS += -L$$WISENET_CRYPTO_PATH/windows/debug/ -lCryptoAdapter
    }
    CONFIG(release, debug|release) {
        PRE_TARGETDEPS += $$WISENET_CRYPTO_PATH/windows/release/CryptoAdapter.lib
        LIBS += -L$$WISENET_CRYPTO_PATH/windows/release/ -lCryptoAdapter
    }
    LIBS += -lPdh
}
macx{
    PRE_TARGETDEPS += $$WISENET_CRYPTO_PATH/macos/libCryptoAdapter.a
    LIBS += -L$$WISENET_CRYPTO_PATH/macos -lCryptoAdapter
}
unix: !macx {
    PRE_TARGETDEPS += $$WISENET_CRYPTO_PATH/linux/libCryptoAdapter.a
    LIBS += -L$$WISENET_CRYPTO_PATH/linux -lCryptoAdapter
}

SOURCES +=

QMAKE_PROJECT_DEPTH = 0
