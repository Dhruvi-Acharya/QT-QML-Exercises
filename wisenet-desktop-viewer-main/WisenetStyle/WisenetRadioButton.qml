import QtQuick 2.15
import QtQuick.Controls 2.15


RadioButton {
    id: radioButton
    text: "abc"

    indicator: Rectangle {
        id: buttonRect
        implicitWidth: 16
        implicitHeight: 16
        radius: 16
        color: "transparent"
        border.color: WisenetGui.contrast_03_light_grey
        border.width: 1

        Rectangle {
            width: 8
            height: 8
            anchors.centerIn: parent
            radius: 8
            color: WisenetGui.contrast_03_light_grey
            visible: radioButton.checked
        }
    }

    contentItem: Text {
        text: radioButton.text
        color: WisenetGui.contrast_03_light_grey
        font.pixelSize: 12
        anchors.left: buttonRect.right
        anchors.leftMargin: 8
        anchors.verticalCenter: buttonRect.verticalCenter
        wrapMode: Text.Wrap
    }
}
