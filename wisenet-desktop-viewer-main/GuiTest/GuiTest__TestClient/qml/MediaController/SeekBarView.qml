import QtQuick 2.15

// SeekBar 표시 영역
Item {
    property double currentTime: mediaControlModel.visibleEndTime   // SeekBar의 현재 시간

    onCurrentTimeChanged: {
        redraw(timelineControlView.timelineStartTime, timelineControlView.timelineEndTime)
    }

    function redraw(startTime, endTime) {
        var currentPos = 0
        if(currentTime !== startTime)
            currentPos = (currentTime - startTime) / (endTime - startTime) * timelineControlView.width

        if((currentPos > -1 && currentPos < 1)
                || (seekbarDragArea.dragging && currentPos < 1)) {
            currentPos = 1
        }
        else if((currentPos > timelineControlView.width - 1 && currentPos < timelineControlView.width + 1)
                || (seekbarDragArea.dragging && currentPos > timelineControlView.width - 1)) {
            currentPos = timelineControlView.width - 1
        }

        if(currentPos < 1 || currentPos > timelineControlView.width - 1)
            visible = false;
        else
            visible = true;

        seekBarRect.x = currentPos - seekBarRect.width / 2
    }

    Rectangle {
        id: seekBarRect
        y: -height - 10
        width: 128
        height: 42
        color: "transparent"

        Canvas {
            id: seekbarCanvas
            width: seekBarRect.width
            height: seekBarRect.height + timelineControlView.height - scrollArea.height + 9

            // SeekBar의 배경을 그림
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                ctx.fillStyle = "white"
                ctx.fillRect(0, 0, seekBarRect.width, seekBarRect.height)
                ctx.fillRect(seekBarRect.width/2 - 1, seekBarRect.height + 9, 2, height - seekBarRect.height)
                ctx.beginPath();
                ctx.moveTo(seekBarRect.width/2 - 6, seekBarRect.height);
                ctx.lineTo(seekBarRect.width/2, seekBarRect.height + 6);
                ctx.lineTo(seekBarRect.width/2 + 6, seekBarRect.height);
                ctx.fill();
            }
        }

        // 날짜 Text
        Text {
            y: 0
            height: seekBarRect.height / 2
            anchors.left: seekBarRect.left
            anchors.right: seekBarRect.right
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: {
                return new Date(currentTime).toLocaleDateString(Qt.locale(), "dd MMMM yyyy");
            }
        }

        // 시간 Text
        Text {
            y: seekBarRect.height / 2
            height: seekBarRect.height / 2
            anchors.left: seekBarRect.left
            anchors.right: seekBarRect.right
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            font.pointSize: 11
            text: {
                return new Date(currentTime).toLocaleTimeString();
            }
        }

        // Drag를 위한 MouseArea
        MouseArea {
            id: seekbarDragArea
            anchors.fill: seekBarRect
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool dragging : false
            property double newPosition: 0
            property double pressedX: -1

            onPressedChanged: {
                if(!pressed) {
                    dragging = false
                    pressedX = -1
                    seekbarScrollTimer.stop()
                }
            }

            onMouseXChanged: {
                if(pressedX == -1)
                    pressedX = mouseX

                if(Math.abs(mouseX - pressedX) >= 1)
                    dragging = true

                if(!dragging)
                    return

                // Drag에 따라 SeekBar 위치를 조정하고, 타임라인 컨트롤을 벗어나면 스크롤 타이머 시작
                newPosition = seekBarRect.x + seekBarRect.width / 2 + mouseX - pressedX
                if(newPosition >= 0 && newPosition <= timelineControlView.width) {
                    seekbarScrollTimer.stop()
                    currentTime = convertPositionToMsec(newPosition)
                }
                else {
                    seekbarScrollTimer.restart()
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
