import QtQuick 2.0
import QtQuick.Controls 2.15

Button{
    id: button
    text : ""
    implicitWidth: 100
    width: contentItem.implicitWidth + leftPadding + rightPadding

    hoverEnabled: true
    font.pixelSize: 14
    property bool useBorder: false


    background: Rectangle {
        color: {
            if(!enabled)
                return WisenetGui.transparent
            else if(pressed)
                return WisenetGui.contrast_07_grey
            else if(hovered)
                return WisenetGui.contrast_06_grey
            else
                return WisenetGui.transparent
        }
    }

    palette.buttonText: {
        if(!enabled)
            return WisenetGui.contrast_06_grey
        else
            return WisenetGui.contrast_01_light_grey
    }

    contentItem: Text {
        text: button.text
        font: button.font
        opacity: enabled ? 1.0 : 0.3
        color: WisenetGui.white
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    Rectangle {
        anchors.fill: parent
        color: WisenetGui.transparent
        border.width: 1
        border.color: {
            if(useBorder){
                return WisenetGui.contrast_01_light_grey
            }

            if(!enabled)
                return WisenetGui.contrast_06_grey
            else if(pressed || hovered)
                return WisenetGui.transparent
            else
                return WisenetGui.transparent

        }
    }
}
