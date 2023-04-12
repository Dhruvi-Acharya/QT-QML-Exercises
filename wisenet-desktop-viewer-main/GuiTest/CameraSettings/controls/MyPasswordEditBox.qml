import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 1.4

Rectangle{
    property alias text: txtInput.text
    Layout.fillWidth: true
    Layout.preferredWidth: 200
    Layout.minimumWidth: 200
    Layout.preferredHeight: 30
    Layout.minimumHeight: 30

    color: "black"
    TextInput {
        id : txtInput
        anchors.fill: parent
        color: "white"
        leftPadding: 5
        rightPadding: 5
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        echoMode: TextInput.Password
        selectByMouse: true
        selectionColor: "orange"
        clip: true
        renderType: Text.NativeRendering
        maximumLength: 20
        cursorVisible: false

        Image {
            id: image
            source: "images/passwordEye.png"
            width : 30
            visible: txtInput.text.length > 0
            height: parent.height
            anchors.right: parent.right
            opacity: 0.5

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true

                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: {
                    txtInput.echoMode = TextInput.Normal
                    image.opacity = 1.0
                }

                onReleased:{
                    txtInput.echoMode = TextInput.Password
                    image.opacity = 0.5
                }
            }
        }
    }

}
