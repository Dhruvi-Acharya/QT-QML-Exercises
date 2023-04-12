import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

WisenetMediaDialog {
    id: window
    width: 450
    height:256
    modal:true
    title: WisenetLinguist.openMediaFiles
    property alias password: textField.text

    function showDialog() {
        textField.selectAll();
        textField.focus = true;
        open()
        forceActiveFocus();
    }

    contentItem: Rectangle {
        anchors.fill: parent
        color: WisenetGui.transparent

        Keys.onPressed: {
            if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                accept();
            }
        }

        Rectangle {
            color: WisenetGui.transparent
            height: 19
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 85
            anchors.right: parent.right
            anchors.rightMargin: 85

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
                focus:true
                color: !enabled ? WisenetGui.contrast_06_grey :
                                  hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                placeholderTextColor: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
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
                anchors.bottom: parent.bottom
                color: WisenetGui.contrast_01_light_grey
            }
        }
    }
}




