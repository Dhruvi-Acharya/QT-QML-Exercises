import QtQuick 2.15

// 스크롤 바 표시 영역
Item {
    onWidthChanged: {
        scrollUpdate(false)
    }

    function scrollUpdate(enableAnimation) {
        // 스크롤바 스크롤 (현재 타임라인 표시 시간 기준으로 위치와 width 재조정)
        var totalMsec = mediaControlModel.recordingEndTime - mediaControlModel.recordingStartTime
        var startTimeDiff = mediaControlModel.visibleStartTime - mediaControlModel.recordingStartTime

        var newWidth = mediaControlModel.visibleTimeRange / totalMsec * scrollArea.width
        var newX = startTimeDiff / totalMsec * scrollArea.width

        scrollRect.actualWidth = newWidth

        if(newWidth < 20)
            newWidth = 20

        widthBehavior.enabled = enableAnimation
        scrollRect.width = newWidth

        xBehavior.enabled = enableAnimation
        if(newX + newWidth > scrollArea.width)
            scrollRect.x = scrollArea.width - newWidth
        else
            scrollRect.x = newX
    }

    Rectangle {
        id: scrollRect
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: "Gray"

        property double actualWidth: 0

        MouseArea {
            id: scrollRectMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            property bool dragging : false
            property double pressedX: -1

            onPressedChanged: {
                if(!pressed) {
                    dragging = false
                    pressedX = -1
                }
            }

            onMouseXChanged: {
                if(pressedX == -1)
                    pressedX = mouseX

                if(Math.abs(pressedX - mouseX) >= 1)
                    dragging = true

                if(dragging)
                {
                    var newX = scrollRect.x + mouseX - pressedX
                    if(newX < 0)
                        newX = 0
                    else if(newX + scrollRect.width > scrollRect.parent.width)
                        newX = scrollRect.parent.width - scrollRect.width

                    xBehavior.enabled = false
                    scrollRect.x = newX
                }
            }
        }

        Behavior on x {
            id: xBehavior
            NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo }
        }

        Behavior on width {
            id: widthBehavior
            NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo }
        }

        onXChanged: {
            if(scrollRectMouseArea.dragging) {
                // 스크롤바 Drag 시, 표시 시간 변경
                var newX = x
                if(newX + scrollRect.width >= scrollArea.width)
                    newX = scrollArea.width - scrollRect.actualWidth

                mediaControlModel.visibleStartTime = convertScrollPositionToMsec(newX)
                mediaControlModel.visibleEndTime = mediaControlModel.visibleStartTime + mediaControlModel.visibleTimeRange

                customAnimation = false
                timelineStartTime = mediaControlModel.visibleStartTime
                timelineEndTime = mediaControlModel.visibleEndTime

                redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
            }
        }

        onWidthChanged: {
            // width 변경 시, 영역 밖으로 벗어나는 것을 방지
            if(x < 0) {
                xBehavior.enabled = false
                x = 0
            }
            else if(x + width > scrollArea.width) {
                xBehavior.enabled = false
                x = scrollArea.width - width
            }
        }

        function convertScrollPositionToMsec(position) {
            var totalMsec = mediaControlModel.recordingEndTime - mediaControlModel.recordingStartTime
            var timeDiff = position / scrollArea.width * totalMsec

            return mediaControlModel.recordingStartTime + timeDiff
        }

        function convertMsecToScrollPosition(msec) {
            var totalMsec = mediaControlModel.recordingEndTime - mediaControlModel.recordingStartTime
            var timeDiff = msec - mediaControlModel.recordingStartTime

            return timeDiff / totalMsec * scrollArea.width
        }
    }

}
