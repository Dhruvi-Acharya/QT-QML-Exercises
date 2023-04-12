TEMPLATE = subdirs


SUBDIRS += \
    $$PWD/WisenetCommonLibrary \
    $$PWD/WisenetMediaFramework \
    $$PWD/RtspClient \
    $$PWD/DeviceClient/SunapiDeviceClient \
    $$PWD/CoreService \
    $$PWD/QCoreServiceManager \
    #GuiTest \
    $$PWD/UnitTest \
    $$PWD/WisenetStyle \
    $$PWD/WisenetViewer

# build the project sequentially as listed in SUBDIRS !
# CONFIG += ordered
WisenetMediaFramework.depends = QCoreServiceManager
CoreService.depends = RtspClient DeviceClient/SunapiDeviceClient WisenetCommonLibrary
UnitTest.depends = CoreService RtspClient DeviceClient/SunapiDeviceClient WisenetCommonLibrary
QCoreServiceManager.depends = RtspClient DeviceClient/SunapiDeviceClient CoreService WisenetCommonLibrary
GuiTest.depends = CoreService RtspClient DeviceClient/SunapiDeviceClient QCoreServiceManager WisenetMediaFramework WisenetCommonLibrary
WisenetViewer.depends = CoreService RtspClient DeviceClient/SunapiDeviceClient QCoreServiceManager WisenetMediaFramework WisenetCommonLibrary
