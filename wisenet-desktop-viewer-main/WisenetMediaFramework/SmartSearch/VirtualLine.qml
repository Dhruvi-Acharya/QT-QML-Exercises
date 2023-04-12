import QtQuick 2.15
import QtQuick.Shapes 1.15
import WisenetStyle 1.0
import Wisenet.Define 1.0

Item {
    id: lineItem

    layer.enabled: true
    layer.samples: 8

    property bool selected: false
    property int boundaryGap: blockAeraWidth + 7
    property real lineShapeWidth: lineShape.width
    property real lineShapeHeight: lineShape.height
    property int direction: SmartSearchLineEventType.BothDirection // BothDirection, LeftDirection, RightDirection

    // (-1,-1) ~ (1,1) 좌표계로 정규화 된 좌표
    property real normalX1: 0
    property real normalY1: 0
    property real normalX2: 0
    property real normalY2: 0

    signal deleteButtonClicked()

    // 시작점 좌표 설정
    function setStartPoint(x, y) {
        x = x<boundaryGap ? boundaryGap : x>lineItem.width - boundaryGap ? lineItem.width - boundaryGap : x
        y = y<boundaryGap ? boundaryGap : y>lineItem.height - boundaryGap ? lineItem.height - boundaryGap : y

        normalX1 = x / lineItem.width * 2 - 1
        normalY1 = 1 - y / lineItem.height * 2
    }

    // 끝점 좌표 설정
    function setEndPoint(x, y) {
        x = x<boundaryGap ? boundaryGap : x>lineItem.width - boundaryGap ? lineItem.width - boundaryGap : x
        y = y<boundaryGap ? boundaryGap : y>lineItem.height - boundaryGap ? lineItem.height - boundaryGap : y

        normalX2 = x / lineItem.width * 2 - 1
        normalY2 = 1 - y / lineItem.height * 2
    }

    // 위치 이동
    function moveArea(dx, dy) {
        if(lineShape.x + dx <= boundaryGap) {
            dx = -lineShape.x + boundaryGap
        }
        else if(lineShape.x + lineShape.width + dx + boundaryGap >= lineItem.width) {
            dx = lineItem.width - lineShape.x - lineShape.width - boundaryGap
        }

        if(lineShape.y + dy <= boundaryGap) {
            dy = -lineShape.y + boundaryGap
        }
        else if(lineShape.y + lineShape.height + dy + boundaryGap >= lineItem.height) {
            dy = lineItem.height - lineShape.y - lineShape.height - boundaryGap
        }

        normalX1 = (lineShape.p1x + dx) / lineItem.width * 2 - 1
        normalY1 = 1 - (lineShape.p1y + dy) / lineItem.height * 2
        normalX2 = (lineShape.p2x + dx) / lineItem.width * 2 - 1
        normalY2 = 1 - (lineShape.p2y + dy) / lineItem.height * 2
    }

    Shape {
        id: lineShape

        property real p1x: (normalX1 + 1) * lineItem.width / 2
        property real p1y: (1- normalY1) * lineItem.height / 2
        property real p2x: (normalX2 + 1) * lineItem.width / 2
        property real p2y: (1- normalY2) * lineItem.height / 2

        x: Math.min(p1x, p2x)
        y: Math.min(p1y, p2y)
        width: Math.abs(p1x - p2x)
        height: Math.abs(p1y - p2y)

        // 라인
        ShapePath {
            strokeWidth: 2
            strokeColor: WisenetGui.color_smartSearch_line
            startX: lineShape.p1x < lineShape.p2x ? 0 : lineShape.width
            startY: lineShape.p1y < lineShape.p2y ? 0 : lineShape.height
            PathLine {
                x: lineShape.p1x < lineShape.p2x ? lineShape.width : 0
                y: lineShape.p1y < lineShape.p2y ? lineShape.height : 0
            }
        }

        // lineShape 전체를 drag로 이동
        MouseDragArea {
            anchors.margins: -7
            onPressed: {
                if(lineDrawControl.currentLine !== null)
                    return  // 현재 그리고 있는 line이 있으면 리턴

                lineDrawControl.selectItem(lineItem)    // line 선택
            }
            onDragPositionChanged: {
                if(lineItem.selected == false)
                    return

                moveArea(dx, dy)
            }
            onClicked: {
                // 현재 그리고 있는 line이 없으면 click 이벤트를 전파하지 않음
                //  => click은 영역 선택 우선순위로 동작
                if(lineDrawControl.currentArea === null)
                    mouse.accepted = true
            }
        }
    }

    Rectangle {
        color: WisenetGui.transparent
        width: 16
        height: 40
        anchors.centerIn: lineShape
        visible: lineShapeWidth > 14 || lineShapeHeight > 14
        rotation: direction !== SmartSearchLineEventType.RightDirection ? degree + 90 : degree -90

        property real degree: lineShape.p1y == lineShape.p2y ? 0
                                : Math.atan(-1 * (lineShape.p2x - lineShape.p1x) / (lineShape.p2y - lineShape.p1y)) * 180 / Math.PI

        Image {
            width: 14
            height: 40
            anchors.centerIn: parent
            visible: direction === SmartSearchLineEventType.BothDirection
            sourceSize: Qt.size(width, height)
            source: "qrc:/images/SmartSearch/direction_both.svg"
        }

        Image {
            width: 16
            height: 31
            anchors.centerIn: parent
            visible: direction !== SmartSearchLineEventType.BothDirection
            sourceSize: Qt.size(width, height)
            source: "qrc:/images/SmartSearch/direction_oneway.svg"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // change direction
                if(direction === SmartSearchLineEventType.BothDirection) {
                    direction = SmartSearchLineEventType.LeftDirection
                }
                else if(direction === SmartSearchLineEventType.LeftDirection) {
                    direction = SmartSearchLineEventType.RightDirection
                }
                else {
                    direction = SmartSearchLineEventType.BothDirection
                }
            }
        }
    }


    // DashLine border
    Shape {
        id: borderShape
        anchors.fill: lineShape
        anchors.margins: -7
        visible: lineItem.selected

        ShapePath {
            strokeWidth: 2
            strokeColor: WisenetGui.color_primary
            strokeStyle: ShapePath.DashLine
            dashPattern: [1, 4]
            fillColor: WisenetGui.transparent
            startX: 0
            startY: 0
            PathLine { x: borderShape.width; y: 0 }
            PathLine { x: borderShape.width; y: borderShape.height }
            PathLine { x: 0; y: borderShape.height }
            PathLine { x: 0; y: 0 }
        }

        Rectangle {
            id: deleteRect
            width: Math.min(sizeBase * 0.07, 24)
            height: Math.min(sizeBase * 0.07, 24)
            anchors.top: parent.top
            anchors.left: parent.right
            color: deleteMouseArea.pressed ? WisenetGui.color_primary_dark : WisenetGui.color_primary

            property real sizeBase: lineItem.height > lineItem.width ? lineItem.width : lineItem.height

            Image {
                width: parent.width / 2
                height: parent.height / 2
                anchors.centerIn: parent
                sourceSize: Qt.size(12, 12)
                source: "qrc:/images/SmartSearch/delete.svg"
            }

            MouseArea {
                id: deleteMouseArea
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                onClicked: deleteButtonClicked()
            }
        }
    }

    // 시작점
    PointImage {
        x: lineShape.p1x - 7
        y: lineShape.p1y -7

        // 시작점을 drag로 이동
        onDragPositionChanged: {
            setStartPoint(lineShape.p1x + dx, lineShape.p1y + dy)
        }
    }

    // 끝점
    PointImage {
        x: lineShape.p2x - 7
        y: lineShape.p2y -7

        // 끝점을 drag로 이동
        onDragPositionChanged: {
            setEndPoint(lineShape.p2x + dx, lineShape.p2y + dy)
        }
    }

    component PointImage: Image {
        id: pointImage
        visible: lineItem.selected
        width: 14
        height: 14
        sourceSize: Qt.size(width, height)
        source: mouseArea.containsMouse || mouseArea.pressed ? "qrc:/images/SmartSearch/point_line_hover.svg" : "qrc:/images/SmartSearch/point_line.svg"

        signal dragPositionChanged(var dx, var dy)

        MouseDragArea {
            id: mouseArea
            hoverEnabled: true
            onPressed: lineDrawControl.selectItem(lineItem)
            onDragPositionChanged: {
                if(lineItem.selected == false)
                    return

                pointImage.dragPositionChanged(dx, dy)
            }
        }
    }
}
