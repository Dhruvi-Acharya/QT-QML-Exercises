import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 1.4

Rectangle{
    property alias text: input.text
    property alias textInput: input

    Layout.fillWidth: true
    Layout.preferredWidth: 120
    Layout.minimumWidth: 70
    Layout.preferredHeight: 30
    Layout.minimumHeight: 30

    color: WisenetGui.black
    TextInput {
        id : input
        anchors.fill: parent
        color: WisenetGui.white
        leftPadding: 5
        rightPadding: 5
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        echoMode: TextInput.Password
        selectByMouse: true
        selectionColor: WisenetGui.orange
        clip: true
        renderType: Text.NativeRendering
        maximumLength: 64
        cursorVisible: false

        Image {
            id: image
            source: "Icon/passwordEye.png"
            width : 30
            visible: input.text.length > 0
            height: parent.height
            anchors.right: parent.right
            opacity: 0.5

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true

                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: {
                    input.echoMode = TextInput.Normal
                    image.opacity = 1.0
                }

                onReleased:{
                    input.echoMode = TextInput.Password
                    image.opacity = 0.5
                }
            }
        }
    }
}
