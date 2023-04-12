import QtQuick 2.15

// 범위 선택 표시 영역
Item {
    property double rangeRectX: rangeRect.x
    property double rangeRectWidth: rangeRect.width

    onRangeRectXChanged: {
        redraw(timelineStartTime, timelineEndTime)
    }

    onRangeRectWidthChanged: {
        redraw(timelineStartTime, timelineEndTime)
    }

    function redraw(startTime, endTime) {
        if(selectionStartTime == 0) {
            rangeRect.x = 0
            rangeRect.width = 0
            selectionEndTime = 0
        }
        else {
            rangeRect.x = (selectionStartTime - startTime) / (endTime - startTime) * timelineControlView.width
            if(selectionEndTime == 0)
                rangeRect.width = 2
            else
                rangeRect.width = (selectionEndTime - selectionStartTime) / (endTime - startTime)* timelineControlView.width

            if(rangeRect.x < 0) {
                rangeRect.width += rangeRect.x
                rangeRect.x = 0
            }

            if(rangeRect.x + rangeRect.width > timelineControlView.width)
                rangeRect.width = timelineControlView.width - rangeRect.x;
        }

        //console.log("startTime:", startTime, " endTime:", endTime, " selectionStartTime:", selectionStartTime, " selectionEndTime:", selectionEndTime, " width:", rangeRect.width)
    }

    Rectangle {
        // 선택 범위 표시 Rectangle
        id: rangeRect
        height: parent.height
        color: "steelblue"
        opacity: 0.5
        visible: width > 0

        // 좌측 경계선
        Rectangle {
            width: 1
            height: parent.height
            anchors.left: parent.left
            color: "lightskyblue"

            // 좌측 경계에서 마우스 커서 변경
            MouseArea {
                width: 8
                height: parent.height
                x: -4
                cursorShape: timelineControlView.selectRangeDragging ? Qt.ArrowCursor : Qt.SplitHCursor
                acceptedButtons: Qt.NoButton
            }
        }

        // 우측 경계선
        Rectangle {
            width: 1
            height: parent.height
            anchors.right: parent.right
            color: "lightskyblue"

            // 우측 경계에서 마우스 커서 변경
            MouseArea {
                width: 8
                height: parent.height
                x: -4
                cursorShape: timelineControlView.selectRangeDragging ? Qt.ArrowCursor : Qt.SplitHCursor
                acceptedButtons: Qt.NoButton
            }
        }
    }
}
