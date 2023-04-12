import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0

Button{
    id: button
    text : WisenetLinguist.add
    implicitWidth: 100
    width: 100//contentItem.implicitWidth + leftPadding + rightPadding

    hoverEnabled: true
    font.pixelSize: 12

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
                return WisenetImage.setup_add_press;
            else if (hovered)
                return WisenetImage.setup_add_hover;
            return WisenetImage.setup_add_default;
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
