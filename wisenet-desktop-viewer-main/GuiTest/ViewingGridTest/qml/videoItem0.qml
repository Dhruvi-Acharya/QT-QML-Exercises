import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: videoItem
    property int gridX: 0
    property int gridY: 0
    property int gridW: 1
    property int gridH: 1
    property bool multipleSelected: false
    property bool focused: false

    property bool moving: false
    property real movingX: 0
    property real movingY: 0
    property real movingW: 0
    property real movingH: 0
    property real cellX: (gridX*parent.gridPixelW)+parent.gridPixelX
    property real cellY: (gridY*parent.gridPixelH)+parent.gridPixelY
    property real cellW: parent.gridPixelW*gridW
    property real cellH: parent.gridPixelH*gridH
    property real cellPadding: 0 // TODO

    width: moving? movingW: cellW
    height: moving? movingH: cellH
    x: moving ? movingX : cellX
    y: moving ? movingY : cellY
    z: moving ? 10 : 1
    opacity: moving ? 0.5 : 1.0

    Image {
        id: testImg
        anchors.fill: parent
        //anchors.margins: 4
        source: "../assets/lighthouse.jpg"
    }

    Rectangle {
        id: multiSelectedRect
        anchors.fill: parent
        visible: parent.multipleSelected
        color: "#80FF6C00"
    }
    Rectangle {
        id: focusedRect
        anchors.fill: parent
        visible: parent.focused
        color: "transparent"
        border.color: "#FF6C00"
        border.width: 2
    }

    function isMouseIn(mx, my) {
        if (mx > x && mx < (x+width) && my > y && my < (y+height))
            return true;
        return false;
    }

    function isMouseResizable(mx, my, limit) {
        // 바깥쪽 사각 체크
        if (mx >= (x-limit) && mx <= (x+width+limit) && my >= (y-limit) && my < (y+height+limit)) {
            // 안쪽 사각 체크
            return (mx <= (x+limit) || mx >= (x+width-limit) || my <= (y+limit) || my >= (y+height-limit));
        }

        return false;
    }

}
