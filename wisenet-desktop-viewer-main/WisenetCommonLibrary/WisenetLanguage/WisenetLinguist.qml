pragma Singleton
import QtQuick 2.15

QtObject{
    readonly property string newWindow: qsTr("New window")
    readonly property string newTab: qsTr("New tab")
    readonly property string logout: qsTr("Logout")
    readonly property string login: qsTr("Login")

    // Tree context
    readonly property string addDevice: qsTr("Add Device")
    readonly property string cameraSetting: qsTr("Camera Setting")

    // Button
    readonly property string ok: qsTr("OK")
    readonly property string apply: qsTr("Apply")
    readonly property string cancel: qsTr("Cancel")
    readonly property string close: qsTr("Close")

    // Popup message
    readonly property string success : qsTr("Success")
    readonly property string failed : qsTr("Failed")

    // Tab context
    readonly property string newLayout: qsTr("New Layout")
    readonly property string saveLayout: qsTr("Save Layout")
    readonly property string saveLayoutAs: qsTr("Save Layout As..")
    readonly property string lock: qsTr("Lock")
    readonly property string unlock: qsTr("Unlock")
    readonly property string closeAllButThis: qsTr("Close all but this")

    readonly property string enterNameMessage: qsTr("Enter the name.")
    readonly property string clear: qsTr("Clear")

    readonly property string newSequence: qsTr("New Sequence")

    // Tree root
    readonly property string resources: qsTr("Resources")
    readonly property string root: qsTr("Root")
    readonly property string devices: qsTr("Devices")
    readonly property string layouts: qsTr("Layouts")
    readonly property string sequences: qsTr("Sequences")
    readonly property string webPages: qsTr("Web Pages")
    readonly property string users: qsTr("Users")
    readonly property string mediaFiles: qsTr("Media Files")

    readonly property string addGroup: qsTr("Add group")
    readonly property string addWebpage: qsTr("Add web page")

    // Login & password
    readonly property string setupAdminPassword: qsTr("Set up admin password")
    readonly property string savePassword: qsTr("Save password")
    readonly property string saveIdPassword: qsTr("Save ID/password")

    readonly property string autoLogin: qsTr("Auto login")
    readonly property string invalidIdorPassword: qsTr("Invalid ID or password.")
    readonly property string loginLocked: qsTr("The user account has been locked. Please try again in %1 second(s).")
    readonly property string licenseAgreement: qsTr("License agreement")
    readonly property string licenseWarn: qsTr("Please read the following license agreement.\r\nYou must accept the terms cotained in this agreement before continuing with this program.")
    readonly property string licenseAccept: qsTr("I accept the license.")


    // password rule message
    readonly property string passwordRuleUnderTen: qsTr("If the password is 8 to 9 characters long, it must include at least 3 of the following character types: English uppercase letters, English lowercase letters, numbers, and special characters.")
    readonly property string passwordRuleOverTen: qsTr("If the password is 10 characters or longer, it must include at least 2 of the following character types: English uppercase letters, English lowercase letters, numbers, and special characters.")
    readonly property string passwordRuleId: qsTr("Do not use identical password with the ID.")
    readonly property string passwordRuleSpecialCharacters: qsTr("The following special characters are available for use. ~`!@#$%^&*()_-+={}[]|\;:‘“<>.,?/")
    readonly property string passwordRuleConsecutive: qsTr("Don't use 4 or more characters consecutive together. (examples : 1234, abcd).")
    readonly property string passwordRuleRepeated: qsTr("Don't use 4 or more characters repeated. (examples : !!!!, 1111, aaaa).")

    // password warning message
    readonly property string passwordWarningSameWithId: qsTr("This password is identical with the ID.")
    readonly property string passwordWarningEmpty: qsTr("Please enter password.")
    readonly property string passwordWarningUnderTen: qsTr("For 8 to 9 characters, it should be a combination of at least three of the following: alphabets (case sensitive), numbers, or special characters.")
    readonly property string passwordWarningOverTen: qsTr("For 10 letters or more, it should be a combination of at least two of the following: alphabets (case sensitive), numbers, or special characters.")
    readonly property string passwordWarningConsecutive: qsTr("The string uses four or more consecutive characters (e.g. 1234, abcd).")
    readonly property string passwordWarningRepeated: qsTr("The string uses the same character four or more times in a row (e.g. 1111, aaaa).")
    readonly property string passwordHighSecurity: qsTr("This password is available.")

    //Web page
    readonly property string newWebpage: qsTr("New Webpage")
    readonly property string modifyWebpage: qsTr("Modify Webpage")
    readonly property string digestAuthorization: qsTr("Digest Authorization")

    //setup
    //devices:
    readonly property string device: qsTr("Device")
    readonly property string deviceList: qsTr("Device List")
    readonly property string maintenance: qsTr("Maintenance")

    //User
    readonly property string user: qsTr("User")
    readonly property string userList: qsTr("User List")
    readonly property string userGroup: qsTr("User group")

    //Event
    readonly property string event: qsTr("Event")
    readonly property string eventRules: qsTr("EventRules")
    readonly property string eventNotifications: qsTr("Notifications")
    readonly property string showAllNotifications: qsTr("Show all notifications")

    //Log
    readonly property string log: qsTr("Log")
    readonly property string eventLog: qsTr("Event log")
    readonly property string systemLog: qsTr("System log")
    readonly property string auditLog: qsTr("Audit log")
    readonly property string deleted: qsTr("Deleted")

    //System
    readonly property string system: qsTr("System")
    readonly property string localSettings: qsTr("Local Settings")

    readonly property string deleteText: qsTr("Delete")

    // System event (BaseStructure.h - Wisenet::FixedAlarmType) : 각 카테고리별 같은 번역 시작

    // Dual Power System Failure / 이중 전원 시스템 오류
    readonly property string dualSmpsFail: qsTr("SystemEvent.DualSMPSFail")

    // Fan Error / 팬 오류
    readonly property string fanError: qsTr("SystemEvent.FanError")
    readonly property string cpuFanError: qsTr("SystemEvent.CPUFanError")
    readonly property string frameFanError: qsTr("SystemEvent.FrameFanError")
    readonly property string leftFanError: qsTr("SystemEvent.LeftFanError")
    readonly property string rightFanError: qsTr("SystemEvent.RightFanError")

    // Disk Full / 디스크 가득 참
    readonly property string hddFull: qsTr("SystemEvent.HDDFull")
    readonly property string sdFull: qsTr("SystemEvent.SDFull")
    readonly property string channelSdFull: qsTr("SDFull")
    readonly property string nasFull: qsTr("SystemEvent.NASFull")

    // No Disk / 디스크 없음
    readonly property string hddNone: qsTr("SystemEvent.HDDNone")

    // Disk Error / 디스크 오류
    readonly property string hddFail: qsTr("SystemEvent.HDDFail")
    readonly property string sdFail: qsTr("SystemEvent.SDFail")
    readonly property string nasFail: qsTr("SystemEvent.NASFail")
    readonly property string channelSdFail: qsTr("SDFail")
    readonly property string hddError: qsTr("SystemEvent.HDDError")

    // RAID degraded / RAID 오류
    readonly property string raidDegrade: qsTr("SystemEvent.RAIDDegrade")

    // RAID rebuilding / RAID 재구성
    readonly property string raidRebuildStart: qsTr("SystemEvent.RAIDRebuildStart")

    // RAID error / RAID 실패
    readonly property string raidFail: qsTr("SystemEvent.RAIDFail")

    // iSCSI disconnected / iSCSI 연결 끊김
    readonly property string iScsiDisconnect: qsTr("SystemEvent.iSCSIDisconnect")

    // NAS disconnected / NAS 연결 끊김
    readonly property string nasDisconnect: qsTr("SystemEvent.NASDisconnect")

    // Allowed bitrate exceeded / 허용 비트레이트 초과
    readonly property string recordFiltering: qsTr("SystemEvent.RecordFiltering")

    // Recording Error / 저장 오류
    readonly property string recordingError: qsTr("SystemEvent.RecordingError")

    // Network traffic overload / 네트워크 과부하
    readonly property string netTxTrafficOverflow: qsTr("SystemEvent.NetTxTrafficOverflow")
    readonly property string netCamTrafficOverFlow: qsTr("SystemEvent.NetCamTrafficOverFlow")

    // System overload / 시스템 과부하
    readonly property string cpuOverload: qsTr("SystemEvent.CpuOverload")

    // 시스템 과부하
    readonly property string vpuError: qsTr("SystemEvent.VPUError")
    readonly property string memoryError: qsTr("SystemEvent.MemoryError")

    // New Firmware Available / 새로운 펌웨어 사용 가능
    readonly property string newFwAvailable: qsTr("SystemEvent.NewFWAvailable")

    readonly property string systemEventDeviceDisconnectedWithRestriction: qsTr("SystemEvent.CoreService.DeviceDisconnectedWithRestriction")
    readonly property string systemEventFirmwareUpgrade: qsTr("SystemEvent.CoreService.FirmwareUpgrade")
    readonly property string systemEventDeviceConnected: qsTr("SystemEvent.CoreService.DeviceConnected")
    readonly property string systemEventDeviceDisconnectedByService: qsTr("SystemEvent.CoreService.DeviceDisconnectedByService")
    readonly property string systemEventDeviceDisconnectedWithError: qsTr("SystemEvent.CoreService.DeviceDisconnectedWithError")
    readonly property string systemEventDeviceDisconnectedWithUnauthorized: qsTr("SystemEvent.CoreService.DeviceDisconnectedWithUnauthorized")
    // System event (BaseStructure.h - Wisenet::FixedAlarmType) : 각 카테고리별 같은 번역 끝

    // System event message : 각 카테고리별 같은 번역 시작
    // 전원에 문제가 발생하였습니다. 전원LED를 확인하십시오.
    readonly property string dualSmpsFailMessage: qsTr("SystemEvent.DualSMPSFail.Message")

    // 팬이 정상 작동하지 않습니다. 팬 연결 상태를 확인하십시오.
    readonly property string fanErrorMessage: qsTr("SystemEvent.FanError.Message")
    readonly property string cpuFanErrorMessage: qsTr("SystemEvent.CPUFanError.Message")
    readonly property string frameFanErrorMessage: qsTr("SystemEvent.FrameFanError.Message")
    readonly property string leftFanErrorMessage: qsTr("SystemEvent.LeftFanError.Message")
    readonly property string rightFanErrorMessage: qsTr("SystemEvent.RightFanError.Message")

    // 디스크가 가득 차서 영상을 녹화할 수 없습니다. 디스크 상태를 확인하십시오.
    readonly property string hddFullMessage: qsTr("SystemEvent.HDDFull.Message")
    readonly property string sdFullMessage: qsTr("SystemEvent.SDFull.Message")
    readonly property string channelSdFullMessage: qsTr("SDFull.Message")
    readonly property string nasFullMessage: qsTr("SystemEvent.NASFull.Message")

    // 연결되어 있는 HDD가 없습니다. 장치를 확인해 주십시오.
    readonly property string hddNoneMessage: qsTr("SystemEvent.HDDNone.Message")

    // HDD에 문제가 있습니다. HDD 상태를 확인하십시오.
    readonly property string hddFailMessage: qsTr("SystemEvent.HDDFail.Message")
    readonly property string sdFailMessage: qsTr("SystemEvent.SDFail.Message")
    readonly property string nasFailMessage: qsTr("SystemEvent.NASFail.Message")
    readonly property string channelSdFailMessage: qsTr("SDFail.Message")
    readonly property string hddErrorMessage: qsTr("SystemEvent.HDDError.Message")

    // RAID에 장착된 HDD에 문제가 있습니다. HDD 상태를 확인하십시오.
    readonly property string raidDegradeMessage: qsTr("SystemEvent.RAIDDegrade.Message")

    // Degrade 상태의 RAID 배열을 복구하고 있습니다.
    readonly property string raidRebuildStartMessage: qsTr("SystemEvent.RAIDRebuildStart.Message")

    // 디스크 장애 또는 RAID 구성 문제로 인해 RAID 배열을 더 이상 사용할 수 없습니다. RAID상태를 확인하십시오.
    readonly property string raidFailMessage: qsTr("SystemEvent.RAIDFail.Message")

    // iSCSI 장치와 연결이 끊겼습니다.
    readonly property string iScsiDisconnectMessage: qsTr("SystemEvent.iSCSIDisconnect.Message")

    // NAS 연결 끊김
    readonly property string nasDisconnectMessage: qsTr("SystemEvent.NASDisconnect.Message")

    // 레코딩 관련
    readonly property string recordFilteringMessage: qsTr("SystemEvent.RecordFiltering.Message")
    readonly property string recordingErrorMessage: qsTr("SystemEvent.RecordingError.Message")

    // 원격전송 중 데이터량이 장비의 전송대역폭을 초과하였습니다. 일부 채널이 I Frame으로 재생됩니다. 모니터링하는 사용자수나 채널수를 줄이시기 바랍니다.
    readonly property string netTxTrafficOverflowMessage: qsTr("SystemEvent.NetTxTrafficOverflow.Message")
    readonly property string netCamTrafficOverFlowMessage: qsTr("SystemEvent.NetCamTrafficOverFlow.Message")

    // 시스템에 과부하가 걸렸습니다. 웹뷰어 또는 VMS에서 원격으로 모니터링 하는 사용자 수를 제한 하거나, 원격 또는 저장장치에서 재생하는 채널 수를 조절하십시오.
    readonly property string cpuOverloadMessage: qsTr("SystemEvent.CpuOverload.Message")
    readonly property string vpuErrorMessage: qsTr("SystemEvent.VPUError.Message")
    readonly property string memoryErrorMessage: qsTr("SystemEvent.MemoryError.Message")

    // 업데이트 서버에 최신 소프트웨어가 있습니다. 최신 버전으로 업그레이드 하십시오.
    readonly property string newFwAvailableMessage: qsTr("SystemEvent.NewFWAvailable.Message")

    readonly property string systemEventDeviceDisconnectedWithRestrictionMessage: qsTr("SystemEvent.CoreService.DeviceDisconnectedWithRestriction.Message")
    readonly property string systemEventFirmwareUpgradeMessage: qsTr("SystemEvent.CoreService.FirmwareUpgrade.Message")
    readonly property string systemEventDeviceConnectedMessage: qsTr("SystemEvent.CoreService.DeviceConnected.Message")
    readonly property string systemEventDeviceDisconnectedByServiceMessage: qsTr("SystemEvent.CoreService.DeviceDisconnectedByService.Message")
    readonly property string systemEventDeviceDisconnectedWithErrorMessage: qsTr("SystemEvent.CoreService.DeviceDisconnectedWithError.Message")
    readonly property string systemEventDeviceDisconnectedWithUnauthorizedMessage: qsTr("SystemEvent.CoreService.DeviceDisconnectedWithUnauthorized.Message")
    readonly property string alarmInputMessage: qsTr("Alarm input occured.")
    // System event message : 각 카테고리별 같은 번역 끝

    // System event description
    readonly property string fanNo: qsTr("Fan %1") //System Log의 Description 팬(Fan) 1
    readonly property string hddNo: qsTr("HDD %1") //System Log의 Description HDD 1
    readonly property string sdcardNo: qsTr("SD card %1") //System Log의 Description SD card 1
    readonly property string channelNo: qsTr("Channel %1") //System Log의 Description Channel(채널) 1
    readonly property string nasNo: qsTr("NAS %1") //System Log의 Description NAS:1
    readonly property string arrayNo: qsTr("RAID Array %1") //System Log의 Description RAID Array(배열) 1
    readonly property string alarmInputNo: qsTr("Alarm input %1") //System Log의 Description Alarm input(알람 입력) 1

    //Record Type Filter
    readonly property string filter: qsTr("filter")
    readonly property string all: qsTr("All")
    readonly property string normalRecording: qsTr("Continuous recording")
    readonly property string manualRecording: qsTr("Manual recording")
    readonly property string faceRecognition: qsTr("Face recognition")
    readonly property string objectDetectionVehicle: qsTr("Vehicle detection")
    readonly property string objectDetectionPerson: qsTr("Person detection")
    readonly property string objectDetectionLicensePlate: qsTr("License plate")
    readonly property string objectDetectionFace: qsTr("Face detection")

    // Record And Event Type(BaseStructure.h - Wisenet::FixedAlarmType)
    readonly property string motionDetection: qsTr("MotionDetection")
    readonly property string faceDetection: qsTr("FaceDetection")
    readonly property string tamperingDetection: qsTr("Tampering")
    readonly property string audioDetection: qsTr("AudioDetection")
    readonly property string iva: qsTr("VideoAnalytics")
    readonly property string autoTracking: qsTr("Tracking")
    readonly property string defocusDetection: qsTr("DefocusDetection")
    readonly property string fogDetection: qsTr("FogDetection")
    readonly property string soundClassification: qsTr("AudioAnalytics")
    readonly property string alarmInput: qsTr("AlarmInput")

    // Event Type(BaseStructure.h - Wisenet::FixedAlarmType)
    readonly property string videoloss: qsTr("Videoloss")
    readonly property string videolossStart: qsTr("Videoloss.Start")
    readonly property string videolossEnd: qsTr("Videoloss.End")
    readonly property string autoTrackingStart: qsTr("Tracking.Start")
    readonly property string autoTrackingEnd: qsTr("Tracking.End")
    readonly property string videoAnalyticsPassing: qsTr("VideoAnalytics.Passing")
    readonly property string videoAnalyticsIntrusion: qsTr("VideoAnalytics.Intrusion")
    readonly property string videoAnalyticsEntering: qsTr("VideoAnalytics.Entering")
    readonly property string videoAnalyticsExiting: qsTr("VideoAnalytics.Exiting")
    readonly property string videoAnalyticsAppearing: qsTr("VideoAnalytics.Appearing")
    readonly property string videoAnalyticsDisappearing: qsTr("VideoAnalytics.Disappearing")
    readonly property string videoAnalyticsLoitering: qsTr("VideoAnalytics.Loitering")
    readonly property string audioAnalyticsScream: qsTr("AudioAnalytics.Scream")
    readonly property string audioAnalyticsGunshot: qsTr("AudioAnalytics.Gunshot")
    readonly property string audioAnalyticsExplosion: qsTr("AudioAnalytics.Explosion")
    readonly property string audioAnalyticsGlassBreak: qsTr("AudioAnalytics.GlassBreak")
    readonly property string networkAlarmInput: qsTr("NetworkAlarmInput")


    //Audit Log type
    readonly property string userLoginAuditType: qsTr("User Login")
    readonly property string backupSettingsAuditType: qsTr("Backup Settings")
    readonly property string restoreSettingsAuditType: qsTr("Restore Settings")
    readonly property string initializeSettingsAuditType: qsTr("Initialize Settings")
    readonly property string logSettingsAuditType: qsTr("Log Settings")
    readonly property string userSettingsAuditType: qsTr("User Settings")
    readonly property string userGroupSettingsAuditType: qsTr("User Group Settings")
    readonly property string eventRuleSettingsAuditType: qsTr("Event Rule Settings")
    readonly property string scheduleSettingsAuditType: qsTr("Schedule Settings")
    readonly property string deviceSettingsAuditType: qsTr("Device Settings")
    readonly property string channelSettingsAuditType: qsTr("Channel Settings")
    readonly property string deviceStatusAuditType: qsTr("Device Status")
    readonly property string ptzControlAuditType: qsTr("PTZ Control audit type")
    readonly property string watchingLiveAuditType: qsTr("Watching Live")
    readonly property string watchingPlaybackAuditType: qsTr("Watching Playback")
    readonly property string exportingVideoAuditType: qsTr("Exporting Video")
    readonly property string updateSoftwareAuditType: qsTr("Software upgrade tried") // 1.1.0 SW update

    readonly property string service: qsTr("Service")

    //Audit Log Description
    readonly property string allUsers : qsTr("All users")       // User Filter of Audit Log
    readonly property string logType: qsTr("Log Type")
    readonly property string logLogin: qsTr("User login. StartTime: %1 , EndTime: %2")
    readonly property string logLoginFail: qsTr("User login failed.")

    readonly property string logBackupSettings: qsTr("System configuration has been backed up.")
    readonly property string logBackupSettingsFail: qsTr("System configuration failed to back up.")

    readonly property string logRestoreSettings: qsTr("System configuration has been restored.")
    readonly property string logRestoreSettingsFail: qsTr("System configuration failed to restore.")

    readonly property string logInitializeSettings: qsTr("System configuration has been reset.")
    readonly property string logInitializeSettingsFail: qsTr("System configuration failed to reset.")

    readonly property string logUpdatingLogRetention: qsTr("The log retention period changed from %1 to %2.")

    readonly property string logUserAdd: qsTr("User(%1) is added.")
    readonly property string logUserRemove: qsTr("User(%1) is deleted.")
    readonly property string logUserUpdate: qsTr("User(%1) is updated.")

    readonly property string logUserGroupAdd: qsTr("User group(%1) is added.")
    readonly property string logUserGroupRemove: qsTr("User group(%1) is deleted.")
    readonly property string logUserGroupUpdate: qsTr("User group(%1) is updated.")

    readonly property string logEventRuleAdd: qsTr("EventRule(%1) is added.")
    readonly property string logEventRuleRemove: qsTr("EventRule(%1) is deleted.")
    readonly property string logEventRuleUpdate: qsTr("EventRule(%1) is updated.")

    readonly property string logScheduleAdd: qsTr("Schedule(%1) is added.")
    readonly property string logScheduleRemove: qsTr("Schedule(%1) is deleted.")
    readonly property string logScheduleUpdate: qsTr("Schedule(%1) is updated.")

    readonly property string logDeviceAdd: qsTr("Device is added.")
    readonly property string logDeviceRemove: qsTr("Device is deleted.")
    readonly property string logDeviceUpdate: qsTr("Device is updated.")

    readonly property string logChannelAdd: qsTr("Channel is added.")
    readonly property string logChannelRemove: qsTr("Channel is deleted.")
    readonly property string logChannelUpdate: qsTr("Channel is updated.")

    readonly property string logConnectDevice: qsTr("Connect Device.")
    readonly property string logDisconnectDevice: qsTr("Disconnect Device.")

    readonly property string logPTZControl: qsTr("PTZcontrol occurred from %1 to %2.")

    readonly property string logWatchingLive: qsTr("User watched live video from %1 to %2.")

    readonly property string logWatchingPlayBack: qsTr("User played video from %1 to %2.")

    readonly property string logExportingVideo: qsTr("Exported from %1 to %2.")

    readonly property string logUpdateSoftware: qsTr("Software upgrade is tried") // 1.1.0 SW update

    // Media controller
    readonly property string today: qsTr("Today")
    readonly property string showCalendar: qsTr("Show Calendar")
    readonly property string hideCalendar: qsTr("Hide Calendar")
    readonly property string bookmark: qsTr("Bookmark")
    readonly property string name: qsTr("Name")
    readonly property string from: qsTr("From")
    readonly property string to: qsTr("To")
    readonly property string description: qsTr("Description")
    readonly property string overlappedData: qsTr("Overlapped data")
    readonly property string noData: qsTr("No data")
    readonly property string overlapped: qsTr("Overlapped")
    readonly property string markSelectionStart: qsTr("Mark Selection Start")
    readonly property string markSelectionEnd: qsTr("Mark Selection End")
    readonly property string clearSelection: qsTr("Clear Selection")
    readonly property string zoomToSelection: qsTr("Zoom to Selection")
    readonly property string addBookmark: qsTr("Add Bookmark")
    readonly property string exportVideo: qsTr("Export video")
    readonly property string recordTypeFilter: qsTr("Record type filter")
    readonly property string previousEvent: qsTr("Previous event")
    readonly property string previousFrame: qsTr("Previous frame")
    readonly property string fastBackward: qsTr("Fast backward")
    readonly property string nextEvent: qsTr("Next event")
    readonly property string nextFrame: qsTr("Next frame")
    readonly property string fastForward: qsTr("Fast forward")
    readonly property string showTimeline: qsTr("Show timeline")
    readonly property string hideTimeline: qsTr("Hide timeline")
    readonly property string zoomToBookmark: qsTr("Zoom to bookmark")
    readonly property string editBookmark: qsTr("Edit bookmark")
    readonly property string deleteBookmark: qsTr("Delete bookmark")
    readonly property string deleteWarning: qsTr("this action cannot be undone.")
    readonly property string exportButton: qsTr("Export")
    readonly property string single: qsTr("Single")
    readonly property string multi: qsTr("Multi")
    readonly property string highResolution: qsTr("High resolution")
    readonly property string enable: qsTr("Enable")
    readonly property string exportForceStop: qsTr("Are you sure you want to cancel the export?")
    readonly property string playFilteredSection: qsTr("Play filtered section")
    readonly property string usePassword: qsTr("Use password")
    readonly property string confirmPwNotMached: qsTr("Passwords didn't matched.")
    readonly property string switchToLive: qsTr("Switch to live")
    readonly property string switchToPlayback: qsTr("Switch to playback")
    readonly property string play: qsTr("play")
    readonly property string pause: qsTr("pause")
    readonly property string exportCancelWarn: qsTr("exportCancelWarn?") // 취소 시 현재 까지 진행 된 구간만 저장됩니다. 취소하시겠습니까?
    readonly property string nameCannotBlank: qsTr("Name does not allow blanks.") // 이름에 공백문자를 사용할 수 없습니다.
    // 비밀번호는 최소 8자 이상이며 알파벳과 숫자의 조합으로 이루어져있어야 합니다.
    readonly property string exportPasswordMust: qsTr("The password must be at least 8 characters long and consist of a combination of alphanumeric characters.")
    // 디스크 공간이 부족하여 저장할 수 없습니다. 여유 공간을 확보해주십시오.
    readonly property string exportDiskFull: qsTr("The recording cannot proceed due to a lack of free space. Please free up space.")
    readonly property string exportDiskError: qsTr("Record disk error.") // 저장 디스크 오류
    // 저장 공간이 부족합니다. 계속하시겠습니까?
    readonly property string exportFreespaceCheck: qsTr("There is not enough storage space. Do you wish to continue?")

    // Context menu
    readonly property string open: qsTr("Open")
    readonly property string openInNewTab: qsTr("Open in New Tab")
    readonly property string openInNewWindow: qsTr("Open in New Window")
    readonly property string openFolder: qsTr("Open folder")
    readonly property string edit: qsTr("Edit")
    readonly property string remove: qsTr("Remove")
    readonly property string settings: qsTr("Settings")
    readonly property string moveTo: qsTr("Move to")
    readonly property string ungroup: qsTr("Ungroup")
    readonly property string newGroupWithSelection: qsTr("New group with selection")
    readonly property string nvrSettings: qsTr("NVR Settings")
    readonly property string autoRegister : qsTr("Auto register")
    readonly property string ddnsAndP2pRegister : qsTr("DDNS/P2P register")
    readonly property string expandAll: qsTr("Expand all");
    readonly property string collapseAll: qsTr("Collapse all");

    //Event tab
    readonly property string allCameras: qsTr("All cameras")
    readonly property string onLayout_: qsTr("On layout")
    readonly property string selectedCamera: qsTr("Selected camera")
    readonly property string channels: qsTr("channels")

    //Bookmark tab
    readonly property string time: qsTr("Time")
    readonly property string anyTime: qsTr("Any time")
    readonly property string lastDay: qsTr("Last day")
    readonly property string last7Days: qsTr("Last 7 days")
    readonly property string last30Days: qsTr("Last 30 days")
    readonly property string selectedOnTimeline: qsTr("Selected on timeline")

    //Event search
    readonly property string eventSearch: qsTr("Event Search")
    readonly property string dateAndTime: qsTr("Date & Time")
    readonly property string eventType: qsTr("Event Type")
    readonly property string search: qsTr("Search")
    readonly property string reset: qsTr("reset")
    readonly property string none: qsTr("None")
    readonly property string allDevices: qsTr("All Devices")
    readonly property string allEvents: qsTr("All Events")
    readonly property string results: qsTr("results")

    //setup - event eventRules:
    readonly property string enterTheRuleName: qsTr("Enter the rule name.")
    readonly property string addAnEventTrigger: qsTr("Add an event trigger.")
    readonly property string ruleInfo: qsTr("Rule info")
    readonly property string addEventRule: qsTr("Add event rule")
    readonly property string editEventRule: qsTr("Edit event rule")
    readonly property string ruleName: qsTr("Rule name")
    readonly property string duration: qsTr("Duration")
    readonly property string addAction: qsTr("Add action")
    readonly property string addTrigger: qsTr("Add trigger")
    readonly property string eventTrigger : qsTr("Event Trigger")
    readonly property string eventAction : qsTr("Event action")
    readonly property string use: qsTr("use")
    readonly property string alarmOutput: qsTr("Alarm output")
    readonly property string alertAlarm: qsTr("AlertAlarm")
    readonly property string on: qsTr("On")
    readonly property string off: qsTr("Off")


    //setup main
    readonly property string setup : qsTr("Setup")

    //Setup DeviceList
    readonly property string pleaseSelectDevice : qsTr("Please select device.")
    readonly property string deviceName : qsTr("Device Name")
    readonly property string guid : qsTr("GUID")
    readonly property string model : qsTr("Model")
    readonly property string type : qsTr("Type")
    readonly property string channelName : qsTr("Channel Name")
    readonly property string useChannel : qsTr("Use channel")
    readonly property string ptz : qsTr("PTZ")
    readonly property string fisheyeDewarping : qsTr("Fisheye dewarping")
    readonly property string mount : qsTr("Mount")
    readonly property string ceiling : qsTr("Ceiling")
    readonly property string wall : qsTr("Wall")
    readonly property string ground : qsTr("Ground")
    readonly property string primaryProfile : qsTr("Primary profile")
    readonly property string codec : qsTr("Codec")
    readonly property string resolution : qsTr("Resolution")
    readonly property string framerate : qsTr("Framerate")
    readonly property string bitrate : qsTr("Bitrate")
    readonly property string secondaryProfile : qsTr("Secondary profile")
    readonly property string password : qsTr("Password")
    readonly property string version : qsTr("Version")
    readonly property string macAddress : qsTr("MAC address")
    readonly property string deviceCertificate : qsTr("Device certificate")
    readonly property string connectedType : qsTr("Connected Type")
    readonly property string network : qsTr("Network")
    readonly property string port : qsTr("Port")
    readonly property string streamingProtocol : qsTr("Streaming protocol")
    readonly property string connection : qsTr("Connection")
    readonly property string authorization : qsTr("Authorization")
    readonly property string change : qsTr("Change")
    readonly property string webViewer : qsTr("WebViewer")
    readonly property string add : qsTr("Add")
    readonly property string lensType : qsTr("LensType")

    readonly property string confirmPassword : qsTr("Confirm password")
    readonly property string manual : qsTr("Manual")
    readonly property string refresh : qsTr("Refresh")
    readonly property string registering : qsTr("Registering")
    readonly property string connectionFailed : qsTr("ConnectionFailed")
    readonly property string authFailed : qsTr("AuthFailed")
    readonly property string initDevicePassword : qsTr("InitDevicePassword")
    readonly property string initDevicePasswordChanging : qsTr("InitDevicePasswordChanging")
    readonly property string initDevicePasswordChanged : qsTr("InitDevicePasswordChanged")
    readonly property string initDevicePasswordChangeFailed : qsTr("InitDevicePasswordChangeFailed")
    readonly property string ipChanging : qsTr("IpChanging")
    readonly property string ipChanged : qsTr("IpChanged")
    readonly property string ipChangeFailed : qsTr("IpChangeFailed")
    readonly property string ipConflict : qsTr("IpConflict")
    readonly property string inputId : qsTr("Input ID")
    readonly property string initialPassword : qsTr("Initial password")
    readonly property string pleaseSelectInitialPasswordDevice : qsTr("Please select initial password device.")
    readonly property string register : qsTr("Register")
    readonly property string ipConfiguration : qsTr("Ip Configuration")
    readonly property string registered : qsTr("Registered")
    readonly property string subnetMask : qsTr("Subnet mask")
    readonly property string gateway : qsTr("Gateway")
    readonly property string pleaseCheckProductId : qsTr("Please check product ID.")
    readonly property string pleaseCheckPort : qsTr("Please check port.")
    readonly property string pleaseCheckIpAddress : qsTr("Please check IP address.")
    readonly property string pleaseCheckAddress : qsTr("Please check address.")
    readonly property string ipType : qsTr("IP Type")
    readonly property string ip : qsTr("IP")
    readonly property string productId : qsTr("Product ID")
    readonly property string alreadyRegistered : qsTr("Already Registered")  // Errorcode
    readonly property string channelNumber : qsTr("Channel number")             //Setup DeviceList

    // Errorcode
    readonly property string invalidRequest : qsTr("Invalid request")
    readonly property string noResponse : qsTr("No response")
    readonly property string authenticationFailed : qsTr("Authentication failed")
    readonly property string networkError : qsTr("Network error")
    readonly property string notSupportDevice : qsTr("Not support device")
    readonly property string accountBlocked : qsTr("Account Blocked")
    readonly property string managementDatabaseError : qsTr("Management database Error")
    readonly property string strongPasswordIsRequired : qsTr("Strong Password is required")
    readonly property string userFull : qsTr("User Full")
    readonly property string permissionDenied: qsTr("Permission Denied")
    readonly property string inCompatibleFile : qsTr("InCompatible File")
    readonly property string updateFail : qsTr("UpdateFail")
    readonly property string dDNSError : qsTr("DDNS Error")
    readonly property string cloudConnectError : qsTr("Wisenet Cloud Connect Error")
    readonly property string cloudInvaildServerValue : qsTr("Failed to aquire a server information from Wisenet Cloud")
    readonly property string cloudAddingUserError : qsTr("Failed to add User to Wisenet Cloud")
    readonly property string cloudAddingDeviceError : qsTr("Failed to add Device to Wisenet Cloud User")
    readonly property string p2PConnectError : qsTr("Failed to connect via P2P")
    readonly property string backupServiceError : qsTr("Backup Service Error")
    readonly property string restoreServiceError : qsTr("Restore Service Error")
    readonly property string resetServiceError : qsTr("ResetServiceError")
    readonly property string deviceMismatchError : qsTr("Device mismatch Error")
    readonly property string unrecognizedError : qsTr("Unrecognized error")
    readonly property string systemMenuUsed : qsTr("System Menu Used")       // Errorcode
    readonly property string mutualAuthenticationError: qsTr("Mutual authentication error") //상호 인증 에러
    readonly property string idDoesNotExist: qsTr("ID does not exist.")


    // Setup - Device maintenance
    readonly property string firmwareUpdate : qsTr("Firmware update")
    readonly property string restore : qsTr("Restore")
    readonly property string backup : qsTr("Backup")
    readonly property string passwordChange : qsTr("Password change")
    readonly property string status : qsTr("Status")
    readonly property string deviceId : qsTr("Device id")
    readonly property string path : qsTr("Path")
    readonly property string uploadFiles : qsTr("Upload files")
    readonly property string allFiles : qsTr("All files")
    readonly property string applyToSameModel : qsTr("Apply to same model")
    readonly property string updateComplete : qsTr("Update complete")
    readonly property string currentPassword : qsTr("Current password")
    readonly property string update : qsTr("Update")
    readonly property string upload : qsTr("Upload")             //Setup firmwareUpdate

    // Setup - local settings:
    readonly property string theSystemRestarts : qsTr("The system restarts")
    readonly property string localSettingsChanged : qsTr("Local settings changed")
    readonly property string language : qsTr("Language")
    readonly property string maximumLiveBufferLength: qsTr("Maximum live buffer length")
    readonly property string mediaFileFolder: qsTr("Media file folder")
    readonly property string maximumRecordingTime: qsTr("Maximum recording time")
    readonly property string recordingFileType: qsTr("Recording file type")
    readonly property string minute: qsTr("minute")
    readonly property string minutes: qsTr("minutes")

    // Setup - system maintenance
    readonly property string currentVersion : qsTr("Current version")
    readonly property string createBackup : qsTr("Create backup")
    readonly property string restoreFromBackup : qsTr("Restore from backup")
    readonly property string configurationsReset : qsTr("Configurations reset")
    readonly property string opensourceLicense : qsTr("Opensource license")
    readonly property string view : qsTr("View")

    // Setup - User
    readonly property string localRecording: qsTr("Local recording")
    readonly property string duplicateIdIsNotAllowed: qsTr("Duplicate ID is not allowed.")
    readonly property string group : qsTr("Group")
    readonly property string allPermission: qsTr("All permission")  // 모든 권한
    readonly property string permission : qsTr("Permission")
    readonly property string pleaseInputUserGroupName : qsTr("Please input user group name.")
    readonly property string deviceControl : qsTr("Device control")
    readonly property string playback : qsTr("Playback")
    readonly property string ptzControl : qsTr("PTZ Control")
    readonly property string audio : qsTr("Audio")
    readonly property string mic : qsTr("Mic")
    readonly property string resource : qsTr("Resource")
    readonly property string layout : qsTr("layout")
    readonly property string sharedLayout: qsTr("Shared layout")  // 공유 레이아웃 //User group
    readonly property string addUserGroup : qsTr("Add user group")
    readonly property string editUserGroup : qsTr("Edit user group")
    readonly property string addUser : qsTr("Add User")
    readonly property string editUser : qsTr("Edit User")

    readonly property string pleaseInputUserName : qsTr("Please input user name.")
    readonly property string pleaseCheckPassword : qsTr("Please check password.")
    readonly property string pleaseSelectUserGroup : qsTr("Please select user group.")
    readonly property string pleaseCheckEmail: qsTr("Please check email.")
    readonly property string id_ : qsTr("ID")
    readonly property string groupId : qsTr("Group ID")
    readonly property string groupName : qsTr("Group name")                 //User
    readonly property string pleaseCheckIdAndPassword : qsTr("Please check Id and Password.")
    readonly property string duplicatedName : qsTr("Duplicated name")

    // Setup messages
    readonly property string doNotAskAgain: qsTr("Don't ask again.") // 다시 묻지 않음.
    readonly property string deviceDeleteConfirmMessage: qsTr("Are you sure you want to delete the selected device?") // 선택한 장비를 삭제하시겠습니까?
    readonly property string checkDeviceNetwork: qsTr("Please check the device's network.")
    readonly property string warningFirmwareUpdateInProgress: qsTr("Firmware update in progress.")  // 펌웨어 업데이트 중입니다.
    readonly property string savedAuthenticationMessage: qsTr("The saved ID/password is used by automatic registration, automatic search, and manual registration (including DDNS/P2P registration).")  //저장한 ID/비밀번호는 자동등록, 자동 검색, 수동 등록(DDNS/P2P 등록 포함)에서 사용합니다.
    readonly property string initialPasswordNeedMessage: qsTr("There are %1 devices that require initial password setting.")  // 초기 비밀번호 설정이 필요한 장치가 n대 있습니다.
    readonly property string ipConflictMessage: qsTr("There are %1 devices with conflict IPs")  // IP가 중복한 장치가 n대 있습니다.
    readonly property string deleteUserConfirmMessage: qsTr("Are you sure you want to delete the selected user?")  // 선택한 사용자를 삭제하시겠습니까?
    readonly property string deleteUserGroupConfirmMessage: qsTr("Are you sure you want to delete the selected user group?")  // 선택한 사용자 그룹을 삭제하시겠습니까?
    readonly property string deleteEventRuleConfirmMessage: qsTr("Are you sure you want to delete the selected event rule?")  // 선택한 이벤트 규칙을 삭제하시겠습니까?
    readonly property string resetConfirmMessage: qsTr("Do you want to reset it? After reset, the program resumes.")  // 초기화 하시겠습니까? 초기화 뒤에는 프로그램이 재시작합니다.
    readonly property string restoreConfirmMessage: qsTr("Do you want to restore it? After restore, the program will restart.")  // 복원 하시겠습니까? 복원 뒤에는 프로그램이 재시작합니다.
    readonly property string languageChangeConfirmMessage: qsTr("Do you want to change language? After change language, the program will restart.")  // 언어를 변경 하시겠습니까? 언어 변경뒤에는 프로그램이 재시작합니다.
    readonly property string checkExistUnregisterDeviceMessage: qsTr("There are devices that could not be registered.")

    //ViewingGrid
    readonly property string autoFit: qsTr("Auto Fit") // 자동맞춤
    readonly property string brightnessContrast : qsTr("Brightness/Contrast") // 밝기/대비
    readonly property string imageRotation : qsTr("Image Rotation") // 이미지 회전
    readonly property string videoFillMode : qsTr("Video Fill Mode") // 영상 표시 비율
    readonly property string stretch : qsTr("Stretch") // 늘림
    readonly property string preserveAspectioRatio : qsTr("Preserve Aspect Ratio") // 화면 비율 유지
    readonly property string fisheyeSetup : qsTr("Fisheye Setup") // 피쉬아이 설정
    readonly property string videoProfile : qsTr("Video Profile") // 비디오 프로파일
    readonly property string auto : qsTr("Auto") // 자동
    readonly property string ptzPreset : qsTr("PTZ Preset") // PTZ 프리셋
    readonly property string videoStatus: qsTr("Video Status") // 비디오 상태
    readonly property string setBackgroundImage: qsTr("Set Background Image") // 배경이미지로 설정
    readonly property string fullScreen: qsTr("Full Screen") // 전체화면
    readonly property string openMediaFiles: qsTr("Open Media Files") // 미디어파일 열기
    readonly property string cellAspectRatio: qsTr("Cell Aspect Ratio") // 셀종횡비
    readonly property string back: qsTr("Back") // 뒤로
    readonly property string forward: qsTr("Forward") // 앞으로
    readonly property string reload: qsTr("Reload") // 새로고침
    readonly property string cameraDisconnectedError: qsTr("Camera disconnected") // 카메라 연결 끊어짐
    readonly property string mediaDisconnectedError: qsTr("Media session disconnected") // 영상 세션 연결 끊어짐
    readonly property string playbackUserFullError: qsTr("Exceeded number of playback users") // 재생 사용자 수 초과
    readonly property string noPermissionError: qsTr("No permission") // 사용 권한 없음
    readonly property string liveVideoLossError: qsTr("Live Video Loss")            // 라이브 비디오를 수신받지 못했습니다.
    readonly property string playbackVideoLossError: qsTr("No Recording Video")  // 녹화된 비디오가 없습니다.
    readonly property string preset : qsTr("Preset")        // 프리셋
    readonly property string videoFileSaved: qsTr("Video File Saved") // 영상이 저장되었습니다.
    readonly property string imageFileSaved: qsTr("Image File Saved") // 이미지가 저장되었습니다.
    readonly property string focus: qsTr("Focus") // 포커스
    readonly property string iris: qsTr("Iris") // 아이리스
    readonly property string brightness: qsTr("Brightness") // 밝기
    readonly property string contrast: qsTr("Contrast") // 대비
    readonly property string defog: qsTr("Defog") // 안개제거
    readonly property string viewMode: qsTr("View Mode") // 뷰모드
    readonly property string screenShot: qsTr("Screenshot") // 이미지 저장
    readonly property string near: qsTr("Near") // 포커스 near
    readonly property string far: qsTr("Far") // 포커스 far
    readonly property string simpleFocus: qsTr("Simple Focus") // Simple focus
    readonly property string autoFocus: qsTr("Auto Focus") // Auto focus
    readonly property string resetFocus: qsTr("Reset Focus") // Reset focus
    readonly property string addPreset: qsTr("Add Preset") // 프리셋 추가
    readonly property string cameraFocusSettings: qsTr("Camera Focus Settings");
    readonly property string no_: qsTr("No.") // 번호
    readonly property string number: qsTr("Number") // 번호
    readonly property string zoomArea: qsTr("Zoom Area") // 줌영역
    readonly property string warning_noInputOfPresetName: qsTr("Please enter preset name.") // 프리셋 이름을 입력하십시오
    readonly property string warning_layoutLocked: qsTr("Layout is locked.") // 레이아웃이 잠금상태입니다.
    readonly property string warning_maxVideoitemOnLayout: qsTr("A maximum of 64 videos can be added on a layout.") // 최대 64개의 비디오가 레이아웃에 추가될 수 있습니다.
    readonly property string warning_maxWebitemOnLayout: qsTr("A maximum of 8 webpages can be added on a layout.") // 최대 8개의 웹페이지가 레이아웃에 추가될 수 있습니다.
    readonly property string warning_sharedLayout: qsTr("Shared layout can not be edited.") // 공유레이아웃은 편집할 수 없습니다.
    readonly property string invalidVideoFileError: qsTr("Can not open the video file.")  // 비디오파일을 열수 없습니다.
    readonly property string fullscreenOnMessage: qsTr("Press the key to exit fullscreen : ");

    // Window close:
    readonly property string closeMessage1: qsTr("Close the current window?")
    readonly property string closeMessage2: qsTr("Exit the program?")
    readonly property string closeWindow: qsTr("Close window")
    readonly property string exitProgram: qsTr("Exit program")
    readonly property string alreadyRunningMessage: qsTr("Wisenet Viewer is already running.")

    // Shortcut box
    readonly property string volume: qsTr("Volume")
    readonly property string clearScreen: qsTr("Remove all video")

    // Certificate
    readonly property string selfSignedCertificate: qsTr("Self Signed Certificate Error")  // Self Signed 인증서 오류
    readonly property string unknownCertificate: qsTr("Unknown Certificate Error")  // 알수없는 인증서 오류
    readonly property string deviceCertificateExpired: qsTr("Device Certificate expired")  // 기기 인증서 기간 만료
    readonly property string invalidHtwRootCA: qsTr("Invalid root CA")  // Root 인증서 오류
    readonly property string expiredHtwRootCA: qsTr("Expired root CA")  // Root 인증서 기간 만료
    readonly property string deviceCerticateMacaddressInvalid: qsTr("Device certicate's macaddress is invalid")  // 기기인증서와 Mac 주소 불일치
    readonly property string deviceCertified: qsTr("Device is certified")  // 기기 인증 됨

    // Language
    readonly property string korean: qsTr("Korean")
    readonly property string english: qsTr("English")
    readonly property string chinese: qsTr("Chinese")
    readonly property string croatian: qsTr("Croatian")
    readonly property string czech: qsTr("Czech")
    readonly property string danish: qsTr("Danish")
    readonly property string dutch: qsTr("Dutch")
    readonly property string finnish: qsTr("Finnish")
    readonly property string french: qsTr("French")
    readonly property string german: qsTr("German")
    readonly property string greek: qsTr("Greek")
    readonly property string hungarian: qsTr("Hungarian")
    readonly property string italian: qsTr("Italian")
    readonly property string japanese: qsTr("Japanese")
    readonly property string norwegian: qsTr("Norwegian")
    readonly property string polish: qsTr("Polish")
    readonly property string portuguese: qsTr("Portuguese")
    readonly property string romanian: qsTr("Romanian")
    readonly property string russian: qsTr("Russian")
    readonly property string serbian: qsTr("Serbian")
    readonly property string spanish: qsTr("Spanish")
    readonly property string taiwanese: qsTr("Taiwanese")
    readonly property string turkish: qsTr("Turkish")

    // new
    readonly property string firmwareUpdateCancelMessage: qsTr("Do you want to suspend the firmware upgrade?")  // 펌웨어 업데이트를 중단하시겠습니까?
    readonly property string fwupdateCanceled: qsTr("Fwupdate canceled")  // 펌웨어 취소됨
    readonly property string selectEventType: qsTr("Event type")  // 이벤트 타입 유형
    readonly property string devicePowerStatus: qsTr("DevicePowerStatus")  // 장치 전원 상태
    readonly property string deviceFanStatus: qsTr("DeviceFanStatus")  // 장치 팬 상태
    readonly property string deviceStorageStatus: qsTr("DeviceStorageStatus")  // 장치 디스크 상태
    readonly property string deviceRecordingStatus: qsTr("DeviceRecordingStatus")  // 장치 저장 에러 상태
    readonly property string deviceSystemStatus: qsTr("DeviceSystemStatus")  // 장치 시스템 에러 상태
    readonly property string sec: qsTr("Seconds")  // 초
    readonly property string continious: qsTr("Continious")  // 연속

    // 1.1.0 ///////////////////////////////////////////////////////////////////////////

    readonly property string fisheyeDewarpingError: qsTr("Fisheye dewarping has failed."); // fisheye 드와핑이 실패하였습니다.
    readonly property string showInBrowser: qsTr("Show in web browser"); // 웹브라우저로 열기
    readonly property string overView: qsTr("Fisheye view"); // Fisheye Overview
    readonly property string singleView: qsTr("Single view"); // Fisheye Single view
    readonly property string quadrView: qsTr("Quad view"); // Fisheye Quadview
    readonly property string panoramaView: qsTr("Panorama view"); // Fisheye paorama view




    // Object search (AI Search)
    readonly property string objectSearch: qsTr("AI search");
    readonly property string objectProperties: qsTr("Object properties");
    readonly property string listView: qsTr("View list");
    readonly property string bestShotView: qsTr("View BestShot");
    readonly property string flagView: qsTr("View flags");
    readonly property string bestShot: qsTr("BestShot");
    readonly property string saveBestShotImageFile: qsTr("Save the BestShot image file");
    readonly property string noResultMessage: qsTr("There are no search results.");

    readonly property string person: qsTr("Person")

    readonly property string gender: qsTr("Gender")
    readonly property string male: qsTr("Male")
    readonly property string female: qsTr("Female")

    readonly property string clothing: qsTr("Clothing")
    readonly property string top: qsTr("Top")
    readonly property string bottom: qsTr("Bottom")

    readonly property string color: qsTr("Color")
    readonly property string black: qsTr("Black")
    readonly property string gray: qsTr("Gray")
    readonly property string white: qsTr("White")
    readonly property string red: qsTr("Red")
    readonly property string orange: qsTr("Orange")
    readonly property string yellow: qsTr("Yellow")
    readonly property string green: qsTr("Green")
    readonly property string blue: qsTr("Blue")
    readonly property string purple: qsTr("Purple")

    readonly property string bag: qsTr("Bag")
    readonly property string noBag: qsTr("No bag")

    readonly property string face: qsTr("Face")

    readonly property string age: qsTr("Age")
    readonly property string young: qsTr("Young")
    readonly property string adult: qsTr("Adult")
    readonly property string middle: qsTr("Middle")
    readonly property string senior: qsTr("Senior")

    readonly property string hat: qsTr("Hat")
    readonly property string noHat: qsTr("No hat")

    readonly property string glasses: qsTr("Glasses")
    readonly property string noGlasses: qsTr("No glasses")

    readonly property string faceMask: qsTr("Mask")
    readonly property string noFaceMask: qsTr("No mask")

    readonly property string vehicle: qsTr("Vehicle")
    readonly property string vehicleType: qsTr("Type")
    readonly property string car: qsTr("Car (Sedan/SUV/Van)")
    readonly property string bus: qsTr("Bus")
    readonly property string truck: qsTr("Truck")
    readonly property string motorcycle: qsTr("Motorcycle")
    readonly property string bicycle: qsTr("Bicycle")

    readonly property string objectType: qsTr("Object Type")
    readonly property string objectTypeInfo: qsTr("Only the supported functions on the selected device are enabled.")
    readonly property string licensePlateInfo: qsTr("Input the text you want to search for and press enter.")

    readonly property string ocr: qsTr("LPR")
    readonly property string licensePlate: qsTr("License plate ")


    // Wisenet setup search dictionary
    readonly property string editDevice: qsTr("Edit device") // 장비 편집
    readonly property string deleteDevice: qsTr("Delete device") // 장비 삭제
    readonly property string changePassword: qsTr("Change password") // 비밀번호 변경
    readonly property string httpsPort: qsTr("HTTPS port") // HTTPS 포트
    readonly property string profile: qsTr("Profile") // 프로파일
    readonly property string firmwareUpdateDevice: qsTr("Firmware upgrade (device)") // 펌웨어 업데이트 (장비)
    readonly property string configurationRestoreDevice: qsTr("Restore configuration (device)") // 설정 복구 (장비)
    readonly property string configurationBackupDevice: qsTr("Back up configuration (device)") // 설정 백업 (장비)
    readonly property string manualRegister: qsTr("Manual registration") // 수동 등록
    readonly property string deleteUser: qsTr("Delete user") // 사용자 삭제
    readonly property string deleteUserGroup: qsTr("Delete user group") // 사용자 그룹 삭제
    readonly property string editPermission: qsTr("Edit permission") // 권한 편집
    readonly property string deleteEventRules: qsTr("Delete event rules") // 이벤트룰 삭제
    readonly property string addEventTrigger: qsTr("Add event trigger") // 이벤트 트리거 추가
    readonly property string addEventAction: qsTr("Add event action") // 이벤트 액션 추가
    readonly property string autoRunWhenOSStart: qsTr("Auto run on startup") // OS 시작 시 자동 실행
    readonly property string showIPAddressInTree: qsTr("Show IP address") // IP 주소 보여주기
    readonly property string doubleBuffering: qsTr("Double buffering") // 더블 버퍼링
    readonly property string hardwareVideoDecoding: qsTr("Hardware video decoding") // 하드웨어 비디오 디코딩
    readonly property string audioInput: qsTr("Audio input") // 오디오 입력
    readonly property string localRecordingFolder: qsTr("Saved recordings folder") // 녹화 저장 폴더
    readonly property string systemNotifications: qsTr("System notifications") // 시스템 알림
    readonly property string softwareUpdate: qsTr("Software upgrade") // 소프트웨어 업데이트
    readonly property string logFiles: qsTr("Log files") // 로그 파일
    readonly property string logFilesFolder: qsTr("Log files folder") // 로그 파일 폴더
    readonly property string deviceSetup: qsTr("Device setup") // 장비 설정
    readonly property string userSetup: qsTr("User setup") // 사용자 설정
    readonly property string eventSetup: qsTr("Event setup") // 이벤트 설정
    readonly property string logDetail: qsTr("Log detail") // 상세 로그
    readonly property string systemSetup: qsTr("System setup") // 시스템 설정
    readonly property string localSetup: qsTr("Local setup") // 설정

    //SW update
    readonly property string latestVersion : qsTr("Latest version")
    readonly property string updateWisenetViewerSoftware : qsTr("Upgrade Wisenet Viewer software")
    readonly property string systemEventUpdateSoftwareMessage: qsTr("A new version of Wisenet Viewer is available for download and installation")
    readonly property string restartMessage: qsTr("After the download is complete, Wisenet Viewer restarts") // 다운로드가 완료되면 Wisenet Viewer가 다시 시작됩니다.

    //장비 등록
    readonly property string registerCompleted: qsTr("Register completed")  //등록 완료
    readonly property string registerFailed: qsTr("Register failed")        //등록 실패

    //장비 설정 페이지
    readonly property string resetDeviceName: qsTr("Load device name")     //장비이름으로 재설정

    // Video Export Authentication
    readonly property string invalidPassword: qsTr("Incorrect password")

    //모니터링 - 리소스 트리
    readonly property string connect: qsTr("Connect")     //연결
    readonly property string disconnect: qsTr("Disconnect")     //연결해제

    //setup - event eventRules:
    readonly property string openLayout: qsTr("Open layout") // 레이아웃 열기

    // Event Notification
    readonly property string deviceSystemConnected: qsTr("DeviceSystemConnected") // 장치 연결
    readonly property string deviceSystemDisconnected: qsTr("DeviceSystemDisconnected") // 장치 연결 해제
    readonly property string firmware: qsTr("Firmware") // 펌웨어
    readonly property string overload: qsTr("Overload") // 과부하

    readonly property string resetDeviceNameMessage: qsTr("Are you sure you want to retrieve it to the name set on your device?"); // 장치에서 설정된 이름으로 변경하시겠습니까?

    readonly property string dtls: qsTr("DTLS"); // DTLS

    // 다국어 재번역 필요함.
    readonly property string shutDownRestartMessage: qsTr("Do you want to shut down and run again?"); // 종료 후 다시 실행하시겠습니까?
    readonly property string downloadErrorMessage: qsTr("The download could not be completed. Please try again later.") // 다운로드를 완료할 수 없습니다. 나중에 다시 시도하십시오.
    readonly property string securityFirst: qsTr("Security first"); // 보안우선
    readonly property string performanceFirst: qsTr("Performance first"); // 성능우선

    // 1.2.0
    // Sequence
    readonly property string selectLayout: qsTr("Select layout") // 레이아웃 선택
    readonly property string interval: qsTr("Interval") // 주기
    readonly property string editSequence: qsTr("Edit sequence") // 시퀀스 편집
    readonly property string sequenceAddCancelMessage: qsTr("There is no layout.") // 레이아웃이 없습니다. // 추가 번역 필요



    //setup - event schedule;
    readonly property string eventSchedule: qsTr("Event schedule") // 이벤트 스케줄
    readonly property string schedule: qsTr("Schedule") // 스케줄
    readonly property string addEventSchedule: qsTr("Add event schedule") // 이벤트 스케줄 추가
    readonly property string editEventSchedule: qsTr("Edit event schedule") // 이벤트 스케줄 편집
    readonly property string scheduleName: qsTr("Schedule name") // 스케줄 이름
    readonly property string deleteEventScheduleConfirmMessage: qsTr("Are you sure you want to delete the selected event schedule?") // 선택된 이벤트 스케줄을 삭제하시겠습니까?
    readonly property string sameNameScheduleExists: qsTr("Same name schedule Exists. Use another name.") // 동일한 이름이 존재합니다. 다시 입력해주시기 바랍니다.

    //setup - local settting - auto start
    readonly property string autoRun: qsTr("Auto run when OS start") // 자동 시작

    //Dashboard
    readonly property string dashboard: qsTr("Dashboard") // Dashboard
    readonly property string camera: qsTr("Camera") // 카메라
    readonly property string connected: qsTr("Connected") // 연결됨
    readonly property string disconnected: qsTr("Disconnected") // 연결 해제됨
    readonly property string error: qsTr("Error") // 오류 (에러?)
    readonly property string powerError: qsTr("Power error") // 전원 오류
    readonly property string connectionError: qsTr("Connection error") // 연결 오류

    readonly property string errorType: qsTr("Error type") // 에러 종류
    readonly property string fan: qsTr("Fan") // 팬
    readonly property string power: qsTr("Power") // 전원
    readonly property string recording: qsTr("Recording") // 저장
    readonly property string cameraDisconnected: qsTr("Camera disconnected2") // 카메라 연결해제
    readonly property string good: qsTr("Good") // 좋음

    // HW Decoding
    readonly property string useHardwareDecoding: qsTr("Use H/W acceleration") // 하드웨어 가속 사용

    // mediaController - Export Video
    readonly property string digitalSignature: qsTr("Digital signature"); // 디지털 서명
    readonly property string fileNaming: qsTr("File naming")
    readonly property string folderNaming: qsTr("Folder naming")
    readonly property string fileFormat: qsTr("File format")
    readonly property string exportTo: qsTr("Export to")
    readonly property string chooseFolder: qsTr("Choose folder later")
    readonly property string pleaseEnterFileName : qsTr("Please enter the file name.")
    readonly property string pleaseEnterFolderName : qsTr("Please enter the folder name.")
    readonly property string verifySignature: qsTr("Verify digital signature")
    readonly property string verificationFinished: qsTr("Verification finished.")

    //initialPasswordSetting - macOS forceQuit
    readonly property string notSupportedMacOs: qsTr("Not supported macOS") // Wisenet Viewer 에서 지원하지 않는 OS 환경입니다. macOS 11 또는 그 이상의 버전으로 업그레이드 하시기 바랍니다.

    // Firmware update with server
    readonly property string channel: qsTr("Channel") // 채널
    readonly property string chId: qsTr("Channel ID") // 채널 아이디
    readonly property string recentVersion: qsTr("Recent version") // 최신 버전
    readonly property string upgradeVersion: qsTr("Upgrade version") // 업그레이드 버전
    readonly property string downloading: qsTr("Downloading") // 다운로드 중
    readonly property string downloadFailed: qsTr("Download failed") // 다운로드 실패
    readonly property string uploading: qsTr("Uploading") // 업로드 중
    readonly property string upgrading: qsTr("Upgrading") // 업그레이드 중

    // setup - event > email (22.05.31. added)
    readonly property string emailPageSubject: qsTr("E-mail")                                       // E-mail
    readonly property string emailServerAddress: qsTr("Server address")                             // 서버 주소
    readonly property string emailAuthentication: qsTr("Authentication")                            // 인증
    readonly property string emailSender: qsTr("Sender")                                            // 발송자
    readonly property string emailSendingTest: qsTr("Sending test")                                 // 발송 테스트
    readonly property string emailRecipientAddress: qsTr("Recipient(for test)")                     // 수신자
    readonly property string emailTest: qsTr("Test")                                                // 테스트
    readonly property string emailTestFail1: qsTr("Host connect fail")                              // 서버 접속 실패
    readonly property string emailTestFail2: qsTr("Host login fail")                                // 서버 로그인 실패
    readonly property string emailTestFail3: qsTr("E-mail send fail")                               // E-mail 전송 실패
    readonly property string emailTestFail4: qsTr("Database insert fail")                           // 데이터베이스 저장 실패
    readonly property string emailTestSuccess: qsTr("E-mail send success")                          // E-mail 전송 성공

    readonly property string fisheyeCameraType: qsTr("Fisheye camera type") // 피쉬아이 카메라 타입

    // Smart Search
    readonly property string smartSearch: qsTr("Smart search"); // 스마트 검색
    readonly property string virtualLine: qsTr("Virtual line"); // 가상선
    readonly property string roiArea: qsTr("ROI area"); // 관심영역
    readonly property string excludeArea: qsTr("Exclude area"); // 제외영역
    readonly property string clearAll: qsTr("Clear all"); // 모두지우기
    readonly property string maxLineWarn: qsTr("Able to draw up to 3 in virtual lines."); // 가상선은 최대 3개까지 그릴 수 있습니다
    readonly property string maxRoiAreaWarn: qsTr("Able to draw up to 3 in region of interest."); // 관심영역은 최대 3개까지 그릴 수 있습니다
    readonly property string maxExcldeAreaWarn: qsTr("Able to draw up to 3 in exclude area."); // 제외영역은 최대 3개까지 그릴 수 있습니다
    readonly property string searchPeroidWarn: qsTr("Please set search period in the timeline."); // 타임라인에서 검색 구간을 설정하세요.
    readonly property string searchAreaWarn: qsTr("Please set at least one virtual line or virtual area."); // 하나 이상의 가상 선이나 가상 영역을 설정하세요.
    readonly property string smartSearchPassing: qsTr("Passing")    // 지나감
    readonly property string smartSearchEntering: qsTr("Entering")  // 들어감
    readonly property string smartSearchExiting: qsTr("Exiting")    // 나감
    readonly property string smartSearchUnknown: qsTr("Unknown")    // 알 수 없음
    readonly property string smartSearchNoMetadataFound: qsTr("No metadata found for smart search at chosen time. Unable to peform smart search")    // 선택한 시간에 스마트 검색을 위한 메타데이터가 없습니다. 스마트 검색을 수행할수 없습니다.

    // Mic
    readonly property string deviceTalkAlreadyUse: qsTr("Audio out is already use.") // 다른 곳에서 오디오 출력을 사용중입니다.
    readonly property string micDeviceUnavailable: qsTr("The microphone device is not available.") // 마이크 장치를 사용할 수 없습니다.

    // setup - event > email (22.07.11. added)
    readonly property string pleaseCheckSender: qsTr("Please check the sender E-mail address.")     // 발신자 메일 주소를 확인해주세요.

    readonly property string eventScheduleDeleteErrorMessage: qsTr("You cannot delete an event schedule that is in use. If you want to delete the event schedule, remove the schedule from the event rule that is in use first.");

    // S1 DDNS, SIP
    readonly property string serialNumber: qsTr("Serial number");
    readonly property string useSerial: qsTr("Use serial"); // Use serial

    // S1 Password Tooltip (22.11.01. added)
    // 아래의 규칙에 맞게 암호를 설정 하십시오
    readonly property string s1PasswordTooltip1: qsTr("Set the password according to the below rules.");
    // 8자리 이상: 영대/영소/숫자/특수문자(예.!@#$%^) 중 3종류
    readonly property string s1PasswordTooltip2: qsTr("8 or more characters: Combination of alphabet, number and special character(Ex.!@#$%^)");
    // 10자리 이상: 영대/영소/숫자/특수문자(예.!@#$%^) 중 2종류
    readonly property string s1PasswordTooltip3: qsTr("10 or more characters: Combination of alphabet, number and speacial character(Ex.!@#$%^)");
    // ID와 동일하지 않도록 설정
    readonly property string s1PasswordTooltip4: qsTr("ID and Password cannot be the same.");
    // 같은 문자/숫자 3번이상 반복금지
    readonly property string s1PasswordTooltip5: qsTr("3 or more contiguous or repeated string character and key arrangement are not allowed.");
    // 연속되는 키보드 배열 3자리 이상 사용금지
    // (abc, 123, 321, !@# 등 순차적 증감 검지)
    readonly property string s1PasswordTooltip6: qsTr("Please don't use more than 3 characters.\n  (Characters with sequential libe abc, 123, 321, !@# etc, are not allowed)");
    // SECOM, S1등 추측하기 쉬운 단어 금지
    readonly property string s1PasswordTooltip7: qsTr("Don't use a password that can be speculated easily such as SECOM and S1");
    // 암호는 공백, ', ", \ 입력이 되지 않습니다.
    readonly property string s1PasswordTooltip8: qsTr("Passwords cannot be blank, ', \", \\");

    // S1 Password Guide (22.11.01. added)
    // 보안을 위해 암호는 영대/영소/숫자/특수문자를 8자 이상으로 설정해주십시오. 10자 이상은 2가지 조합 이상, 10자 미만일 경우 3가지 조합 이상으로 설정되어야 합니다.
    readonly property string s1PasswordGuide: qsTr("For security, set the password to at least 8 characters in alphabet uppercases / alphabet lowercases / number / special characters. More then 10 characters must be set with 2 or more combinations, and less than 10 characters must be set 3 or more combinations");

    // Video overload
    readonly property string systemEventIframeMode: qsTr("SystemEvent.IFrameMode"); // CPU 사용량 증가 (I-프레임 재생)
    readonly property string systemEventFullMode: qsTr("SystemEvent.FullFrameMode"); // CPU 사용량 안정 (모든 프레임 재생)
    readonly property string systemEventIframeModeMessage: qsTr("SystemEvent.IFrameMode.Message"); // CPU 사용량이 증가하여 프레임 건너뜀이 발생할 수 있습니다. 재생 중인 채널 수를 줄이십시오.
    readonly property string systemEventFullModeMessage: qsTr("SystemEvent.FullFrameMode.Message"); // CPU 사용량이 안정되어 모든 프레임을 재생합니다.

    // Text Search (22.08.24. added)
    readonly property string textSearch: qsTr("Text search");                                       // TEXT 검색
    readonly property string text: qsTr("Text")                                                     // 텍스트
    readonly property string keyword: qsTr("Keyword")                                               // 키워드
    readonly property string caseSensitivity: qsTr("Match with case sensitivity")                   // 대소문자 일치
    readonly property string wholeWord: qsTr("Match whole words")                                   // 모든 단어 일치
    readonly property string posName: qsTr("Pos name")                                              // Pos 이름
    readonly property string inputKeyword: qsTr("Please input keyword.")                            // 키워드를 입력하세요.
    readonly property string receiptView: qsTr("View Text");                                        // 영수증 보기

    // 타임라인
    readonly property string link: qsTr("Link");
    readonly property string unlink: qsTr("Unlink");
    readonly property string showThumbnail: qsTr("Show thumbnail");
    readonly property string hideThumbnail: qsTr("Hide thumbnail");
    readonly property string canceled: qsTr("Canceled");
    readonly property string switchToSingleTimeline: qsTr("Switch to single timeline");
    readonly property string switchToMultiTimeline: qsTr("Switch to multi Timeline");

    readonly property string pleaseSelectFilePath : qsTr("Please select file path.")

    // PTZ Controller
    readonly property string enableController: qsTr("Enable Controller") // 컨트롤러 사용
    readonly property string controllerConnected: qsTr("Controller connected") // 컨트롤러 연결됨

    // LDAP
    readonly property string serverUrl: qsTr("Server URL") // 서버 URL
    readonly property string adminDn: qsTr("Admin DN") // 도메인 DN
    readonly property string searchBase: qsTr("Search base") // 검색 기준
    readonly property string searchFilter: qsTr("Search filter") // 검색 필터
    readonly property string optional: qsTr("optional")
    readonly property string pleaseEnterServerUrl: qsTr("Please enter the Server URL.")
    readonly property string pleaseEnterAdminDn: qsTr("Please enter the Admin DN.")
    readonly property string pleaseEnterSearchBase: qsTr("Please enter the Search Base.")
    readonly property string ldapConnFailed: qsTr("Connection to LDAP server is failed.") // LDAP 서버 연결 실패
    readonly property string ldapConnSuccess: qsTr("Connection is successful") // LDAP 서버 연결 성공
    readonly property string searchFailed: qsTr("Search Failed") // LDAP 서버 검색 실패

    readonly property string ldapServerChanged: qsTr("Changing LDAP server URL will result in removing for all LDAP fetched users in Wisenet Viewer's user list."); // LDAP 서버 정보가 변경되면 기존에 등록된 LDAP ID 정보가 모두 삭제됩니다.
    readonly property string loginNewLdapID: qsTr("If you are using with LDAP ID now, you have to use new registered ID from next login.") // 현재 LDAP ID를 사용중인 경우 다음 로그인부터 새로 등록된 ID를 사용해야 합니다.
    readonly property string sameLDAPIdUser: qsTr("This ID is already in use. Do you want to overwrite user information? ") // 이미 사용중인 ID입니다. 사용자 정보를 덮어씌우겠습니까?
    readonly property string idAlreadyExist: qsTr("ID Already Exists")
    readonly property string idFounded: qsTr("ID founded")

    // Shortcut Box
    readonly property string editShortcutBox: qsTr("Edit Shortcut box");

    //login
    readonly property string saveId: qsTr("Save ID")

    // PTZ Swing/Tour/Trace
    readonly property string swing: qsTr("Swing"); // 스윙
    readonly property string tour: qsTr("Tour"); // 투어
    readonly property string trace: qsTr("Trace"); // 추적
    readonly property string start: qsTr("Start"); // 시작
    readonly property string end: qsTr("End"); // 끝
    readonly property string stop: qsTr("Stop"); // 정지
    readonly property string presetList: qsTr("Preset list"); // 프리셋 목록
    readonly property string groupList: qsTr("Group list"); // 그룹 목록

    // E-mail (22.11.10. added)
    readonly property string  emailNotSet: qsTr("Please set up your E-mail before using it.")

    // NVR Password Expiry
    readonly property string devicePasswordExpiry: qsTr("Device Password Expiration") // 장치 비밀번호 만료
    readonly property string devicePasswordExpiryMessage: qsTr("Device password expired. Please set a new password.") // 장치 비밀번호가 만료되었습니다. 새 비밀번호를 설정하세요.

    // Device config backup/restore password
    readonly property string deviceConfigBackupMessage: qsTr("Only devices that support encryption are supported.") // 암호화를 지원하는 장비만 지원합니다.

    readonly property string unsupported: qsTr("Unsupported") // 미지원


    // v1.04.00
    readonly property string masking: qsTr("Masking") // 마스킹백업 (용어미확정)
    readonly property string fixedMask: qsTr("Fixed mask") // 고정 마스크 (용어미확정)
    readonly property string manualMask: qsTr("Manual mask") // 수동 마스크 (용어미확정)
    readonly property string autoMask: qsTr("Auto mask") // 자동 마스크 (용어미확정)
    readonly property string trackingStart: qsTr("Start tracking") // 추적 시작 (용어미확정)
    readonly property string trackingStop: qsTr("Stop tracking") // 추적 중지 (용어미확정)
    readonly property string maskPreview: qsTr("Preview") // 미리보기
    readonly property string trackingFailed: qsTr("Tracking failed"); // 영상 추적에 실패하였습니다 (용어 미확정)
    readonly property string maskingType: qsTr("Masking type") // 마스킹 타입 (용어미확정)
    readonly property string mosaic: qsTr("Mosaic") // 모자이크 (용어미확정)
    readonly property string solid: qsTr("Solid") // 채우기 (용어미확정)
    readonly property string area: qsTr("Area") // 영역 (용어미확정)
    readonly property string include: qsTr("Include") // 포함 (용어미확정)
    readonly property string exclude: qsTr("Exclude") // 제외 (용어미확정)
    readonly property string cellSize: qsTr("Cell size") // 영역 (용어미확정)

    readonly property string emerald: qsTr("Emerald")
    readonly property string skyblue: qsTr("Skyblue")

    readonly property string license: qsTr("License") // 라이선스
    readonly property string enterLicenseKey: qsTr("Enter the license key") // 라이선스 키
    readonly property string activate: qsTr("Activate") // 활성화
    readonly property string activated: qsTr("Activated") // 활성화 됨

    readonly property string appImprovementSentence: qsTr("Log information is collected to improve usability and performance of Wisenet Viewer. We do not collect personal information. Would you like to participate in app improvement activities?") // Wisenet Viewer의 사용성 개선 및 성능향상을 위해 로그 정보를 수집하고 있습니다. 개인정보는 수집하지 않습니다. 앱 개선활동에 참여하시겠습니까?
    readonly property string participateImprovement: qsTr("I accept to participate in application improvement activities.") // 앱 개선 활동에 참여하겠습니다.
    readonly property string acceptAll: qsTr("Accept all") // 모두 수락

    /* dynamic event start - 소스 검색시 안나옴 */
    readonly property string digitalAutoTracking: qsTr("DigitalAutoTracking") //디지털 자동 추적
    readonly property string shockDetection: qsTr("ShockDetection") // 충격 감
    readonly property string mqttSubscription: qsTr("MQTTSubscription") // MQTT 구독
    readonly property string openSDKWiseAIIvaArea: qsTr("OpenSDK.WiseAI.IvaArea") // WiseAI.IVA 영역
    readonly property string openSDKWiseAILineCrossing: qsTr("OpenSDK.WiseAI.LineCrossing") // WiseAI.가상선
    readonly property string openSDKWiseAIMaskDetection: qsTr("OpenSDK.WiseAI.MaskDetection") // WiseAI. 얼굴 마스크 감지
    readonly property string openSDKWiseAIObjectDetection: qsTr("OpenSDK.WiseAI.ObjectDetection") // WiseAI.객체 감지
    readonly property string openSDKWiseAIQueueHigh: qsTr("OpenSDK.WiseAI.QueueHigh") // WiseAI.대기열 수준(높음)
    readonly property string openSDKWiseAIQueueMedium: qsTr("OpenSDK.WiseAI.QueueMedium") // WiseAI.대기열 수준(보통)
    readonly property string openSDKWiseAISlipAndFallDetection: qsTr("OpenSDK.WiseAI.SlipAndFallDetection") // WiseAI.쓰러짐 감지
    readonly property string openSDKWiseAISocialDistancingViolation: qsTr("OpenSDK.WiseAI.SocialDistancingViolation") // WiseAI.사회적 거리 감지
    readonly property string queueLevelHigh: qsTr("Queue.Level.High") // 대기열 수준(높음)
    readonly property string queueLevelMedium: qsTr("Queue.Level.Medium") // 대기열 수준(보통)
    readonly property string socialDistancingViolation: qsTr("SocialDistancingViolation") // 사회적 거리 감지
    readonly property string dynamicObjectDetectionVehicle: qsTr("ObjectDetection.Vehicle") // 차량 감지
    readonly property string dynamicObjectDetectionPerson: qsTr("ObjectDetection.Person") // 사람 감지
    readonly property string dynamicObjectDetectionLicensePlate: qsTr("ObjectDetectionLicense.Plate") // 번호판 감지
    readonly property string dynamicObjectDetectionFace: qsTr("ObjectDetection.Face")  // 얼굴 감지
    //dynamic event end

}
