import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Dialogs 1.3

import Wisenet.Qmls 0.1
import "../customControls"
import "."

//state로 값들 visibility 변경해주기
Item {
    id : loginItem
    focus: true

    function loginResponse(data){
        if(data === true){
            navigate("MonitoringView")
        }
        else{
            failMD.visible =true
        }
    }

    function getServerStatusResponse(result){
        console.log("getServerStatusResponse" + result)
        if(result){
            setAdminPasswordContainer.visible = false
            loginContainer.visible = true
        }
        else{
            loginContainer.visible = false
            setAdminPasswordContainer.visible = true
        }
    }

    Connections{
        target: loginModel
        function onDoLoginResponse(result){
            loginResponse(result)
        }
        function onDoGetServerStatusResponse(result){
            getServerStatusResponse(result)
        }
        function onDoSetPassword(password, isSavePassword){
            loginPassword.text = password;
            savePassword.checked = isSavePassword;
        }
        function onDoLoadingVisible(){
            loginContainer.visible = false
            loadingWindow.visible = true
        }
    }

    MessageDialog {
        id: failMD
        title: "Wisenet Viewer?"
        icon: StandardIcon.Warning
        text: "Fail. Incorrext password"
    }

    Keys.onPressed:{
        if(event.key === Qt.Key_Down)
        {
            loginButton.clicked()
        }
    }

    //로그인
    Rectangle{
        anchors.fill: parent
        color: "#101010"
        id : loginContainer
        visible: true

        ColumnLayout{
            id: columnLayout
            anchors.centerIn: parent
            anchors.fill: parent
            spacing: 2

            //로고 이미지
            Image {
                id: wisenetLogo
                Layout.alignment: Qt.AlignCenter
                source: 'icon/wisenetlogo.PNG'
                Layout.bottomMargin: 10
                Layout.topMargin: 10
            }

            //로그인 영역
            GroupBox{
                id: control
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillHeight: true
                Layout.fillWidth: false

                background: Rectangle {
                    y: control.topPadding - control.padding
                    width: parent.width
                    height: parent.height - control.topPadding + control.padding
                    color: "#383838"
                    border.color: "#383838"
                    radius: 2
                }

                ColumnLayout{
                    id: loginArea
                    anchors.fill: parent
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

                    Text {
                        text: "name"
                        color: "white"
                        Layout.fillWidth: true
                    }

                    TextBox{
                        id : loginId
                        implicitWidth: 400
                        placeholderText: qsTr("Enter name")
                        text:"admin"
                        Layout.bottomMargin: 20
                    }

                    Text {
                        text: "password"
                        color: "white"
                        Layout.fillWidth: true
                    }

                    TextBox{
                        id : loginPassword
                        implicitWidth: 400
                        echomode: TextInput.Password
                        placeholderText: qsTr("Enter pw")
                        Layout.bottomMargin: 20
                    }

                    Rectangle{
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: "transparent"
                    }

                    CheckBox{
                        id: savePassword
                        text: "Save password"
                        onCheckedChanged: {
                            if(checked == false)
                                autoLogin.checked = false
                        }
                    }

                    CheckBox{
                        id: autoLogin
                        text: "Auto-login"
                        enabled: savePassword.checked
                    }

                    Button{
                        id:loginButton
                        implicitWidth: 400
                        highlighted: true
                        text: "Login"
                        onClicked: {
                            loginModel.login(loginId.text, loginPassword.text, savePassword.checked, autoLogin.checked);
                        }
                    }
                }
            }

            Rectangle{
                height: 200
                width: parent.width
                color: "transparent"
            }
        }
    }

    SetupAdminPasswordView{
        id : setAdminPasswordContainer
        visible: false
        onSetPassword: {loginModel.setAdminPassword(password, isSavePassword);}
    }

    LoadingView{
        id: loadingWindow
        visible: false
    }
}
