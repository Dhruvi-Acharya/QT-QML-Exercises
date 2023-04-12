import QtQuick 2.15
import QtQuick.Shapes 1.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import Wisenet.Define 1.0
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0
import "qrc:/"

Item {
    id: areaItem

    layer.enabled: true
    layer.samples: 8

    property bool selected: false
    property int currentPointIndex: -1
    property int areaType: SmartSearchAreaType.Inside // Inside(roi) or Outside(exclude)
    property real areaShapeWidth: areaShape.width
    property real areaShapeHeight: areaShape.height
    property int boundaryGap: blockAeraWidth + 7
    property bool aiFilterVisible: false

    signal deleteButtonClicked()

    onWidthChanged: areaShape.refresh()
    onHeightChanged: areaShape.refresh()

    // (-1,-1) ~ (1,1) 좌표계로 정규화 된 좌표
    property var normalXs: []
    property var normalYs: []

    function getPointsCount() {
        return normalYs.length
    }

    function addPoint(x, y) {
        x = x<boundaryGap ? boundaryGap : x>areaItem.width - boundaryGap ? areaItem.width - boundaryGap : x
        y = y<boundaryGap ? boundaryGap : y>areaItem.height - boundaryGap ? areaItem.height - boundaryGap : y

        normalXs.push(x / areaItem.width * 2 - 1)
        normalYs.push(1 - y / areaItem.height * 2)

        if(normalYs.length === 1) {
            // 1번 점을 추가하면 같은 위치에 2번 점도 추가
            normalXs.push(x / areaItem.width * 2 - 1)
            normalYs.push(1 - y / areaItem.height * 2)
        }

        currentPointIndex = normalYs.length - 1

        areaShape.refresh()
    }

    // top-left가 (x1, y1), bottom-right가 (x2, y2)가 되도록 4점을 추가
    function addRectangle(x1, y1, x2, y2) {
        x1 = x1<boundaryGap ? boundaryGap : x1>areaItem.width - boundaryGap ? areaItem.width - boundaryGap : x1
        y1 = y1<boundaryGap ? boundaryGap : y1>areaItem.height - boundaryGap ? areaItem.height - boundaryGap : y1
        x2 = x2<boundaryGap ? boundaryGap : x2>areaItem.width - boundaryGap ? areaItem.width - boundaryGap : x2
        y2 = y2<boundaryGap ? boundaryGap : y2>areaItem.height - boundaryGap ? areaItem.height - boundaryGap : y2

        normalXs.push(x1 / areaItem.width * 2 - 1)
        normalYs.push(1 - y1 / areaItem.height * 2)

        normalXs.push(x2 / areaItem.width * 2 - 1)
        normalYs.push(1 - y1 / areaItem.height * 2)

        normalXs.push(x2 / areaItem.width * 2 - 1)
        normalYs.push(1 - y2 / areaItem.height * 2)

        normalXs.push(x1 / areaItem.width * 2 - 1)
        normalYs.push(1 - y2 / areaItem.height * 2)

        areaShape.refresh()
    }

    function updateCurrentPoint(x, y) {
        if(currentPointIndex < 0 || currentPointIndex >= normalYs.length)
            return

        x = x<boundaryGap ? boundaryGap : x>areaItem.width - boundaryGap ? areaItem.width - boundaryGap : x
        y = y<boundaryGap ? boundaryGap : y>areaItem.height - boundaryGap ? areaItem.height - boundaryGap : y

        var normalX = x / areaItem.width * 2 - 1
        var normalY = 1 - y / areaItem.height * 2

        if(lineCrossCheck(normalX, normalY)) {
            // 라인 교차가 없으면 좌표 업데이트
            normalXs[currentPointIndex] = normalX
            normalYs[currentPointIndex] = normalY
            areaShape.refresh()
        }
    }

    // 위치 이동
    function moveArea(dx, dy) {
        if(getPointsCount() !== 4)
            return  // 4점 완성된 영역만 이동 가능

        if(areaShape.x + dx <= boundaryGap) {
            dx = -areaShape.x + boundaryGap
        }
        else if(areaShape.x + areaShape.width + dx + boundaryGap >= areaItem.width) {
            dx = areaItem.width - areaShape.x - areaShape.width - boundaryGap
        }

        if(areaShape.y + dy <= boundaryGap) {
            dy = -areaShape.y + boundaryGap
        }
        else if(areaShape.y + areaShape.height + dy + boundaryGap >= areaItem.height) {
            dy = areaItem.height - areaShape.y - areaShape.height - boundaryGap
        }

        normalXs[0] = (areaShape.p1x + areaShape.leftX + dx) / areaItem.width * 2 - 1
        normalYs[0] = 1 - (areaShape.p1y + areaShape.topY + dy) / areaItem.height * 2
        normalXs[1] = (areaShape.p2x + areaShape.leftX + dx) / areaItem.width * 2 - 1
        normalYs[1] = 1 - (areaShape.p2y + areaShape.topY + dy) / areaItem.height * 2
        normalXs[2] = (areaShape.p3x + areaShape.leftX + dx) / areaItem.width * 2 - 1
        normalYs[2] = 1 - (areaShape.p3y + areaShape.topY + dy) / areaItem.height * 2
        normalXs[3] = (areaShape.p4x + areaShape.leftX + dx) / areaItem.width * 2 - 1
        normalYs[3] = 1 - (areaShape.p4y + areaShape.topY + dy) / areaItem.height * 2

        areaShape.refresh()
    }

    function getPoints() {
        var points = []

        for(var i=0 ; i<getPointsCount() ; i++) {
            points.push(normalXs[i])
            points.push(normalYs[i])
        }

        return points
    }

    function getEventTypes() {
        var types = []

        if(motionButton.toggleOn)
            types.push(SmartSearchAreaEventType.Motion)
        if(enterButton.toggleOn)
            types.push(SmartSearchAreaEventType.Enter)
        if(exitButton.toggleOn)
            types.push(SmartSearchAreaEventType.Exit)

        return types
    }

    function getAITypes() {
        var types = []

        if(personButton.toggleOn)
            types.push(SmartSearchAIType.Person)
        if(vehicleButton.toggleOn)
            types.push(SmartSearchAIType.Vehicle)
        if(unknownButton.toggleOn)
            types.push(SmartSearchAIType.Unknown)

        return types
    }

    Shape {
        id: areaShape

        property real p1x
        property real p1y
        property real p2x
        property real p2y
        property real p3x
        property real p3y
        property real p4x
        property real p4y

        property real leftX
        property real rightX
        property real topY
        property real bottomY

        x: leftX
        y: topY
        z: 1
        width: rightX - leftX
        height: bottomY - topY

        function refresh() {
            var x1 = normalXs.length > 0 ? (normalXs[0] + 1) * areaItem.width / 2 : -1
            var y1 = normalYs.length > 0 ? (1- normalYs[0]) * areaItem.height / 2 : -1
            var x2 = normalXs.length > 1 ? (normalXs[1] + 1) * areaItem.width / 2 : x1
            var y2 = normalYs.length > 1 ? (1- normalYs[1]) * areaItem.height / 2 : y1
            var x3 = normalXs.length > 2 ? (normalXs[2] + 1) * areaItem.width / 2 : x2
            var y3 = normalYs.length > 2 ? (1- normalYs[2]) * areaItem.height / 2 : y2
            var x4 = normalXs.length > 3 ? (normalXs[3] + 1) * areaItem.width / 2 : x3
            var y4 = normalYs.length > 3 ? (1- normalYs[3]) * areaItem.height / 2 : y3

            leftX = Math.min(x1, x2, x3, x4)
            rightX = Math.max(x1, x2, x3, x4)
            topY = Math.min(y1, y2, y3, y4)
            bottomY = Math.max(y1, y2, y3, y4)

            p1x = x1 - leftX
            p1y = y1 - topY
            p2x = x2 - leftX
            p2y = y2 - topY
            p3x = x3 - leftX
            p3y = y3 - topY
            p4x = x4 - leftX
            p4y = y4 - topY
        }

        ShapePath {
            id: path
            strokeWidth: 2
            strokeColor: areaType == SmartSearchAreaType.Inside ? WisenetGui.color_smartSearch_roi : WisenetGui.color_smartSearch_exclude
            fillColor: areaType == SmartSearchAreaType.Inside ? WisenetGui.color_smartSearch_roi_30 : WisenetGui.color_smartSearch_exclude_30

            startX: areaShape.p1x
            startY: areaShape.p1y
            PathLine {
                x: areaShape.p2x
                y: areaShape.p2y
            }
            PathLine {
                x: areaShape.p3x
                y: areaShape.p3y
            }
            PathLine {
                x: areaShape.p4x
                y: areaShape.p4y
            }
            PathLine {
                x: areaShape.p1x
                y: areaShape.p1y
            }
        }

        // areaShape 전체를 drag로 이동
        MouseDragArea {
            onPressed: {
                if(roiDrawControl.currentArea !== null || excludeDrawControl.currentArea !== null)
                    return  // 현재 그리고 있는 area가 있으면 리턴

                if(areaItem.areaType == SmartSearchAreaType.Inside)
                    roiDrawControl.selectItem(areaItem)
                else
                    excludeDrawControl.selectItem(areaItem)
            }
            onDragPositionChanged: {
                if(areaItem.selected == false)
                    return

                moveArea(dx, dy)
            }
            onClicked: {
                // 현재 그리고 있는 영역이 없으면 click 이벤트를 전파하지 않음
                //  => click은 영역 선택 우선순위로 동작
                if(roiDrawControl.currentArea == null && excludeDrawControl.currentArea == null)
                    mouse.accepted = true
            }
        }

        PointImage {
            id: p1
            x: areaShape.p1x - 7
            y: areaShape.p1y -7

            onPressed: currentPointIndex = 0
            onDragPositionChanged: updateCurrentPoint(areaShape.x + areaShape.p1x + dx, areaShape.y + areaShape.p1y + dy)
        }

        PointImage {
            id: p2
            x: areaShape.p2x - 7
            y: areaShape.p2y -7

            onPressed: currentPointIndex = 1
            onDragPositionChanged: updateCurrentPoint(areaShape.x + areaShape.p2x + dx, areaShape.y + areaShape.p2y + dy)
        }

        PointImage {
            id: p3
            x: areaShape.p3x - 7
            y: areaShape.p3y -7

            onPressed: currentPointIndex = 2
            onDragPositionChanged: updateCurrentPoint(areaShape.x + areaShape.p3x + dx, areaShape.y + areaShape.p3y + dy)
        }

        PointImage {
            id: p4
            x: areaShape.p4x - 7
            y: areaShape.p4y -7

            onPressed: currentPointIndex = 3
            onDragPositionChanged: updateCurrentPoint(areaShape.x + areaShape.p4x + dx, areaShape.y + areaShape.p4y + dy)
        }
    }

    // DashLine border
    Shape {
        id: borderShape
        anchors.fill: areaShape
        anchors.margins: -2
        visible: areaItem.selected

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

            property real sizeBase: areaItem.height > areaItem.width ? areaItem.width : areaItem.height

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

    property real buttonRowMinWidth: eventTypeRow.width + aiTypeRow.width + 45

    Rectangle {
        color: WisenetGui.transparent
        width: Math.max(buttonRowMinWidth, borderShape.width)
        height: personButton.height
        z: 2
        visible: areaType == SmartSearchAreaType.Inside && areaItem.selected

        // 버튼을 영역 가운데 정렬시 예상 좌표
        property real leftX: borderShape.x + (borderShape.width - width) / 2
        property real rightX: borderShape.x + (borderShape.width + width) / 2
        property real topY: borderShape.y + borderShape.height + 7
        property real bottomY: borderShape.y + borderShape.height + height + 7

        // 실제 좌표
        x: leftX < boundaryGap ? boundaryGap : rightX > areaItem.width - boundaryGap ? areaItem.width - boundaryGap - width : leftX
        y: bottomY < areaItem.height - boundaryGap - 7 ? topY : areaItem.height - boundaryGap - height - 7

        Row {
            id: eventTypeRow
            spacing: 3
            anchors.left: parent.left
            anchors.leftMargin: 7

            TypeButton {
                id: motionButton
                buttonImage.source: toggleOn ? "qrc:/images/SmartSearch/motion_on.svg" : "qrc:/images/SmartSearch/motion_off.svg"
                onClicked: {
                    if(!enterButton.toggleOn && !exitButton.toggleOn)
                        toggleOn = true
                    else
                        toggleOn = !toggleOn
                }
                WisenetMediaToolTip {
                    text: WisenetLinguist.motionDetection
                    visible: parent.hovered
                }
            }
            TypeButton {
                id: enterButton
                buttonImage.source: toggleOn ? "qrc:/images/SmartSearch/enter_on.svg" : "qrc:/images/SmartSearch/enter_off.svg"
                onClicked: {
                    if(!motionButton.toggleOn && !exitButton.toggleOn)
                        toggleOn = true
                    else
                        toggleOn = !toggleOn
                }
                WisenetMediaToolTip {
                    text: WisenetLinguist.smartSearchEntering
                    visible: parent.hovered
                }
            }
            TypeButton {
                id: exitButton
                buttonImage.source: toggleOn ? "qrc:/images/SmartSearch/exit_on.svg" : "qrc:/images/SmartSearch/exit_off.svg"
                onClicked: {
                    if(!motionButton.toggleOn && !enterButton.toggleOn)
                        toggleOn = true
                    else
                        toggleOn = !toggleOn
                }
                WisenetMediaToolTip {
                    text: WisenetLinguist.smartSearchExiting
                    visible: parent.hovered
                }
            }
        }

        Row {
            id: aiTypeRow
            spacing: 3
            anchors.right: parent.right
            anchors.rightMargin: 7

            TypeButton {
                id: personButton
                visible: areaItem.aiFilterVisible
                buttonImage.source: toggleOn ? "qrc:/images/SmartSearch/person_on.svg" : "qrc:/images/SmartSearch/person_off.svg"
                onClicked: {
                    if(!vehicleButton.toggleOn && !unknownButton.toggleOn)
                        toggleOn = true
                    else
                        toggleOn = !toggleOn
                }
                WisenetMediaToolTip {
                    text: WisenetLinguist.person
                    visible: parent.hovered
                }
            }
            TypeButton {
                id: vehicleButton
                visible: areaItem.aiFilterVisible
                buttonImage.source: toggleOn ? "qrc:/images/SmartSearch/vehicle_on.svg" : "qrc:/images/SmartSearch/vehicle_off.svg"
                onClicked: {
                    if(!personButton.toggleOn && !unknownButton.toggleOn)
                        toggleOn = true
                    else
                        toggleOn = !toggleOn
                }
                WisenetMediaToolTip {
                    text: WisenetLinguist.vehicle
                    visible: parent.hovered
                }
            }
            TypeButton {
                id: unknownButton
                visible: areaItem.aiFilterVisible
                buttonImage.source: toggleOn ? "qrc:/images/SmartSearch/unknown_on.svg" : "qrc:/images/SmartSearch/unknown_off.svg"
                onClicked: {
                    if(!personButton.toggleOn && !vehicleButton.toggleOn)
                        toggleOn = true
                    else
                        toggleOn = !toggleOn
                }
                WisenetMediaToolTip {
                    text: WisenetLinguist.smartSearchUnknown
                    visible: parent.hovered
                }
            }
        }
    }

    component PointImage: Image {
        id: pointImage
        visible: areaItem.selected
        width: 14
        height: 14
        sourceSize: Qt.size(width, height)
        source: {
            if(areaType == SmartSearchAreaType.Inside) {
                if(mouseArea.containsMouse || mouseArea.pressed)
                    return "qrc:/images/SmartSearch/point_roi_hover.svg"
                else
                    return "qrc:/images/SmartSearch/point_roi.svg"
            }
            else {
                if(mouseArea.containsMouse || mouseArea.pressed)
                    return "qrc:/images/SmartSearch/point_exclude_hover.svg"
                else
                    return "qrc:/images/SmartSearch/point_exclude.svg"
            }
        }

        signal pressed()
        signal dragPositionChanged(var dx, var dy)

        MouseDragArea {
            id: mouseArea
            hoverEnabled: true
            onDragPositionChanged: {
                if(areaItem.selected == false)
                    retuen

                pointImage.dragPositionChanged(dx, dy)
            }
            onPressedChanged: {
                if(pressed) {
                    // areaItem 선택 및 currentPointIndex 업데이트
                    if(areaItem.areaType == SmartSearchAreaType.Inside)
                        roiDrawControl.selectItem(areaItem)
                    else
                        excludeDrawControl.selectItem(areaItem)

                    pointImage.pressed()
                }
                else {
                    areaItem.currentPointIndex = -1
                }
            }
        }
    }

    component TypeButton: Button {
        width: Math.min(sizeBase * 0.15, 31)
        height: Math.min(sizeBase * 0.15, 30)
        background: Rectangle {
            color: WisenetGui.transparent
        }

        property alias buttonImage: buttonImage
        property bool toggleOn: true
        property real sizeBase: areaItem.height > areaItem.width ? areaItem.width : areaItem.height

        Image {
            id: buttonImage
            anchors.fill: parent
            sourceSize: Qt.size(31, 30)
        }
    }

    function ccw(x1, y1, x2, y2, x3, y3) {
        // Counter-ClockWise 알고리즘
        // 점 3개를 이은 직선의 방향 체크
        var value = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1)
        if(value < 0)
            return -1   // 시계방향
        else if(value > 0)
            return 1    // 반시계방향
        else
            return 0    // 평행
    }

    // line 교차가 있으면 false, 없으면 true를 리턴하는 함수
    // normalX, normalY는 현재 이동 중인 점(currentPointIndex)의 정규화 좌표
    function lineCrossCheck(normalX, normalY) {
        if(getPointsCount() !== 4)
            return true // 3점 이하면 교차 없음

        var xList = []
        var yList = []

        for(var i=0 ; i<normalYs.length ; i++) {
            if(i == currentPointIndex) {
                xList.push(normalX)
                yList.push(normalY)
            }
            else {
                xList.push(normalXs[i])
                yList.push(normalYs[i])
            }
        }

        // 점 A,B,C,D의 index
        var a = currentPointIndex
        var b = (a+1) % 4
        var c = (b+1) % 4
        var d = (c+1) % 4

        // 선분AB와 선분CD의 교차 여부
        // CCW(A,B,C)*CCW(A,B,D)<0 && CCW(C,D,A)*CCW(C,D,B)<0 이면 선분AB와 선분CD가 교차
        var ccw1 = ccw(xList[a], yList[a], xList[b], yList[b], xList[c], yList[c])
        var ccw2 = ccw(xList[a], yList[a], xList[b], yList[b], xList[d], yList[d])
        var ccw3 = ccw(xList[c], yList[c], xList[d], yList[d], xList[a], yList[a])
        var ccw4 = ccw(xList[c], yList[c], xList[d], yList[d], xList[b], yList[b])

        if(ccw1 * ccw2 < 0 && ccw3 * ccw4 < 0)
            return false

        // 선분AD와 선분BC의 교차 여부
        // CCW(A,D,B)*CCW(A,D,C)<0 && CCW(B,C,A)*CCW(B,C,D)<0 이면 선분AD와 선분BC가 교차
        ccw1 = ccw(xList[a], yList[a], xList[d], yList[d], xList[b], yList[b])
        ccw2 = ccw(xList[a], yList[a], xList[d], yList[d], xList[c], yList[c])
        ccw3 = ccw(xList[b], yList[b], xList[c], yList[c], xList[a], yList[a])
        ccw4 = ccw(xList[b], yList[b], xList[c], yList[c], xList[d], yList[d])

        if(ccw1 * ccw2 < 0 && ccw3 * ccw4 < 0)
            return false
        else
            return true
    }
}
