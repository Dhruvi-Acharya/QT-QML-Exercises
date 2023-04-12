import QtQuick 2.4
import QtQuick.Controls 2.15

Item {
    width: switchId.width + textId.width+ 40
    height: switchId.height

    property alias text: textId.text
    property alias checked: switchId.checked

    Rectangle {
        id: rectangle
        color: "#505050"
        anchors.fill: parent

        Switch {
            id: switchId
            x: 20
            y: 0
            height: 40
            width: 30
            text: ""
            anchors.right: parent.right
            anchors.rightMargin: 10

            transformOrigin: Item.Center
            font.hintingPreference: Font.PreferNoHinting
            display: AbstractButton.IconOnly
        }

        Text {
            id: textId
            y: 14
            color: "#ffffff"
            text: qsTr("Text")
            anchors.left: parent.left
            font.pixelSize: 12
            anchors.leftMargin: 5
        }
    }
}
