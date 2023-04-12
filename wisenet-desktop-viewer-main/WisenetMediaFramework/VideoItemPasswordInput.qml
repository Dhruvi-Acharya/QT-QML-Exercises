import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Item {
    id: passwordInput

    property alias password: textField.text

    signal passwordEntered()

    width: parent.width * 0.56
    height: 90

    Keys.onPressed: {
        if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
            passwordEntered()
    }

    Rectangle {
        anchors.fill: parent
        color: WisenetGui.transparent


        TextField{
            id: textField
            height: 16
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.right: parent.right
            anchors.rightMargin: 6
            leftInset: 0
            rightInset: 0
            topInset: 0
            bottomInset: 0
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
            hoverEnabled: true
            selectByMouse: true
            font.pixelSize: 14
            color: hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
            placeholderTextColor: WisenetGui.contrast_06_grey
            selectionColor: WisenetGui.color_primary_deep_dark
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignLeft
            echoMode: TextInput.Password
            passwordCharacter: "*"
            clip: true
            renderType: Text.NativeRendering
            maximumLength: 64
            cursorVisible: false
            placeholderText: WisenetLinguist.password
            background: Rectangle {
                color: WisenetGui.transparent
            }
            validator: RegExpValidator{
                regExp: /[0-9a-zA-Z!@#$%^&*()-_=+\{\}\[\]\\|:;\'\"<>,\.\/?~]+/
            }
        }

        Rectangle{
            width: parent.width
            height: 1
            anchors.top: textField.bottom
            anchors.topMargin: 2
            color: WisenetGui.contrast_01_light_grey
        }

        WisenetMediaTextButtonItem {
            isOrangeType: true
            text: WisenetLinguist.ok
            width: 120
            height: 28
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                passwordEntered()
            }
        }
    }
}
