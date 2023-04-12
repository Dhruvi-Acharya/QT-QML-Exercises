import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Setup 1.0

WisenetPopupWindowBase{
    id: addUserWindow
    //title: WisenetLinguist.addEditUser
    minimumWidth: WisenetGui.popupSecondWindowDefaultWidth
    minimumHeight: WisenetGui.popupSecondWindowDefaultHeight
    applyButtonText: WisenetLinguist.ok
    property string userGuid : ""
    property bool usingAddUser: false
    property alias addUserViewModelAlais : addUserViewModel

    AddUserViewModel{
        id: addUserViewModel
    }

    Connections{
        target: addUserViewModel
        function onResultMessage(msg){
            if(msg ==="Success"){
                addUserWindow.visible = false
            }
            else{
                successMsgDialog.message = msg
                successMsgDialog.isSuccess = false
                successMsgDialog.visible = true
            }
        }
    }

    Connections{
        target: addUserGroupComponentViewId.userGroupView
        function onResultMessage(success, guid){
            if(success && usingAddUser){
                addUserGroupComponentViewId.groupGuid = guid
                usingAddUser = false
                addUser()
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
        onClosing:  {
            if(isSuccess)
                addUserWindow.visible = false
        }
    }

    onVisibleChanged:{
        if(visible){
            addUserViewModel.readUser(userGuid);
            addUserGroupComponentViewId.isOwner = addUserViewModel.isOwner
            addUserGroupComponentViewId.groupGuid = addUserViewModel.groupId
            addUserGroupComponentViewId.isGroupView = false

            // cannot change loginId, password and email if the user is from LDAP
            console.log("[AddUserView] addUserViewModel isLdapUser",addUserViewModel.isLdapUser )
            addUserComponentViewId.isLdap = addUserViewModel.isLdapUser
            addUserWindow.title = ((userGuid && userGuid.length >0 )? addUserComponentViewId.isLdap? WisenetLinguist.editUser + " (LDAP)": WisenetLinguist.editUser : WisenetLinguist.addUser)
        }
        else{

        }

        addUserComponentViewId.visible = addUserWindow.visible
        addUserGroupComponentViewId.visible = addUserWindow.visible
    }

    function addUser(){
        addUserViewModel.loginId = addUserComponentViewId.userId
        addUserViewModel.password = addUserComponentViewId.password
        addUserViewModel.name = addUserComponentViewId.userName
        addUserViewModel.description = addUserComponentViewId.description
        addUserViewModel.groupId = addUserGroupComponentViewId.groupGuid
        addUserViewModel.email = addUserComponentViewId.email
        addUserViewModel.isLdapUser = addUserComponentViewId.isLdap
        addUserViewModel.saveUser();
    }

    function hasOnlySpace(userField){
        var str = userField;
        str = str.replace(/\s/g, '')
        if(str.length === 0)
            return true
        else
            return false
    }

    onApplyButtonClicked:
    {
        if(hasOnlySpace(addUserComponentViewId.userId)){
            successMsgDialog.message = WisenetLinguist.pleaseInputUserName
            successMsgDialog.isSuccess = false
            successMsgDialog.visible= true
        }
        else if(duplicateId.visible){
            successMsgDialog.message = WisenetLinguist.duplicateIdIsNotAllowed
            successMsgDialog.isSuccess = false
            successMsgDialog.visible= true
        }
        else if((addUserComponentViewId.password.length === 0 ||
                (addUserComponentViewId.password !== addUserComponentViewId.passwordConfirm) ||
                (passwordStrengthChecker.test(addUserComponentViewId.password) !== 0)
                ) && !addUserWindow.ldapUser){
            successMsgDialog.message = WisenetLinguist.pleaseCheckPassword
            successMsgDialog.isSuccess = false
            successMsgDialog.visible= true
        }
        else if(!addUserGroupComponentViewId.isGroupSelect()){
            successMsgDialog.message = WisenetLinguist.pleaseSelectUserGroup
            successMsgDialog.isSuccess = false
            successMsgDialog.visible= true
        }
        else if(addUserGroupComponentViewId.groupGuid ===""){
            usingAddUser = true
            addUserGroupComponentViewId.acceptAddGroup()
        }
        else if ((addUserComponentViewId.email.length == 0 ||
                 addUserComponentViewId.email == "" ||
                 !addUserComponentViewId.email.search("@")) && !addUserWindow.ldapUser)
        {
            successMsgDialog.message = WisenetLinguist.pleaseCheckEmail
            successMsgDialog.isSuccess = false
            successMsgDialog.visible = true
        }

        else{
            addUser()
            addUserGroupComponentViewId.acceptAddGroup()
        }

    }
    Rectangle{
        anchors.fill: parent
        color: "transparent"
        Column{
            id: mainLayout
            property int layoutMargin: 10
            anchors.fill: parent
            anchors.margins: layoutMargin
            //spacing: 10
            AddUserComponentView{
                id : addUserComponentViewId
                visible: false
            }

            AddUserGroupComponentView{
                id : addUserGroupComponentViewId
                visible: false
                isGroupView: false
            }
        }
    }
    Rectangle{
        id : duplicateId
        visible: false
        x : 160
        y : 55
        width: 140
        height: 15
        color: "transparent"
        Text{
            anchors.fill: parent
            text: WisenetLinguist.duplicateIdIsNotAllowed
            color: WisenetGui.color_accent
            font.pixelSize:  11
        }
    }
}
