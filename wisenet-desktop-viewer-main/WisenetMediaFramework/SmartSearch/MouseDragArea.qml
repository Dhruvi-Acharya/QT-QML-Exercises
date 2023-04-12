import QtQuick 2.15

MouseArea {
    anchors.fill: parent
    anchors.margins: -2
    propagateComposedEvents: true
    acceptedButtons: Qt.LeftButton

    property real pressedX
    property real pressedY
    property bool dragging: false

    signal dragPositionChanged(var dx, var dy)

    onClicked: {
        mouse.accepted = dragging
    }

    onPressed: {
        if(pressed) {
            pressedX = mouseX
            pressedY = mouseY
            dragging = false
        }
        else {
            dragging = false
        }
    }

    onPositionChanged: {
        if(!pressed)
            return

        var dx = mouseX - pressedX
        var dy = mouseY - pressedY

        if(Math.abs(dx) >= 2 || Math.abs(dy) >= 2)
            dragging = true

        if(dragging == false)
            return

        dragPositionChanged(dx, dy)
    }
}
