import QtQuick 2.15
import "qrc:/WisenetStyle/"

Item {
    id: toggleButtonItem

    property real imgWidth : 20
    property real imgHeight : 20
    property size imgSourceSize : "20x20"

    property url normalSource
    property url normalOnSource
    property url pressSource
    property url pressOnSource
    property url hoverSource
    property url hoverOnSource

    property color normalColor: "transparent"
    property color normalOnColor: WisenetGui.contrast_08_dark_grey
    property color pressColor : WisenetGui.contrast_08_dark_grey
    property color hoverColor : WisenetGui.contrast_08_dark_grey

    property real bgOpacity : 0.3
    property real normalOpacity: bgOpacity
    property real normalOnOpacity: bgOpacity
    property real pressOpacity: bgOpacity
    property real hoverOpacity: bgOpacity

    property bool toggleOn: false
    property bool hover: false
    property alias radius : buttonBg.radius

    signal mouseClicked(bool isOn)

    Rectangle {
        id: buttonBg
        anchors.fill: parent
        color: toggleButtonItem.normalColor
        opacity: toggleButtonItem.normalOpacity
    }

    Image {
        id: toggleButtonImage
        source: toggleButtonItem.normalSource
        anchors.centerIn:parent
        width: toggleButtonItem.imgWidth
        height: toggleButtonItem.imgHeight
        sourceSize: toggleButtonItem.imgSourceSize
    }

    MouseArea{
        id: buttonMouseArea
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        anchors.fill:toggleButtonItem

        onEntered: {
            hover = true;
            toggleButtonItem.updateColor();
        }
        onExited: {
            hover = false;
            toggleButtonItem.updateColor();
        }
        onPressed: {
            toggleButtonImage.source = toggleOn ? parent.pressOnSource : parent.pressSource
            buttonBg.opacity = parent.pressOpacity
            buttonBg.color = parent.pressColor
        }
        onClicked: {
            var isOn = !toggleButtonItem.toggleOn
            toggleButtonItem.mouseClicked(isOn);
        }
        onReleased: {
            hover = containsMouse;
            toggleButtonItem.updateColor();
        }
    }

    function updateColor()
    {
        if (hover) {
            toggleButtonImage.source = toggleOn ? hoverOnSource : hoverSource
            buttonBg.opacity = hoverOpacity
            buttonBg.color = hoverColor
        }
        else {
            toggleButtonImage.source = toggleOn ? normalOnSource : normalSource
            buttonBg.opacity = toggleOn ? normalOnOpacity : normalOpacity
            buttonBg.color = toggleOn ? normalOnColor : normalColor
        }
    }

    onToggleOnChanged: {
        updateColor()
    }

}
