QT += quick quickcontrols2 qml webengine concurrent svg
CONFIG += c++11
#just for debugging
#CONFIG += console

#CONFIG += c++17
win32 : QMAKE_LFLAGS_RELEASE+=/MAP
win32 : QMAKE_CFLAGS_RELEASE += /Zi
win32 : QMAKE_LFLAGS_RELEASE +=/debug /opt:ref

include($$PWD/../WisenetDesktopCommon.pri)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Cpp/HealthMonitoringParentWindow.cpp \
        Cpp/MaskingParentWindow.cpp \
        Cpp/Model/BaseTreeItemInfo.cpp \
        Cpp/Model/BaseTreeModel.cpp \
        Cpp/Model/BestShotItemModel.cpp \
        Cpp/Model/BestShotListModel.cpp \
        Cpp/Model/BookmarkItemModel.cpp \
        Cpp/Model/BookmarkListModel.cpp \
        Cpp/Model/BookmarkListSortFilterProxyModel.cpp \
        Cpp/Model/ContextMenuGroupListModel.cpp \
        Cpp/Model/Dashboard/DashboardCameraSortFilterProxyModel.cpp \
        Cpp/Model/Dashboard/DashboardDeviceErrorListModel.cpp \
        Cpp/Model/Dashboard/DashboardDeviceTreeModel.cpp \
        Cpp/Model/Dashboard/DashboardErrorListModel.cpp \
        Cpp/Model/Dashboard/DashboardErrorListSortProxyModel.cpp \
        Cpp/Model/Dashboard/DashboardGroupTreeModel.cpp \
        Cpp/Model/DeviceListModel.cpp \
        Cpp/Model/DeviceTreeItemInfo.cpp \
        Cpp/Model/EventListFilterProxyModel.cpp \
        Cpp/Model/EventListModel.cpp \
        Cpp/Model/EventModel.cpp \
        Cpp/Model/HealthMonitoring/AlertLogListModel.cpp \
        Cpp/Model/HealthMonitoring/AlertLogListSortFilterProxyModel.cpp \
        Cpp/Model/HealthMonitoring/AlertLogModel.cpp \
        Cpp/Model/HealthMonitoring/HealthMonitoringCameraListModel.cpp \
        Cpp/Model/HealthMonitoring/HealthMonitoringCameraListSortFilterProxyModel.cpp \
        Cpp/Model/HealthMonitoring/HealthMonitoringCameraModel.cpp \
        Cpp/Model/HealthMonitoring/HealthMonitoringDeviceListModel.cpp \
        Cpp/Model/HealthMonitoring/HealthMonitoringDeviceListSortFilterProxyModel.cpp \
        Cpp/Model/HealthMonitoring/HealthMonitoringDeviceModel.cpp \
        Cpp/Model/ImageItem.cpp \
        Cpp/Model/LayoutTabListModel.cpp \
        Cpp/Model/LayoutTabModel.cpp \
        Cpp/Model/LayoutTreeItemInfo.cpp \
        Cpp/Model/LayoutTreeModel.cpp \
        Cpp/Model/ResourceTreeModel.cpp \
        Cpp/Model/SequenceListModel.cpp \
        Cpp/Model/SetupTreeNode.cpp \
        Cpp/Model/ShortcutBoxListModel.cpp \
        Cpp/Model/TreeItemModel.cpp \
        Cpp/Model/TreeProxyModel.cpp \
        Cpp/Model/UserGroupModel.cpp \
        Cpp/PopupParentWindow.cpp \
        Cpp/ProcessModel.cpp \
        Cpp/SearchParentWindow.cpp \
        Cpp/SetupParentWindow.cpp \
        Cpp/Utility/PasswordStrengthChecker.cpp \
        Cpp/Utility/PasswordStrengthCheckerS1.cpp \
        Cpp/ViewModel/Bookmark/BookmarkListViewModel.cpp \
        Cpp/ViewModel/Dashboard/DashboardViewModel.cpp \
        Cpp/ViewModel/DeviceFilterTree/DeviceFilterTreeSourceModel.cpp \
        Cpp/ViewModel/DeviceSelectionTree/DeviceAlarmInTreeSourceModel.cpp \
        Cpp/ViewModel/DeviceSelectionTree/DeviceAlarmOutTreeSourceModel.cpp \
        Cpp/ViewModel/DeviceSelectionTree/DeviceEmailTreeSourceModel.cpp \
        Cpp/ViewModel/DeviceSelectionTree/DeviceEventTreeSourceModel.cpp \
        Cpp/ViewModel/Event/EventListViewModel.cpp \
        Cpp/ViewModel/HealthMonitoring/HealthMonitoringViewModel.cpp \
        Cpp/ViewModel/ObjectSearch/BaseFilterModel.cpp \
        Cpp/ViewModel/ObjectSearch/BestShotViewModel.cpp \
        Cpp/ViewModel/ObjectSearch/BestshotFilterTooltipModel.cpp \
        Cpp/ViewModel/ObjectSearch/GridListThread.cpp \
        Cpp/ViewModel/ObjectSearch/GridListViewModel.cpp \
        Cpp/ViewModel/ObjectSearch/ObjectSearchFilterViewModel.cpp \
        Cpp/ViewModel/ObjectSearch/ObjectSearchViewModel.cpp \
        Cpp/ViewModel/ResourceUsage/ResourceUsageViewModel.cpp \
        Cpp/ViewModel/Setup/Device/ChannelListViewModel.cpp \
        Cpp/ViewModel/Setup/Device/DeviceListViewModel.cpp \
        Cpp/ViewModel/Setup/Device/AutoDiscoveryViewModel.cpp \
        Cpp/ViewModel/Setup/Device/ManualDiscoveryViewModel.cpp \
        Cpp/ViewModel/DeviceSelectionTree/DevicePhysicalTreeSourceModel.cpp \
        Cpp/ViewModel/DeviceSelectionTree/DeviceSelectionTreeSourceModel.cpp \
        Cpp/ViewModel/Setup/DeviceMaintenance/DeviceConfigViewModel.cpp \
        Cpp/ViewModel/Setup/Event/AddEventRuleViewModel.cpp \
        Cpp/ViewModel/Setup/Event/EventEmailViewModel.cpp \
        Cpp/ViewModel/Setup/Event/EventNotificationsViewModel.cpp \
        Cpp/ViewModel/Setup/Event/EventRulesViewModel.cpp \
        Cpp/ViewModel/EventFilterTree/EventFilterTreeSourceModel.cpp \
        Cpp/ViewModel/EventLog/EventLogThread.cpp \
        Cpp/ViewModel/EventLog/EventLogViewModel.cpp \
        Cpp/ViewModel/InitialPasswordSettingViewModel.cpp \
        Cpp/ViewModel/Layout/LayoutNavigationViewModel.cpp \
        Cpp/ViewModel/LoginViewModel.cpp \
        Cpp/ViewModel/MainViewModel.cpp \
        Cpp/ViewModel/MenuViewModel.cpp \
        Cpp/ViewModel/Notification/NotificationViewModel.cpp \
        Cpp/ViewModel/ResourceViewModel.cpp \
        Cpp/MultiWindow.cpp \
        Cpp/MultiWindowHandler.cpp \
        Cpp/ViewModel/Sequence/SequenceAddViewModel.cpp \
        Cpp/ViewModel/Setup/Device/DeviceListTreeSourceModel.cpp \
        Cpp/ViewModel/Setup/Event/EventScheduleViewModel.cpp \
        Cpp/ViewModel/Setup/System/LocalSettingViewModel.cpp \
        Cpp/ViewModel/Setup/System/SystemMaintenanceViewModel.cpp \
        Cpp/ViewModel/Setup/User/LdapUserViewModel.cpp \
        Cpp/ViewModel/SmartSearch/SmartSearchViewModel.cpp \
        Cpp/ViewModel/SystemAdministration/AuditLogThread.cpp \
        Cpp/ViewModel/SystemAdministration/AuditLogViewModel.cpp \
        Cpp/ViewModel/Setup/SetupMainTreeFilterProxyModel.cpp \
        Cpp/ViewModel/Setup/SetupMainTreeViewModel.cpp \
        Cpp/ViewModel/Setup/User/AddUserGroupViewModel.cpp \
        Cpp/ViewModel/Setup/User/AddUserViewModel.cpp \
        Cpp/ViewModel/Setup/User/UserGroupViewModel.cpp \
        Cpp/ViewModel/Setup/User/UserLayoutTreeSourceModel.cpp \
        Cpp/ViewModel/Setup/User/UserListViewModel.cpp \
        Cpp/ViewModel/SystemAdministration/UserFilterTreeSourceModel.cpp \
        Cpp/ViewModel/SystemLog/SystemLogThread.cpp \
        Cpp/ViewModel/SystemLog/SystemLogViewModel.cpp \
        Cpp/ViewModel/TextSearch/TextItemViewModel.cpp \
        Cpp/ViewModel/TextSearch/TextSearchViewModel.cpp \
        Cpp/ViewModel/Webpage/WebpageAddViewModel.cpp \
        Cpp/WisenetViewer.cpp \
        main.cpp

RESOURCES += qml.qrc \


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += \
        $$PWD \
        $$PWD/.. \
        $$PWD/../WisenetStyle \
        $$PWD/../WisenetMediaFramework \
        $$PWD/../WisenetMediaFramework/MediaController \
        $$PWD/../WisenetCommonLibrary/WisenetLanguage \


# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH += \
        $$PWD

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Cpp/HealthMonitoringParentWindow.h \
    Cpp/MaskingParentWindow.h \
    Cpp/Model/BaseTreeItemInfo.h \
    Cpp/Model/BaseTreeModel.h \
    Cpp/Model/BestShotItemModel.h \
    Cpp/Model/BestShotListModel.h \
    Cpp/Model/BookmarkItemModel.h \
    Cpp/Model/BookmarkListModel.h \
    Cpp/Model/BookmarkListSortFilterProxyModel.h \
    Cpp/Model/ContextMenuGroupListModel.h \
    Cpp/Model/Dashboard/DashboardCameraSortFilterProxyModel.h \
    Cpp/Model/Dashboard/DashboardDeviceErrorListModel.h \
    Cpp/Model/Dashboard/DashboardDeviceTreeModel.h \
    Cpp/Model/Dashboard/DashboardErrorListModel.h \
    Cpp/Model/Dashboard/DashboardErrorListSortProxyModel.h \
    Cpp/Model/Dashboard/DashboardGroupTreeModel.h \
    Cpp/Model/DeviceListModel.h \
    Cpp/Model/DeviceTreeItemInfo.h \
    Cpp/Model/EventListFilterProxyModel.h \
    Cpp/Model/EventListModel.h \
    Cpp/Model/EventModel.h \
    Cpp/Model/HealthMonitoring/AlertLogListModel.h \
    Cpp/Model/HealthMonitoring/AlertLogListSortFilterProxyModel.h \
    Cpp/Model/HealthMonitoring/AlertLogModel.h \
    Cpp/Model/HealthMonitoring/HealthMonitoringCameraListModel.h \
    Cpp/Model/HealthMonitoring/HealthMonitoringCameraListSortFilterProxyModel.h \
    Cpp/Model/HealthMonitoring/HealthMonitoringCameraModel.h \
    Cpp/Model/HealthMonitoring/HealthMonitoringDeviceListModel.h \
    Cpp/Model/HealthMonitoring/HealthMonitoringDeviceListSortFilterProxyModel.h \
    Cpp/Model/HealthMonitoring/HealthMonitoringDeviceModel.h \
    Cpp/Model/ImageItem.h \
    Cpp/Model/LayoutTabListModel.h \
    Cpp/Model/LayoutTabModel.h \
    Cpp/Model/LayoutTreeItemInfo.h \
    Cpp/Model/LayoutTreeModel.h \
    Cpp/Model/QMetaAttribute.h \
    Cpp/Model/ResourceTreeModel.h \
    Cpp/Model/SequenceListModel.h \
    Cpp/Model/SetupTreeNode.h \
    Cpp/Model/ShortcutBoxListModel.h \
    Cpp/Model/TreeItemModel.h \
    Cpp/Model/TreeProxyModel.h \
    Cpp/Model/UserGroupModel.h \
    Cpp/PopupParentWindow.h \
    Cpp/ProcessModel.h \
    Cpp/SearchParentWindow.h \
    Cpp/SetupParentWindow.h \
    Cpp/Utility/PasswordStrengthChecker.h \
    Cpp/Utility/PasswordStrengthCheckerS1.h \
    Cpp/Utility/TranslationSort.h \
    Cpp/ViewModel/Bookmark/BookmarkListViewModel.h \
    Cpp/ViewModel/Dashboard/DashboardViewModel.h \
    Cpp/ViewModel/DeviceFilterTree/DeviceFilterTreeSourceModel.h \
    Cpp/ViewModel/DeviceSelectionTree/DeviceAlarmInTreeSourceModel.h \
    Cpp/ViewModel/DeviceSelectionTree/DeviceAlarmOutTreeSourceModel.h \
    Cpp/ViewModel/DeviceSelectionTree/DeviceEmailTreeSourceModel.h \
    Cpp/ViewModel/DeviceSelectionTree/DeviceEventTreeSourceModel.h \
    Cpp/ViewModel/Event/EventListViewModel.h \
    Cpp/ViewModel/HealthMonitoring/HealthMonitoringViewModel.h \
    Cpp/ViewModel/ObjectSearch/BaseFilterModel.h \
    Cpp/ViewModel/ObjectSearch/BestShotViewModel.h \
    Cpp/ViewModel/ObjectSearch/BestshotFilterTooltipModel.h \
    Cpp/ViewModel/ObjectSearch/GridListThread.h \
    Cpp/ViewModel/ObjectSearch/GridListViewModel.h \
    Cpp/ViewModel/ObjectSearch/ObjectSearchFilterViewModel.h \
    Cpp/ViewModel/ObjectSearch/ObjectSearchViewModel.h \
    Cpp/ViewModel/ResourceUsage/ResourceUsageViewModel.h \
    Cpp/ViewModel/Setup/Device/ChannelListViewModel.h \
    Cpp/ViewModel/Setup/Device/DeviceListViewModel.h \
    Cpp/ViewModel/Setup/Device/AutoDiscoveryViewModel.h \
    Cpp/ViewModel/Setup/Device/ManualDiscoveryViewModel.h \
    Cpp/ViewModel/DeviceSelectionTree/DevicePhysicalTreeSourceModel.h \
    Cpp/ViewModel/DeviceSelectionTree/DeviceSelectionTreeSourceModel.h \
    Cpp/ViewModel/Setup/DeviceMaintenance/DeviceConfigViewModel.h \
    Cpp/ViewModel/Setup/Event/AddEventRuleViewModel.h \
    Cpp/ViewModel/Setup/Event/EventEmailViewModel.h \
    Cpp/ViewModel/Setup/Event/EventNotificationsViewModel.h \
    Cpp/ViewModel/Setup/Event/EventRulesViewModel.h \
    Cpp/ViewModel/EventFilterTree/EventFilterTreeSourceModel.h \
    Cpp/ViewModel/EventLog/EventLogThread.h \
    Cpp/ViewModel/EventLog/EventLogViewModel.h \
    Cpp/ViewModel/InitialPasswordSettingViewModel.h \
    Cpp/ViewModel/Layout/LayoutNavigationViewModel.h \
    Cpp/ViewModel/LoginViewModel.h \
    Cpp/ViewModel/MainViewModel.h \
    Cpp/ViewModel/MenuViewModel.h \
    Cpp/ViewModel/Notification/NotificationViewModel.h \
    Cpp/ViewModel/ResourceViewModel.h \
    Cpp/MultiWindow.h \
    Cpp/MultiWindowHandler.h \
    Cpp/ViewModel/Sequence/SequenceAddViewModel.h \
    Cpp/ViewModel/Setup/Device/DeviceListTreeSourceModel.h \
    Cpp/ViewModel/Setup/Event/EventScheduleViewModel.h \
    Cpp/ViewModel/Setup/SetupCommon.h \
    Cpp/ViewModel/Setup/System/LocalSettingViewModel.h \
    Cpp/ViewModel/Setup/System/SystemMaintenanceViewModel.h \
    Cpp/ViewModel/Setup/User/AddUserGroupViewModel.h \
    Cpp/ViewModel/Setup/User/AddUserViewModel.h \
    Cpp/ViewModel/Setup/User/LdapUserViewModel.h \
    Cpp/ViewModel/Setup/User/UserGroupViewModel.h \
    Cpp/ViewModel/Setup/User/UserLayoutTreeSourceModel.h \
    Cpp/ViewModel/Setup/User/UserListViewModel.h \
    Cpp/ViewModel/SmartSearch/SmartSearchViewModel.h \
    Cpp/ViewModel/SystemAdministration/AuditLogThread.h \
    Cpp/ViewModel/SystemAdministration/AuditLogViewModel.h \
    Cpp/ViewModel/Setup/SetupMainTreeFilterProxyModel.h \
    Cpp/ViewModel/Setup/SetupMainTreeViewModel.h \
    Cpp/ViewModel/SystemAdministration/UserFilterTreeSourceModel.h \
    Cpp/ViewModel/SystemLog/SystemLogThread.h \
    Cpp/ViewModel/SystemLog/SystemLogViewModel.h \
    Cpp/ViewModel/TextSearch/TextItemViewModel.h \
    Cpp/ViewModel/TextSearch/TextSearchViewModel.h \
    Cpp/ViewModel/Webpage/WebpageAddViewModel.h \
    Cpp/WisenetViewer.h \

win32:CONFIG(release, debug|release): {
    LIBS += -L$$OUT_PWD/../CoreService/release/ -lCoreService \
            -L$$OUT_PWD/../QCoreServiceManager/release/ -lQCoreServiceManager \
            -L$$OUT_PWD/../DeviceClient/SunapiDeviceClient/release/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../RtspClient/release/ -lRtspClient \
            -L$$OUT_PWD/../WisenetMediaFramework/release/ -lWisenetMediaFramework \
            -L$$OUT_PWD/../WisenetCommonLibrary/release/ -lWisenetCommonLibrary
    LIBS += -lPdh
}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -L$$OUT_PWD/../CoreService/debug/ -lCoreService \
            -L$$OUT_PWD/../QCoreServiceManager/debug/ -lQCoreServiceManager \
            -L$$OUT_PWD/../DeviceClient/SunapiDeviceClient/debug/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../RtspClient/debug/ -lRtspClient \
            -L$$OUT_PWD/../WisenetMediaFramework/debug/ -lWisenetMediaFramework \
            -L$$OUT_PWD/../WisenetCommonLibrary/debug/ -lWisenetCommonLibrary

    LIBS += -lPdh
}
else:unix: {
    # link order is important for g++
    LIBS += -L$$OUT_PWD/../WisenetMediaFramework/ -lWisenetMediaFramework \
            -L$$OUT_PWD/../QCoreServiceManager/ -lQCoreServiceManager \
            -L$$OUT_PWD/../CoreService/ -lCoreService \
            -L$$OUT_PWD/../DeviceClient/SunapiDeviceClient/ -lSunapiDeviceClient \
            -L$$OUT_PWD/../RtspClient/ -lRtspClient \
            -L$$OUT_PWD/../WisenetCommonLibrary/ -lWisenetCommonLibrary

}

DEPENDPATH += $$PWD/../CoreService \
              $$PWD/../QCoreServiceManager \
              $$PWD/../DeviceClient/SunapiDeviceClient \
              $$PWD/../RtspClient \
              $$PWD/../WisenetMediaFramework \
              $$PWD/../WisenetCommonLibrary


win32-g++:CONFIG(release, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../CoreService/release/libCoreService.a \
                      $$OUT_PWD/../QCoreServiceManager/release/libQCoreServiceManager.a \
                      $$OUT_PWD/../DeviceClient/SunapiDeviceClient/release/libSunapiDeviceClient.a \
                      $$OUT_PWD/../RtspClient/release/libRtspClient.a \
                      $$OUT_PWD/../WisenetMediaFramework/release/libWisenetMediaFramework.a \
                      $$OUT_PWD/../WisenetCommonLibrary/release/libWisenetCommonLibrary.a
}
else:win32-g++:CONFIG(debug, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../CoreService/debug/libCoreService.a \
                      $$OUT_PWD/../QCoreServiceManager/debug/libQCoreServiceManager.a \
                      $$OUT_PWD/../DeviceClient/SunapiDeviceClient/debug/libSunapiDeviceClient.a \
                      $$OUT_PWD/../RtspClient/debug/libRtspClient.a \
                      $$OUT_PWD/../WisenetMediaFramework/debug/libWisenetMediaFramework.a \
                      $$OUT_PWD/../WisenetCommonLibrary/debug/libWisenetCommonLibrary.a
}
else:win32:!win32-g++:CONFIG(release, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../CoreService/release/CoreService.lib \
                      $$OUT_PWD/../QCoreServiceManager/release/QCoreServiceManager.lib \
                      $$OUT_PWD/../DeviceClient/SunapiDeviceClient/release/SunapiDeviceClient.lib \
                      $$OUT_PWD/../RtspClient/release/RtspClient.lib \
                      $$OUT_PWD/../WisenetMediaFramework/release/WisenetMediaFramework.lib \
                      $$OUT_PWD/../WisenetCommonLibrary/release/WisenetCommonLibrary.lib
}
else:win32:!win32-g++:CONFIG(debug, debug|release): {
    PRE_TARGETDEPS += $$OUT_PWD/../CoreService/debug/CoreService.lib \
                      $$OUT_PWD/../QCoreServiceManager/debug/QCoreServiceManager.lib \
                      $$OUT_PWD/../DeviceClient/SunapiDeviceClient/debug/SunapiDeviceClient.lib \
                      $$OUT_PWD/../RtspClient/debug/RtspClient.lib \
                      $$OUT_PWD/../WisenetMediaFramework/debug/WisenetMediaFramework.lib \
                      $$OUT_PWD/../WisenetCommonLibrary/debug/WisenetCommonLibrary.lib
}
else:unix: {
    PRE_TARGETDEPS += $$OUT_PWD/../CoreService/libCoreService.a \
                      $$OUT_PWD/../QCoreServiceManager/libQCoreServiceManager.a \
                      $$OUT_PWD/../DeviceClient/SunapiDeviceClient/libSunapiDeviceClient.a \
                      $$OUT_PWD/../RtspClient/libRtspClient.a \
                      $$OUT_PWD/../WisenetMediaFramework/libWisenetMediaFramework.a \
                      $$OUT_PWD/../WisenetCommonLibrary/libWisenetCommonLibrary.a
}
WISENET_COMMON_PATH = $$PWD/../Common
WISENET_OPENSOURCE_PATH = $$PWD/OpenSource
WISENET_CRYPTO_PATH = $$WISENET_OPENSOURCE_PATH/cryptoadapter

unix: !macx {
    LIBS += -L$$WISENET_CRYPTO_PATH/linux -lCryptoAdapter
}

VERSION = 1.04.00
QMAKE_TARGET_COMPANY = Hanwha Vision Co., Ltd.
QMAKE_TARGET_COPYRIGHT = Copyright. 2023. Hanwha Vision Co., Ltd. All rights Reserved.
QMAKE_TARGET_PRODUCT = Wisenet Viewer

win32: {
    RC_ICONS = $$PWD/../Installer/WisenetViewer_new.ico
}

macx: {
    QMAKE_TARGET_BUNDLE_PREFIX = com.HanwhaVision
    ICON = $$PWD/../Installer/WisenetViewer.icns
    QMAKE_INFO_PLIST = $$PWD/../Info.plist
}

DISTFILES +=

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
