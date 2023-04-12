import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Button{
    id: textButtonItem
    width: 120
    height: 28
    hoverEnabled: true

    property real radius: 0
    property real textImplicitWidth: contentItem.implicitWidth
    property bool isOrangeType : false
    //property bool autoResize : false

    background: Rectangle {
        radius : textButtonItem.radius
        color: {
            if(!enabled)
                return WisenetGui.transparent
            if(pressed) {
                if (textButtonItem.isOrangeType)
                    return WisenetGui.color_primary_dark
                else
                    return WisenetGui.contrast_07_grey
            }
            if(hovered) {
                if (textButtonItem.isOrangeType)
                    return WisenetGui.color_primary
                else
                    return WisenetGui.contrast_06_grey
            }

            return WisenetGui.transparent
        }
    }

    /* TextButton의 autoResize 사용 코드가 없고, 실시간 언어 변경 시 정렬 오류로 주석 처리
    contentItem: Text {
        id: contentText
        text: textButtonItem.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        color: textButtonItem.palette.buttonText
        fontSizeMode: autoResize ? Text.VerticalFit : Text.FixedSize
        minimumPixelSize: 4
        height: textButtonItem.height
        font.pixelSize: 12
    }
    */
    font.pixelSize: 12

    palette.buttonText: {
        if(!enabled)
            return WisenetGui.contrast_06_grey

        if(pressed || hovered)
            return WisenetGui.contrast_01_light_grey

        if (textButtonItem.isOrangeType) {
            return WisenetGui.color_primary
        }

        return WisenetGui.contrast_01_light_grey
    }

    Rectangle {
        radius : textButtonItem.radius
        anchors.fill: parent
        color: WisenetGui.transparent
        border.width: 1
        border.color: {
            if(!enabled)
                return WisenetGui.contrast_06_grey
            if(pressed || hovered)
                return WisenetGui.transparent
            if (textButtonItem.isOrangeType)
                return WisenetGui.color_primary
            return WisenetGui.contrast_01_light_grey
        }
    }
}
