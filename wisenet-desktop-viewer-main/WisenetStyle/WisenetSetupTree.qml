pragma Singleton
import QtQuick 2.15

Item {
    enum MenuType{
        Main,
        Sub
    }

    enum SubPageIndex{
        //Device
        DeviceList,
        DeviceMainteance,

        //User
        UserList,
        UserGroup,
        Ldap,

        //Event
        EventRules,
        EventSchedule,
        Email,

        //Log
        EventLog,
        SystemLog,
        AuditLog,

        //System
        LocalSettings,
        EventNotifications,
        SystemMaintenance,

        //additionalIndex
        SetupQuit,

        //DeviceList subIndex
        DeviceListAddDevice,
        //UserList subIndex
        UserListAddUser,
        //UserGroup subIndex
        UserGroupAddUserGroup,
        //EventRules subIndex
        EventRulesAddEventRule,
        // P2pRegister
        DdnsP2pRegister,

        //DeviceMaintenance subIndex
        DeviceMainteanceFirmwareUpdate,
        DeviceMainteanceConfigRestore,
        DeviceMainteanceConfigBackup,
        DeviceMainteanceChangePassword


    }

}
