import QtQuick 2.15
import WisenetStyle 1.0

Rectangle {
    id: root
    width: 25; height: 25
    color: WisenetGui.contrast_10_dark

    property alias textLabel: label
    property alias text: label.text
    property color fontColor: WisenetGui.contrast_05_grey

    property alias topLineWidth: topLine.height
    property alias bottomLineWidth: bottomLine.height
    property alias leftLineWidth: leftLine.width
    property alias rightLineWidth: rightLine.width

    property string borderColor: WisenetGui.contrast_07_grey

    Text {
        id: label
        font.pixelSize: 12
        wrapMode: Text.Wrap
        color: root.fontColor
    }

    Rectangle {
        id: topLine
        width: root.width; height:0.5
        color: root.borderColor
        anchors.top: root.top
        visible: true
    }

    Rectangle {
        id: bottomLine
        width: root.width; height:0.5
        color: root.borderColor
        anchors.bottom: root.bottom
        visible: true

    }

    Rectangle {
        id: leftLine
        width: 0.5; height: root.height
        color: root.borderColor
        anchors.left: root.left
        visible: true
    }

    Rectangle {
        id: rightLine
        width: 0.5; height: root.height
        color: root.borderColor
        anchors.right: root.right
        visible: true
    }
}
