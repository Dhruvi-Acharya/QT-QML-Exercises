import QtQuick 2.15
import "qrc:/WisenetStyle/"

// RangeBar 표시 영역
Item {
    property double currentTime: 0
    property double rectHeight: rangeBarRect.height

    height: rangeBarRect.height + positionBar.height
    visible: currentTime != 0 && !graphAreaView.hoveredThumbnailVisible

    onCurrentTimeChanged: {
        redraw(timelineControlView.timelineStartTime, timelineControlView.timelineEndTime)
        if(!mediaControlModel.isMultiTimelineMode && currentTime != 0) {
            graphAreaView.hoveredThumbnailTime = currentTime
        }
    }

    function redraw(startTime, endTime) {
        if(currentTime == 0) {
            return
        }

        var currentPos = 0
        if(currentTime !== startTime)
            currentPos = (currentTime - startTime) / (endTime - startTime) * graphWidth

        if(currentPos < 1)
            currentPos = 1
        else if(currentPos > graphWidth - 1)
            currentPos = graphWidth - 1

        currentPos += sideMargin

        positionBar.x = currentPos
        rangeBarRect.x = currentPos - rangeBarRect.width / 2 < 0 ? 0 :
                         currentPos + rangeBarRect.width / 2 > timelineControlView.width ? timelineControlView.width - rangeBarRect.width :
                         currentPos - rangeBarRect.width / 2
    }

    Rectangle {
        id: rangeBarRect
        width: 105
        height: supportDateDisplay ? 47 : 30
        color: WisenetGui.contrast_11_bg
        border.color: "#D8D8D8"
        border.width: 1

        // 날짜 Text
        Text {
            height: supportDateDisplay ? 18 : 0
            visible: supportDateDisplay
            font.pixelSize: 11
            color: "#D8D8D8"
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: {
                return localeManager.getDateFromMsec(currentTime)
            }
        }

        // 시간 Text
        Text {
            height: 18
            font.pixelSize: 14
            color: "#D8D8D8"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: {
                return new Date(currentTime).toLocaleTimeString(Qt.locale(), "hh:mm:ss");
            }
        }
    }

    Rectangle {
        id: positionBar
        width: 1
        height: mediaControlModel.isMultiTimelineMode ? 24 : 30
        color: "#D8D8D8"
        anchors.top: rangeBarRect.bottom
    }
}
