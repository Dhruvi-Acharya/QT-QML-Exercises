import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Qt.labs.qmlmodels 1.0
import Wisenet.Setup 1.0

Page{
    id : root

    property int xMargin : 15
    property int yMargin: 20
    property color backgroundColor: WisenetGui.color_setup_Background
    property int defaultCellHeight: 36

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    function openAddUserGroup(){
                addUserGroup.groupGuid = ""
                addUserGroup.visible = true
                root.allUncheckedDisplay()
    }

    function getResourceIcon(displayName){
        if("Playback" === displayName){
            return WisenetImage.setup_user_permission_search_normal;
        }
        if("Export video" === displayName){
            return WisenetImage.setup_user_permission_backup_normal;
        }
        if("Local recording" === displayName){
            return WisenetImage.setup_user_permission_localRecording_normal;
        }
        if("Ptz control" === displayName){
            return WisenetImage.setup_user_permission_ptz_normal;
        }
        if("Audio" === displayName){
            return WisenetImage.setup_user_permission_audio_normal;
        }
        if("Mic" === displayName){
            return WisenetImage.setup_user_permission_mic_normal;
        }
        else
            return "";
    }


    function reset(){
        userGroupTable.verticalScrollPosition = 0
        userGroupViewModel.reset()
    }

    function allUncheckedDisplay(){
        userGroupTable.allCheckedStatus = false
    }

    UserGroupViewModel{
        id: userGroupViewModel
    }

    Connections{
        target: userGroupTable
        onAllChecked:{
            userGroupViewModel.setAllCheckState(checked)
        }
    }
    Connections{
        target: userGroupViewModel
        function onResultMessage(success, msg){
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }
    }

    Connections{
        target: addUserGroup.addUserGroupAlais
        function onResultMessage(success, msg){
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }
    }

    onVisibleChanged: if(visible == true) reset()


    ListModel {
        id: userGroupColumns
        Component.onCompleted: {
            append({ "columnWidth": 0, "title": qsTr("ID"), "sort": 3, "splitter": 0})
            append({ "columnWidth": 50, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({ "columnWidth": 165, "title": WisenetLinguist.userGroup, "sort": 1, "splitter": 1})
            append({ "columnWidth": 0, "title": qsTr("Is Admin"), "sort": 0, "splitter": 1})
            append({ "columnWidth": 250, "title": WisenetLinguist.permission, "sort": 3, "splitter": 1})
            append({ "columnWidth": 250, "title": WisenetLinguist.resources, "sort": 3, "splitter": 1})
            append({ "columnWidth": 250, "title": WisenetLinguist.layout, "sort": 3, "splitter": 1})
        }
    }

    DelegateChooser {
        id: userGroupChooser

        DelegateChoice {
            id : delegateCheck
            column: 1
            delegate:
                WisenetSetupTableCheckRectangle{
                checkedValue: display
                onContainsMouseChanged: {
                    userGroupViewModel.setHoveredRow(row, containsMouse);
                }
                onClicked: {
                    userGroupViewModel.setCheckState(row, checkedValue)
                    userGroupViewModel.setHighlightRow(row, modifier);
                    if(display === false)
                        root.allUncheckedDisplay()
                }
            }
        }
        DelegateChoice {
            column: 4
            delegate:Rectangle {
                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

                Row{
                    spacing: 10
                    anchors{
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    Repeater{
                        model : display
                        Item{
                            width:  ((modelData === "All permission") ? 100 : 16)
                            height: 16
                            WisenetTableCellText{
                                id : allText
                                color: highlightRole ?  WisenetGui.contrast_00_white :hoveredRole ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                                visible:  (modelData === "All permission")
                                width: 100
                                text: WisenetLinguist.allPermission
                            }
                            Image{
                                id: eventImage
                                visible: !allText.visible
                                width: (visible? 16 : 0)
                                height: (visible? 16 : 0)
                                anchors.verticalCenter: parent.verticalCenter
                                sourceSize: Qt.size(width, height)
                                source: root.getResourceIcon(modelData)
                            }

                        }
                    }
                }
                WisenetTableCellLine{
                    anchors.bottom: parent.bottom
                }
                MouseArea {
                    anchors.fill : parent
                    hoverEnabled: true

                    onPressed: {
                        userGroupViewModel.setHighlightRow(row, mouse.modifiers);
                    }
                    onDoubleClicked: {
                        addUserGroup.groupGuid = userGroupViewModel.getUserGroupId(row)
                        addUserGroup.visible = true
                    }
                    onContainsMouseChanged: {
                        userGroupViewModel.setHoveredRow(row, containsMouse);
                    }
                }
            }
        }

        DelegateChoice {
            delegate:
                WisenetSetupTableDefaultRect{
                text: display
                onPressed: {
                    userGroupViewModel.setHighlightRow(row, modifier);
                }
                onDoubleClicked: {
                    addUserGroup.groupGuid = userGroupViewModel.getUserGroupId(row)
                    addUserGroup.visible = true
                }
                onContainsMouseChanged: {
                    userGroupViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }
    }

    WisenetDivider{
        id : divider
        x : xMargin
        y : 20
        Layout.fillWidth: true
        label: WisenetLinguist.userGroup
    }
    WisenetSetupApplyResultMessage{
        id: applyResultMessage
        anchors {
            right : divider.right
            rightMargin: 30
            verticalCenter: divider.bottom
        }
    }

    Row{
        id: buttonLayout
        Layout.fillWidth: true
        anchors.left: divider.left
        anchors.top: divider.bottom
        anchors.topMargin: 20

        WisenetSetupAddButton{
            text : WisenetLinguist.add
            onClicked: {
                addUserGroup.groupGuid = ""
                addUserGroup.visible = true
                root.allUncheckedDisplay()
            }
        }
        AddUserGroupView{
            id : addUserGroup
            visible: false
            width: WisenetGui.popupSecondWindowDefaultWidth
            height: WisenetGui.popupSecondWindowDefaultHeight
        }
        WisenetSetupDeleteButton{
            id : deleteButton
            text : WisenetLinguist.deleteText
            enabled: (userGroupViewModel.selectedRowCount > 0)
            onClicked: {
                if (!deleteDialog.dontAskAgain) {
                    deleteDialog.open();
                }
                else {
                    userGroupViewModel.deleteUserGroup()
                    root.allUncheckedDisplay()
                }
            }
        }
    }
    WisenetSetupAskDialog {
        id: deleteDialog
        msg : WisenetLinguist.deleteUserGroupConfirmMessage
        onAccepted: {
            userGroupViewModel.deleteUserGroup()
            root.allUncheckedDisplay()
        }
        onRejected: {
            dontAskAgain = false
        }
    }

    Rectangle{
        anchors.left: buttonLayout.left
        anchors.top: buttonLayout.bottom
        width: parent.width - (xMargin *2) - 10
        height: parent.height - buttonLayout.y - buttonLayout.height - buttonRect.height
        color: "transparent"

        WisenetTable {
            id: userGroupTable
            anchors.fill: parent
            columns: userGroupColumns
            cellDelegate: userGroupChooser
            tableModel : userGroupViewModel
            contentColor: "transparent"
        }
    }
    Rectangle{
        id : buttonRect
        width: parent.width
        height: 40
        color: "transparent"
    }
}

