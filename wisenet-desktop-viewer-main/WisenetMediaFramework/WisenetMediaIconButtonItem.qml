import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Button{
    id: button
    implicitWidth: 20 + buttonText.width + 4
    implicitHeight: buttonImage.height
    hoverEnabled: true

    padding: 0
    property url normalSource
    property url pressSource
    property url hoverSource

    property color normalColor: WisenetGui.contrast_04_light_grey
    property color pressColor : WisenetGui.contrast_06_grey
    property color hoverColor : WisenetGui.white

    property alias textWidth:buttonText.implicitWidth
    font.pixelSize: 11

    background: Rectangle {
        x:0
        y:0
        width: button.width
        height: button.height
        color: WisenetGui.transparent
    }

    contentItem: Item {
        Image {
            id: buttonImage
            width: 18
            height: 18
            sourceSize: "18x18"
            source: {
                if(pressed)
                    return button.pressSource
                else if(hovered)
                    return button.hoverSource
                return button.normalSource
            }
        }
        Text {
            id: buttonText
            anchors.leftMargin: 2
            anchors.left: buttonImage.right
            y: 0
            height: 18
            width: implicitWidth+4

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            text: button.text
            font: button.font

            elide: Text.ElideRight
            color: {
                if(pressed)
                    return button.pressColor
                else if(hovered)
                    return button.hoverColor
                return button.normalColor
            }

        }
    }
}
