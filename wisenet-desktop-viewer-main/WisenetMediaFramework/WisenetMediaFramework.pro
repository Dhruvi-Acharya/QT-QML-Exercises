QT -= gui
QT += core multimedia qml concurrent quick
!contains(DEFINES, MEDIA_FILE_ONLY): QT += webengine

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include($$PWD/../WisenetDesktopCommon.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioInputManager.cpp \
    AudioOutputManager.cpp \
    ControllerManager.cpp \
    ControllerModel.cpp \
    ControllerSpc2000.cpp \
    Decoder/PcmuEncoder.cpp \
    Decoder/VideoDecoder.cpp \
    Decoder/VideoEncoder.cpp \
    DecoderManager.cpp \
    FisheyeDewarp/ImvDewarp.cpp \
    Masking/CvAutoTracker.cpp \
    Masking/CvMaskPainter.cpp \
    Masking/MaskItem.cpp \
    Masking/MaskItemListModel.cpp \
    Masking/MaskingExportProgressModel.cpp \
    Masking/MaskingTranscoder.cpp \
    MediaControl/ExportMediaControl.cpp \
    MediaControl/PosMediaControl.cpp \
    MediaControl/SecMediaControl.cpp \
    MediaControl/WnmMediaControl.cpp \
    MediaController/BookmarkModel.cpp \
    MediaController/CalendarTimelineModel.cpp \
    MediaController/DateAreaModel.cpp \
    MediaController/ExportVideoModel.cpp \
    MediaController/ExportVideoProgressModel.cpp \
    MediaController/GraphAreaModel.cpp \
    MediaController/MediaControlModel.cpp \
    MediaController/MultiGraphAreaModel.cpp \
    MediaController/PlaybackSpeedModel.cpp \
    MediaController/RecordTypeFilterViewModel.cpp \
    MediaController/ThumbnailImage.cpp \
    MediaController/ThumbnailModel.cpp \
    MediaController/TimeAreaModel.cpp \
    MediaSource/CoreServiceFrameSource.cpp \
    MediaSource/FFmpegFrameSource.cpp \
    MediaSource/FFmpegReader.cpp \
    MediaSource/SafeMediaSourceFrameHandler.cpp \
    MediaPlayer/WisenetMediaPlayer.cpp \
    MediaPlayer/WisenetVideoBuffer.cpp \
    MediaControl/CoreServiceMediaControl.cpp \
    MediaControl/FFmpegMediaControl.cpp \
    Decoder/AudioDecoder.cpp \
    Decoder/VideoScaler.cpp \
    MediaSource/SecReader.cpp \
    MediaSource/WnmReader.cpp \
    MediaSourceFrameQueue.cpp \
    MediaThreadPool.cpp \
    MediaWriter/DigitalSignature.cpp \
    MediaWriter/ExportMediaItem.cpp \
    MediaWriter/ExportProgressItem.cpp \
    MediaWriter/FFmpegFileOutContext.cpp \
    MediaWriter/FFmpegFileWriter.cpp \
    MediaWriter/FFmpegStreamInfo.cpp \
    MediaWriter/FileWriteManager.cpp \
    MediaWriter/FileWriterBase.cpp \
    MediaWriter/WnmFileOutContext.cpp \
    MediaWriter/WnmFileWriter.cpp \
    PosMetaManager.cpp \
    ViewModel/DragItemListModel.cpp \
    ViewModel/DragItemModel.cpp \
    ViewModel/MediaLayoutItemViewModel.cpp \
    ViewModel/MediaLayoutViewModel.cpp \
    ViewModel/PtzPresetItemModel.cpp

HEADERS += \
    AudioInputManager.h \
    AudioOutputManager.h \
    ControllerManager.h \
    ControllerModel.h \
    ControllerSpc2000.h \
    Decoder/PcmuEncoder.h \
    Decoder/VideoDecoder.h \
    Decoder/VideoEncoder.h \
    DecoderManager.h \
    FisheyeDewarp/ImvDewarp.h \
    Masking/CvAutoTracker.h \
    Masking/CvMaskPainter.h \
    Masking/MaskItem.h \
    Masking/MaskItemListModel.h \
    Masking/MaskingExportProgressModel.h \
    Masking/MaskingTranscoder.h \
    MediaControl/ExportMediaControl.h \
    MediaControl/PosMediaControl.h \
    MediaControl/SecMediaControl.h \
    MediaControl/WnmMediaControl.h \
    MediaController/BookmarkModel.h \
    MediaController/CalendarTimelineModel.h \
    MediaController/DateAreaModel.h \
    MediaController/ExportVideoModel.h \
    MediaController/ExportVideoProgressModel.h \
    MediaController/GraphAreaModel.h \
    MediaController/MediaControlModel.h \
    MediaController/MultiGraphAreaModel.h \
    MediaController/PlaybackSpeedModel.h \
    MediaController/RecordTypeFilterViewModel.h \
    MediaController/ThumbnailImage.h \
    MediaController/ThumbnailModel.h \
    MediaController/TimeAreaModel.h \
    MediaPlayer/WisenetMediaParam.h \
    MediaSource/CoreServiceFrameSource.h \
    MediaSource/FFmpegFrameSource.h \
    MediaSource/FFmpegReader.h \
    MediaSource/FrameSourceBase.h \
    MediaSource/SafeMediaSourceFrameHandler.h \
    MediaPlayer/WisenetMediaPlayer.h \
    MediaPlayer/WisenetVideoBuffer.h \
    MediaControl/FFmpegMediaControl.h \
    MediaControl/CoreServiceMediaControl.h \
    MediaControl/MediaControlBase.h \
    Decoder/BaseDecoder.h \
    Decoder/AudioDecoder.h \
    Decoder/VideoScaler.h \
    FFmpegStruct.h \
    MediaSource/SecReader.h \
    MediaSource/WnmReader.h \
    MediaSourceFrameQueue.h \
    MediaThreadPool.h \
    MediaRunnable.h \
    MediaWriter/DigitalSignature.h \
    MediaWriter/ExportMediaItem.h \
    MediaWriter/ExportProgressItem.h \
    MediaWriter/FFmpegFileOutContext.h \
    MediaWriter/FFmpegFileWriter.h \
    MediaWriter/FFmpegStreamInfo.h \
    MediaWriter/FFmpegWrapper.h \
    MediaWriter/FileWriteManager.h \
    PosMetaManager.h \
    OpenLayoutIItem.h \
    MediaWriter/FileWriterBase.h \
    MediaWriter/WnmFileOutContext.h \
    MediaWriter/WnmFileWriter.h \
    MediaWriter/WnmStruct.h \
    Runnable.h \
    VideoFilterStruct.h \
    ViewModel/DragItemListModel.h \
    ViewModel/DragItemModel.h \
    ViewModel/MediaLayoutItemViewModel.h \
    ViewModel/MediaLayoutViewModel.h \
    ViewModel/PtzPresetItemModel.h


#win32 {
#    SOURCES += Decoder/CudaManager.cpp Decoder/NvVideoDecoder.cpp
#    HEADERS += Decoder/CudaManager.h Decoder/NvVideoDecoder.h
#}

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DeviceClient/SunapiDeviceClient/release/ -lSunapiDeviceClient
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DeviceClient/SunapiDeviceClient/debug/ -lSunapiDeviceClient
#else:unix: LIBS += -L$$OUT_PWD/../DeviceClient/SunapiDeviceClient/ -lSunapiDeviceClient

#INCLUDEPATH += $$PWD/../DeviceClient/SunapiDeviceClient $$PWD/MediaWriter
#DEPENDPATH += $$PWD/../DeviceClient/SunapiDeviceClient

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DeviceClient/SunapiDeviceClient/release/libSunapiDeviceClient.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DeviceClient/SunapiDeviceClient/debug/libSunapiDeviceClient.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DeviceClient/SunapiDeviceClient/release/SunapiDeviceClient.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DeviceClient/SunapiDeviceClient/debug/SunapiDeviceClient.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../DeviceClient/SunapiDeviceClient/libSunapiDeviceClient.a

RESOURCES += \
    WisenetMediaFramework.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += \
        $$PWD \
        $$PWD/../WisenetStyle \

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
