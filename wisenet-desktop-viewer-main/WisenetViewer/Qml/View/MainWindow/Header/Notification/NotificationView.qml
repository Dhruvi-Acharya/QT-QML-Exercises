import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "qrc:/"

Popup{
    id: notificationView

    signal openNotificationSetting()

    function getDisplayName(name){
        if(name === "SystemEvent.DualSMPSFail")
            return WisenetLinguist.dualSmpsFail;
        else if(name === "SystemEvent.FanError")
            return WisenetLinguist.fanError;
        else if(name === "SystemEvent.CPUFanError")
            return WisenetLinguist.cpuFanError;
        else if(name === "SystemEvent.FrameFanError")
            return WisenetLinguist.frameFanError;
        else if(name === "SystemEvent.LeftFanError")
            return WisenetLinguist.leftFanError;
        else if(name === "SystemEvent.RightFanError")
            return WisenetLinguist.rightFanError;

        else if(name === "SystemEvent.HDDFull")
            return WisenetLinguist.hddFull;
        else if(name === "SystemEvent.SDFull")
            return WisenetLinguist.sdFull;
        else if(name === "SDFull")
            return WisenetLinguist.channelSdFull;
        else if(name === "SystemEvent.NASFull")
            return WisenetLinguist.nasFull;
        else if(name === "SystemEvent.HDDNone")
            return WisenetLinguist.hddNone;
        else if(name === "SystemEvent.HDDFail")
            return WisenetLinguist.hddFail;
        else if(name === "SystemEvent.SDFail")
            return WisenetLinguist.sdFail;
        else if(name === "SystemEvent.NASFail")
            return WisenetLinguist.nasFail;
        else if(name === "SDFail")
            return WisenetLinguist.sdFail;
        else if(name === "SystemEvent.HDDError")
            return WisenetLinguist.hddError;
        else if(name === "SystemEvent.RAIDDegrade")
            return WisenetLinguist.raidDegrade;
        else if(name === "SystemEvent.RAIDRebuildStart")
            return WisenetLinguist.raidRebuildStart;
        else if(name === "SystemEvent.RAIDFail")
            return WisenetLinguist.raidFail;
        else if(name === "SystemEvent.iSCSIDisconnect")
            return WisenetLinguist.iScsiDisconnect;
        else if(name === "SystemEvent.NASDisconnect")
            return WisenetLinguist.nasDisconnect;

        else if(name === "SystemEvent.RecordFiltering")
            return WisenetLinguist.recordFiltering;
        else if(name === "SystemEvent.RecordingError")
            return WisenetLinguist.recordingError;

        else if(name === "SystemEvent.CpuOverload")
            return WisenetLinguist.cpuOverload;
        else if(name === "SystemEvent.NetTxTrafficOverflow")
            return WisenetLinguist.netTxTrafficOverflow;
        else if(name === "SystemEvent.NetCamTrafficOverFlow")
            return WisenetLinguist.netCamTrafficOverFlow;
        else if(name === "SystemEvent.VPUError")
            return WisenetLinguist.vpuError;
        else if(name === "SystemEvent.MemoryError")
            return WisenetLinguist.memoryError;
        else if(name === "SystemEvent.NewFWAvailable")
            return WisenetLinguist.newFwAvailable;

        else if(name === "SystemEvent.CoreService.DeviceConnected")
            return WisenetLinguist.systemEventDeviceConnected;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedByService")
            return WisenetLinguist.systemEventDeviceDisconnectedByService;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedWithError")
            return WisenetLinguist.systemEventDeviceDisconnectedWithError;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedWithUnauthorized")
            return WisenetLinguist.systemEventDeviceDisconnectedWithUnauthorized;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedWithRestriction")
            return WisenetLinguist.systemEventDeviceDisconnectedWithRestriction;
        else if(name === "SystemEvent.CoreService.FirmwareUpgrade")
            return WisenetLinguist.systemEventFirmwareUpgrade;
        else if(name === "AlarmInput")
            return WisenetLinguist.alarmInput;
        else if(name === "SystemEvent.UpdateSoftware")
            return WisenetLinguist.updateWisenetViewerSoftware;
        else if(name === "SystemEvent.IFrameMode")
            return WisenetLinguist.systemEventIframeMode
        else if(name === "SystemEvent.FullFrameMode")
            return WisenetLinguist.systemEventFullMode
        else if(name === "SystemEvent.DevicePasswordExpiry")
            return WisenetLinguist.devicePasswordExpiry;

        return name;
    }

    function getMessage(name){
        if(name === "SystemEvent.DualSMPSFail")
            return WisenetLinguist.dualSmpsFailMessage;
        else if(name === "SystemEvent.FanError")
            return WisenetLinguist.fanErrorMessage;
        else if(name === "SystemEvent.CPUFanError")
            return WisenetLinguist.cpuFanErrorMessage;
        else if(name === "SystemEvent.FrameFanError")
            return WisenetLinguist.frameFanErrorMessage;
        else if(name === "SystemEvent.LeftFanError")
            return WisenetLinguist.leftFanErrorMessage;
        else if(name === "SystemEvent.RightFanError")
            return WisenetLinguist.rightFanErrorMessage;

        else if(name === "SystemEvent.HDDFull")
            return WisenetLinguist.hddFullMessage;
        else if(name === "SystemEvent.SDFull")
            return WisenetLinguist.sdFullMessage;
        else if(name === "SDFull")
            return WisenetLinguist.channelSdFullMessage;
        else if(name === "SystemEvent.NASFull")
            return WisenetLinguist.nasFullMessage;
        else if(name === "SystemEvent.HDDNone")
            return WisenetLinguist.hddNoneMessage;
        else if(name === "SystemEvent.HDDFail")
            return WisenetLinguist.hddFailMessage;
        else if(name === "SystemEvent.SDFail")
            return WisenetLinguist.sdFailMessage;
        else if(name === "SystemEvent.NASFail")
            return WisenetLinguist.nasFailMessage;
        else if(name === "SDFail")
            return WisenetLinguist.sdFailMessage;
        else if(name === "SystemEvent.HDDError")
            return WisenetLinguist.hddErrorMessage;
        else if(name === "SystemEvent.RAIDDegrade")
            return WisenetLinguist.raidDegradeMessage;
        else if(name === "SystemEvent.RAIDRebuildStart")
            return WisenetLinguist.raidRebuildStartMessage;
        else if(name === "SystemEvent.RAIDFail")
            return WisenetLinguist.raidFailMessage;
        else if(name === "SystemEvent.iSCSIDisconnect")
            return WisenetLinguist.iScsiDisconnectMessage;
        else if(name === "SystemEvent.NASDisconnect")
            return WisenetLinguist.nasDisconnectMessage;

        else if(name === "SystemEvent.RecordFiltering")
            return WisenetLinguist.recordFilteringMessage;
        else if(name === "SystemEvent.RecordingError")
            return WisenetLinguist.recordingErrorMessage;

        else if(name === "SystemEvent.CpuOverload")
            return WisenetLinguist.cpuOverloadMessage;
        else if(name === "SystemEvent.NetTxTrafficOverflow")
            return WisenetLinguist.netTxTrafficOverflowMessage;
        else if(name === "SystemEvent.NetCamTrafficOverFlow")
            return WisenetLinguist.netCamTrafficOverFlowMessage;
        else if(name === "SystemEvent.VPUError")
            return WisenetLinguist.vpuErrorMessage;
        else if(name === "SystemEvent.MemoryError")
            return WisenetLinguist.memoryErrorMessage;
        else if(name === "SystemEvent.NewFWAvailable")
            return WisenetLinguist.newFwAvailableMessage;

        else if(name === "SystemEvent.CoreService.DeviceConnected")
            return WisenetLinguist.systemEventDeviceConnectedMessage;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedByService")
            return WisenetLinguist.systemEventDeviceDisconnectedByServiceMessage;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedWithError")
            return WisenetLinguist.systemEventDeviceDisconnectedWithError;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedWithUnauthorized")
            return WisenetLinguist.systemEventDeviceDisconnectedWithUnauthorizedMessage;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedWithRestriction")
            return WisenetLinguist.systemEventDeviceDisconnectedWithRestrictionMessage;
        else if(name === "SystemEvent.CoreService.FirmwareUpgrade")
            return WisenetLinguist.systemEventFirmwareUpgradeMessage;
        else if(name === "AlarmInput")
            return WisenetLinguist.alarmInputMessage;
        else if(name === "SystemEvent.UpdateSoftware")
            return WisenetLinguist.systemEventUpdateSoftwareMessage;

        else if(name === "SystemEvent.IFrameMode")
            return WisenetLinguist.systemEventIframeModeMessage
        else if(name === "SystemEvent.FullFrameMode")
            return WisenetLinguist.systemEventFullModeMessage

        else if(name === "SystemEvent.DevicePasswordExpiry")
            return WisenetLinguist.devicePasswordExpiryMessage;

        return name;
    }

    topInset: 0
    leftInset: 0
    rightInset: 0
    bottomInset: 0
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    width: 307
    height: (notificationList.contentHeight + 2) > 580 ? 580 : (notificationList.contentHeight + 2);

    background: Rectangle {
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1
        //opacity: 0.7
    }

    ListView{
        id: notificationList
        anchors.fill: parent
        anchors.margins: 1

        clip: true

        model: notificationViewModel.eventListModel

        delegate: Rectangle{
            id: notificationItem

            width: parent ? parent.width - 2 : 0
            height: 77 + deviceText.height - 14 + messageText.height - 14

            color: notificationItemMouseArea.containsMouse ?  WisenetGui.contrast_08_dark_grey : WisenetGui.contrast_09_dark

            Text{
                id: deviceText
                anchors.top: parent.top
                anchors.topMargin: 13
                anchors.left: parent.left
                anchors.leftMargin: 14
                anchors.right: parent.right
                anchors.rightMargin: 30

                wrapMode: Text.Wrap
                //width: parent.width - 30
                //height: 14

                text: {
                    if(sourceName)
                        "[" + sourceName + "] " + notificationView.getDisplayName(alarmType)
                    else
                        notificationView.getDisplayName(alarmType)
                }

                color: WisenetGui.color_caution
                font.pixelSize: 12

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            WisenetImageButton{
                id: closeButton
                anchors.top: parent.top
                anchors.right: parent.right

                anchors.topMargin: 8
                anchors.rightMargin: 15

                width:12
                height:12

                visible: notificationItemMouseArea.containsMouse

                checkable: false
                imageSelected: "qrc:/Assets/Icon/Header/layout_x_button.svg"
                imageNormal: "qrc:/Assets/Icon/Header/layout_x_button.svg"
                imageHover: "qrc:/Assets/Icon/Header/layout_x_button_hover.svg"

                onClicked: {
                    console.log("notificationItem closeButton clicked - index:", index)
                    notificationViewModel.eventListModel.removeAt(index)
                }
            }

            Text{
                id: messageText

                anchors.top: deviceText.bottom
                anchors.topMargin: 6
                anchors.left: parent.left
                anchors.leftMargin: 14
                anchors.right: parent.right
                anchors.rightMargin: 15

                wrapMode: Text.Wrap
                text: notificationView.getMessage(alarmType)
                color: WisenetGui.contrast_05_grey
                font.pixelSize: 12

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Text{
                id: timeText

                anchors.top: messageText.bottom
                anchors.topMargin: 5
                anchors.left: parent.left
                anchors.leftMargin: 14
                anchors.right: passwordExtensionButton.left
                anchors.rightMargin: 15
                height: 13
                width: parent.width

                text: occurrenceTime

                color: WisenetGui.contrast_05_grey
                font.pixelSize: 11

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            WisenetImageButton{
                id: passwordExtensionButton
                anchors.top: timeText.top
                anchors.right: passwordChangeButton.left

                anchors.rightMargin: 12

                width:12
                height:12

                visible: messageText.text === WisenetLinguist.devicePasswordExpiryMessage

                checkable: false
                imageSelected: WisenetImage.object_search_filter_bus_selected
                imageNormal: WisenetImage.object_search_filter_bus_default
                imageHover: WisenetImage.object_search_filter_bus_hover

                onClicked: {
                    console.log("notificationItem passwordExtensionButton clicked - index:", index)
                    if(userGroupModel.isAdmin) {
                        switch(messageText.text) {
                        case WisenetLinguist.devicePasswordExpiryMessage:
                            notificationViewModel.eventListModel.extendDevicePassword(index)
                            notificationViewModel.eventListModel.removeAt(index)
                            break
                        default:
                            break
                        }
                    }
                }
            }

            WisenetImageButton{
                id: passwordChangeButton
                anchors.top: passwordExtensionButton.top
                anchors.right: parent.right

                anchors.rightMargin: 15

                width:12
                height:12

                visible: messageText.text === WisenetLinguist.devicePasswordExpiryMessage

                checkable: false
                imageSelected: WisenetImage.object_search_filter_glasses_selected
                imageNormal: WisenetImage.object_search_filter_glasses_default
                imageHover: WisenetImage.object_search_filter_glasses_hover

                onClicked: {
                    console.log("notificationItem passwordChangeButton clicked - index:", index)

                    if(userGroupModel.isAdmin) {
                        switch(messageText.text) {
                        case WisenetLinguist.devicePasswordExpiryMessage:
                            notificationViewModel.eventListModel.removeAt(index)
                            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceMainteanceChangePassword)
                            break
                        default:
                            break
                        }
                    }
                }
            }

            Rectangle{
                anchors.left: parent.left
                anchors.leftMargin: 14
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.bottom: parent.bottom
                height: 1
                width: parent.width

                color: WisenetGui.contrast_07_grey
            }

            MouseArea{
                id: notificationItemMouseArea
                hoverEnabled: true
//                anchors.fill: parent
                anchors {
                    top : parent.top
                    bottom: messageText.text === WisenetLinguist.devicePasswordExpiryMessage ? timeText.top : parent.bottom
                    left : parent.left
                    right : parent.right
                }

                onPressed: {
                    if(userGroupModel.isAdmin) {
//                        console.log("messageText.text:",messageText.text)
                        switch(messageText.text) {
                        case WisenetLinguist.systemEventUpdateSoftwareMessage:
                            notificationViewModel.eventListModel.removeAt(index)
                            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.SystemMaintenance)
                            break
                        case WisenetLinguist.devicePasswordExpiryMessage:
                            notificationViewModel.eventListModel.removeAt(index)
                            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceMainteanceChangePassword)
                            break
                        default:
                            break
                        }
                    }
                    mouse.accepted = false
                }
            }
        }

        ScrollBar.vertical: WisenetScrollBar {}
    }

    Connections{
        target: notificationViewModel.eventListModel

        onCountChanged:{
            if(notificationViewModel.eventListModel.count === 0)
                notificationView.close()
        }
    }

    MouseArea{
        anchors.fill: notificationList

        acceptedButtons: Qt.RightButton

        onPressed: {
            console.log("notificationView onPressed")
            if (mouse.button == Qt.RightButton)
            {
                contextMenu.popup()
            }
        }
    }

    // event search, notifications(settings), clear
    WisenetMediaMenu{
        id: contextMenu

        Action{
            text: WisenetLinguist.clear
            onTriggered: {
                notificationViewModel.eventListModel.removeAll()
            }
        }
        Action{
            text: WisenetLinguist.settings
            onTriggered: {
                openNotificationSetting()
            }
        }
    }
}
