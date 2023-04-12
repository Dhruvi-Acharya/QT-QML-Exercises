import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Setup 1.0

Page {
    id: mainPage
    property int xMargin : 15
    property int yMargin: 20
    property alias deviceFirmwareUpdate : firmwareUpdate

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    onVisibleChanged: {
        if (visible == true){
            buttonLayout.resetButton()
            firmwareButton.checked = true
        }
    }

    Connections{
        target: firmwareUpdate
        onCancelDeviceMainteanceConfigRestore :{
            buttonLayout.resetButton()
            restoreButton.checked = true
        }

        onCancelDeviceMainteanceConfigBackup : {
            buttonLayout.resetButton()
            backupButton.checked = true
        }

        onCancelDeviceMainteanceChangePassword :{
            buttonLayout.resetButton()
            passwordChangeButton.checked = true
        }
    }

    function openPasswordChange() {
        buttonLayout.resetButton()
        passwordChangeButton.checked = true
    }

    DeviceConfigViewModel{
        id : deviceConfigViewModel
    }

    WisenetDivider{
        id : divider
        x : xMargin
        y : 20
        Layout.fillWidth: true
        label: WisenetLinguist.maintenance
    }

    Row {
        id: buttonLayout
        Layout.fillWidth: true
        anchors.left: divider.left
        anchors.top: divider.bottom
        anchors.topMargin: 20

        property int textWidth: 160
        property int textHeight: 30

        function resetButton(){
            firmwareButton.checked = restoreButton.checked = backupButton.checked = passwordChangeButton.checked = false
        }

        WisenetSetupTextButton {
            id : firmwareButton
            text: WisenetLinguist.firmwareUpdate
            checked: false
            width: buttonLayout.textWidth
            height: buttonLayout.textHeight

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    buttonLayout.resetButton()
                    firmwareButton.checked = true

                }
            }
        }
        SetupTabLine{ y : 10}

        WisenetSetupTextButton {
            id : restoreButton
            leftPadding:10
            width: buttonLayout.textWidth
            height: buttonLayout.textHeight
            text: WisenetLinguist.restore
            textHAlignment : Text.AlignHCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    firmwareUpdate.askCancelFirmwareUpdate(WisenetSetupTree.SubPageIndex.DeviceMainteanceConfigRestore)
                }
            }
        }
        SetupTabLine{ y : 10}

        WisenetSetupTextButton {
            id : backupButton
            leftPadding:10
            width: buttonLayout.textWidth
            height: buttonLayout.textHeight
            textHAlignment : Text.AlignHCenter
            text: WisenetLinguist.backup
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    firmwareUpdate.askCancelFirmwareUpdate(WisenetSetupTree.SubPageIndex.DeviceMainteanceConfigBackup)
                }
            }
        }
        SetupTabLine{ y : 10}

        WisenetSetupTextButton {
            id : passwordChangeButton
            leftPadding:10
            width: buttonLayout.textWidth
            height: buttonLayout.textHeight
            textHAlignment : Text.AlignHCenter
            text: WisenetLinguist.passwordChange
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    firmwareUpdate.askCancelFirmwareUpdate(WisenetSetupTree.SubPageIndex.DeviceMainteanceChangePassword)
                }
            }
        }

    }
    Rectangle{
        anchors.left: buttonLayout.left
        anchors.top: buttonLayout.bottom
        width: parent.width - (xMargin *2) - 10
        height: parent.height - buttonLayout.y - buttonLayout.height
        color: "transparent"
        DeviceFirmwareRestorePageView {
            id : firmwareUpdate
            isFirmwareView : true
            anchors.fill: parent
            visible: firmwareButton.checked
        }
        DeviceFirmwareRestorePageView{
            id : configRestore
            isFirmwareView : false
            anchors.fill: parent
            visible: restoreButton.checked
        }
        DeviceConfigBackupPageView{
            id : configBackup
            anchors.fill: parent
            visible: backupButton.checked
        }

        DeviceChangePasswordPageView {
            id : changePassword
            anchors.fill: parent
            visible: passwordChangeButton.checked
        }
    }
}

