import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import Wisenet.MediaController 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"

Item {
    height: 32

    Component.onCompleted: {
        playbackSpeedModel.connectMediaControlModelSignals(mediaControlModel)
        if(mediaControlModel.isMultiTimelineMode) {
            mediaController.height = 100 + additionalHeight
        }
    }

    property double controlMargin: 26
    property bool controlEnabled: focusedViewingGridItem && focusedViewingGridItem.mediaSeekable
    property bool autoskipEnabled: autoskipButton.autoskip

    // 입력 시간 기준으로, 현재 block의 끝 시간과 다음 block의 시작 시간을 갱신하는 함수
    // autoSkip 중이면 record 단위로 동작, autoSkip이 아니면 OverlappedId 단위로 동작
    function updateAutoskipTime(time) {
        var isForward = focusedViewingGridItem.mediaPlaySpeed >= 0
        var overlappedId = isSmartSearchMode && autoskipButton.autoskip ? -1 : mediaControlModel.selectedOverlappedID

        if(isSmartSearchMode && autoskipButton.autoskip) {
            autoskipButton.currentBlockEndTime = smartSearchGraphModel.getCurrentRecordBlockEndTime(time, isForward)
        }
        else {
            if(autoskipButton.autoskip)
                autoskipButton.currentBlockEndTime = graphAreaModel.getCurrentRecordBlockEndTime(time, isForward, false, overlappedId)
            else
                autoskipButton.currentBlockEndTime = graphAreaModel.getCurrentOverlappedIdTime(isForward, overlappedId)
        }

        console.log("updateAutoskipTime() currentBlockEndTime :", autoskipButton.currentBlockEndTime)

        if(autoskipButton.currentBlockEndTime !== 0) {
            time = autoskipButton.currentBlockEndTime
            //time = isForward ? autoskipButton.currentBlockEndTime + 1 : autoskipButton.currentBlockEndTime - 1

            // I-Frame 재생인 경우 overlappedId 녹화 구간 끝에 도달할 수 없는 경우가 있음
            // 배속에 따라 끝 시간 보정
            var speed =  focusedViewingGridItem.mediaPlaySpeed
            if(!autoskipButton.autoskip && speed !== 1 && speed !== 2) {
                var iFrameGap = speed > 0 ? 1 : -1  // 배속 별 다음 I-Frmae과의 시간차 (초)
                if(Math.abs(speed) >= 32) {
                    iFrameGap = iFrameGap * Math.abs(speed) / 16
                }
                autoskipButton.currentBlockEndTime -= iFrameGap * 2000
            }
        }

        if(isSmartSearchMode && autoskipButton.autoskip) {
            autoskipButton.nextBlockStartTime = smartSearchGraphModel.getNextRecordBlockTime(time, isForward, isForward)
        }
        else {
            if(autoskipButton.autoskip)
                autoskipButton.nextBlockStartTime = graphAreaModel.getNextRecordBlockTime(time, isForward, isForward, false, overlappedId)
            else
                autoskipButton.nextBlockStartTime = graphAreaModel.getNextOverlappedIdTime(time, isForward, overlappedId)
        }

        console.log("updateAutoskipTime() nextBlockStartTime :", autoskipButton.nextBlockStartTime)
    }

    function changeAutoskipMode(autoSkip) {
        if(autoskipButton.autoskip !== autoSkip)
            autoskipButton.autoskip = autoSkip
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: contextMenuView.showMenu(mouseX)
    }

    component BaseToggleButton: Button{
        flat: true
        width: visible ? 20 : 0
        height: 20
        anchors.verticalCenter: parent.verticalCenter
        hoverEnabled: true
        WisenetMediaToolTip {
            delay: 1000
            visible: parent.hovered
            text: parent.ToolTip.text
        }
        background: Rectangle {
            color: WisenetGui.transparent
        }
    }

    // Background Rect
    Rectangle {
        anchors.fill: parent
        color: WisenetGui.contrast_08_dark_grey
    }

    // Thumbnail Button
    BaseToggleButton {
        id: thumbnailButton
        anchors.left: parent.left
        anchors.leftMargin: 15
        ToolTip.text: isThumbnailMode ? WisenetLinguist.hideThumbnail : WisenetLinguist.showThumbnail
        enabled: !mediaControlModel.isMultiTimelineMode
        visible: {
            if(isMediaFileOnly)
                return false
            if(!simpleTimeline)
                return true
            // dateText와 timeText가 잘리지 않는 width가 되면 표시
            var newCenterRectWidth = playPauseButton.width*5 + controlMargin*5 + 205
            var neededWidth = dateText.contentWidth + timeText.contentWidth + 60
            if(calendarButton.visible)
                neededWidth += 29
            if(filterButton.visible)
                neededWidth += 29
            return (parent.width - newCenterRectWidth) / 2 > neededWidth
        }

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(!thumbnailButton.enabled)
                    return isThumbnailMode ? WisenetImage.mediaController_thumbnail_Disable_On : WisenetImage.mediaController_thumbnail_Disable
                else if(thumbnailButton.hovered)
                    return WisenetImage.mediaController_thumbnail_Hover
                else if(isThumbnailMode)
                    return WisenetImage.mediaController_thumbnail_Select
                else
                    return WisenetImage.mediaController_thumbnail_Default
            }
        }

        onClicked: {
            isThumbnailMode = !isThumbnailMode

            if(mediaControlModel.isMultiTimelineMode)
                return  // isMultiTimelineMode 이면 높이 조정을 하지 않음

            if(isThumbnailMode) {
                mediaController.height = 100 + additionalHeight
                requestTimelineThumbnail(additionalHeight + 4)
            }
            else {
                additionalHeight = mediaController.height - 100
                mediaController.height = 100
            }
        }
    }

    // MultiTimeline Button
    BaseToggleButton {
        id: multiTimelineButton
        anchors.left: thumbnailButton.right
        anchors.leftMargin: visible && thumbnailButton.visible ? 9 : 0
        visible: !isMediaFileOnly && !simpleTimeline
        enabled: !focusedViewingGridItem || !focusedViewingGridItem.isLocalResource // isLocalResource이면 비활성화
        ToolTip.text: mediaControlModel.isMultiTimelineMode ? WisenetLinguist.switchToSingleTimeline : WisenetLinguist.switchToMultiTimeline

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(!multiTimelineButton.enabled)
                    return WisenetImage.mediaController_multiTimeline_Disable
                else if(multiTimelineButton.hovered)
                    return WisenetImage.mediaController_multiTimeline_Hover
                else if(mediaControlModel.isMultiTimelineMode)
                    return WisenetImage.mediaController_multiTimeline_Select
                else
                    return WisenetImage.mediaController_multiTimeline_Default
            }
        }

        onClicked: {
            mediaControlModel.isMultiTimelineMode = !mediaControlModel.isMultiTimelineMode

            if(isThumbnailMode)
                return  // isThumbnailMode 이면 높이 조정을 하지 않음

            if(mediaControlModel.isMultiTimelineMode) {
                mediaController.height = 100 + additionalHeight
            }
            else {
                additionalHeight = mediaController.height - 100
                mediaController.height = 100
            }
        }

        Connections {
            target: mediaController
            function onFocusedViewingGridItemChanged() {
                // 동영상 파일(wnm, mkv, etc.)이 선택되면 multiTimelineMode 종료
                var item = focusedViewingGridItem
                if(!item)
                    return

                if (mediaControlModel.isMultiTimelineMode && item.isLocalResource && item.hasPlayer)
                    multiTimelineButton.clicked()
            }
        }
    }

    // Filter Button
    BaseToggleButton {
        id: filterButton
        anchors.left: multiTimelineButton.right
        anchors.leftMargin: visible && (thumbnailButton.visible || multiTimelineButton.visible) ? 9 : 0
        visible: !isMediaFileOnly && (!simpleTimeline || isSmartSearchMode)
        enabled: focusedItemIsCamera
        ToolTip.text: WisenetLinguist.recordTypeFilter

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(!filterButton.enabled)
                    return WisenetImage.mediaController_filter_Disable
                else if(filterButton.hovered)
                    return WisenetImage.mediaController_filter_Hover
                else if(recordTypeFilter.visible || smartSearchFilterView.visible)
                    return WisenetImage.mediaController_filter_Select
                else
                    return WisenetImage.mediaController_filter_Default
            }
        }

        onClicked: {
            if(isSmartSearchMode)
                smartSearchFilterView.open()
            else
                recordTypeFilter.open()

            if(calendarControl.visible)
                calendarControl.visible = false
        }
    }

    // Calendar Button
    BaseToggleButton {
        id: calendarButton
        anchors.left: filterButton.right
        anchors.leftMargin: visible && (thumbnailButton.visible || multiTimelineButton.visible || filterButton.visible) ? 9 : 0
        visible: !simpleTimeline || isSmartSearchMode
        enabled: supportDateDisplay
        ToolTip.text: calendarControl.visible ? WisenetLinguist.hideCalendar : WisenetLinguist.showCalendar

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(!calendarButton.enabled)
                    return WisenetImage.calendar_Disable
                else if(calendarButton.hovered)
                    return WisenetImage.calendar_Hover
                else if(calendarControl.visible)
                    return WisenetImage.calendar_Select
                else
                    return WisenetImage.calendar_Default
            }
        }

        onClicked: {
            calendarControl.visible = !calendarControl.visible
        }

        onEnabledChanged: {
            if(!enabled)
                calendarControl.visible = false // 버튼 disable 되면 캘린더를 닫음
        }
    }

    // AutoSkip Button
    Button {
        id: autoskipButton
        flat: true
        width: 24
        height: 24
        hoverEnabled: true
        background: Rectangle {
            color: WisenetGui.transparent
        }
        Image {
            anchors.fill: parent
            source: parent.autoskip ? WisenetImage.mediaController_autoskip_s : WisenetImage.mediaController_autoskip_n
        }
        visible: false

        property double mediaPosition: focusedMediaPlayPosition
        property double mediaPlaySpeed: (focusedViewingGridItem && focusedViewingGridItem.mediaPlaySpeed) ? focusedViewingGridItem.mediaPlaySpeed : 0
        property bool autoskip: false
        property double currentBlockEndTime: 0
        property double nextBlockStartTime: 0

        onClicked: {
            autoskip = !autoskip
        }

        onAutoskipChanged: {
            updateAutoskipTime(mediaPosition)
        }

        onMediaPositionChanged: {
            if(!focusedViewingGridItem || !focusedViewingGridItem.mediaSeekable || mediaPlaySpeed == 0 || nextBlockStartTime == 0)
                return

            if(currentBlockEndTime === 0 ||
                    (mediaPlaySpeed > 0 && mediaPosition >= currentBlockEndTime) ||
                    (mediaPlaySpeed < 0 && mediaPosition <= currentBlockEndTime))
            {
                console.log("AutoSkip triggered. mediaPosition :", mediaPosition,
                            "currentBlockEndTime :", currentBlockEndTime,
                            "nextBlockStartTime :", nextBlockStartTime)

                if(mediaControlModel.selectedOverlappedID !== graphAreaModel.nextRecordOverlappedId) {
                    // overlappedId 변경이 필요 한 경우 처리
                    mediaControlModel.selectedOverlappedID = graphAreaModel.nextRecordOverlappedId
                    mediaControllerView.selectedTrackIdChanged(mediaControlModel.selectedOverlappedID)
                }

                requestSeek(nextBlockStartTime) // seek 수행
            }
        }
    }

    Rectangle {
        id: datetimeRect
        width: centerRect.x - (calendarButton.x + calendarButton.width) - 18
        height: parent.height
        visible: focusedMediaPlayPosition !== 0 && (!simpleTimeline || width > timeText.contentWidth)
        anchors.left: calendarButton.right
        anchors.leftMargin: 9
        anchors.right: centerRect.left
        anchors.rightMargin: 9
        color: WisenetGui.transparent

        Text {
            id: dateText
            visible: supportDateDisplay && (!simpleTimeline || parent.width > contentWidth + timeText.contentWidth + 6)
            width: visible ? contentWidth : 0
            height: 14
            anchors.top: timeEditor.top
            anchors.topMargin: -1.5
            verticalAlignment: Text.AlignVCenter
            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 12
            text: localeManager.getDateFromMsec(focusedMediaPlayPosition)

            MouseArea {
                enabled: calendarButton.visible && calendarButton.enabled
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                hoverEnabled: true
                onClicked: {
                    calendarControl.visible = !calendarControl.visible
                }

                WisenetMediaToolTip {
                    delay: 1000
                    visible: parent.containsMouse
                    text: calendarControl.visible ? WisenetLinguist.hideCalendar : WisenetLinguist.showCalendar
                }
            }

            onTextChanged: {
                // 날짜 표시를 지원하지 않거나, 멀티 타임라인이 아닌 경우 별도 처리 없음
                if(!supportDateDisplay || focusedMediaPlayPosition === 0 || !mediaControlModel.isMultiTimelineMode)
                    return

                // 영상 날짜와 캘린더 선택 날짜가 다른지 체크
                calendarControl.updateSelectedDate(new Date(focusedMediaPlayPosition))
            }
        }

        Text {
            id: timeText
            height: 14
            anchors.left: dateText.right
            anchors.leftMargin: dateText.visible ? 6 : 0
            anchors.top: timeEditor.top
            anchors.topMargin: -1.5
            verticalAlignment: Text.AlignVCenter
            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 12
            text: new Date(focusedMediaPlayPosition).toLocaleTimeString(Qt.locale(), "hh:mm:ss");

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                onClicked: {
                    timeEditor.visible = true
                    timeText.visible = false
                }
            }
        }

        Rectangle {
            width: timeText.width
            height: 1
            color: WisenetGui.contrast_06_grey
            anchors.left: timeEditor.left
            anchors.bottom: timeEditor.bottom
        }

        WisenetTimeEditor {
            id: timeEditor
            visible: false
            anchors.left: dateText.right
            anchors.leftMargin: dateText.visible ? 6 : 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            width: timeText.width
            buttonVisible: false
            leftPadding: 0

            onVisibleChanged: {
                if(visible) {
                    var datetime = new Date(focusedMediaPlayPosition)
                    setTimeText(datetime.getHours(), datetime.getMinutes(), datetime.getSeconds())
                }
            }

            onControlFocusChanged: {
                if(!controlFocus) {
                    timeText.visible = true
                    timeEditor.visible = false

                    var datetime = new Date(focusedMediaPlayPosition)
                    datetime.setHours(timeEditor.hour)
                    datetime.setMinutes(timeEditor.minute)
                    datetime.setSeconds(timeEditor.second)
                    requestSeektoRecordExist(datetime.getTime())
                }
            }
        }
    }

    component BaseMediaControlButton: Button{
        flat: true
        width: visible ? 22 : 0
        height: 16
        anchors.verticalCenter: parent.verticalCenter
        enabled: controlEnabled
        hoverEnabled: true
        WisenetMediaToolTip {
            delay: 1000
            visible: parent.hovered
            text: parent.ToolTip.text
        }
        background: Rectangle {
            color: WisenetGui.transparent
        }
    }

    Rectangle {
        id: centerRect
        color: WisenetGui.transparent
        anchors.centerIn: parent
        height: parent.height
        width: previousEventButton.width + backwardButton.width + playPauseButton.width + forwardButton.width + nextEventButton.width
               + speedSlider.width + speedComboBox.width + totalMargin

        property real totalMargin: {
            if(speedSlider.visible) {
                return controlMargin * 5 + speedComboBox.anchors.leftMargin
            }
            else if(nextEventButton.visible) {
                return controlMargin * 5
            }
            else if(speedComboBox.visible) {
                return controlMargin * 3
            }
            else {
                return controlMargin * 2
            }
        }
    }

    BaseMediaControlButton {
        id: previousEventButton
        anchors.left: centerRect.left
        enabled: controlEnabled && playPauseButton.state == "playing"
        ToolTip.text: WisenetLinguist.previousEvent
        visible: {
            if(!simpleTimeline)
                return true
            // timeText가 잘리지 않는 width가 됐을 때 표시
            var newCenterRectWidth = playPauseButton.width*5 + controlMargin*5 + 65 // 65 : speedCombobox visible width
            var neededWidth = calendarButton.x + calendarButton.width + timeText.contentWidth + 19
            return (parent.width - newCenterRectWidth) / 2 > neededWidth
        }

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(!previousEventButton.enabled)
                    return WisenetImage.mediaController_prevEvent_dim
                else if(previousEventButton.pressed)
                    return WisenetImage.mediaController_prevEvent_press
                else if(previousEventButton.hovered)
                    return WisenetImage.mediaController_prevEvent_hover
                else
                    return WisenetImage.mediaController_prevEvent_default
            }
        }

        onClicked: {
            var currentTime = focusedMediaPlayPosition
            var getStartTime = focusedViewingGridItem.mediaPlaySpeed >= 0;
            var overlappedId = !isSmartSearchMode ? mediaControlModel.selectedOverlappedID : -1
            var nextRecordTime = !isSmartSearchMode ? graphAreaModel.getNextRecordBlockTime(currentTime, false, getStartTime, false, overlappedId)
                                                    : smartSearchGraphModel.getNextRecordBlockTime(currentTime, false, getStartTime)

            if(nextRecordTime !== 0) {
                if(overlappedId !== graphAreaModel.nextRecordOverlappedId) {
                    // overlappedId 변경이 필요 한 경우 처리
                    mediaControlModel.selectedOverlappedID = graphAreaModel.nextRecordOverlappedId
                    mediaControllerView.selectedTrackIdChanged(mediaControlModel.selectedOverlappedID)
                }
                requestSeek(nextRecordTime) // seek 수행
            }
        }
    }

    BaseMediaControlButton {
        id: backwardButton
        visible: !simpleTimeline || parent.width > 120
        anchors.left: previousEventButton.right
        anchors.leftMargin: previousEventButton.visible ? controlMargin : 0
        state: "previousFrame"
        ToolTip.text: WisenetLinguist.previousFrame

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(backwardButton.state == "speedDown") {
                    if(!backwardButton.enabled)
                        return WisenetImage.mediaController_speedBackward_dim
                    else if(backwardButton.pressed)
                        return WisenetImage.mediaController_speedBackward_press
                    else if(backwardButton.hovered)
                        return WisenetImage.mediaController_speedBackward_hover
                    else
                        return WisenetImage.mediaController_speedBackward_default
                }
                else {
                    if(!backwardButton.enabled)
                        return WisenetImage.mediaController_stepBackward_dim
                    else if(backwardButton.pressed)
                        return WisenetImage.mediaController_stepBackward_press
                    else if(backwardButton.hovered)
                        return WisenetImage.mediaController_stepBackward_hover
                    else
                        return WisenetImage.mediaController_stepBackward_default
                }
            }
        }

        onClicked: {
            if(backwardButton.state == "speedDown") {
                var newVal = Math.floor(speedSlider.value) - 1
                if(newVal < speedSlider.from || speedSlider.speed > 0)
                    newVal = -1

                speedSlider.value = newVal
                requestSpeed(speedSlider.valueToSpeed(newVal))
                speedToolTip.show(speedToolTip.text, 3000)
            }
            else {
                // previousFrame
                requestStep(false)
            }
        }

        states: [
            State {
                name: "previousFrame"
                PropertyChanges {
                    target: backwardButton
                    ToolTip.text: WisenetLinguist.previousFrame
                }
            },
            State {
                name: "speedDown"
                PropertyChanges {
                    target: backwardButton
                    ToolTip.text: WisenetLinguist.fastBackward
                }
            }
        ]
    }

    BaseMediaControlButton {
        id: playPauseButton
        anchors.left: backwardButton.right
        anchors.leftMargin: controlMargin
        state: !focusedViewingGridItem || !focusedViewingGridItem.hasPlayer
               || focusedViewingGridItem.player.playbackState === WisenetMediaPlayer.PausedState
               ? "paused" : "playing"

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(playPauseButton.state == "playing") {
                    if(!playPauseButton.enabled)
                        return WisenetImage.mediaController_pause_dim
                    else if(playPauseButton.pressed)
                        return WisenetImage.mediaController_pause_press
                    else if(playPauseButton.hovered)
                        return WisenetImage.mediaController_pause_hover
                    else
                        return WisenetImage.mediaController_pause_default
                }
                else {
                    if(!playPauseButton.enabled)
                        return WisenetImage.mediaController_play_dim
                    else if(playPauseButton.pressed)
                        return WisenetImage.mediaController_play_press
                    else if(playPauseButton.hovered)
                        return WisenetImage.mediaController_play_hover
                    else
                        return WisenetImage.mediaController_play_default
                }
            }
        }

        onClicked: {
            if(state == "playing") {
                requestPause()
                speedSlider.value = 0
            }
            else if(state == "paused") {
                requestPlay()
                speedSlider.value = speedSlider.speedToValue(speedSlider.speed)
            }

            speedToolTip.show(speedToolTip.text, 3000)
        }

        states: [
            State { name: "playing" },
            State { name: "paused" }
        ]

        onStateChanged: {
            if(state == "playing") {
                backwardButton.state = "speedDown"
                forwardButton.state = "speedUp"
                playPauseButton.ToolTip.text = WisenetLinguist.pause
            }
            else if(state == "paused") {
                backwardButton.state = "previousFrame"
                forwardButton.state = "nextFrame"
                playPauseButton.ToolTip.text = WisenetLinguist.play
            }
        }
    }

    BaseMediaControlButton {
        id: forwardButton
        visible: !simpleTimeline || parent.width > 120
        anchors.left: playPauseButton.right
        anchors.leftMargin: controlMargin
        state: "nextFrame"
        ToolTip.text: WisenetLinguist.nextFrame

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(forwardButton.state == "speedUp") {
                    if(!forwardButton.enabled)
                        return WisenetImage.mediaController_speedForward_dim
                    else if(forwardButton.pressed)
                        return WisenetImage.mediaController_speedForward_press
                    else if(forwardButton.hovered)
                        return WisenetImage.mediaController_speedForward_hover
                    else
                        return WisenetImage.mediaController_speedForward_default
                }
                else {
                    if(!forwardButton.enabled)
                        return WisenetImage.mediaController_stepForward_dim
                    else if(forwardButton.pressed)
                        return WisenetImage.mediaController_stepForward_press
                    else if(forwardButton.hovered)
                        return WisenetImage.mediaController_stepForward_hover
                    else
                        return WisenetImage.mediaController_stepForward_default
                }
            }
        }

        onClicked: {
            if(forwardButton.state == "speedUp"){
                var newVal = Math.ceil(speedSlider.value) + 1
                if(newVal > speedSlider.to || speedSlider.speed < 0)
                    newVal = 1

                speedSlider.value = newVal
                requestSpeed(speedSlider.valueToSpeed(newVal))
                speedToolTip.show(speedToolTip.text, 3000)
            }
            else {
                // nextFrame
                requestStep(true)
            }
        }

        states: [
            State {
                name: "nextFrame"
                PropertyChanges {
                    target: forwardButton
                    ToolTip.text: WisenetLinguist.nextFrame
                }
            },
            State {
                name: "speedUp"
                PropertyChanges {
                    target: forwardButton
                    ToolTip.text: WisenetLinguist.fastForward
                }
            }
        ]
    }

    BaseMediaControlButton {
        id: nextEventButton
        anchors.left: forwardButton.right
        anchors.leftMargin: visible ? controlMargin : 0
        enabled: controlEnabled && playPauseButton.state == "playing"
        ToolTip.text: WisenetLinguist.nextEvent
        visible: {
            if(!simpleTimeline)
                return true
            // timeText가 잘리지 않는 width가 됐을 때 표시
            var newCenterRectWidth = playPauseButton.width*5 + controlMargin*5 + 65 // 65 : speedCombobox visible width
            var neededWidth = calendarButton.x + calendarButton.width + timeText.contentWidth + 19
            return (parent.width - newCenterRectWidth) / 2 > neededWidth
        }

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(!nextEventButton.enabled)
                    return WisenetImage.mediaController_nextEvent_dim
                else if(nextEventButton.pressed)
                    return WisenetImage.mediaController_nextEvent_press
                else if(nextEventButton.hovered)
                    return WisenetImage.mediaController_nextEvent_hover
                else
                    return WisenetImage.mediaController_nextEvent_default
            }
        }

        onClicked: {
            var currentTime = focusedMediaPlayPosition
            var getStartTime = focusedViewingGridItem.mediaPlaySpeed >= 0;
            var overlappedId = !isSmartSearchMode ? mediaControlModel.selectedOverlappedID : -1
            var nextRecordTime = !isSmartSearchMode ? graphAreaModel.getNextRecordBlockTime(currentTime, true, getStartTime, false, overlappedId)
                                                    : smartSearchGraphModel.getNextRecordBlockTime(currentTime, true, getStartTime)

            if(nextRecordTime !== 0) {
                if(overlappedId !== graphAreaModel.nextRecordOverlappedId) {
                    // overlappedId 변경이 필요 한 경우 처리
                    mediaControlModel.selectedOverlappedID = graphAreaModel.nextRecordOverlappedId
                    mediaControllerView.selectedTrackIdChanged(mediaControlModel.selectedOverlappedID)
                }
                requestSeek(nextRecordTime) // seek 수행
            }
        }
    }

    // Speed Slider
    Slider {
        id: speedSlider
        hoverEnabled: true
        width: visible ? 130 : 0        
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: nextEventButton.right
        anchors.leftMargin: visible && nextEventButton.visible ? controlMargin : 0
        enabled: controlEnabled && enableSpeedControl
        focusPolicy: Qt.NoFocus
        visible: {
            if(!simpleTimeline)
                return true
            // dateText, timeText가 잘리지 않는 width가 됐을 때 표시
            var newCenterRectWidth = playPauseButton.width*5 + controlMargin*5 + 205
            var neededWidth = calendarButton.x + calendarButton.width + dateText.contentWidth + timeText.contentWidth + 25
            return (parent.width - newCenterRectWidth) / 2 > neededWidth
        }


        value: speedToValue(speed)
        from: speedToValue(-128)
        to: speedToValue(128)
        stepSize: 1

        property double speed: (focusedViewingGridItem && focusedViewingGridItem.mediaPlaySpeed) ? focusedViewingGridItem.mediaPlaySpeed : 0
        property double lastSpeed : 0

        onSpeedChanged: {
            value = speedToValue(speed)
        }

        function updateFromToValue() {
            var fromSpeed = playbackSpeedModel.maximumBackwardSpeed < -128 ? -128 : playbackSpeedModel.maximumBackwardSpeed
            from = speedToValue(fromSpeed)

            var toSpeed = playbackSpeedModel.maximumForwardSpeed > 128 ? 128 : playbackSpeedModel.maximumForwardSpeed
            to = speedToValue(toSpeed)
        }

        // Custom background
        background: Rectangle {
            x: speedSlider.leftPadding
            y: speedSlider.topPadding + speedSlider.availableHeight / 2 - height / 2
            implicitWidth: 200
            implicitHeight: 1
            width: speedSlider.availableWidth
            height: implicitHeight
            color: enabled ? WisenetGui.contrast_05_grey : WisenetGui.contrast_07_grey

            Rectangle {
                width: speedSlider.visualPosition * parent.width
                height: parent.height
                color: enabled ? WisenetGui.contrast_05_grey : WisenetGui.contrast_07_grey
            }
        }

        // Custom handle
        handle: Rectangle {
            x: speedSlider.leftPadding + speedSlider.visualPosition * (speedSlider.availableWidth - width)
            y: speedSlider.topPadding + speedSlider.availableHeight / 2 - height / 2
            implicitWidth: 12
            implicitHeight: 12
            radius: 6
            color: {
                if(!enabled)
                    return WisenetGui.contrast_07_grey
                else if(speedSlider.pressed)
                    return WisenetGui.contrast_06_grey
                else if(speedSlider.hovered)
                    return WisenetGui.contrast_00_white
                else
                    return WisenetGui.contrast_02_light_grey
            }
        }

        WisenetMediaToolTip {
            id: speedToolTip
            parent: speedSlider.handle
            visible: speedSlider.hovered | speedSlider.pressed
            text: {
                if(focusedViewingGridItem)
                    return speedToText(focusedViewingGridItem.mediaPlaySpeed)
                else
                    return speedToText(0)
            }

            function speedToText(speed) {
                if(speedSlider.value == 0)
                    return "x0"

                var absSpeed = Math.abs(speed)
                var speedString = ""

                if(absSpeed == 0.125)
                    speedString = "1/8"
                else if(absSpeed == 0.25)
                    speedString = "1/4"
                else if(absSpeed == 0.5)
                    speedString = "1/2"
                else
                    speedString = absSpeed

                if(speed < 0)
                    return "-x" + speedString;
                else
                    return "x" + speedString;
            }
        }

        onValueChanged: {
            if(pressed) {
                var currentValue = Math.floor(value)
                requestSpeed(valueToSpeed(currentValue))   // Drag에 의한 속도 변경
            }
        }

        onPressedChanged: {
            // pressed 해제 시 마지막 value로 복귀
            if(pressed) {
                var currentValue = Math.floor(value)
                lastSpeed = valueToSpeed(currentValue)
                if(lastSpeed == 0) {
                    from = speedToValue(-4)
                    to = speedToValue(4)
                }
            }
            else {
                updateFromToValue()
                if(lastSpeed == 0) {
                    requestPause()
                    speedSlider.value = 0
                }
                else {
                    requestSpeed(lastSpeed)
                }
            }
        }

        Behavior on value {
            NumberAnimation {
                duration: 300
            }
        }

        function speedToValue(speedParam) {
            if(speedParam === 0)
                return 0
            else if(speedParam < 0 && speedParam > -1)
                return -1
            else if(speedParam > 0 && speedParam < 1)
                return 1

            var returnValue = Math.log2(Math.abs(speedParam)) + 1
            if(speedParam < 0)
                return -returnValue
            else
                return returnValue
        }

        function valueToSpeed(valueParam) {
            if(valueParam === 0)
                return 0;

            var speedValue = Math.pow(2, Math.abs(valueParam) - 1)
            if(valueParam < 0)
                return -speedValue
            else
                return speedValue
        }
    }

    WisenetComboBox {
        id: speedComboBox
        width: visible ? 65 : 0
        height: 21        
        font.pixelSize: 13
        rightPadding: 1
        anchors.left: speedSlider.right
        anchors.leftMargin: speedSlider.visible ? 10 : controlMargin
        anchors.verticalCenter: parent.verticalCenter
        model: [" x256", " x128", " x64", " x32", " x16", " x8", " x4", " x2", " x1", " x1/2", " x1/4", " x1/8"]
        displayText: speed == 0 ? " x0" : currentValue
        currentIndex: playbackSpeedModel.speedToIndex(speed)
        enabled: controlEnabled && playPauseButton.state == "playing" && enableSpeedControl
        visible: {
            if(!simpleTimeline)
                return true
            // timeText가 잘리지 않는 width가 됐을 때 표시
            var newCenterRectWidth = playPauseButton.width*3 + controlMargin*3 + 65 // 65 : speedCombobox visible width
            var neededWidth = calendarButton.x + calendarButton.width + timeText.contentWidth + 19
            return (parent.width - newCenterRectWidth) / 2 > neededWidth
        }

        property double speed: (focusedViewingGridItem && focusedViewingGridItem.mediaPlaySpeed) ? focusedViewingGridItem.mediaPlaySpeed : 0
        property double lastSpeed: 0

        onSpeedChanged: {
            if((lastSpeed <= 0 && speed > 0) || (lastSpeed => 0 && speed < 0)) {
                // 재생 방향이 바뀌면 콤보박스 모델 갱신
                //console.log("speedComboBox::onSpeedChanged() lastSpeed=", lastSpeed, " speed=", speed)
                model = playbackSpeedModel.getSupportedPlaybackSpeeds(speed >= 0)
            }
            currentIndex = playbackSpeedModel.speedToIndex(speed)
            lastSpeed = speed
        }

        onActivated: {
            requestSpeed(playbackSpeedModel.indexToSpeed(currentIndex))
        }
    }

    PlaybackSpeedModel {
        id: playbackSpeedModel
        onSelectedDeviceChanged: {
            speedSlider.updateFromToValue()
            speedComboBox.model = playbackSpeedModel.getSupportedPlaybackSpeeds(speedComboBox.speed >= 0)
            speedComboBox.currentIndex = playbackSpeedModel.speedToIndex(speedComboBox.speed)
        }
    }

    Text {
        id: channelNameText
        height: 14
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: centerRect.right
        anchors.leftMargin: 9
        anchors.right: linkButton.left
        anchors.rightMargin: 9
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        color: WisenetGui.contrast_04_light_grey
        font.pixelSize: 12
        elide: Text.ElideRight
        text: (focusedViewingGridItem && focusedViewingGridItem.mediaName) ? focusedViewingGridItem.mediaName : ""
    }

    // Link Button
    BaseToggleButton {
        id: linkButton
        anchors.right: livePlybackButton.left
        anchors.rightMargin: livePlybackButton.visible ? 9 : 0
        //visible: !isMediaFileOnly
        enabled: !mediaControlModel.isMultiTimelineMode
        ToolTip.text: isLinkedMode ? WisenetLinguist.unlink : WisenetLinguist.link

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: {
                if(!linkButton.enabled)
                    return isLinkedMode ? WisenetImage.mediaController_link_Disable_linked : WisenetImage.mediaController_link_Disable
                else if(linkButton.hovered)
                    return WisenetImage.mediaController_link_Hover
                else if(isLinkedMode)
                    return WisenetImage.mediaController_link_Select
                else
                    return WisenetImage.mediaController_link_Default
            }
        }

        onClicked: {
            isLinkedMode = !isLinkedMode
            if(isLinkedMode) {
                if(isMediaFileOnly)
                    linkAllMediaFiles()
                else
                    linkAllCamera()
            }
        }

        Connections {
            target: mediaControlModel
            function onIsMultiTimelineModeChanged() {
                // multi timeline mode 에서는 link mode 고정
                if(mediaControlModel.isMultiTimelineMode && !isLinkedMode)
                    linkButton.clicked()
            }
        }
    }

    // Live / Playback
    Button {
        id: livePlybackButton
        width: visible ? 160 : 0
        height: 18
        visible: !simpleTimeline && !isMediaFileOnly
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 14
        hoverEnabled: true
        enabled: focusedItemIsCamera
        background: Rectangle {
            color: WisenetGui.transparent
        }

        state: (focusedViewingGridItem && focusedViewingGridItem.mediaSeekable) ? "Playback" : "Live"
        states: [
            State { name: "Live" },
            State { name: "Playback" }
        ]

        onClicked: {
            if(state === "Playback") {
                changeCameraStream(true)
                timelineControlView.selectionStartTime = 0  // Live 전환 시 구간 선택 초기화
            }
            else {
                changeCameraStream(false)
            }
            gc();
        }

        Image {
            anchors.fill: parent
            sourceSize: Qt.size(width, height)
            source: !enabled ? WisenetImage.mediaController_livePlayback_dim :
                               parent.state === "Playback" ? WisenetImage.mediaController_playback : WisenetImage.mediaController_live
        }

        WisenetMediaToolTip {
            delay: 1000
            visible: parent.hovered
            text: parent.state === "Playback" ? WisenetLinguist.switchToLive : WisenetLinguist.switchToPlayback
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hovered ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
    }


    // Keyboard shortcut actions
    function actionPlayPause() {
        if(playPauseButton.enabled)
            playPauseButton.clicked()
    }

    function actionBackward() {
        if(backwardButton.enabled)
            backwardButton.clicked()
    }

    function actionForward() {
        if(forwardButton.enabled)
            forwardButton.clicked()
    }

    function actionPrevEvent() {
        if(previousEventButton.enabled)
            previousEventButton.clicked()
    }

    function actionNextEvent() {
        if(nextEventButton.enabled)
            nextEventButton.clicked()
    }
}
