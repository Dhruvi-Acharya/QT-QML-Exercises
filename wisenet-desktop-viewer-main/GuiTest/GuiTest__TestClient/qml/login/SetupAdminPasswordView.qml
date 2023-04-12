import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import "../customControls"

Rectangle{
    id: setAdminPasswordRect
    anchors.fill: parent
    color: "transparent"
    visible: false

    signal setPassword(string password, bool isSavePassword);

    function setOkButtonEnable(){
        if(password.isEnabled && (password.text == confirmPassword.text))
            okButton.enabled = true;
        else
            okButton.enabled = false;
    }

    ColumnLayout{
        id: columnLayout
        anchors.fill: parent
        anchors.bottomMargin: 50
        anchors.topMargin: 50
        anchors.rightMargin: 100
        anchors.leftMargin: 100
        spacing: 10

        Text{
            id: label
            text: "Set up administrator password"
            color: "white"
            font.pixelSize: 30
            Layout.bottomMargin: 50
            Layout.alignment: Qt.AlignHCenter
        }

        TextBox{
            text: "admin"
            readOnly: true
            implicitWidth: label.implicitWidth
            Layout.alignment: Qt.AlignHCenter
        }

        PasswordBox{
            id: password
            focus: true
            implicitWidth: label.implicitWidth
            Layout.alignment: Qt.AlignHCenter
            onIsEnabledChanged: {setOkButtonEnable()}
        }

        TextBox{
            id: confirmPassword
            implicitWidth: label.implicitWidth
            Layout.alignment: Qt.AlignHCenter
            onTextChanged: {setOkButtonEnable()}
            echomode: TextInput.Password
        }

        CheckBox{
            id: savePasswordAdmin
            text: "Save Password"
            implicitWidth: label.implicitWidth
            Layout.alignment: Qt.AlignHCenter
        }

        Button{
            id: okButton
            text: "ok"
            implicitWidth: label.implicitWidth
            Layout.alignment: Qt.AlignHCenter
            enabled: false
            onClicked:{ setAdminPasswordRect.setPassword(password.text, savePasswordAdmin.checked)}
        }

        Rectangle{
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
