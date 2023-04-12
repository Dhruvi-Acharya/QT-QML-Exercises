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
    property string defaultImage
    property string hoveredImage

    implicitHeight: defaultCellHeight
    color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

    Rectangle{
        id: rect
        width: cellImage.width+cellText.width; height:defaultCellHeight
        anchors.verticalCenter: root.verticalCenter
        anchors.left: root.left
        anchors.leftMargin: 25
        color:"transparent"

        Image{
            id: cellImage
            width: 16; height:16
            anchors.verticalCenter: rect.verticalCenter
            sourceSize: Qt.size(16, 16)
            source: pressed ? defaultImage : hoveredImage
        }

        Text {
            id : cellText
            width: 130
            anchors.left: cellImage.right
            anchors.leftMargin: 4
            anchors.verticalCenter: cellImage.verticalCenter

            color: highlightRole ?  WisenetGui.contrast_00_white :hoveredRole ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
            font.pixelSize: 12
            elide: Text.ElideRight
        }
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
