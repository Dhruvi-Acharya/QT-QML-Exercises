import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0
import "qrc:/"
import "LayoutNavigator"
import "Notification"

Rectangle {
    id: headerView
    property int menuPopupViewWidth : menuPopupView.width
    property int layoutNavigationViewWidth : layoutNavigationView.width
    property int timeTextWidth : digitalClock.width
    property int utilityViewWidth : utilityView.width
    property int windowsControlViewWidth : windowsControlView.width
    property alias layoutNavigationView : layoutNavigationView

    property alias layoutList : layoutNavigationView.layoutList

    property alias currentLayout : layoutNavigationView.currentLayout
    property alias isFocusedWindow : layoutNavigationView.isFocusedWindow

    color: WisenetGui.layoutTabColor

    signal closeButtonClicked()

    signal tabChanged(var itemType, var id, var name)
    signal tabClosedWithoutSave(var closedLayouts)

    signal closeRightPanelFilterPopups()
    signal openNotificationSetup()

    signal sequencePlayStatusChanged(var isPlaying) // 기존 시퀀스 탭의 상태 변경 전달

    function makeLayoutWithItems(itemType, parentId, channelNumber){
        layoutNavigationView.makeNewLayoutwithItems(itemType, parentId, channelNumber)
    }

    function checkEmptyNavigator(){
        layoutNavigationView.checkEmptyNavigator()
    }

    function openInNewTab(items){
        layoutNavigationView.openInNewTab(items)
    }

    function openNavigationBarContext(x, y){
        navigationBarMenu.popup()
    }

    MenuPopupView{
        id: menuPopupView
        z: 6
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }

    LayoutNavigationView{
        z: 1
        id: layoutNavigationView
        anchors.left: menuPopupView.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        menuButtonWidth : menuPopupView.width
        clockWidth : digitalClock.width
        utilityButtonsWidth : utilityView.width
        windowButtonsWidth : windowsControlView.width

        onTabChanged: headerView.tabChanged(itemType, id, name)
        onTabClosedWithoutSave: headerView.tabClosedWithoutSave(closedLayouts)
        onSequencePlayStatusChanged: headerView.sequencePlayStatusChanged(isPlaying)
    }

    Rectangle{
        id: digitalClock

        height: 14
        width: 117
        z:2
        color: WisenetGui.contrast_09_dark

        anchors{
            top: parent.top
            right: utilityView.left
            bottom: parent.bottom
        }

        Text{
            id: digitalClockText

            anchors.fill: parent
            anchors.rightMargin: 17

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter

            color: WisenetGui.white
            font.pixelSize: 12
            text: localeManager.getClock()

            // Refresh Timer
            Timer {
                property int gcCount: 0
                interval: 1000; running: true; repeat: true;
                onTriggered: {
                    gcCount++;
                    // gc() call per 60 seconds
                    if (gcCount >= 60) {
                        gcCount = 0;
                        //console.log("Call GC()!!", localeManager.getClock());
                        gc();
                    }
                    digitalClockText.timeChanged()
                }
            }

            // Update time text
            function timeChanged() {
                digitalClockText.text = localeManager.getClock()
            }
        }
    }

    UtilityView{
        id: utilityView
        z: 6

        anchors{
            top: parent.top
            right: windowsControlView.left
            bottom: parent.bottom
        }

        onOpenNotificationPopup: {
            notificationView.x = utilityView.x - notificationView.width + 20
            notificationView.y = headerView.height
            notificationView.open()
        }

        onOpenLogoutMenu: {
            logoutMenu.x = utilityView.x
            logoutMenu.y = headerView.height
            logoutMenu.open()
        }

        onOpenResourceUsagePopup:{
            resourceUsageView.x = utilityView.x - resourceUsageView.width + 80
            resourceUsageView.y = headerView.height
            resourceUsageView.open()
        }
    }

    ResourceUsageView{
        id: resourceUsageView
    }

    WindowsControlView{
        id: windowsControlView
        z: 6

        anchors{
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            rightMargin: 1
        }

        onCloseButtonClicked : headerView.closeButtonClicked()
    }

    NotificationView{
        id: notificationView
        onOpenNotificationSetting:openNotificationSetup()
    }

    WisenetMediaMenu{
        id: logoutMenu

        Action{
            text: WisenetLinguist.logout + "(" + loginViewModel.loginId + ")"
            onTriggered: {
                headerView.closeRightPanelFilterPopups()
                loginViewModel.logoutExecute()
            }
        }
    }

    WisenetMediaMenu{
        id: navigationBarMenu

        Action{
            text: WisenetLinguist.newTab
            onTriggered: layoutNavigationView.makeNewLayout()
        }

        Action{
            text: WisenetLinguist.newWindow
            onTriggered: windowHandler.initializeNewWindow(false)
        }
    }

    // Alarm signal을 받음
    Connections{
        target: notificationViewModel

        function onNewSystemAlarmReceived(sourceName, alarmType){
            console.log("onNewSystemAlarmReceived", sourceName, alarmType)

            if(notificationView.visible)
                return

            switch(alarmType) {
            case "SystemEvent.UpdateSoftware":
                notificationToolTipText.text = notificationView.getDisplayName(alarmType)
                break
            case "SystemEvent.DevicePasswordExpiry":
                notificationToolTipText.text = notificationView.getDisplayName(alarmType)
                break
            default:
                notificationToolTipText.text = "[" + sourceName + "] "+ notificationView.getDisplayName(alarmType)
                break
            }

            notificationToolTip.open()
        }
    }

    // 툴팁
    ToolTip{
        id: notificationToolTip

        x : utilityView.x - width + 20
        y : headerView.height

        leftInset: 0
        rightInset: 0
        topInset: 0
        bottomInset: 0
        leftPadding: 16
        rightPadding: 16
        topPadding: 16
        bottomPadding: 16

        background: Rectangle {
            color: WisenetGui.contrast_09_dark
            border.color: WisenetGui.contrast_07_grey
            border.width: 1
        }

        height :  notificationToolTipText.height > 14 ? notificationToolTipText.height + 32 : 46
        width : 250
        timeout: 5000

        Text{
            id: notificationToolTipText
            color: WisenetGui.orange
            font.pixelSize: 12
            width: 218

            wrapMode: Text.WrapAnywhere

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        MouseArea{
            id: notificationToolTipMouseArea
            anchors.fill: parent
            onPressed: {
                if(userGroupModel.isAdmin) {
//                    console.log("notificationToolTipText.text:",notificationToolTipText.text)
                    switch(notificationToolTipText.text) {
                    case WisenetLinguist.updateWisenetViewerSoftware:
                        windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.SystemMaintenance)
                        break
                    case WisenetLinguist.devicePasswordExpiry:
                        windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceMainteanceChangePassword)
                        break
                    default:
                        break
                    }
                }
            }
        }
    }

    function changeLayoutNavigatorVisible(value){
        layoutNavigationView.visible = value
    }

    // test code
    Component.onCompleted: {
        //layoutNavigationView.makeNewLayout();
    }
}
