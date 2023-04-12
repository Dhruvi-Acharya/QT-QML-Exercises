import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Button{
    id: textButtonItem
    width: implicitWidth
    height: implicitHeight
    hoverEnabled: true
    padding: 0
    property real textImplicitWidth: contentItem.implicitWidth

    property color normalColor: WisenetGui.color_live_control_bg
    property color pressColor : WisenetGui.contrast_08_dark_grey
    property color hoverColor : WisenetGui.contrast_08_dark_grey

    property color textNormalColor:WisenetGui.contrast_02_light_grey
    property color textPressColor:WisenetGui.color_primary
    property color textHoverColor:WisenetGui.contrast_00_white

    property real bgOpacity : 0.3
    property real normalOpacity: bgOpacity
    property real pressOpacity: bgOpacity
    property real hoverOpacity: bgOpacity

    property alias minimumPixelSize: textContentItem.minimumPixelSize

    property alias radius : bgRect.radius
    background: Rectangle {
        id: bgRect
        opacity: {
            if(pressed) {
                return textButtonItem.pressOpacity
            }
            if(hovered) {
                return textButtonItem.hoverOpacity
            }
            return textButtonItem.normalOpacity
        }

        color: {
            if(pressed) {
                return textButtonItem.pressColor
            }
            if(hovered) {
                return textButtonItem.hoverColor
            }
            return textButtonItem.normalColor
        }
    }

    contentItem: Text {
        id: textContentItem
        text: textButtonItem.text
        wrapMode: Text.WrapAnywhere
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: textButtonItem.palette.buttonText
        //fontSizeMode: Text.VerticalFit
        minimumPixelSize: 4
        height: textButtonItem.height
        font.pixelSize: 12
    }

    palette.buttonText: {
        if(pressed)
            return textButtonItem.textPressColor
        if(hovered)
            return textButtonItem.textHoverColor
        return textButtonItem.textNormalColor
    }
}
