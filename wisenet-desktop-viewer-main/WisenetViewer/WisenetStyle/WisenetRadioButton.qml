import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0

RadioButton {
    id: control
    text: qsTr("RadioButton")
    checked: true
    font.pixelSize: 12
    leftPadding: 0

    indicator: Rectangle {
        implicitWidth: 16
        implicitHeight: 16
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 8
        border.color: hovered? WisenetGui.contrast_01_light_grey:WisenetGui.contrast_03_light_grey
        color: WisenetGui.transparent
        Rectangle {
            width: 8
            height: 8
            x: 4
            y: 4
            radius: 4
            color: hovered? WisenetGui.contrast_01_light_grey:WisenetGui.contrast_03_light_grey
            visible: control.checked
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: hovered? WisenetGui.contrast_01_light_grey: WisenetGui.contrast_03_light_grey
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.Wrap
        leftPadding: control.indicator.width + control.spacing
    }
}
