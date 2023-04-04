import QtQuick 2.12

Rectangle {
    color: "#b0dfb0"
    implicitHeight: txt.implicitHeight + 4

    property alias text: txt.text

    signal clicked()

    Text {
        id: txt
        anchors.centerIn: parent
        font.pixelSize: 16
        font.bold: true
    }
    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked()
    }
}
