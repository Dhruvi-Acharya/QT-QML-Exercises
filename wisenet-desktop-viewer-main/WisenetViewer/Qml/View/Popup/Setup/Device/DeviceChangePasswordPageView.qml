import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import WisenetStyle 1.0
import QtQuick.Dialogs 1.3
import WisenetLanguage 1.0
import Qt.labs.platform 1.1
import Wisenet.Setup 1.0

Page {
    id: root
    bottomPadding: 10
    anchors.fill: parent
    implicitWidth: WisenetGui.popupSecondWindowDefaultWidth
    implicitHeight: WisenetGui.popupSecondWindowDefaultHeight
    onVisibleChanged: if(visible) reset()
    background: Rectangle{
        anchors.fill: parent
        color: root.backgroundColor
    }

    property bool isFirmwareView : true
    property color backgroundColor: WisenetGui.color_setup_Background

    function reset(){
        deviceConfigViewModel.reset(DeviceConfigViewModel.PasswordChangeViewType)
        inputCurrentDevicePassword.text = inputDevicePassword.text = inputDevicePasswordConfirm.text = ""
    }
    function allUncheckedDisplay(){
        deviceListTable.allCheckedStatus = false
    }

    Connections{
        target: deviceListTable
        onAllChecked:{
            console.log("all checked", checked)
            deviceConfigViewModel.setAllCheckState(checked)
        }
    }

    ListModel{
        id: deviceListColumns
        //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
        //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
        Component.onCompleted: {
            append({ "columnWidth": 0, "title": WisenetLinguist.deviceId, "sort": 0, "splitter": 0})
            append({ "columnWidth": 50, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({ "columnWidth": 190, "title": WisenetLinguist.model, "sort": 0, "splitter": 1})
            append({ "columnWidth": 190, "title": WisenetLinguist.device, "sort": 0, "splitter": 1})
            append({ "columnWidth": 240, "title": WisenetLinguist.ip, "sort": 0, "splitter": 1})
            append({ "columnWidth": 295, "title": WisenetLinguist.status, "sort": 0, "splitter": 1})
        }
    }
    DelegateChooser {
        id: deviceListChooser

        DelegateChoice {
            id : delegateChoiceCheck
            column: 1
            delegate: WisenetSetupTableCheckRectangle{
                implicitWidth: 300
                checkedValue: display
                onContainsMouseChanged: {
                    deviceConfigViewModel.setHoveredRow(row, containsMouse);
                }
                onClicked: {
                    deviceConfigViewModel.setCheckState(row, checkedValue)
                    deviceConfigViewModel.setHighlightRow(row, modifier);
                    if(display === false)
                        root.allUncheckedDisplay()
                }
            }
        }
        DelegateChoice {
            column : 5
            delegate:  WisenetSetupTableDefaultRect{
                implicitWidth: 300
                text: deviceConfigViewModel.translate(display)
                onPressed: {
                    deviceConfigViewModel.setHighlightRow(row, modifier);
                }
                onContainsMouseChanged: {
                    deviceConfigViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }

        DelegateChoice {
            id : defaultDele
            delegate:  WisenetSetupTableDefaultRect{
                implicitWidth: 300
                text: display
                onPressed: {
                    deviceConfigViewModel.setHighlightRow(row, modifier);
                }
                onContainsMouseChanged: {
                    deviceConfigViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }
    }

    Rectangle{
        color:  WisenetGui.transparent
        anchors.fill: parent
        Column {
            id: mainLayout
            anchors.fill: parent
            spacing: 10
            topPadding: 10

            Rectangle{
                id : sameModelRect
                width: parent.width
                height: 15
                color : "transparent"
            }

            Rectangle {
                width: parent.width
                height: parent.height -  passwordRect.height - sameModelRect.height - (mainLayout.spacing * 3) - mainLayout.topPadding
                color: "transparent"
                WisenetTable {
                    id: deviceListTable
                    anchors.fill: parent
                    columns: deviceListColumns
                    cellDelegate: deviceListChooser
                    tableModel : deviceConfigViewModel
                }
            }
            Rectangle{
                id : passwordRect
                width: parent.width
                height: 25
                color: "transparent"
                Row{
                    anchors.fill: parent
                    spacing: 20

                    property int passLengh: 200

                    WisenetLabel {
                        id: txtCurrentPassword
                        text: WisenetLinguist.currentPassword
                    }
                    WisenetPasswordBox{
                        id : inputCurrentDevicePassword
                        text : ""
                        width: parent.passLengh
                        showPasswordButtonVisible: false
                    }

                    WisenetLabel {
                        id: txtPassword
                        width: 50
                        text: WisenetLinguist.password
                    }
                    WisenetPasswordBox{
                        id : inputDevicePassword
                        text : ""
                        width: parent.passLengh
                        showPasswordButtonVisible: true
                    }
                    WisenetLabel {
                        id: txtPasswordConfirm
                        text: WisenetLinguist.confirmPassword
                        wrapMode: TextEdit.WordWrap

                    }
                    WisenetPasswordBox{
                        id : inputDevicePasswordConfirm
                        text : ""
                        width: parent.passLengh
                        showPasswordButtonVisible: true
                    }
                }
            }
            WisenetMessageDialog{
                id: successMsgDialog
                visible: false
                property bool isSuccess : false
                applyButtonVisible: false
                cancelButtonText: WisenetLinguist.close
            }
        }
    }
    footer:
        WisenetDialogButtonBox{
        id: registerButton
        okText: WisenetLinguist.change
        onAccepted: {
            if(inputDevicePassword.text.length === 0 ||
                    (inputDevicePassword.text !== inputDevicePasswordConfirm.text)){
                successMsgDialog.message = WisenetLinguist.pleaseCheckPassword
                successMsgDialog.isSuccess = false
                successMsgDialog.visible= true
            }
            else
                deviceConfigViewModel.startChangePasswordSetting(inputCurrentDevicePassword.text, inputDevicePassword.text);
        }
    }
}
