import QtQuick 2.15
import "qrc:/WisenetStyle/"

Item {
    id: buttonItem

    property url normalSource
    property url pressSource
    property url hoverSource

    property real imgWidth : 20
    property real imgHeight : 20
    property size imgSourceSize : "20x20"

    property color normalColor: "transparent"
    property color pressColor : WisenetGui.contrast_08_dark_grey
    property color hoverColor : WisenetGui.contrast_08_dark_grey

    property real bgOpacity : 0.3
    property real normalOpacity: bgOpacity
    property real pressOpacity: bgOpacity
    property real hoverOpacity: bgOpacity

    property alias radius : buttonBg.radius

    signal mouseClicked();
    signal mousePressed();
    signal mouseReleased();

    property bool hover: false;

    Rectangle {
        id: buttonBg
        anchors.fill: parent
        color: buttonItem.normalColor
        opacity: buttonItem.normalOpacity
    }

    Image {
        id: buttonImage
        source: {
            if (buttonMouseArea.pressed && buttonItem.pressSource)
                return buttonItem.pressSource;
            if (buttonItem.hover && buttonItem.hoverSource)
                return buttonItem.hoverSource;
            return buttonItem.normalSource;
        }

        anchors.centerIn:parent
        width: buttonItem.imgWidth
        height: buttonItem.imgHeight
        sourceSize: buttonItem.imgSourceSize
    }

    MouseArea{
        id: buttonMouseArea
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        anchors.fill:parent

        onEntered: {
            hover = true;
            buttonBg.color = buttonItem.hoverColor;
            buttonBg.opacity = buttonItem.hoverOpacity;
        }
        onExited: {
            hover = false;
            buttonBg.color = buttonItem.normalColor;
            buttonBg.opacity = buttonItem.normalOpacity;
        }
        onPressed: {
            hover = false;
            buttonBg.color = buttonItem.pressColor;
            buttonBg.opacity = buttonItem.pressOpacity;
            buttonItem.mousePressed();
        }
        onClicked: {
            buttonItem.mouseClicked();
        }
        onReleased: {
            hover = containsMouse;
            buttonBg.color = containsMouse ? buttonItem.hoverColor : buttonItem.normalColor
            buttonBg.opacity = containsMouse ? buttonItem.hoverOpacity : buttonItem.normalOpacity
            buttonItem.mouseReleased();
        }
    }
}
