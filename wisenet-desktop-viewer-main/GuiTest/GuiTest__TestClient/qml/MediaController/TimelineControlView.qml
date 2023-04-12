import QtQuick 2.15
import QtQuick.Controls 2.15

// MediaController 내부의 타임라인 영역 Main View
Item {
    id: timelineControlView

    property int animationDuration: 2000    // customAnimation의 duration
    property bool customAnimation: false    // customAnimation 사용 여부
    property double timelineStartTime: mediaControlModel.visibleStartTime   // customAnimation 사용 시 Easing에 의해 변경되는 타임라인 시작시간
    property double timelineEndTime: mediaControlModel.visibleEndTime       // customAnimation 사용 시 Easing에 의해 변경되는 타임라인 끝
    property double selectionStartTime: 0   // 범위 선택 시작 시간
    property double selectionEndTime: 0     // 범위 선택 끝 시간
    property bool selectRangeDragging: selectRangeMouseArea.dragging    // 범위 선택 Drag 상태 값

    // CustomAnimation에 의한 타임라인 redraw
    Behavior on timelineStartTime {
        enabled: customAnimation
        NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo; }
    }

    Behavior on timelineEndTime {
        enabled: customAnimation
        NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo; }
    }

    onTimelineStartTimeChanged: {
        //console.log("onStartTimeChanged:", startTime)
        if(customAnimation && timelineEndTime === mediaControlModel.recordingEndTime) {
            redraw(timelineStartTime, timelineEndTime)
        }
    }

    onTimelineEndTimeChanged: {
        //console.log("onEndTimeChanged:", endTime)
        if(customAnimation) {
            redraw(timelineStartTime, timelineEndTime)
        }
    }
    //

    // 범위 선택 변경 시 rangeAreaView 업데이트
    onSelectionStartTimeChanged: {
        if(selectionStartTime == 0) {
            rangeAreaView.rangeRectX = 0
            rangeAreaView.rangeRectWidth = 0
            selectionEndTime = 0
        }
        else {
            rangeAreaView.rangeRectX = (selectionStartTime - mediaControlModel.visibleStartTime) / mediaControlModel.visibleTimeRange * timelineControlView.width
            if(selectionEndTime == 0)
                rangeAreaView.rangeRectWidth = 2
            else
                rangeAreaView.rangeRectWidth = (selectionEndTime - selectionStartTime) / mediaControlModel.visibleTimeRange * timelineControlView.width
        }
    }

    onSelectionEndTimeChanged: {
        if(selectionEndTime != 0) {
            rangeAreaView.rangeRectWidth = (selectionEndTime - selectionStartTime) / mediaControlModel.visibleTimeRange * timelineControlView.width
        }
    }
    //

    // UI 리사이즈 시 redraw
    onWidthChanged: {
        redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
    }

    function redraw(startTime, endTime) {
        dateAreaModel.updateDateDataList(startTime, endTime, timelineControlView.width)
        timeModel.updateTimeDataList(startTime, endTime, timelineControlView.width)
        graphAreaView.redraw(startTime, endTime)
        rangeAreaView.redraw(startTime, endTime)
        seekBarView.redraw(startTime, endTime)
    }
    //

    // 좌표를 시간으로 변환하는 함수
    function convertPositionToMsec(pos) {
        if(pos === 0)
            return timelineStartTime

        var timeDiff = pos / timelineControlView.width * (timelineEndTime - timelineStartTime)
        return timelineStartTime + timeDiff
    }

    // 특정 좌표에 대응되는 시간이 타임라인 시작점으로 이동하도록 타임라인을 스크롤 하는 함수
    function scrollToPosition(pos) {
        var time = 0

        if(pos < 0) {
            time = convertPositionToMsec(pos)
            if(time < mediaControlModel.recordingStartTime)
                time = mediaControlModel.recordingStartTime

            mediaControlModel.visibleStartTime = time
            mediaControlModel.visibleEndTime = time + mediaControlModel.visibleTimeRange
        }
        else if(pos > width) {
            time = convertPositionToMsec(pos)
            if(time > mediaControlModel.recordingEndTime)
                time = mediaControlModel.recordingEndTime

            mediaControlModel.visibleEndTime = time
            mediaControlModel.visibleStartTime = time - mediaControlModel.visibleTimeRange
        }
        else {
            return 0
        }

        customAnimation = false
        timelineStartTime = mediaControlModel.visibleStartTime
        timelineEndTime = mediaControlModel.visibleEndTime

        scrollAreaView.scrollUpdate(false)
        redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)

        return time
    }

    Connections {
        target: mediaControlModel
        // 타임라인 표시 범위가 변경되는 경우 호출되는 slot 함수
        function onVisibleTimeRangeChanged(range) {
            // 타임라인 위치와 크기 조정
            customAnimation = true
            timelineStartTime = mediaControlModel.visibleStartTime
            timelineEndTime = mediaControlModel.visibleEndTime
            //console.log("[Timeline] TimeDataCount:", timeModel.rowCount(), " GraphDataCount:", graphAreaModel.dataCount)

            // 스크롤 바 위치와 크기 조정
            scrollAreaView.scrollUpdate(true)

            // animation 속도 초기화
            timelineControlView.animationDuration = 2000;
        }
    }

    // Zoom In & Zoom Out
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton
        hoverEnabled: true

        // 타임라인 전체 영역에서, Mouse Wheel에 의한 확대 축소 동작
        onWheel: {
            if(mediaControlModel.visibleTimeRange <= 1500 && wheel.angleDelta.y > 0)
                return

            var rangeDelta = mediaControlModel.visibleTimeRange * 0.1  // 표시 범위 변화량
            var startTimeDelta = mouseX / width * rangeDelta    // 표시 시작 시간 변화량

            if(wheel.angleDelta.y > 0) {
                // Zoom In
                mediaControlModel.setVisibleTimeRange(startTimeDelta, -rangeDelta)
            }
            else {
                // Zoom Out
                mediaControlModel.setVisibleTimeRange(-startTimeDelta, rangeDelta)
            }
        }
    }

    // Select & Resize Range
    MouseArea {
        id: selectRangeMouseArea
        width: parent.width
        anchors.top: dateArea.top
        anchors.bottom: graphArea.bottom
        acceptedButtons: Qt.LeftButton

        property bool dragging: false
        property bool resizing: false
        property double pressedX: -1
        property double pivotTime: 0

        onPressedChanged: {
            if(!pressed) {
                // property 초기화
                if(!dragging && !resizing) {
                    //  drag or resize를 하지 않았으면 범위 선택 초기화
                    selectionStartTime = 0
                    selectionEndTime = 0
                }

                dragging = false
                resizing = false
                pressedX = -1
                pivotTime = 0

                timelineScrollTimer.stop()
            }
        }

        // Mouse 이동에 따라 범위 재선택 or 리사이즈
        // dragging 중이면 기존 선택 범위를 초기화 하고 범위 재선택
        // resizing 중이면 기존 선택 범위의 축을 유지한 상태로 리사이즈
        onMouseXChanged: {
            if(pressedX == -1)
                pressedX = mouseX

            if(!resizing && !dragging && rangeAreaView.rangeRectX - 4 <= mouseX && mouseX <= rangeAreaView.rangeRectX + 4) {
                resizing = true
                pivotTime = selectionEndTime
                return
            }
            else if(!resizing && !dragging && rangeAreaView.rangeRectX + rangeAreaView.rangeRectWidth - 4 <= mouseX
                    && mouseX <= rangeAreaView.rangeRectX + rangeAreaView.rangeRectWidth + 4) {
                resizing = true
                pivotTime = selectionStartTime
                return
            }
            else if(pivotTime == 0) {
                pivotTime = convertPositionToMsec(mouseX)
                seekBarView.currentTime = pivotTime
            }

            if(!resizing && !dragging && Math.abs(pressedX - mouseX) >= 1) {
                dragging = true
                selectionStartTime = pivotTime
                selectionEndTime = 0
            }

            if(!dragging && !resizing)
                return

            if(mouseX >= 0 && mouseX <= width) {
                timelineScrollTimer.stop()

                var time = convertPositionToMsec(mouseX)
                seekBarView.currentTime = time

                if(time < pivotTime) {
                    selectionStartTime = time
                    selectionEndTime = pivotTime
                }
                else {
                    selectionStartTime = pivotTime
                    selectionEndTime = time
                }
            }
            else {
                timelineScrollTimer.restart()
            }
        }

        Timer {
            id: timelineScrollTimer
            interval: 10; running: false; repeat: true
            onTriggered: {
                var time = scrollToPosition(parent.mouseX)
                if(time !== 0) {
                    seekBarView.currentTime = time
                    if(time < parent.pivotTime) {
                        selectionStartTime = time
                        selectionEndTime = parent.pivotTime
                    }
                    else {
                        selectionStartTime = parent.pivotTime
                        selectionEndTime = time
                    }
                }
            }
        }
    }

    // Timeline Drag & Flick
    MouseArea {
        width: parent.width
        anchors.top: dateArea.top
        anchors.bottom: graphArea.bottom
        acceptedButtons: Qt.RightButton

        property bool dragging : false
        property double pressedX: -1
        property double pressedStartTime: 0
        property double velocity: 0
        property double lastX: 0

        Timer {
            id: flickVelocityTimer
            interval: 200; running: true; repeat: true
            onTriggered: {
                parent.velocity = 0
                parent.lastX = 0
            }
        }

        onDraggingChanged: {
            if(dragging) {
                cursorShape = Qt.ClosedHandCursor
                flickVelocityTimer.start()
            }
            else {
                cursorShape = Qt.ArrowCursor
                flickVelocityTimer.stop()
            }
        }

        onPressedChanged: {
            if(!pressed) {
                // Flick
                if(velocity != 0) {
                    var timeDiff = velocity / width * mediaControlModel.visibleTimeRange * 2
                    scrollUpdate(mediaControlModel.visibleStartTime + timeDiff, true)
                }

                // Show context menu
                if(!dragging) {
                    contextMenuView.showMenu(mouseX);
                }

                // property 초기화
                dragging = false
                pressedX = -1
                pressedStartTime = 0
                velocity = 0
                lastX = 0
            }
        }

        onMouseXChanged: {
            if(pressedX == -1)
                pressedX = mouseX

            if(pressedStartTime == 0)
                pressedStartTime = mediaControlModel.visibleStartTime

            if(dragging == false && Math.abs(pressedX - mouseX) >= 1)
                dragging = true

            if(lastX == 0)
                lastX = mouseX

            if(dragging && mouseX >= 0 && mouseX <= width)
            {
                // Flick 가속도 계산
                velocity += lastX - mouseX
                lastX = mouseX

                // Drag
                var timeDiff = (pressedX - mouseX) / width * mediaControlModel.visibleTimeRange
                scrollUpdate(pressedStartTime + timeDiff, false)
            }
        }

        function scrollUpdate(visibleStartTime, enableAnimation) {
            // visible time 업데이트
            mediaControlModel.visibleStartTime = visibleStartTime
            if(mediaControlModel.visibleStartTime < mediaControlModel.recordingStartTime)
                mediaControlModel.visibleStartTime = mediaControlModel.recordingStartTime

            mediaControlModel.visibleEndTime = mediaControlModel.visibleStartTime + mediaControlModel.visibleTimeRange
            if(mediaControlModel.visibleEndTime > mediaControlModel.recordingEndTime) {
                mediaControlModel.visibleEndTime = mediaControlModel.recordingEndTime
                mediaControlModel.visibleStartTime = mediaControlModel.visibleEndTime - mediaControlModel.visibleTimeRange
            }

            // timeline 스크롤
            customAnimation = enableAnimation
            timelineStartTime = mediaControlModel.visibleStartTime
            timelineEndTime = mediaControlModel.visibleEndTime

            redraw(timelineStartTime, timelineEndTime)

            // 스크롤바 스크롤
            scrollAreaView.scrollUpdate(enableAnimation)
        }
    }

    // Date Area
    Rectangle {
        id: dateArea
        width: parent.width
        height: 18
        anchors.top: parent.top
        color: "transparent"
        border.width: 1
        border.color: "#444444"

        DateAreaView {
            id: dateAreaView
            anchors.fill: parent
        }
    }

    // Time Area
    Rectangle {
        id: timeArea
        width: parent.width
        anchors.top: dateArea.bottom
        anchors.bottom: graphArea.top
        color: "black"
        border.width: 1
        border.color: "#444444"

        TimeAreaView {
            id: timeAreaView
            anchors.fill: parent
        }
    }

    // Graph Area
    Rectangle {
        id: graphArea
        width: parent.width
        height: 18
        anchors.bottom: scrollArea.top
        color: "transparent"
        border.width: 1
        border.color: "#444444"

        GraphAreaView {
            id: graphAreaView
            anchors.fill: parent
        }
    }

    // Scroll Area
    Rectangle {
        id: scrollArea
        width: parent.width
        height: 18
        anchors.bottom: parent.bottom
        color: "black"

        ScrollAreaView {
            id: scrollAreaView
            anchors.fill: parent
        }
    }

    // Range Area
    Rectangle {
        id: rangeArea
        anchors.top: parent.top
        anchors.bottom: scrollArea.top
        width: parent.width
        color: "transparent"

        RangeAreaView {
            id: rangeAreaView
            anchors.fill: parent
        }
    }

    // Context Menu
    ContextMenuView {
        id: contextMenuView
    }

    // Seek Bar
    SeekBarView {
        id: seekBarView
    }
}
