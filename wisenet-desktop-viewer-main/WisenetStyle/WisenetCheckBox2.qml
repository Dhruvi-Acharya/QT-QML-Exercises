import QtQuick 2.15
import QtQuick.Controls 2.15

CheckBox {
    id: control
    height: 16
    font.pixelSize: 12

    property alias showImage: image.visible
    property alias imageSource: image.source
    property alias textLeftPadding: txt.leftPadding
    property alias textWrapMode :txt.wrapMode

    indicator: Image {
        height: control.height
        width: control.height
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
        source: {
            if(control.checkState == Qt.Checked) {
                return !control.enabled ? WisenetImage.check_on_dim :
                        control.pressed ? WisenetImage.check_on_press : WisenetImage.check_on
            }
            else {
                return !control.enabled ? WisenetImage.check_off_dim :
                        control.pressed ? WisenetImage.check_off_press : WisenetImage.check_off
            }
        }
        sourceSize: Qt.size(16, 16)
    }

    contentItem: Rectangle {
        anchors.fill: parent
        anchors.leftMargin: control.height * 2
        color: "transparent"
        Image {
            id: image
            visible: false
            width: control.height
            height: control.height
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            sourceSize: Qt.size(16, 16)
        }
        Text {
            id: txt
            anchors.left: showImage ? image.right : parent.left
            anchors.leftMargin: showImage ? 12 : 0
            anchors.verticalCenter: parent.verticalCenter
            width: control.width - image.width
            text: control.text
            font: control.font
            leftPadding: -6
            verticalAlignment: Text.AlignVCenter
            color: !control.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_01_light_grey
        }
    }
}
