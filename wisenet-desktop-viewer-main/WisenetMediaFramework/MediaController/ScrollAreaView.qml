import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

// 스크롤 바 표시 영역
Item {
    id: rootItem
    onWidthChanged: {
        scrollUpdate(false)
    }

    function scrollUpdate(enableAnimation) {
        // 스크롤바 스크롤 (현재 타임라인 표시 시간 기준으로 위치와 width 재조정)
        var totalMsec = mediaControlModel.timelineEndTime - mediaControlModel.timelineStartTime
        var startTimeDiff = mediaControlModel.visibleStartTime - mediaControlModel.timelineStartTime

        var newWidth = mediaControlModel.visibleTimeRange / totalMsec * rootItem.width
        var newX = startTimeDiff == 0 ? 0 : startTimeDiff / totalMsec * rootItem.width

        scrollRect.actualWidth = newWidth

        if(newWidth < 20)
            newWidth = 20

        xBehavior.enabled = enableAnimation
        if(newX + newWidth > rootItem.width)
            scrollRect.x = rootItem.width - newWidth
        else
            scrollRect.x = newX

        widthBehavior.enabled = enableAnimation
        scrollRect.width = newWidth
    }

    Rectangle {
        id: scrollRect
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: !rootItem.enabled ? WisenetGui.contrast_10_dark : scrollRectMouseArea.pressed ? WisenetGui.contrast_07_grey : WisenetGui.contrast_08_dark_grey
        radius: 2

        property double actualWidth: 0

        Rectangle {
            width: 7
            height: 8
            anchors.centerIn: parent
            color: WisenetGui.transparent
            Rectangle {
                width: 1
                height: parent.height
                color: !rootItem.enabled ? WisenetGui.contrast_07_grey : scrollRectMouseArea.pressed ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                anchors.left: parent.left
            }
            Rectangle {
                width: 1
                height: parent.height
                color: !rootItem.enabled ? WisenetGui.contrast_07_grey : scrollRectMouseArea.pressed ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Rectangle {
                width: 1
                height: parent.height
                color: !rootItem.enabled ? WisenetGui.contrast_07_grey : scrollRectMouseArea.pressed ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                anchors.right: parent.right
            }
        }

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

                    timelineVisibleTimeChanged(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
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
                if(newX + scrollRect.width >= rootItem.width)
                    newX = rootItem.width - scrollRect.actualWidth

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
            else if(x + width > rootItem.width) {
                xBehavior.enabled = false
                x = rootItem.width - width
            }
        }

        function convertScrollPositionToMsec(position) {
            var totalMsec = mediaControlModel.timelineEndTime - mediaControlModel.timelineStartTime
            var timeDiff = position / rootItem.width * totalMsec

            return mediaControlModel.timelineStartTime + timeDiff
        }
    }

    Button {
        width: 10
        height: 10
        z: 1
        anchors.verticalCenter: parent.verticalCenter
        background: Rectangle {
            color: WisenetGui.transparent
        }
        visible: focusedViewingGridItem && focusedViewingGridItem.hasPlayer && focusedViewingGridItem.mediaSeekable
                 && (x+5 < scrollRect.x || x+5 > scrollRect.x+scrollRect.width)
        x: convertMsecToScrollPosition(focusedMediaPlayPosition)

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: parent.pressed ? WisenetImage.mediaController_scroll_mediaPos_press : WisenetImage.mediaController_scroll_mediaPos
        }

        onClicked: {            
            scrollToCenter()    // 현재 재생 시간이 화면 가운데 오도록 스크롤
        }

        function convertMsecToScrollPosition(msec) {
            var totalMsec = mediaControlModel.timelineEndTime - mediaControlModel.timelineStartTime
            var timeDiff = msec - mediaControlModel.timelineStartTime

            return timeDiff / totalMsec * rootItem.width - 5
        }
    }
}
