import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

// MediaController 내부의 타임라인 영역 Main View
Item {
    property int animationDuration: 500    // customAnimation의 duration
    property bool customAnimation: false    // customAnimation 사용 여부
    property double timelineStartTime: mediaControlModel.visibleStartTime   // customAnimation 사용 시 Easing에 의해 변경되는 타임라인 시작시간
    property double timelineEndTime: mediaControlModel.visibleEndTime       // customAnimation 사용 시 Easing에 의해 변경되는 타임라인 끝
    property double selectionStartTime: 0   // 범위 선택 시작 시간
    property double selectionEndTime: 0     // 범위 선택 끝 시간
    property alias seekbarTime: seekBarView.currentTime

    property real graphWidth: width - (sideMargin * 2)  // 좌우 Margin을 제외한 graph 영역의 width
    property real sideMargin: 10    // 좌우 Margin

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
        if(customAnimation && timelineEndTime === mediaControlModel.visibleEndTime) {
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
            rangeAreaView.rangeRectX = (selectionStartTime - mediaControlModel.visibleStartTime) / mediaControlModel.visibleTimeRange * graphWidth
            if(selectionEndTime == 0)
                rangeAreaView.rangeRectWidth = 2
            else
                rangeAreaView.rangeRectWidth = (selectionEndTime - selectionStartTime) / mediaControlModel.visibleTimeRange * graphWidth
        }
        rangeAreaView.redraw(timelineStartTime, timelineEndTime)
    }

    onSelectionEndTimeChanged: {
        if(selectionEndTime != 0) {
            rangeAreaView.rangeRectWidth = (selectionEndTime - selectionStartTime) / mediaControlModel.visibleTimeRange * graphWidth
            rangeAreaView.redraw(timelineStartTime, timelineEndTime)
        }
    }
    //

    // UI 리사이즈 시 redraw
    onWidthChanged: {
        graphWidth = width - (sideMargin * 2)
        redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
    }

    function redraw(startTime, endTime) {
        if(mediaController.state != "show")
            return

        dateAreaModel.refreshDateDataList(startTime, endTime, graphWidth)
        timeModel.updateTimeDataList(startTime, endTime, graphWidth)
        graphAreaView.redraw(startTime, endTime)
        multiGraphAreaView.redraw(startTime, endTime)
        rangeAreaView.redraw(startTime, endTime)
        seekBarView.redraw(startTime, endTime)

        scrollLeftButton.enabled = mediaControlModel.timelineStartTime < mediaControlModel.visibleStartTime
        scrollRightButton.enabled = mediaControlModel.timelineEndTime > mediaControlModel.visibleEndTime
        scrollLeftButton.updatesHighlighted()
        scrollRightButton.updatesHighlighted()
    }
    //

    // 좌표를 시간으로 변환하는 함수
    function convertPositionToMsec(pos) {
        if(pos === 0)
            return timelineStartTime

        var timeDiff = pos / graphWidth * (timelineEndTime - timelineStartTime)
        return timelineStartTime + timeDiff
    }

    // 특정 좌표에 대응되는 시간이 타임라인 시작점으로 이동하도록 타임라인을 스크롤 하는 함수
    function scrollToPosition(pos) {
        var time = 0

        if(pos < 0) {
            time = convertPositionToMsec(pos)
            if(time < mediaControlModel.timelineStartTime)
                time = mediaControlModel.timelineStartTime

            mediaControlModel.visibleStartTime = time
            mediaControlModel.visibleEndTime = time + mediaControlModel.visibleTimeRange
        }
        else if(pos > graphWidth) {
            time = convertPositionToMsec(pos)
            if(time > mediaControlModel.timelineEndTime)
                time = mediaControlModel.timelineEndTime

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

    function singleClicked(clickedX) {
        // Mouse 위치 Seek
        var seekTime = convertPositionToMsec(clickedX)
        //seekBarView.restartPositionBlockTimer()
        seekTime = requestSeektoRecordExist(seekTime)

        var startTime = seekTime - mediaControlModel.visibleTimeRange / 20
        var endTime = seekTime + mediaControlModel.visibleTimeRange / 20

        selectionStartTime = startTime < mediaControlModel.timelineStartTime ? mediaControlModel.timelineStartTime : startTime
        selectionEndTime = endTime > mediaControlModel.timelineEndTime ? mediaControlModel.timelineEndTime : endTime
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
            scrollLeftButton.enabled = mediaControlModel.timelineStartTime < mediaControlModel.visibleStartTime
            scrollRightButton.enabled = mediaControlModel.timelineEndTime > mediaControlModel.visibleEndTime
            scrollLeftButton.updatesHighlighted()
            scrollRightButton.updatesHighlighted()

            // animation 속도 초기화
            timelineControlView.animationDuration = 500;

            timelineVisibleTimeChanged(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
        }
    }

    // Date Area
    Rectangle {
        id: dateArea
        width: parent.width
        height: 20
        anchors.top: parent.top
        color: WisenetGui.contrast_08_dark_grey

        DateAreaView {
            id: dateAreaView
            anchors.fill: parent
            anchors.leftMargin: sideMargin
            anchors.rightMargin: sideMargin
        }
    }

    // Zoom In & Zoom Out
    property double zoomX: 0    // 썸네일 MouseArea에 의해 Zoom 기준 좌표가 업데이트 되지 않아, 별도 property로 마지막 좌표 업데이트

    MouseArea {
        anchors.fill: parent
        anchors.leftMargin: sideMargin
        anchors.rightMargin: sideMargin
        acceptedButtons: Qt.MiddleButton
        hoverEnabled: true

        onMouseXChanged: zoomX = mouseX

        // 타임라인 전체 영역에서, Mouse Wheel에 의한 확대 축소 동작
        onWheel: {
            if( wheel.angleDelta.y === 0 ||
                    (wheel.angleDelta.y > 0 && mediaControlModel.visibleTimeRange <= 10000) ||
                    (wheel.angleDelta.y < 0 && mediaControlModel.visibleTimeRange >= 365*24*3600*1000))
            {
                return
            }

            //hh3.kim, high resolution trackpad support for macOS
            var offset = (wheel.angleDelta.y / 120) * -0.1

            var rangeDelta = mediaControlModel.visibleTimeRange * offset  // 표시 범위 변화량
            if(rangeDelta + mediaControlModel.visibleTimeRange < 10000)
                rangeDelta = 10000 - mediaControlModel.visibleTimeRange
            else if(rangeDelta + mediaControlModel.visibleTimeRange > 365*24*3600*1000)
                rangeDelta = 365*24*3600*1000 - mediaControlModel.visibleTimeRange

            var startTimeDelta = zoomX / width * rangeDelta * -1.0    // 표시 시작 시간 변화량

            //console.log("onWheel() zoomX :", zoomX, "width :", width, "rangeDelta :", rangeDelta, "startTimeDelta :", startTimeDelta)

            mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)
        }
    }

    property double velocity: 0
    property double lastX: 0
    property double pressedStartTime: 0
    property double pressedX: -1
    property bool dragging : false

    onDraggingChanged: {
        if(dragging) {
            flickVelocityTimer.start()
        }
        else {
            flickVelocityTimer.stop()
        }
    }

    Timer {
        id: flickVelocityTimer
        interval: 100; running: true; repeat: true
        onTriggered: {
            velocity = 0
            lastX = 0
        }
    }

    // Select & Resize Range
    MouseArea {
        id: selectRangeMouseArea
        x: sideMargin
        width: parent.width - (sideMargin*2)
        anchors.top: parent.top
        anchors.bottom: scrollArea.top
        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true
        cursorShape: dragging ? Qt.ClosedHandCursor : Qt.ArrowCursor

        property bool resizing: false
        property double pivotTime: 0

        // 구간 좌우측 끝 Pressed인지 체크하여 resizing flag 업데이트
        onPressedChanged: {
            if(pressed) {
                //console.log("selectRangeMouseArea onPressedChanged, mouseX=", mouseX, " rangeRectX=", rangeAreaView.rangeRectX)
                pressedX = mouseX
                lastX = mouseX
                pressedStartTime = mediaControlModel.visibleStartTime
                resizing = false
                dragging = false

                if(rangeAreaView.leftImageX <= mouseX && mouseX <= rangeAreaView.leftImageX + rangeAreaView.leftImageWidth) {
                    resizing = true                    
                    pivotTime = selectionEndTime != 0 ? selectionEndTime : selectionStartTime
                    return
                }
                else if(rangeAreaView.rightImageX <= mouseX && mouseX <= rangeAreaView.rightImageX + rangeAreaView.rightImageWidth) {
                    resizing = true
                    pivotTime = selectionStartTime
                    return
                }
            }
            else {
                // Flick
                if(velocity != 0) {
                    var timeDiff = velocity / width * mediaControlModel.visibleTimeRange * 2
                    scrollUpdate(mediaControlModel.visibleStartTime + timeDiff, true)
                }

                if(dragging)
                    timelineVisibleTimeChanged(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)

                // property 초기화
                pressedX = -1
                pressedStartTime = 0
                velocity = 0
                lastX = 0
                pressedX = -1
                pivotTime = 0
                timelineScrollTimer.stop()
                rangeBarView.currentTime = 0
            }
        }

        // resizing 중이면 기존 선택 범위의 축을 유지한 상태로 리사이즈, resizing이 아니면 dragging
        onMouseXChanged: {
            //console.log("selectRangeMouseArea onMouseXChanged, mouseX=", mouseX, " pressedX=", pressedX)
            if(pressedX == -1)
                return

            if(resizing) {
                if(mouseX >= 0 && mouseX <= width) {
                    timelineScrollTimer.stop()

                    var time = convertPositionToMsec(mouseX)

                    if(time < pivotTime) {
                        selectionStartTime = time
                        selectionEndTime = pivotTime
                    }
                    else {
                        selectionStartTime = pivotTime
                        selectionEndTime = time
                    }

                    rangeBarView.currentTime = time
                }
                else {
                    timelineScrollTimer.restart()
                }
            }
            else {
                if(dragging == false && Math.abs(pressedX - mouseX) >= 2)
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
        }

        onClicked: {
            if (mouse.modifiers & Qt.ControlModifier) {
                // ctrl+click 시 구간 재선택
                var selectionTime = convertPositionToMsec(mouseX)
                if(selectionStartTime == 0) {
                    selectionStartTime = selectionTime
                }
                else if(selectionEndTime != 0) {
                    selectionStartTime = 0
                    selectionStartTime = selectionTime
                }
                else if(selectionStartTime > selectionTime){
                    selectionEndTime = selectionStartTime
                    selectionStartTime = selectionTime
                }
                else {
                    selectionEndTime = selectionTime
                }
            }
            else if(!resizing && !dragging) {
                // resize & dragging 하지 않은 경우만 Seek
                doubleClickTimer.clickedX = graphAreaView.selectedThumbnailVisible ? graphAreaView.selectedThumbnailPosition : mouseX
                doubleClickTimer.restart()

                if(mediaControlModel.isMultiTimelineMode && mouseY >= dateArea.height) {
                    // 멀티 타임라인 모드이면 타임라인 선택 및 Focuse이동
                    multiGraphAreaView.mouseClicked(mouseY - dateArea.height)
                }
            }

            resizing = false
            dragging = false
        }

        onDoubleClicked: {
            // DoubleClick 시 Click에 의한 Seek 동작 방지
            doubleClickTimer.stop()

            if(mouseY <= dateArea.height)
                mouse.accepted = false
            else
                mouse.accepted = true
        }

        Timer {
            id: doubleClickTimer
            interval: 300; running: false; repeat: false
            onTriggered: singleClicked(clickedX)
            property real clickedX: 0
        }

        Timer {
            id: timelineScrollTimer
            interval: 10; running: false; repeat: true
            onTriggered: {
                var time = scrollToPosition(parent.mouseX)
                if(time !== 0) {
                    //seekBarView.currentTime = time
                    if(time < parent.pivotTime) {
                        selectionStartTime = time
                        selectionEndTime = parent.pivotTime
                    }
                    else {
                        selectionStartTime = parent.pivotTime
                        selectionEndTime = time
                    }

                    rangeBarView.currentTime = time
                }
            }
        }
    }

    // Timeline Drag & Flick
    MouseArea {
        id: dragMouseArea
        x: sideMargin
        width: parent.width - (sideMargin*2)
        anchors.top: parent.top
        anchors.bottom: scrollArea.top
        acceptedButtons: Qt.RightButton
        cursorShape: dragging ? Qt.ClosedHandCursor : Qt.ArrowCursor

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
                else {
                    timelineVisibleTimeChanged(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
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

            if(dragging == false && Math.abs(pressedX - mouseX) >= 2)
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
    }

    function scrollUpdate(visibleStartTime, enableAnimation) {
        // 카메라 Item을 최대 타임라인 범위를 넘어 스크롤 할 경우, 최대 범위를 늘림
        if(focusedItemIsCamera && !isMultiTimelineMode) {
            if(mediaControlModel.timelineStartTime > visibleStartTime)
                mediaControlModel.timelineStartTime = visibleStartTime
            if(mediaControlModel.timelineEndTime < visibleStartTime + mediaControlModel.visibleTimeRange)
                mediaControlModel.timelineEndTime = visibleStartTime + mediaControlModel.visibleTimeRange
        }

        // visible time 업데이트
        mediaControlModel.visibleStartTime = visibleStartTime
        if(mediaControlModel.visibleStartTime < mediaControlModel.timelineStartTime)
            mediaControlModel.visibleStartTime = mediaControlModel.timelineStartTime

        mediaControlModel.visibleEndTime = mediaControlModel.visibleStartTime + mediaControlModel.visibleTimeRange
        if(mediaControlModel.visibleEndTime > mediaControlModel.timelineEndTime) {
            mediaControlModel.visibleEndTime = mediaControlModel.timelineEndTime
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

    // Graph Area
    Rectangle {
        id: graphArea
        width: parent.width
        //height: 30
        anchors.top: dateArea.bottom
        anchors.bottom: timeArea.top
        color: WisenetGui.transparent

        GraphAreaView {
            id: graphAreaView
            anchors.fill: parent
            anchors.leftMargin: sideMargin
            anchors.rightMargin: sideMargin
            visible: !mediaControlModel.isMultiTimelineMode
        }

        MultiGraphAreaView {
            id: multiGraphAreaView
            anchors.fill: parent
            anchors.leftMargin: sideMargin
            visible: mediaControlModel.isMultiTimelineMode
        }
    }

    // Time Area
    Rectangle {
        id: timeArea
        width: parent.width
        height: 23
        //anchors.top: graphArea.bottom
        anchors.bottom: scrollArea.top
        color: WisenetGui.transparent

        TimeAreaView {
            id: timeAreaView
            anchors.fill: parent
            anchors.leftMargin: sideMargin
            anchors.rightMargin: sideMargin
        }
    }

    // Scroll Area
    Rectangle {
        id: scrollArea
        width: parent.width
        height: 12        
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 3
        color: WisenetGui.contrast_09_dark

        property double mediaPosition: focusedMediaPlayPosition
        onMediaPositionChanged: {
            scrollLeftButton.updatesHighlighted()
            scrollRightButton.updatesHighlighted()
        }

        TimelineScrollButton {
            id: scrollLeftButton
            height: parent.height
            anchors.left: parent.left
            isLeftButton: true
        }

        TimelineScrollButton {
            id: scrollRightButton
            height: parent.height
            anchors.right: parent.right
            isLeftButton: false
            visible: true
        }

        ScrollAreaView {
            id: scrollAreaView
            height: parent.height
            anchors.left: scrollLeftButton.right
            anchors.right: scrollRightButton.left
        }
    }

    // RangeRectBackground
    Rectangle {
        x: rangeAreaView.rangeRectX + sideMargin
        width: rangeAreaView.rangeRectWidth
        height: mediaControlModel.isMultiTimelineMode ? graphArea.height - anchors.bottomMargin :
                isThumbnailMode && graphAreaView.thumbnailHovered ? 15 + graphAreaView.graphHeight : 15
        anchors.bottom: graphArea.bottom
        anchors.bottomMargin: -6
        visible: width > 0
        color: "#00D1FF"
        opacity: 0.3
    }

    // Range Area
    Rectangle {
        id: rangeArea
        x: sideMargin
        width: parent.width - (sideMargin*2)
        height: 12
        anchors.bottom: mediaControlModel.isMultiTimelineMode ? graphArea.top : graphArea.bottom
        anchors.bottomMargin: mediaControlModel.isMultiTimelineMode ? -7 : -3
        color: WisenetGui.transparent

        RangeAreaView {
            id: rangeAreaView
            anchors.fill: parent
        }
    }

    // Seek Bar
    SeekBarView {
        id: seekBarView
        anchors.bottom: mediaControlModel.isMultiTimelineMode ? graphArea.top : graphArea.bottom
        anchors.bottomMargin: mediaControlModel.isMultiTimelineMode ? -4 : 0
    }

    // Range Bar
    RangeBarView {
        id: rangeBarView
        anchors.bottom: mediaControlModel.isMultiTimelineMode ? graphArea.top : graphArea.bottom
        anchors.bottomMargin: mediaControlModel.isMultiTimelineMode ? -4 : 0
    }

    // Bookmark ToolTip
    BookmarkTooltipView {
        id: bookmarkTooltip
        anchors.bottom: dateArea.top
        anchors.bottomMargin: 2
    }

    BookmarkContextMenu {
        id: bookmarkContextMenu
    }


    // Keyboard shortcut actions
    function actionZoomIn() {
        if(mediaControlModel.visibleTimeRange <= 10000)
            return

        var rangeDelta = mediaControlModel.visibleTimeRange * -0.1  // 표시 범위 변화량
        if(rangeDelta + mediaControlModel.visibleTimeRange < 10000)
            rangeDelta = 10000 - mediaControlModel.visibleTimeRange

        var startTimeDelta = -0.5 * rangeDelta  // 표시 시작 시간 변화량

        mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)
    }

    function actionZoomOut() {
        if(mediaControlModel.visibleTimeRange >= 365*24*3600*1000)
            return

        var rangeDelta = mediaControlModel.visibleTimeRange * 0.1   // 표시 범위 변화량
        if(rangeDelta + mediaControlModel.visibleTimeRange > 365*24*3600*1000)
            rangeDelta = 365*24*3600*1000 - mediaControlModel.visibleTimeRange

        var startTimeDelta = -0.5 * rangeDelta  // 표시 시작 시간 변화량

        mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)
    }
}
