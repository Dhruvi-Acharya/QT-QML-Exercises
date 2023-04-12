import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import Wisenet.Tree 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.UserLayoutTreeSourceModel 1.0
import WisenetLanguage 1.0

WisenetPopupWindowBase{
    id: addUserGroupWindow
    title: WisenetLinguist.addUserGroup
    minimumWidth: WisenetGui.popupSecondWindowDefaultWidth
    minimumHeight: WisenetGui.popupSecondWindowDefaultHeight
    applyButtonText: WisenetLinguist.ok

    property string groupGuid : ""
    property alias addUserGroupAlais : addUserGroupComponentViewId.userGroupView

    Connections{
        target: addUserGroupComponentViewId
        function onResultMessage(success, msg){
            successMsgDialog.isSuccess = success
            successMsgDialog.message = msg
            successMsgDialog.visible = true
        }
    }

    Connections{
        target: addUserGroupComponentViewId.userGroupView
        function onResultMessage(msg, guid){
            if(msg ==="Success"){
               addUserGroupWindow.visible = false
            }
            else{
                successMsgDialog.message = msg
                successMsgDialog.isSuccess = false
                successMsgDialog.visible = true
            }
        }
    }

    WisenetMessageDialog{
        id: successMsgDialog
        visible: false
        width: 400
        height: 200
        property bool isSuccess : false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        onClosing: {
            if(isSuccess)
                addUserGroupWindow.visible = false
        }
    }

    onVisibleChanged: {
        if(visible){
            addUserGroupComponentViewId.groupGuid = groupGuid
            addUserGroupComponentViewId.isGroupView = true
            addUserGroupWindow.title = ((addUserGroupWindow.groupGuid && addUserGroupWindow.groupGuid.length > 0)?WisenetLinguist.editUserGroup:WisenetLinguist.addUserGroup)
        }
        else
        {
            groupGuid = ""
        }

        addUserGroupComponentViewId.visible = visible
    }
    onApplyButtonClicked:
        addUserGroupComponentViewId.acceptAddGroup()

    Rectangle{
        anchors.fill: parent
        color: "transparent"

        AddUserGroupComponentView{
            id : addUserGroupComponentViewId
            anchors.fill: parent
            visible: false
        }
    }
}
