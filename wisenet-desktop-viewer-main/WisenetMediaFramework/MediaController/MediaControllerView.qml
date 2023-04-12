import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Wisenet.MediaController 1.0
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"

// 미디어 컨트롤러의 Main View
Item {
    id: mediaController
    state: "show"
    visible: width > showHideButton.width
    enabled: (userGroupModel && userGroupModel.playback) || supportDuration || supportTimeline  // Playback 권한이 있거나, 동영상 파일일 때만 전체 UI 활성화

    Component.onCompleted: {
        // S1 버전이고 simpleTimeline이나 file전용이 아니면, Default로 MultiTimelineMode 적용
        if(versionManager.s1Support && !simpleTimeline && !isMediaFileOnly)
            mediaControlModel.isMultiTimelineMode = true
    }

    signal timelineVisibleTimeChanged(var startTime, var endTime);  // 타임라인 표시범위 변경 signal. Zoom이나 Drag&Drop 시 emit.
    signal selectedBookmarkIdChanged(var bookmarkId);   // 선택 북마크 변경 signal. 타임라인에서 북마크 아이콘 선택 시 emit.
    signal selectedTrackIdChanged(var trackId)  // 선택 trackId 변경 signal
    signal exportProgressView_Open()
    signal selectedTimelineUpdated()

    function openBookmarkPopup(bookMarkId) {
        bookmarkModel.id = bookMarkId
        bookmarkPopup.editBookmark = true
        bookmarkPopup.show()
    }

    function setSelectedBookmarkId(bookMarkId) {
        if(bookmarkModel.id !== bookMarkId)
            bookmarkModel.id = bookMarkId
    }

    function addEventBookmark(deviceID, channelID, eventLogID, startTime, endTime) {
        bookmarkModel.newBookmark(deviceID, channelID, -1, startTime, endTime, eventLogID)
        bookmarkPopup.setThumbanilImage()
        bookmarkPopup.editBookmark = false
        bookmarkPopup.show()
    }

    function openBookmarkExportVideoPopup(bookmarkId) {
        bookmarkModel.id = bookmarkId
        showExportVideoPopup(bookmarkModel.startTime, bookmarkModel.endTime,
                             [bookmarkModel.deviceId + "_" + bookmarkModel.channelId + "_" + bookmarkModel.trackId])
    }

    function showExportVideoPopup(from, to, channelList) {
        if(to > from + 24*3600000)
            to = from + 24*3600000
        exportPopupView.exportVideoModel.fromDateTime = new Date(from)
        exportPopupView.exportVideoModel.toDateTime = new Date(to)

        var multiModeOlny = channelList.length !== 0    // 사용자 선택 채널이 하나라도 있으면 multiMode로만 동작 (viewingGridItem을 참조하지 않음)
        exportPopupView.multiModeOlny = multiModeOlny

        if(multiModeOlny) {
            exportPopupView.exportVideoModel.setMultiChannelList(channelList)
        }
        else {
            var playerList = []
            for(var gridItem of selectedViewingGrid.gridItems) {
                if(gridItem.hasPlayer && gridItem.isCamera) {
                    playerList.push(gridItem.player)
                }
            }
            exportPopupView.exportVideoModel.setMultiChannelPlayerList(playerList)
        }

        if(loginViewModel.exportAuthenticated) {
            exportPopupView.show()
        }
        else {
            exportAuthenticationPopup.show()
        }
    }

    function deleteBookmark(bookmarkID) {
        bookmarkModel.id = bookmarkID
        bookmarkDeleteConfirmPopup.show()
    }

    function showAndHide(show) {
        if (show && mediaController.state != "show")
        {
            mediaController.state = "show"
            baseArea.visible = true;
            showHideToolTip.text = WisenetLinguist.hideTimeline
            timelineControlView.redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
        }
        else if(!show && mediaController.state == "show")
        {
            mediaController.lastHeight = mediaController.height
            mediaController.state = "hide"
            baseArea.visible = false;
            showHideToolTip.text = WisenetLinguist.showTimeline
        }
    }

    function checkSelectionRangeLimit(rangeLimit) {
        if(selectionEndTime - selectionStartTime > rangeLimit) {
            var endTime = selectionStartTime + rangeLimit
            selectionEndTime = endTime > mediaControlModel.timelineEndTime ? mediaControlModel.timelineEndTime : endTime
        }
    }

    function setIsLinkedMode(linkedMode) {
        console.log("setIsLinkedMode", linkedMode)
        if(isLinkedMode === linkedMode)
            return

        isLinkedMode = linkedMode

        if(isLinkedMode) {
            if(isMediaFileOnly)
                linkAllMediaFiles()
            else
                linkAllCamera()
        }
    }

    function setIsThumbnailMode(thumbnailMode) {
        console.log("setIsThumbnailMode", thumbnailMode)
        if(isThumbnailMode === thumbnailMode)
            return

        isThumbnailMode = thumbnailMode

        if(state == "hide" || mediaControlModel.isMultiTimelineMode)
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

    function setIsMultiTimelineMode(multiTimelineMode) {
        console.log("setIsMultiTimelineMode", multiTimelineMode)
        if(isMultiTimelineMode === multiTimelineMode)
            return

        mediaControlModel.isMultiTimelineMode = multiTimelineMode

        if(state == "hide" || isThumbnailMode)
            return  // isThumbnailMode 이면 높이 조정을 하지 않음

        if(isMultiTimelineMode) {
            mediaController.height = 100 + additionalHeight
        }
        else {
            additionalHeight = mediaController.height - 100
            mediaController.height = 100
        }
    }

    function setTimelineHeight(timelineHeight) {
        console.log("setTimelineHeight", timelineHeight)
        lastHeight = (isThumbnailMode || isMultiTimelineMode) ? timelineHeight : 100
        additionalHeight = timelineHeight - 100

        if(state == "show" && (isThumbnailMode || isMultiTimelineMode)) {
            mediaController.height = lastHeight
        }
    }

    property bool simpleTimeline: false
    property var selectedViewingGrid: null
    property var focusedViewingGridItem: null
    property bool focusedItemIsCamera: focusedViewingGridItem && focusedViewingGridItem.isCamera
    property double focusedMediaPlayPosition: !focusedViewingGridItem ? 0 :
                                              !supportDuration ? focusedViewingGridItem.mediaPlayPosition
                                                               : focusedViewingGridItem.mediaPlayPosition + mediaControlModel.recordingStartTime
    property bool firstMediaReceived: false // focusedViewingGridItem이 바뀐 후, focusedMediaPlayPosition의 변경을 체크하기 위한 flag

    property bool supportDuration: {
        if(focusedViewingGridItem && focusedViewingGridItem.hasPlayer)
            return focusedViewingGridItem.player.supportDuration
        else
            return false
    }
    property bool supportTimeline: {
        if(focusedViewingGridItem && focusedViewingGridItem.hasPlayer)
            return focusedViewingGridItem.player.supportTimeline
        else
            return false
    }
    property bool supportDateDisplay: focusedItemIsCamera || supportTimeline
    property bool isMediaFileOnly: false
    property alias timelineStartTime: mediaControlModel.timelineStartTime
    property alias timelineEndTime: mediaControlModel.timelineEndTime
    property alias selectionStartTime: timelineControlView.selectionStartTime
    property alias selectionEndTime: timelineControlView.selectionEndTime
    property bool isSmartSearchMode: false // SmartSearch Mode로 타임라인 색상 및 필터 기능 동작
    property bool isLinkedMode: true   // 동기화 재생 모드
    property bool isThumbnailMode: false    // 타임라인 썸네일 표시 모드
    property real lastHeight: 100   // showHide 및 isThumbnailMode, isMultiTimelineMode on/off에 따라 복귀 할 컨트롤의 height
    property real additionalHeight: 120 // 섬네일이나 멀티타임라인으로 추가되는 높이
    property bool heightChanging: showHideAnimation.running || resizeMouseArea.pressed
    property alias resizeMousePressed: resizeMouseArea.pressed
    property alias isMultiTimelineMode: mediaControlModel.isMultiTimelineMode
    property double selectedMaskStartTime: 0
    property double selectedMaskEndTime: 0
    property bool enableSpeedControl: true

    onFocusedViewingGridItemChanged: {
        firstMediaReceived = false  // focusedMediaPlayPosition 변경 flag 초기화
        selectFocusedItem()
    }

    onFocusedMediaPlayPositionChanged: {
        if(!firstMediaReceived) {
            // focusedViewingGridItem 변경 후 focusedMediaPlayPosition 변경을 체크. Pause 상태에서도 변경 됨.
            //console.log("onFocusedMediaPlayPositionChanged VisibleTime", new Date(focusedMediaPlayPosition))
            firstMediaReceived = true
            scrollToCenter()
        }
    }

    onSupportDurationChanged: {
        var item = focusedViewingGridItem
        if(supportDuration) {
            mediaControlModel.getLocalResourceTimeline(item.player);
        }
    }

    onSupportTimelineChanged: {
        var item = focusedViewingGridItem
        if(supportTimeline) {
            mediaControlModel.getLocalResourceTimeline(item.player);
        }
    }

    onSelectedViewingGridChanged: {
        if(selectedViewingGrid != null && isLinkedMode !== selectedViewingGrid.isLinkedMode)
            isLinkedMode = selectedViewingGrid.isLinkedMode
    }

    onIsLinkedModeChanged: {
        if(selectedViewingGrid != null && isLinkedMode !== selectedViewingGrid.isLinkedMode)
            selectedViewingGrid.isLinkedMode = isLinkedMode
    }

    onSelectedMaskStartTimeChanged: {
        timelineControlView.redraw(timelineStartTime, timelineEndTime)
    }

    onSelectedMaskEndTimeChanged: {
        timelineControlView.redraw(timelineStartTime, timelineEndTime)
    }

    // 데이터 모델들
    MediaControlModel {
        id: mediaControlModel

        onIsMultiTimelineModeChanged: {
            if(!isMultiTimelineMode) {
                // single timeline으로 전환 시 Focused 채널 재설정 -> 싱글 타임라인 갱신
                selectFocusedItem()
                mediaController.refreshCalendar(true)
            }
            else {
                // multi timeline으로 전환 시 현재 재생 날짜 재선택 -> 타임라인 스크롤
                if(supportDateDisplay && focusedMediaPlayPosition != 0)
                    calendarControl.updateSelectedDate(new Date(focusedMediaPlayPosition))
            }
        }

        onSelectedTimelineUpdated: mediaController.selectedTimelineUpdated()
    }

    TimeAreaModel {
        id: timeModel
    }

    DateAreaModel {
        id: dateAreaModel
    }

    GraphAreaModel {
        id: graphAreaModel
        onTimelineUpdated: {
            var videoItem = focusedViewingGridItem
            if(!videoItem || !videoItem.hasPlayer || videoItem.isLocalResource)
                return

            requestTimelineThumbnail(additionalHeight + 4)

            /* 전체 Overlapped Timeline 통합으로 주석처리
            if(!videoItem.mediaParam.isLive && videoItem.mediaParam.trackId !== -1 && mediaControlModel.selectedOverlappedID != -1 &&
                    videoItem.mediaParam.trackId !== mediaControlModel.selectedOverlappedID) {
                // 새로 선택 된 OverlappedID의 타임라인 범위로 Zoom In
                var rangeDelta = lastRecordTime - firstRecordTime - mediaControlModel.visibleTimeRange  // 표시 범위 변화량
                var startTimeDelta = firstRecordTime - mediaControlModel.visibleStartTime    // 표시 시작 시간 변화량

                if(mediaControlModel.visibleTimeRange + rangeDelta <= 10000) {
                    rangeDelta = 10000 - mediaControlModel.visibleTimeRange
                    startTimeDelta = firstRecordTime - (10000 - lastRecordTime + firstRecordTime) / 2 - mediaControlModel.visibleStartTime
                }

                timelineControlView.animationDuration = 500;
                mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)

                // Overlapped ID를 변경하여 MediaOpen을 재수행 하기 위해 firstRecordTime으로 Seek
                requestSeek(firstRecordTime)
            }
            */
        }
    }

    GraphAreaModel {
        id: smartSearchGraphModel
        isSmartSearchMode: mediaController.isSmartSearchMode
        smartSearchTypeFilter: [
            SmartSearchResultType.Pass,
            SmartSearchResultType.Motion,
            SmartSearchResultType.Enter,
            SmartSearchResultType.Exit
        ]
    }

    MultiGraphAreaModel {
        id: multiGraphAreaModel
    }

    BookmarkModel {
        id: bookmarkModel
    }

    ThumbnailModel {
        id: thumbnailModel
    }

    BookmarkPopupView {
        id: bookmarkPopup
    }

    WisenetMessageDialog {
        id: bookmarkDeleteConfirmPopup
        width: 449
        height: 257
        applyButtonText: WisenetLinguist.ok

        onApplyButtonClicked: {
            bookmarkModel.removeBookmark(bookmarkModel.id)
        }

        Rectangle {
            anchors.fill: parent
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            color: WisenetGui.contrast_09_dark
            //z: 1
            Text {
                id: nameText
                font.pixelSize: 14
                color: WisenetGui.contrast_00_white
                anchors.top: parent.top
                anchors.topMargin: 26
                anchors.left: parent.left
                anchors.leftMargin: 35
                anchors.right: parent.right
                anchors.rightMargin: 35
                horizontalAlignment: lineCount == 1 ? Text.AlignHCenter : Text.AlignLeft
                wrapMode: Text.Wrap
                text: WisenetLinguist.deleteBookmark + " " + bookmarkModel.name
            }
            Text {
                height: 14
                font.pixelSize: 12
                color: WisenetGui.color_accent
                anchors.top: nameText.bottom
                anchors.topMargin: 31
                anchors.horizontalCenter: parent.horizontalCenter
                text: WisenetLinguist.deleteWarning
            }
        }
    }

    ExportVideoPopupView {
        id: exportPopupView
    }

    ExportAuthenticationPopup {
        id: exportAuthenticationPopup
    }

    // Seek할 위치에 녹화 데이터가 없으면 가까운 Record 검색하여 seek하는 함수
    function requestSeektoRecordExist(seekTime) {
        var item = focusedViewingGridItem
        if(!item || !item.isVideoItem)
            return

        var overlappedId = getValidOverlappedId(seekTime)
        if(overlappedId !== mediaControlModel.selectedOverlappedID) {
            // seek 할 위치의 overlappedId가 기존과 다르면 자동 변경
            mediaControlModel.selectedOverlappedID = overlappedId
            selectedTrackIdChanged(mediaControlModel.selectedOverlappedID)
        }

        // seek할 위치를 찾아서 seek 요청
        var isForward = focusedViewingGridItem.mediaPlaySpeed >= 0
        var ignoreRecordTypeFilter = !controlButtonAreaView.autoskipEnabled
        var validSeekTime = graphAreaModel.getValidSeekTime(mediaControlModel.selectedOverlappedID,
                                                   seekTime, isForward, ignoreRecordTypeFilter)
        if(validSeekTime === 0 && mediaControlModel.isMultiTimelineMode && focusedItemIsCamera) {
            // singleTimeline에서 validSeekTime 획득 실패 시 multiTimeline에서 한번 더 검색 (singleTimeline 로딩 전인 경우)
            validSeekTime = multiGraphAreaModel.getValidSeekTime(item.mediaParam.deviceId, item.mediaParam.channelId, mediaControlModel.selectedOverlappedID,
                                                                 seekTime, isForward, ignoreRecordTypeFilter)
        }

        if(validSeekTime === 0)
            return 0

        requestSeek(validSeekTime)

        return validSeekTime
    }

    function requestSeek(pos) {
        var item = focusedViewingGridItem
        if(!item || !item.isVideoItem)
            return

        var overlappedIdChanged = item.mediaParam.trackId !== mediaControlModel.selectedOverlappedID

        timelineControlView.seekbarTime = pos   // seek한 위치로 seekbar를 미리 옮김
        if(supportDuration)
            pos -= mediaControlModel.recordingStartTime

        controlButtonAreaView.updateAutoskipTime(pos)   // Seek 시 updateAutoskipTime

        selectedViewingGrid.mediaSeek(item, pos, mediaControlModel.selectedOverlappedID);

        // overlappedId를 변경하여 seek한 후 타임라인 redraw
        if(overlappedIdChanged) {
            timelineControlView.redraw(timelineControlView.timelineStartTime, timelineControlView.timelineEndTime)
        }
    }

    function requestSpeed(speed) {
        var item = focusedViewingGridItem
        if(!item || !item.isVideoItem || !item.mediaSeekable)
            return

        var oldSpeed = item.mediaPlaySpeed;

        selectedViewingGrid.mediaSetSpeed(item, speed);

        if(oldSpeed > 0 && speed < 0 || oldSpeed < 0 && speed > 0)
            controlButtonAreaView.updateAutoskipTime(focusedMediaPlayPosition)   // 진행 방향 변경 시 updateAutoskipTime
        else if(!controlButtonAreaView.autoskipEnabled)
            controlButtonAreaView.updateAutoskipTime(focusedMediaPlayPosition)   // autoskip이 아닐 때는 속도 변경 마다 updateAutoskipTime
    }

    function requestPause() {
        var item = focusedViewingGridItem
        if(!item || !item.isVideoItem || !item.mediaSeekable)
            return

        selectedViewingGrid.mediaPause(item);
    }

    function requestPlay() {
        var item = focusedViewingGridItem
        if(!item || !item.isVideoItem || !item.mediaSeekable)
            return

         selectedViewingGrid.mediaPlay(item);
    }

    function requestStep(isForward) {
        var item = focusedViewingGridItem
        if(!item || !(item.isCamera || item.isLocalResource) || !item.mediaSeekable)
            return

        selectedViewingGrid.mediaStep(item, isForward);
    }

    function changeCameraStream(toLive) {
        var selectedOverlappedId = focusedViewingGridItem.mediaParam.trackId
        if(selectedOverlappedId === -1)
            selectedOverlappedId = mediaControlModel.getDefaultOverlappedId(focusedViewingGridItem.mediaParam.deviceId,
                                                                    focusedViewingGridItem.mediaParam.channelId)

        var playTime = mediaControlModel.getLiveToPlaybackTime(focusedViewingGridItem.mediaParam.deviceId,
                                                               focusedViewingGridItem.mediaParam.channelId,
                                                               selectedOverlappedId,
                                                               focusedMediaPlayPosition)
        // 선택 타일의 StreamType 전환
        if(toLive) {
            // Live로 전환
            focusedViewingGridItem.player.changeCameraStream(WisenetMediaParam.LiveAuto, "", 0, 0, -1);
        }
        else {
            // Playback으로 전환
            focusedViewingGridItem.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", playTime, 0, selectedOverlappedId);
            scrollToCenter(playTime)
        }

        // 전체 카메라 재생 동기화
        if(isLinkedMode) {
            linkAllCamera(focusedViewingGridItem, playTime)
        }
    }

    function linkAllCamera(targetItem = null, playTime = 0) {
        if(!isLinkedMode)
            return  // linked Mode가 아니면 리턴

        if(!targetItem)
            targetItem = selectedViewingGrid.getLinkedModeTargetItem("") // 지정한 targetItem이 없으면 자동 선택

        if(!targetItem)
            return  // targetItem을 찾을 수 없으면 리턴 (viewingGrid에 camrea가 없는 경우)

        if(playTime === 0)
            playTime = targetItem.mediaPlayPosition
        var toLive = targetItem.mediaParam.isLive
        var speed = targetItem.mediaPlaySpeed
        var paused = targetItem.player.playbackState === WisenetMediaPlayer.PausedState
        var selectedOverlappedId = targetItem.mediaParam.trackId
        if(selectedOverlappedId === -1)
            selectedOverlappedId = mediaControlModel.getDefaultOverlappedId(targetItem.mediaParam.deviceId, targetItem.mediaParam.channelId)

        // 전체 채널의 Live/Playback 상태 동기화
        for(var gridItem of selectedViewingGrid.gridItems) {
            if(!gridItem.isCamera)
                continue    // gridItem이 카메라가 아닌 경우

            if(toLive) {
                // Live로 전환
                if(gridItem.mediaParam.isLive)
                    continue    // 이미 Live인 경우

                gridItem.player.changeCameraStream(WisenetMediaParam.LiveAuto, "", 0, 0, -1);
            }
            else {
                // Playback으로 전환
                if(!gridItem.mediaParam.isLive)
                    continue    // 이미 Playback인 경우

                var overlappedId = selectedOverlappedId
                if(targetItem.mediaParam.deviceId !== gridItem.mediaParam.deviceId) {
                    // 선택 타일과 다른 장비면 DefaultOverlappedId 다시 획득
                    overlappedId = gridItem.mediaParam.trackId
                    if(overlappedId === -1)
                        overlappedId = mediaControlModel.getDefaultOverlappedId(gridItem.mediaParam.deviceId, gridItem.mediaParam.channelId)
                }

                gridItem.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", playTime, 0, overlappedId, speed, paused);
            }
        }

        // Playback 전환인 경우 재생시점 동기화 수행
        if(!toLive) {
            selectedViewingGrid.linkAllVideoChannels(targetItem, playTime)
        }
    }

    function linkAllMediaFiles() {
        if(!isLinkedMode)
            return  // linked Mode가 아니면 리턴

        selectedViewingGrid.linkAllMediaFiles()
    }

    // ShowHide 상태
    states: [
        State {
            name: "show"
            PropertyChanges {
                target: mediaController
                height: mediaController.lastHeight
            }
        },
        State {
            name: "hide"
            PropertyChanges {
                target: mediaController
                height: 0
            }
        }
    ]

    // ShowHide Animation
    Behavior on height {
        enabled: !resizeMouseArea.pressed
        NumberAnimation {
            id: showHideAnimation
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }

    // ShowHide Button
    Item {
        id: showHideButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.top
        width: 30
        height: 18
        visible: selectedViewingGrid? selectedViewingGrid.gridLineVisible : true
        Rectangle {
            anchors.fill: parent
            color: WisenetGui.color_live_control_bg
            opacity: showHideButtonMouseArea.containsMouse ? 1.0 : 0.7
        }

        Image{
            width: 18
            height: 18
            anchors.centerIn: parent
            sourceSize: "18x18"
            source: {
                if(showHideButtonMouseArea.pressed) {
                    if (mediaController.state =="hide")
                        return WisenetImage.mediaController_open_press
                    else
                        return WisenetImage.mediaController_fold_press
                }
                else if(showHideButtonMouseArea.containsMouse) {
                    if (mediaController.state =="hide")
                        return WisenetImage.mediaController_open_over
                    else
                        return WisenetImage.mediaController_fold_over
                }
                else {
                    if (mediaController.state == "hide")
                        return WisenetImage.mediaController_open_default
                    else
                        return WisenetImage.mediaController_fold_default
                }
            }
        }

        MouseArea {
            id: showHideButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked:
            {
                if (mediaController.state == "hide") {
                    showAndHide(true)
                }
                else {
                    showAndHide(false)
                }
            }
        }

        WisenetMediaToolTip {
            id: showHideToolTip
            parent: showHideButtonMouseArea
            visible: showHideButtonMouseArea.containsMouse
            delay: 1000
            text: WisenetLinguist.hideTimeline
        }
    }

    // Calendar
    CalendarControlView {
        id: calendarControl
        anchors.bottom: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 15
        visible: false
        Component.onCompleted: {
            calendar.minimumDate = new Date(mediaControlModel.recordingStartTime)
            calendar.maximumDate = new Date() //today
        }
    }

    RecordTypeFilterView {
        id: recordTypeFilter
        x: 38
        y: -height
    }

    SmartSearchFilterView {
        id: smartSearchFilterView
        x: 38
        y: -height
    }

    // Context Menu
    ContextMenuView {
        id: contextMenuView
    }

    Rectangle {
        id: baseArea
        anchors.fill: parent
        color: WisenetGui.contrast_10_dark

        Behavior on visible  {
            NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
        }

        MouseArea {
            // for block viewingGrid mouse action
            anchors.fill: parent
            acceptedButtons: Qt.MiddleButton | Qt.LeftButton | Qt.RightButton
            onWheel: {}
            onClicked: {
                focus = true
            }
        }

        Rectangle {
            id: controlButtonArea
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 32
            color: WisenetGui.transparent

            ControlButtonAreaView {
                id: controlButtonAreaView
                anchors.fill: parent
            }

            Rectangle {
                width: parent.width
                height: 1
                color: WisenetGui.contrast_10_dark
                anchors.bottom: parent.bottom
            }
        }

        Rectangle {
            id: timelineArea
            anchors.top: controlButtonArea.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            color: WisenetGui.transparent

            TimelineControlView {
                id: timelineControlView
                anchors.fill: parent
            }
        }
    }

    MouseArea {
        id: resizeMouseArea
        height: 4
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: -2
        acceptedButtons: Qt.LeftButton
        cursorShape: enabled ? Qt.SizeVerCursor : Qt.ArrowCursor
        enabled: mediaController.state == "show" && (isThumbnailMode || mediaControlModel.isMultiTimelineMode)

        property real pressedY: 0

        onPressedChanged: {
            if(pressed)
                pressedY = mouseY
        }

        onMouseYChanged: {
            var newHeight = mediaController.height + pressedY - mouseY
            if(newHeight > 340)
                newHeight = 340
            else if(newHeight < 220)
                newHeight = 220

            mediaController.height = newHeight

            requestTimelineThumbnail(newHeight - 96)
        }
    }

    // keyboard shortcut actions
    function actionPlayPause() {
        controlButtonAreaView.actionPlayPause()
    }

    function actionBackward() {
        controlButtonAreaView.actionBackward()
    }

    function actionForward() {
        controlButtonAreaView.actionForward()
    }

    function actionZoomIn() {
        timelineControlView.actionZoomIn()
    }

    function actionZoomOut() {
        timelineControlView.actionZoomOut()
    }

    function actionPrevEvent() {
        controlButtonAreaView.actionPrevEvent()
    }

    function actionNextEvent() {
        controlButtonAreaView.actionNextEvent()
    }

    // eventContextMenu
    function showEventSearchView() {
        windowService.eventSearchView_SetVisible(true)
        windowService.eventSearchView_ResetFilter();

        if(focusedItemIsCamera) {
            var channelGuids = [];
            channelGuids.push(focusedViewingGridItem.mediaParam.deviceId + "_" + focusedViewingGridItem.mediaParam.channelId)
            windowService.eventSearchView_SetChannelFilter(channelGuids)
        }

        if(timelineControlView.selectionStartTime !== 0 && timelineControlView.selectionEndTime !== 0) {
            windowService.eventSearchView_SetDateFilter(new Date(timelineControlView.selectionStartTime), new Date(timelineControlView.selectionEndTime))
        }
        else {
            var today = new Date()
            var fromDate = new Date(today.getFullYear(), today.getMonth(), today.getDate(), 0, 0, 0)
            var toDate = new Date(today.getFullYear(), today.getMonth(), today.getDate(), 23, 59, 59)
            windowService.eventSearchView_SetDateFilter(fromDate, toDate)
        }

        windowService.eventSearchView_SearchByFilter();
    }

    function showEventLogView() {
        windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.EventLog)
        windowService.setupMainView_SearchEventLog()
    }

    function getDefaultOverlappedId(deviceId, channelId) {
        return mediaControlModel.getDefaultOverlappedId(deviceId, channelId)
    }

    function setSmartSearchResult(resultList) {
        smartSearchGraphModel.setSmartSearchResult(resultList)
    }

    function requestTimelineThumbnail(height) {
        if(!isThumbnailMode || !focusedItemIsCamera)
            return

        var item = focusedViewingGridItem
        thumbnailModel.refreshTimelineThumbnail(item.mediaParam.deviceId, item.mediaParam.channelId, mediaControlModel.selectedOverlappedID,
                                                mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime, timelineControlView.graphWidth, height)
    }

    function getHashPassword(userGuid, password) {
        return mediaControlModel.getHashPassword(userGuid, password)
    }

    function openExportProgressPopup() {
        exportProgressView_Open()
    }

    function refreshCalendar(recordMapOnly) {
        if(!selectedViewingGrid)
            return

        var deviceChannelList = []
        if(mediaControlModel.isMultiTimelineMode) {
            // MultiTimelineMode 이면 전체 viewingGridItem 기준으로 캘린더의 선택 가능 날짜와 녹화가 있는 날짜 갱신
            for(var item of selectedViewingGrid.gridItems) {
                if(item.isCamera) {
                    deviceChannelList.push(item.mediaParam.deviceId + "_" + item.mediaParam.channelId)
                }
            }
        }
        else if(focusedItemIsCamera && focusedViewingGridItem.mediaParam !== undefined){
            // SingleTimelineMode이면 선택 된 채널의 녹화 날짜 갱신
            recordMapOnly = true
            deviceChannelList.push(focusedViewingGridItem.mediaParam.deviceId + "_" + focusedViewingGridItem.mediaParam.channelId)
        }

        // 녹화가 있는 날짜 초기화
        calendarControl.calendar.calendarTimelineModel.clearRecordMap()

        // 캘린더 갱신 요청 (RecordingPeriod와 CalendarSearch 수행)
        mediaControlModel.refreshCalendar(deviceChannelList, calendarControl.calendar.visibleYear, calendarControl.calendar.visibleMonth+1, recordMapOnly)
    }

    function selectFocusedItem() {
        var item = focusedViewingGridItem
        if(!item) {
            mediaControlModel.setSelectedChannel("", "", -1, 0)
            thumbnailModel.thumbnailCap = false
            return
        }

        if (item.isCamera){
            mediaControlModel.setSelectedChannel(item.mediaParam.deviceId, item.mediaParam.channelId, item.mediaParam.trackId, item.mediaPlayPosition)
            thumbnailModel.checkThumbnailCap(item.mediaParam.deviceId)  // 선택 된 item의 thumbnail cap 체크
        }
        else {
            if (item.isLocalResource && item.hasPlayer) {
                mediaControlModel.getLocalResourceTimeline(item.player)
            }
            else {
                mediaControlModel.setSelectedChannel("", "", -1, 0)
            }
            thumbnailModel.thumbnailCap = false
        }
    }

    function getValidOverlappedId(time) {
        // time 위치의 overlappedIdList에 현재 선택 된 OverlappedId가 있는지 확인
        var overlappedIdList = graphAreaModel.getOverlappedIdList(time)
        if(overlappedIdList.length === 0 && mediaControlModel.isMultiTimelineMode && focusedItemIsCamera) {
            // singleTimeline에서 List 획득 실패 시 multiTimeline에서 한번 더 검색 (singleTimeline 로딩 전인 경우)
            var item = focusedViewingGridItem
            overlappedIdList = multiGraphAreaModel.getOverlappedIdList(item.mediaParam.deviceId, item.mediaParam.channelId, time)
            //console.log("getValidOverlappedId() multiGraphAreaModel.getOverlappedIdList =", overlappedIdList)
        }

        for(var overlappedId of overlappedIdList) {
            if(overlappedId === mediaControlModel.selectedOverlappedID) {
                return overlappedId // 기존 OverlappedId가 List에 있는 경우
            }
        }

        if(overlappedIdList.length > 0) {
            return overlappedIdList[0]  // 기존 OverlappedId가 List에 없는 경우
        }

        return mediaControlModel.selectedOverlappedID
    }

    function scrollToCenter(playTime = 0) {
        if(!focusedViewingGridItem || !focusedViewingGridItem.hasPlayer || focusedViewingGridItem.mediaParam.isLive)
            return  // Playback 중이 아니면 리턴

        // 현재 재생 시간이 화면 가운데 오도록 스크롤
        if(playTime === 0) {
            // supportDuration(mkv 등의 범용 동영상 파일)이면 시간값 보정 (1970년으로 스크롤 되는 것을 방지)
            playTime = !focusedViewingGridItem.player.supportDuration ? focusedViewingGridItem.mediaPlayPosition
                                                                      : focusedViewingGridItem.mediaPlayPosition + mediaControlModel.recordingStartTime
        }
        if(playTime <= 0) {
            return
        }

        var newStartTime = playTime - (mediaControlModel.visibleTimeRange / 2)        
        var startTimeDelta = newStartTime - mediaControlModel.visibleStartTime
        mediaControlModel.setVisibleTimeRange(startTimeDelta, 0)
        //console.log("scrollToCenter() VisibleTime", new Date(playTime), new Date(newStartTime))
    }
}
