import QtQuick 2.15
import QtQuick.Controls 2.15

Slider {
    id: slider
    hoverEnabled: true
    width: 140
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: nextChunkButton.right
    //anchors.leftMargin: controlMargin
    stepSize: 1
    property bool valid: true

    // Custom background
    background: Rectangle {
        x: slider.leftPadding
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 1
        width: slider.availableWidth
        height: implicitHeight
        color: enabled&&valid ? WisenetGui.contrast_07_grey : WisenetGui.contrast_08_dark_grey

        Rectangle {
            width: slider.visualPosition * parent.width
            height: parent.height
            color: enabled&&valid  ? WisenetGui.color_primary : WisenetGui.contrast_08_dark_grey
        }
    }

    // Custom handle
    handle: Rectangle {
        x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 12
        implicitHeight: 12
        radius: 6
        color: enabled&&valid ? WisenetGui.color_primary : WisenetGui.contrast_07_grey
    }
}
