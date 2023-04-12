import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0

Button{
    id: button
    implicitWidth: 100
    width: addButtonImage.width +contentItem.implicitWidth + leftPadding + rightPadding
    text : "test"
    hoverEnabled: true
    font.pixelSize: 12

    property string imageSourceDefault : WisenetImage.setup_device_resetName_normal
    property string imageSourcePress : WisenetImage.setup_device_resetName_press
    property string imageSourceHovered : WisenetImage.setup_device_resetName_hover

    background: Rectangle {
        color: WisenetGui.transparent
    }

    Image {
        id: addButtonImage
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 18
        height: 18
        sourceSize: Qt.size(18, 18)
        source: {
            if (!enabled || pressed)
                return button.imageSourcePress;
            else if (hovered)
                return button.imageSourceHovered;
            return button.imageSourceDefault;
        }
    }

    palette.buttonText: {
        if(!enabled || pressed)
            return WisenetGui.contrast_06_grey
        else if (hovered)
            return WisenetGui.contrast_00_white
        else
            return WisenetGui.contrast_04_light_grey
    }


    contentItem: Text {
        text: button.text
        font: button.font
        color: palette.buttonText
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        anchors.leftMargin: 5
        anchors.left: addButtonImage.right
    }

}
