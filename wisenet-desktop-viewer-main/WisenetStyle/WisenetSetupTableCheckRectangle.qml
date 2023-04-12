import QtQuick 2.0
import Wisenet.Define 1.0
import Wisenet.Setup 1.0

Rectangle {
    id : root

    signal clicked(int row,  int modifier)
    signal containsMouseChanged(int row, bool containsMouse)
    property bool checkedValue : false
    property int defaultCellHeight: 36

    implicitHeight: defaultCellHeight
    color: highlightRole ?  WisenetGui.color_primary_press : hoveredRole ? WisenetGui.color_primary : "transparent"

    WisenetCheckBox2{
        id: cellCheck
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        checkState: root.checkedValue ? Qt.Checked : Qt.Unchecked
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
        onClicked: {
            root.checkedValue != root.checkedValue
            root.clicked(row, mouse.modifiers)
        }
    }
}
