import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.0

Button{
    id : root
    width: 120
    height: 28
    hoverEnabled: true

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


    font.pixelSize: 12
    palette.buttonText: {
        if(!enabled)
            return WisenetGui.contrast_06_grey
        else
            return WisenetGui.contrast_01_light_grey
    }

    contentItem: Label {
        text: root.text
        font: root.font
        color: palette.buttonText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        wrapMode: Label.Wrap
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
                return WisenetGui.contrast_01_light_grey
        }
    }
}
