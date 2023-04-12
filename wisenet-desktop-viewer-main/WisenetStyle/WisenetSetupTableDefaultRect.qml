import QtQuick 2.0
import Wisenet.Define 1.0
import Wisenet.Setup 1.0

Rectangle {
    id: root

    signal pressed(int row,  int modifier)
    signal containsMouseChanged(int row, bool containsMouse)
    signal doubleClicked(int row)
    property int defaultCellHeight: 36
    property alias text : cellText.text
    property alias textColor : cellText.color

    implicitHeight: defaultCellHeight
    color: highlightRole ?  WisenetGui.color_primary_press : hoveredRole ? WisenetGui.color_primary : "transparent"

    WisenetTableCellText{
        id : cellText
        color: highlightRole ?  WisenetGui.contrast_00_white :hoveredRole ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
    }
    WisenetTableCellLine{
        anchors.bottom: parent.bottom
    }
    MouseArea {
        anchors.fill : parent
        hoverEnabled: true
        onContainsMouseChanged: {
            root.containsMouseChanged(row, containsMouse);
        }
        onPressed: {
            root.pressed(row, mouse.modifiers)
        }
        onDoubleClicked: {
            root.doubleClicked(row)
        }
    }
}
