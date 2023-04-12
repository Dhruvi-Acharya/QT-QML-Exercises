import QtQuick 2.15

// 타임라인 Record Graph 표시 영역
Item {
    Component.onCompleted: {
        redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
    }

    Canvas {
        id: graphCanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            // graphAreaModel의 데이터를 차례대로 그림
            var index = 0
            var count = graphAreaModel.dataCount
            for(; index < count ; index++) {
                if(graphAreaModel.isEventRecord(index))
                    ctx.fillStyle = "Red"
                else
                    ctx.fillStyle = "Green"

                ctx.fillRect(graphAreaModel.startPosition(index), 0, graphAreaModel.width(index), height)
            }
        }
    }

    function redraw(startTime, endTime) {
        graphAreaModel.refreshGraphDataList(startTime, endTime, timelineControlView.width)
        graphCanvas.requestPaint()
    }
}
