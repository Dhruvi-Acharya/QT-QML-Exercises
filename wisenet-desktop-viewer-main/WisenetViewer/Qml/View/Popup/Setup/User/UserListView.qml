import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
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

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    function reset(){
        userListViewModel.reset()
    }

    function allUncheckedDisplay(){
        userListTable.allCheckedStatus = false
    }

    function openAddUser(){
        addUser.userGuid = ""
        addUser.visible = true
        root.allUncheckedDisplay()
    }

    onVisibleChanged: if(visible == true) reset()

    UserListViewModel{
        id: userListViewModel
    }

    Connections{
        target: userListTable
        onAllChecked:{
            userListViewModel.setAllCheckState(checked)
        }
    }

    Connections{
        target: userListViewModel
        function onResultMessage(success, msg){
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }
    }
    Connections{
        target: addUser.addUserViewModelAlais
        function onResultMessage(success, msg){
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }
    }

    ListModel {
        id: userListColumns

        Component.onCompleted: {
            append({ "columnWidth": 0, "title": qsTr("UserGuid"), "sort": 3, "splitter": 0})
            append({ "columnWidth": 50, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({ "columnWidth": 170, "title": WisenetLinguist.id_, "sort": 1, "splitter": 1})
            append({ "columnWidth": 50, "title": qsTr("LDAP"), "sort": 1, "splitter": 1})
            append({ "columnWidth": 170, "title": WisenetLinguist.name, "sort": 1, "splitter": 1})
            append({ "columnWidth": 0, "title": WisenetLinguist.type, "sort": 3, "splitter": 0})
            append({ "columnWidth": 0, "title": "Group Id", "sort": 0, "splitter": 0})
            append({ "columnWidth": 170, "title": WisenetLinguist.groupName, "sort": 0, "splitter": 1})
            append({ "columnWidth": 170, "title": WisenetLinguist.emailPageSubject, "sort": 0, "splitter": 1})
            append({ "columnWidth": 170, "title": WisenetLinguist.description, "sort": 0, "splitter": 1})
        }
    }

    DelegateChooser {
        id: userListChooser

        DelegateChoice {
            column: 1
            delegate:WisenetSetupTableCheckRectangle{
                checkedValue: display
                onContainsMouseChanged: {
                    userListViewModel.setHoveredRow(row, containsMouse);
                }
                onClicked: {
                    userListViewModel.setCheckState(row, checkedValue)
                    userListViewModel.setHighlightRow(row, modifier);
                    if(display === false)
                        root.allUncheckedDisplay()
                }
            }
        }

        DelegateChoice {
            column: 3
            delegate: Rectangle {
                visible: true
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

                Image{
                    id: ldapImage
                    visible: display? true : false
                    width: (visible? 22 : 0)
                    height: (visible? 22 : 0)
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    sourceSize: Qt.size(width, height)
                    source: ldapMouseArea.containsMouse? WisenetImage.checked_hover : WisenetImage.checked
                }

                WisenetTableCellLine{
                    anchors.bottom: parent.bottom
                }

                MouseArea {
                    id: ldapMouseArea
                    anchors.fill : parent
                    hoverEnabled: true

                    onPressed: userListViewModel.setHighlightRow(row, mouse.modifier);
                    onContainsMouseChanged: userListViewModel.setHoveredRow(row, containsMouse);
                    onDoubleClicked: {
                        addUser.userGuid = userListViewModel.getUserId(row)
                        addUser.visible = true
                    }
                }
            }
        }

        DelegateChoice {
            delegate:WisenetSetupTableDefaultRect{
                text: display
                onPressed: {
                    userListViewModel.setHighlightRow(row, modifier);
                }
                onDoubleClicked: {
                    addUser.userGuid = userListViewModel.getUserId(row)
                    addUser.visible = true
                }
                onContainsMouseChanged: {
                    userListViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }
    }



    WisenetDivider{
        id : divider
        x : xMargin
        y : 20
        Layout.fillWidth: true
        label: WisenetLinguist.userList
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
                addUser.userGuid = ""
                addUser.visible = true
                root.allUncheckedDisplay()
            }
        }
        AddUserView{
            id : addUser
            visible: false
            width: WisenetGui.popupSecondWindowDefaultWidth
            height: WisenetGui.popupSecondWindowDefaultHeight
        }
        WisenetSetupDeleteButton{
            id : deleteButton
            text : WisenetLinguist.deleteText
            enabled: (userListViewModel.selectedRowCount > 0)
            onClicked: {
                if (!deleteDialog.dontAskAgain) {
                    deleteDialog.open();
                }
                else {
                    userListViewModel.deleteUser()
                    root.allUncheckedDisplay()
                }
            }
        }
    }

    WisenetSetupAskDialog {
        id: deleteDialog
        msg : WisenetLinguist.deleteUserConfirmMessage
        onAccepted: {
            userListViewModel.deleteUser()
            root.allUncheckedDisplay()
        }
        onRejected: {
            dontAskAgain= false
        }
    }

    Rectangle{
        anchors.left: buttonLayout.left
        anchors.top: buttonLayout.bottom
        width: parent.width - (xMargin *2) - 10
        height: parent.height - buttonLayout.y - buttonLayout.height - buttonRect.height
        color: "transparent"
        WisenetTable {
            id: userListTable
            anchors.fill: parent
            columns: userListColumns
            cellDelegate: userListChooser
            tableModel : userListViewModel
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

