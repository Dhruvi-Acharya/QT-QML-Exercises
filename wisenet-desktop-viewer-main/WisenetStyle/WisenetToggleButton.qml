import QtQuick 2.0
import QtQuick.Controls 2.15

Rectangle{
    id : root
    width: 42
    height: 18

    color: "transparent"
    border.width: 1
    radius: 38
    property bool checked: false
    property color myColor: root.checked?WisenetGui.color_secondary : WisenetGui.contrast_06_grey
    signal clicked()
    signal containsMouseChanged(int row, bool containsMouse)

    border.color: myColor

    TextEdit {
        id: name
        anchors.fill: parent
        text:  root.checked?"On":"Off"
        color: myColor
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 12
        readOnly: true
    }
    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
        onContainsMouseChanged: {
            root.containsMouseChanged(row, containsMouse);
        }
    }
}
