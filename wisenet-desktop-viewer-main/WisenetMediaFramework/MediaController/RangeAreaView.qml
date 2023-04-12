import QtQuick 2.15
import "qrc:/WisenetStyle/"

// 범위 선택 표시 영역
Item {
    property alias rangeRectX: rangeRect.x
    property alias rangeRectWidth: rangeRect.width
    property alias leftImageX: rangeLeftImage.x
    property alias leftImageWidth: rangeLeftImage.width
    property alias rightImageX: rangeRightImage.x
    property alias rightImageWidth: rangeRightImage.width
    property bool mouseHovered: leftMouseArea.containsMouse || rightMouseArea.containsMouse

    function redraw(startTime, endTime) {
        if(selectionStartTime == 0) {
            rangeRect.x = 0
            rangeRect.width = 0
            selectionEndTime = 0
        }
        else {
            rangeRect.x = (selectionStartTime - startTime) / (endTime - startTime) * graphWidth
            if(selectionEndTime == 0)
                rangeRect.width = 2
            else
                rangeRect.width = (selectionEndTime - selectionStartTime) / (endTime - startTime)* graphWidth

            if(rangeRect.x < 0) {
                rangeRect.width += rangeRect.x
                rangeRect.x = 0
            }

            if(rangeRect.x + rangeRect.width > graphWidth)
                rangeRect.width = graphWidth - rangeRect.x;
        }

        var leftTime = convertPositionToMsec(-1)
        var rightTime = convertPositionToMsec(graphWidth + 1)
        rangeLeftImage.visible = selectionStartTime >= leftTime && selectionStartTime <= rightTime
        rangeRightImage.visible = selectionEndTime >= leftTime && selectionEndTime <= rightTime

        //console.log("startTime:", startTime, " endTime:", endTime, " selectionStartTime:", selectionStartTime, " selectionEndTime:", selectionEndTime, " width:", rangeRect.width)
    }    

    Rectangle {
        // 선택 범위 표시 Rectangle
        id: rangeRect
        height: parent.height
        color: WisenetGui.transparent
        //opacity: 0.5
        visible: width > 0
    }

    // 좌측 경계선
    Image {
        id: rangeLeftImage
        width: 8
        height: 12
        x: rangeRect.x - 4
        /*
        x: {
            // 좌우측 경계를 벗어나지 않도록 함
            var imageX = rangeRect.x - 4
            if(imageX < 0)
                return 0
            else if(imageX + width > graphWidth)
                return graphWidth - width
            else
                imageX
        }
        */
        sourceSize: Qt.size(8, 12)
        source: WisenetImage.mediaController_range

        // 좌측 경계에서 마우스 커서 변경
        MouseArea {
            id: leftMouseArea
            anchors.fill: parent
            cursorShape: Qt.SplitHCursor
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            onContainsMouseChanged: {
                if(containsMouse)
                    rangeBarView.currentTime = selectionStartTime
                else
                    rangeBarView.currentTime = 0
            }
        }
    }

    // 우측 경계선
    Image {
        id: rangeRightImage
        width: 8
        height: 12
        x: rangeRect.x + rangeRect.width - 4
        /*
        x: {
            // 좌우측 경계를 벗어나지 않도록 함
            var imageX = rangeRect.x + rangeRect.width - 4
            if(imageX < 0)
                return 0
            else if(imageX + width > graphWidth)
                return graphWidth - width
            else
                imageX
        }
        */
        sourceSize: Qt.size(8, 12)
        source: WisenetImage.mediaController_range

        // 우측 경계에서 마우스 커서 변경
        MouseArea {
            id: rightMouseArea
            anchors.fill: parent
            cursorShape: Qt.SplitHCursor
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            onContainsMouseChanged: {
                if(containsMouse)
                    rangeBarView.currentTime = selectionEndTime
                else
                    rangeBarView.currentTime = 0
            }
        }
    }
}
