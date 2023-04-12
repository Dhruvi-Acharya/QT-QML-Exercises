TEMPLATE = subdirs


SUBDIRS += \
    WisenetCommonLibrary \
    RtspClient \
    DeviceClient/SunapiDeviceClient \
    CoreService \
    QCoreServiceManager \
    WisenetStyle \
    WisenetMediaFramework \
    GuiTest/GuiTest__ViewingGrid

# build the project sequentially as listed in SUBDIRS !
CONFIG += ordered
CoreService.depends = WisenetCommonLibrary RtspClient DeviceClient/SunapiDeviceClient
QCoreServiceManager.depends = CoreService
WisenetMediaFramework.depends = QCoreServiceManager WisenetStyle
GuiTest/GuiTest__ViewingGrid.depends = WisenetMediaFramework
