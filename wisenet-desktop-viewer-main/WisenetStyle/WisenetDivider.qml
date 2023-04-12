import QtQuick 2.15
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0

Column {
    id : col
    spacing: 5
    width: parent.width
    padding : 0
    property alias label: textLabel.text
    property alias font: textLabel.font
    property alias lineVisible : line.visible

    WisenetLabel {
        id: textLabel
        width: parent.width - col.padding
        horizontalAlignment: Text.AlignLeft
        color: WisenetGui.contrast_00_white
        font.pixelSize: 24

    }

    Rectangle {
        id : line
        border.width: 1
        height: 2
        width: parent.width - col.padding
        //anchors.margins: 20
        border.color: WisenetGui.setupPageLine
        visible: false
    }
}
