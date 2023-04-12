import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

Item{
    id: loginView
    width: 604
    height: 472
    //anchors.fill: parent
    //color: WisenetGui.contrast_11_bg

    property int textBoxWidth : 280

    Component.onCompleted: {
        nameTextBox.setTextFocus();
    }
    onVisibleChanged: {
        if(visible){
            loginViewModel.loadLocalSetting()
            nameTextBox.text = loginViewModel.id
            passwordTextBox.text = loginViewModel.password
            saveId.checked = loginViewModel.savePasswordChecked
            autoLogin.checked = loginViewModel.autologinChecked
        }
        else{
            pulseView.close()
        }
    }

    function showIndicator(){
        pulseView.open()
    }

    Popup {
        id: pulseView

        width: 100; height: 100
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose

        background: Rectangle{
            color: WisenetGui.transparent
        }

        contentItem: WisenetMediaBusyIndicator {
            id: processingIndicator
            width: 100; height: 100
            sourceWidth: 100; sourceHeight: 100
            anchors.centerIn: parent
            visible: true
            running: true
        }
    }

    Rectangle{
        anchors.fill:parent

        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1


        Image{
            id: title

            source: logoManager.useCustomLogo? "file:///" + logoManager.customLogoPath : WisenetImage.wisenet_viewer_logo
            sourceSize: "185x19"
            height: 25
            fillMode: Image.PreserveAspectFit
            anchors.top: parent.top
            anchors.topMargin: 71
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text{
            id: version

            anchors.top: title.bottom
            anchors.topMargin: 13
            anchors.horizontalCenter: parent.horizontalCenter

            color: WisenetGui.contrast_06_grey
            font.pixelSize: 11
            text: Qt.application.version

            horizontalAlignment: Text.AlignHCenter
        }

        WisenetTextBox{
            id: nameTextBox

            width: 280
            anchors.top: version.bottom
            anchors.topMargin: 55
            anchors.horizontalCenter: parent.horizontalCenter

            placeholderText: "ID"
            text: loginViewModel.id
        }

        WisenetPasswordBox{
            id: passwordTextBox

            width: 280
            anchors.top: nameTextBox.bottom
            anchors.topMargin: 27
            anchors.horizontalCenter: parent.horizontalCenter

            showPasswordButtonVisible : false

            placeholderText: WisenetLinguist.password
        }

        WisenetCheckBox{
            id: saveId

            anchors.top: passwordTextBox.bottom
            anchors.topMargin: 27
            anchors.left: passwordTextBox.left

            text: WisenetLinguist.saveId
            checked: loginViewModel.savePasswordChecked
        }

        WisenetCheckBox{
            id: autoLogin

            anchors.top: saveId.bottom
            anchors.topMargin: 14
            anchors.left: passwordTextBox.left

            text: WisenetLinguist.autoLogin
            checked: loginViewModel.autologinChecked
        }

        WisenetButton2{
            id: loginButton

            anchors.top: autoLogin.bottom
            anchors.topMargin: 61
            anchors.horizontalCenter: parent.horizontalCenter

            width: 282
            height: 36

            text: WisenetLinguist.login

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("LoginView loginButton onClicked")
                    loginViewModel.loginExecute(nameTextBox.text, passwordTextBox.text, saveId.checked, autoLogin.checked);
                    pulseView.open()
                }
            }
        }
    }

    /* Login창 닫힐때 mac에서 깜빡이는 문제로 dialog 컨트롤 변경함.
    WisenetMessageDialog{
        id  : errorDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }
    */
    WisenetMediaDialog {
        id: errorDialog
        width: 440
        height: 180
        onlyCloseType: true
        property alias message : messageText.text
        contentItem: Item {
            Text {
                id: messageText
                anchors.fill: parent
                anchors.leftMargin: 36
                anchors.rightMargin: 36
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: lineCount == 1 ? Text.AlignHCenter : Text.AlignLeft
                font.pixelSize: 12
                color: WisenetGui.contrast_00_white
                wrapMode: Text.Wrap
            }
        }
    }

    Connections{
        target: loginViewModel
        function onOpenErrorMessageDialog(errorCode, lockedSeconds){
            if(errorCode === LoginErrorCode.InvalidIdOrPassword)
                errorDialog.message = WisenetLinguist.invalidIdorPassword
            else if(errorCode === LoginErrorCode.AccountBlocked)
                errorDialog.message = WisenetLinguist.loginLocked.arg(lockedSeconds)
            else if(errorCode === LoginErrorCode.LdapConnectionFailed)
                errorDialog.message = WisenetLinguist.ldapConnFailed
            errorDialog.open()
        }

        function onResetPassword(){
            passwordTextBox.text = ""

        }

        function onLoginFinished(){
            pulseView.close()
        }
    }
}
