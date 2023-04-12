import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

CheckBox {
    id: control
    height: 16
    font.pixelSize: 12

    indicator: Image {
        height: control.height
        width: control.height
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
        source: {
            if(control.checkState == Qt.Checked) {
                return (control.hovered || control.pressed) ? "images/check_singleOn_pressed.svg" : "images/check_singleOn.svg";
            }
            else {
                return (control.hovered || control.pressed) ? "images/check_singleOff_pressed.svg" : "images/check_singleOff.svg";
            }
        }
        sourceSize: Qt.size(16, 16)
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: WisenetGui.contrast_01_light_grey
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
