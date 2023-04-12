import QtQuick 2.15
import QtQuick.Controls 2.15

Button{
    width: 120
    height: 28
    hoverEnabled: true

    background: Rectangle {
        color: {
            if(!enabled)
                return WisenetGui.transparent
            else if(pressed)
                return WisenetGui.color_primary_dark
            else if(hovered)
                return WisenetGui.color_primary
            else
                return WisenetGui.transparent
        }
    }

    font.pixelSize: 12
    palette.buttonText: {
        if(!enabled)
            return WisenetGui.contrast_06_grey
        else if(pressed || hovered)
            return WisenetGui.contrast_01_light_grey
        else
            return WisenetGui.color_primary
    }

    Rectangle {
        anchors.fill: parent
        color: WisenetGui.transparent
        border.width: 1
        border.color: {
            if(!enabled)
                return WisenetGui.contrast_06_grey
            else if(pressed || hovered)
                return WisenetGui.transparent
            else
                return WisenetGui.color_primary
        }
    }
}
