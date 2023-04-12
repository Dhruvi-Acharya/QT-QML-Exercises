import QtQuick 2.12
import QtQuick.Controls 2.12

TabButton {
    id : control
    text: qsTr("TabButton")
    checkable: true
    font: Font.capitalization

    contentItem: Text {
        text: control.text
        font : control.font
        anchors.fill: parent
        color: (checked? WisenetGui.textBoxSelectColor: (control.hovered? WisenetGui.hoveredTextColor : WisenetGui.tabDefaultTextColor))
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

    }
    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 30
        //border.color: (control.checked? "red" : WisenetGui.transparent)
        color: (control.checked? WisenetGui.transparent : (control.hovered? WisenetGui.buttonHovered : WisenetGui.transparent))
    }

}
