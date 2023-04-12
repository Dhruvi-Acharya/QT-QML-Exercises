TEMPLATE = subdirs

SUBDIRS += \
    $$PWD/WisenetCommonLibrary \
    $$PWD/RtspClient \
    $$PWD/DeviceClient/SunapiDeviceClient \
    $$PWD/CoreService \
    $$PWD/QCoreServiceManager \
    $$PWD/WisenetStyle \
    $$PWD/WisenetMediaFramework \
    $$PWD/WisenetViewer

# build the project sequentially as listed in SUBDIRS !
CONFIG += ordered

CoreService.depends = RtspClient DeviceClient/SunapiDeviceClient WisenetCommonLibrary
QCoreServiceManager.depends = RtspClient DeviceClient/SunapiDeviceClient CoreService WisenetCommonLibrary
WisenetMediaFramework.depends = QCoreServiceManager WisenetStyle
WisenetViewer.depends = CoreService RtspClient DeviceClient/SunapiDeviceClient QCoreServiceManager WisenetMediaFramework WisenetCommonLibrary WisenetStyle
