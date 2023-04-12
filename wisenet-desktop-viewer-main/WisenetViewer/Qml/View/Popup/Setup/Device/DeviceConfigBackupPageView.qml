import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import WisenetStyle 1.0
import QtQuick.Dialogs 1.3
import WisenetLanguage 1.0
//import Qt.labs.platform 1.1
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
    property string filePath : ""

    function reset(){
        deviceConfigViewModel.reset(DeviceConfigViewModel.ConfigBackupViewType)
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
            append({ "columnWidth": 120, "title": WisenetLinguist.model, "sort": 0, "splitter": 1})
            append({ "columnWidth": 180, "title": WisenetLinguist.device, "sort": 0, "splitter": 1})
            append({ "columnWidth": 200, "title": WisenetLinguist.ip, "sort": 0, "splitter": 1})
            append({ "columnWidth": 200, "title": WisenetLinguist.version, "sort": 0, "splitter": 1})
            append({ "columnWidth": 215, "title": WisenetLinguist.status, "sort": 0, "splitter": 1})
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
            column: 6
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
        color: WisenetGui.transparent
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
                height: parent.height -  sameModelRect.height - (mainLayout.spacing * 2) - mainLayout.topPadding
                color: "transparent"

                WisenetTable {
                    id: deviceListTable
                    anchors.fill: parent
                    columns: deviceListColumns
                    cellDelegate: deviceListChooser
                    tableModel : deviceConfigViewModel
                }
            }
            FileDialog {
                id: folderDialog
                visible: false
                //flags: FolderDialog.ShowDirsOnly
                folder: shortcuts.documents
                selectFolder: true
                onAccepted: {
                    console.log("folderDialog folder", folder);
                    console.log("folderDialog file  ", fileUrl);
                    filePath = filePathConveter.getPathByUrl(fileUrl)
                    if(deviceConfigViewModel.isConfigBackupRequirePassword())
                        passwordRequired.showDialog()
                    else
                        deviceConfigViewModel.startConfigBackup(filePath, "")
                }
            }
        }
    }
    footer:
        WisenetDialogButtonBox{
        id: registerButton
        okText: WisenetLinguist.backup
        onAccepted: {
            if(deviceConfigViewModel.selectedRowCount > 0)
                folderDialog.visible = true
            else{
                msgDialog.message = WisenetLinguist.pleaseSelectDevice
                msgDialog.show()
            }
        }
    }

    DeviceConfigPasswordRequired{
        id : passwordRequired
        needConfirm: true
        onPasswordEntered:{
            deviceConfigViewModel.startConfigBackup(filePath, password.toString())
        }
    }
    WisenetMessageDialog{
        id: msgDialog
        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }
}
