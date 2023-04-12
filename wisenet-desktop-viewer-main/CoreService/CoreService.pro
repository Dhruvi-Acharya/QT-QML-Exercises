QT -= gui
QT += core network \
    widgets
QT += sql

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11
#CONFIG += c++17

include($$PWD/../WisenetDesktopCommon.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AlarmOutActionManager.cpp \
    CoreService.cpp \
    CoreServiceConfig.cpp \
    CoreService_Device.cpp \
    CoreService_Event.cpp \
    CoreService_EventAction.cpp \
    DashboardManager.cpp \
    DatabaseManager.cpp \
    DeviceDiscovery.cpp \
    EmailManager.cpp \
    IPv6DeviceDiscovery.cpp \
    EventRuleManager.cpp \
    InitPassword.cpp \
    IpSetting.cpp \
    LdapManager.cpp \
    LogManager.cpp \
    MediaSession.cpp \
    PTZLogManager.cpp \
    SunapiBroadcastProtocol.cpp \
    UserSession.cpp \
    database/AuditLogTable.cpp \
    database/BookmarkTable.cpp \
    database/ChannelTable.cpp \
    database/CloudTable.cpp \
    database/DashboardDatabase.cpp \
    database/DashboardDatabaseMigration.cpp \
    database/DashboardTable.cpp \
    database/DeletedChannelTable.cpp \
    database/DeletedDeviceTable.cpp \
    database/DeviceTable.cpp \
    database/EventEmailTable.cpp \
    database/EventLogTable.cpp \
    database/EventRuleTable.cpp \
    database/EventScheduleTable.cpp \
    database/FileTable.cpp \
    database/GroupMappingTable.cpp \
    database/GroupTable.cpp \
    database/LayoutItemTable.cpp \
    database/LayoutTable.cpp \
    database/LdapTable.cpp \
    database/LicenseTable.cpp \
    database/LogDatabase.cpp \
    database/LogDatabaseMigration.cpp \
    database/ManagementDatabase.cpp \
    database/ManagementDatabaseMigration.cpp \
    database/SequenceItemTable.cpp \
    database/SequenceTable.cpp \
    database/ServiceSettingsTable.cpp \
    database/ServiceTable.cpp \
    database/StatisticsTable.cpp \
    database/SystemLogTable.cpp \
    database/UserGroupResourceTable.cpp \
    database/UserGroupTable.cpp \
    database/UserTable.cpp \
    database/WebPageTable.cpp

HEADERS += \
    AlarmOutActionManager.h \
    CoreService.h \
    CoreServiceConfig.h \
    CoreServiceLogSettings.h \
    CoreServiceUtil.h \
    DashboardManager.h \
    DatabaseManager.h \
    DeviceDiscovery.h \
    EmailManager.h \
    IPv6DeviceDiscovery.h \
    DummyDevice.h \
    EventRuleManager.h \
    InitPassword.h \
    IpSetting.h \
    LdapManager.h \
    LogManager.h \
    MediaSession.h \
    PTZLogManager.h \
    SunapiBroadcastProtocol.h \
    UserSession.h \
    database/AuditLogTable.h \
    database/BookmarkTable.h \
    database/ChannelTable.h \
    database/CloudTable.h \
    database/DashboardDatabase.h \
    database/DashboardDatabaseMigration.h \
    database/DashboardTable.h \
    database/DatabaseException.h \
    database/DeletedChannelTable.h \
    database/DeletedDeviceTable.h \
    database/DeviceTable.h \
    database/EventEmailTable.h \
    database/EventLogTable.h \
    database/EventRuleTable.h \
    database/EventScheduleTable.h \
    database/FileTable.h \
    database/GroupMappingTable.h \
    database/GroupTable.h \
    database/LayoutItemTable.h \
    database/LayoutTable.h \
    database/LdapTable.h \
    database/LicenseTable.h \
    database/LogDatabase.h \
    database/LogDatabaseMigration.h \
    database/ManagementDatabase.h \
    database/ManagementDatabaseMigration.h \
    database/SequenceItemTable.h \
    database/SequenceTable.h \
    database/ServiceSettingsTable.h \
    database/ServiceTable.h \
    database/StatisticsTable.h \
    database/SystemLogTable.h \
    database/UserGroupResourceTable.h \
    database/UserGroupTable.h \
    database/UserTable.h \
    database/WebPageTable.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
