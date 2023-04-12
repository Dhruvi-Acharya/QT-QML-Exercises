import QtQuick 2.15
import "qrc:/WisenetStyle/"

// SeekBar 표시 영역
Item {
    height: seekBarRect.height + positionBar.height

    property bool dragging: false
    property double mediaPosition: focusedMediaPlayPosition
    property double currentTime: 0  // SeekBar의 현재 시간
    property double rectHeight: seekBarRect.height
    property bool mouseHovered: handleDragArea.pressed || handleDragArea.containsMouse

    onMediaPositionChanged: {
        if(!dragging)
            currentTime = mediaPosition
    }

    onCurrentTimeChanged: {
        redraw(timelineControlView.timelineStartTime, timelineControlView.timelineEndTime)
        if(!mediaControlModel.isMultiTimelineMode && mouseHovered) {
            graphAreaView.hoveredThumbnailTime = currentTime
        }
    }

    function redraw(startTime, endTime) {
        if(currentTime == 0 || !focusedViewingGridItem.hasPlayer || focusedViewingGridItem.mediaParam.isLive) {
            // 시간이 초기화 되었거나, 라이브인 경우 SeekBar 숨김
            visible = false
            return
        }

        var currentPos = 0
        if(currentTime !== startTime)
            currentPos = (currentTime - startTime) / (endTime - startTime) * graphWidth

        if((currentPos > -1 && currentPos < 1)
                || (handleDragArea.dragging && currentPos < 1)) {
            currentPos = 1
        }
        else if((currentPos > graphWidth - 1 && currentPos < graphWidth + 1)
                || (handleDragArea.dragging && currentPos > graphWidth - 1)) {
            currentPos = graphWidth - 1
        }

        if(currentPos < 1 || currentPos > graphWidth - 1)
            visible = false;
        else
            visible = true;

        currentPos += sideMargin

        positionBar.x = currentPos
        seekBarRect.x = currentPos - seekBarRect.width / 2 < 0 ? 0 :
                         currentPos + seekBarRect.width / 2 > timelineControlView.width ? timelineControlView.width - seekBarRect.width :
                         currentPos - seekBarRect.width / 2
    }

    Rectangle {
        id: seekBarRect
        width: 105
        height: supportDateDisplay ? 47 : 30
        color: WisenetGui.contrast_11_bg
        border.color: "#D8D8D8"
        border.width: 1
        visible: (handleDragArea.pressed || handleDragArea.containsMouse) && !graphAreaView.hoveredThumbnailVisible

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
        anchors.top: seekBarRect.bottom
        visible: (handleDragArea.pressed || handleDragArea.containsMouse) && !graphAreaView.hoveredThumbnailVisible
    }

    Rectangle {
        id: bottomPositionBar
        x: positionBar.x
        width: 1
        height: graphArea.height - 12
        color: "#D8D8D8"
        anchors.top: positionHandle.bottom
        visible: mediaControlModel.isMultiTimelineMode
    }

    // Position Handle
    Rectangle {
        id: positionHandle
        width: 22
        height: 22
        radius: 11
        color: WisenetGui.contrast_11_bg
        anchors.horizontalCenter: positionBar.horizontalCenter
        anchors.bottom: positionBar.bottom
        anchors.bottomMargin: - 8

        Rectangle {
            width: 18
            height: 18
            radius: 9
            color:WisenetGui.contrast_02_light_grey
            anchors.centerIn: parent

            Rectangle {
                width: 8
                height: 8
                radius: 4
                color:WisenetGui.contrast_11_bg
                anchors.centerIn: parent
            }
        }

        MouseArea {
            id: handleDragArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            hoverEnabled: true
            cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor

            property double newPosition: 0
            property double pressedX: -1
            property int pressedButton: Qt.NoButton

            onPressedChanged: {
                if(!pressed) {
                    if(dragging) {
                        requestSeektoRecordExist(currentTime)   // drag 했을 때만 seek
                    }
                    else if(pressedButton === Qt.RightButton) {
                        contextMenuView.showMenu(mouseX)    // drag 하지 않고 우클릭 시 contextMenu
                    }

                    dragging = false
                    pressedX = -1
                    seekbarScrollTimer.stop()
                }
                else {
                    pressedButton = pressedButtons
                }
            }

            onMouseXChanged: {
                if(!pressed)
                    return

                if(pressedX == -1)
                    pressedX = mouseX

                if(Math.abs(mouseX - pressedX) >= 1)
                    dragging = true

                if(!dragging)
                    return

                // Drag에 따라 SeekBar 위치를 조정하고, 타임라인 컨트롤을 벗어나면 스크롤 타이머 시작
                newPosition = parent.x + parent.width / 2 + mouseX - pressedX - sideMargin
                if(newPosition >= 0 && newPosition <= graphWidth) {
                    seekbarScrollTimer.stop()
                    currentTime = convertPositionToMsec(newPosition)
                }
                else {
                    seekbarScrollTimer.restart()
                }
            }

            onContainsMouseChanged: {
                if(!mediaControlModel.isMultiTimelineMode && containsMouse) {
                    graphAreaView.hoveredThumbnailTime = currentTime
                }
            }

            Timer {
                id: seekbarScrollTimer
                interval: 10; running: false; repeat: true
                onTriggered: {
                    var time = scrollToPosition(parent.newPosition)
                    if(time !== 0)
                        seekBarView.currentTime = time
                }
            }
        }
    }
}
