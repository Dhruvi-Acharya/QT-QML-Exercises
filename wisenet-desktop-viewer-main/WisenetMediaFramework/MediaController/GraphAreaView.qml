import QtQuick 2.15
import Wisenet.MediaController 1.0
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"
import "qrc:/"

// 타임라인 Record Graph 표시 영역
Item {
    property alias selectedThumbnailVisible: thumbnailSelectRect.visible
    property alias selectedThumbnailPosition: thumbnailSelectRect.x
    property alias hoveredThumbnailVisible: hoveredThumbnailRect.visible
    property real graphHeight: 6

    Component.onCompleted: {
        redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
        graphAreaModel.connectMediaControlModelSignals(mediaControlModel)
        bookmarkModel.connectMediaControlModelSignals(mediaControlModel)
    }

    Connections {
        target: mediaControlModel
        function onSelectedOverlappedIDChanged() {
            redraw(timelineStartTime, timelineEndTime)
        }
    }

    Connections {
        target: graphAreaModel
        function onTimelineUpdated () {
            graphAreaModel.refreshGraphDataList(timelineStartTime, timelineEndTime, graphWidth, mediaControlModel.selectedOverlappedID)
            graphCanvas.requestPaint()
            overlappedRepeater.model = 0
            overlappedRepeater.model = graphAreaModel.overlappedDataCount
        }
    }

    Connections {
        target: smartSearchGraphModel
        function onTimelineUpdated () {
            smartSearchGraphModel.refreshGraphDataList(timelineStartTime, timelineEndTime, graphWidth, mediaControlModel.selectedOverlappedID)
            graphCanvas.requestPaint()
        }
    }

    Connections {
        target: thumbnailModel
        enabled: isThumbnailMode
        function onAdditionalThumbnailLoaded(key) {
            // 추가 섬네일 로드 signal -> 현재 표시범위 기준으로 redraw 요청
            if(mediaController.state != "show" || !focusedItemIsCamera)
                return

            var item = focusedViewingGridItem
            var currentKey = item.mediaParam.deviceId + "_" + item.mediaParam.channelId + "_" + mediaControlModel.selectedOverlappedID
            //console.log("onAdditionalThumbnailLoaded()", key, currentKey)
            if(key === currentKey) {
                thumbnailModel.refreshTimelineThumbnail(item.mediaParam.deviceId, item.mediaParam.channelId, mediaControlModel.selectedOverlappedID,
                                                        timelineStartTime, timelineEndTime, graphWidth, additionalHeight + 4)
            }
        }
    }

    Rectangle {
        id: backgroundRect
        /*
        width: parent.width
        height: 6
        anchors.bottom: parent.bottom
        */
        anchors.fill: parent
        anchors.topMargin: 4
        color: WisenetGui.contrast_08_dark_grey

        ThumbnailImage {
            width: parent.width
            height: backgroundRect.height
            anchors.bottom: parent.bottom
            stretch: true
            image: graphAreaModel.graphImage
        }
    }

    Canvas {
        id: graphCanvas
        width: parent.width
        height: backgroundRect.height
        anchors.bottom: parent.bottom
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            // graphAreaModel의 데이터를 차례대로 그림
            var index = 0
            var count = graphAreaModel.graphDataCount
            /* graph를 단일 QImage로 그리도록 수정
            for(; index < count ; index++) {
                ctx.globalAlpha = 1
                ctx.fillStyle = WisenetGui.color_secondary   // normal

                // SmartSearchMode인 경우 항상 normal 색상
                if(!isSmartSearchMode) {
                    var recordType = graphAreaModel.recordType(index)
                    //console.log("recordtype ", recordType, "event ", GraphAreaModel.Event)

                    if(recordType === GraphAreaModel.RecType_Event || recordType === GraphAreaModel.RecType_EventFiltered)
                    {
                        //console.log("eventRecord")
                        ctx.fillStyle = WisenetGui.color_accent   // event
                    }

                    if(recordType === GraphAreaModel.RecType_NormalFiltered || recordType === GraphAreaModel.RecType_EventFiltered)
                        ctx.globalAlpha = 0.3   // filtered
                }

                ctx.fillRect(graphAreaModel.graphStartPosition(index), 0, graphAreaModel.graphWidth(index), height)
            }
            */

            // smartSearchGraphModel의 데이터를 차례대로 그림
            if(isSmartSearchMode)
            {
                ctx.fillStyle = WisenetGui.color_accent   // 항상 event color
                count = smartSearchGraphModel.graphDataCount
                for(index=0 ; index < count ; index++) {
                    var recordType = smartSearchGraphModel.recordType(index)
                    ctx.globalAlpha = recordType === GraphAreaModel.RecType_EventFiltered ? 0.3 : 1
                    ctx.fillRect(smartSearchGraphModel.graphStartPosition(index), 0, smartSearchGraphModel.graphWidth(index), height)
                }
            }

            // highlight selected mask range
            if(selectedMaskStartTime != 0 && selectedMaskEndTime != 0 && selectedMaskStartTime <= selectedMaskEndTime) {
                ctx.fillStyle = WisenetGui.color_primary
                ctx.globalAlpha = 1
                var maskStatPos = convertMsecToX(selectedMaskStartTime)
                var maskEndPos = convertMsecToX(selectedMaskEndTime)
                var maskWidth = maskEndPos - maskStatPos >= 1 ? maskEndPos - maskStatPos : 1
                ctx.fillRect(maskStatPos, 0, maskWidth, height)
            }
        }
    }

    // bookmarkModel의 데이터를 차례대로 표시
    Repeater {
        id: bookmarkImageRepeater
        model: bookmarkModel
        delegate: Image {
            id: image
            x: convertMsecToX(model.startTime)
            z: model.selected ? 2 : 1
            width: 14
            height: 14
            anchors.bottom: parent.top
            anchors.bottomMargin: 3
            source: model.selected ? WisenetImage.mediaController_bookmark_clicked : WisenetImage.mediaController_bookmark
            sourceSize: { Qt.size(width, height) }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                hoverEnabled: true

                onContainsMouseChanged: {
                    if(containsMouse) {
                        bookmarkTooltip.showBookmarkTooltip(image.x + image.width/2 + sideMargin, model.name, model.description)
                    }
                    else {
                        bookmarkTooltip.visible = false
                    }
                }

                onClicked: {
                    bookmarkModel.id = model.id
                    selectedBookmarkIdChanged(model.id)

                    if(mouse.button == Qt.RightButton) {
                        bookmarkContextMenu.popup()
                    }
                }

                onDoubleClicked: {
                    if(mouse.button == Qt.LeftButton) {
                        requestSeektoRecordExist(bookmarkModel.startTime)
                    }
                }
            }
        }
    }

    Repeater {
        id: bookmarkRectRepeater
        model: bookmarkModel
        delegate: Rectangle {
            id: rect
            x: convertMsecToX(model.startTime)
            width: convertMsecToX(model.endTime) - convertMsecToX(model.startTime)
            height: backgroundRect.height
            anchors.bottom: parent.bottom
            color: WisenetGui.color_primary
            visible: model.selected
        }
    }

    Repeater {
        id: overlappedRepeater
        model: graphAreaModel.overlappedDataCount
        Image {
            x: graphAreaModel.overlappedStartPosition(index)
            width: graphAreaModel.overlappedWidth(index)
            height: backgroundRect.height
            anchors.bottom: backgroundRect.bottom
            sourceSize: Qt.size(4, 4)
            source: WisenetImage.mediaController_overlappedMask
            fillMode: Image.Tile
            horizontalAlignment: Image.AlignLeft
            verticalAlignment: Image.AlignTop
        }
    }


    //********** 타임라인 썸네일 (타임라인 표시영역 전체의 썸네일) **********
    property bool thumbnailHovered: ((isThumbnailMode && focusedItemIsCamera && (hoveredThumbnailMouseArea.containsMouse || fixedPositionMouseArea.containsMouse))
                                     || rangeAreaView.mouseHovered
                                     || seekBarView.mouseHovered)

    Rectangle {
        id: timelineThumbnailRect
        width: parent.width
        height: !isThumbnailMode ? backgroundRect.height - graphHeight
                                 : thumbnailHovered ? parent.height - graphHeight*2 : parent.height - graphHeight
        anchors.top: parent.top
        anchors.topMargin: isThumbnailMode ? 0 : backgroundRect.anchors.topMargin
        color: "#101010"

        Behavior on height {
            enabled: !mediaController.heightChanging
            NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo; }
        }

        Rectangle {
            id: clipRect
            anchors.fill: parent
            color: WisenetGui.transparent
            clip: true
            visible: thumbnailModel.thumbnailCap

            ThumbnailImage {
                id: frontThumbnail
                width: parent.width + thumbnailModel.thumbnailWidth
                height: parent.height
                x: thumbnailModel.frontThumbnailPosition
                y: 0
                image: thumbnailModel.frontThumbnail
                visible: isThumbnailMode && focusedItemIsCamera
                clip: true
            }

            ThumbnailImage {
                id: rearThumbnail
                width: parent.width + thumbnailModel.thumbnailWidth
                height: parent.height
                x: thumbnailModel.rearThumbnailPosition
                y: 0
                image: thumbnailModel.rearThumbnail
                visible: isThumbnailMode && focusedItemIsCamera
                clip: true
            }

            Rectangle {
                id: thumbnailSelectRect
                width: thumbnailModel.thumbnailWidth
                height: parent.height
                x: thumbnailModel.selectRectanglePosition
                y: 0
                visible: isThumbnailMode && focusedItemIsCamera && thumbnailModel.selectRectangleVisible && !thumbnailModel.isLoading
                color: WisenetGui.transparent
                border.width: 2
                border.color: WisenetGui.color_primary
            }
        }

        Rectangle {
            width: thumbnailSelectRect.border.width
            height: graphHeight
            x: thumbnailSelectRect.x
            y: thumbnailSelectRect.height
            color: WisenetGui.color_primary
            visible: thumbnailSelectRect.visible && x >= 0
        }

        Rectangle {
            anchors.fill: parent
            color: WisenetGui.contrast_12_black
            opacity: hoveredThumbnailRect.visible || thumbnailModel.isLoading ? 0.8 : 0
            visible: thumbnailModel.thumbnailCap

            Behavior on opacity {
                NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo; }
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: timelineControlView.dragging ? Qt.ClosedHandCursor : Qt.ArrowCursor
            enabled: isThumbnailMode && focusedItemIsCamera
            hoverEnabled: true

            onMouseXChanged: {
                thumbnailModel.mouseX = mouseX
                timelineControlView.zoomX = mouseX
            }

            onContainsMouseChanged: {
                if(!containsMouse) {
                    thumbnailModel.selectRectangleVisible = false
                }
            }
        }

        WisenetMediaBusyIndicator {
            visible: thumbnailModel.thumbnailCap && thumbnailModel.isLoading && timelineThumbnailRect.height > 100
            running: true
            anchors.centerIn: parent
            sourceWidth: 100
            sourceHeight: 100
            width: 100
            height: 100
        }

        Text {
            text: WisenetLinguist.unsupported
            visible: isThumbnailMode && !thumbnailModel.thumbnailCap
            anchors.centerIn: parent
            font.pixelSize: 18
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: WisenetGui.contrast_05_grey
            font.bold: true
        }
    }
    //********************


    //********** Hovered Thumbnail (마우스 hover 위치의 썸네일 1장) **********
    property real hoveredThumbnailTime: 0
    property real lastRefreshTime: 0

    Connections {
        target: mediaController
        function onFocusedViewingGridItemChanged() {
            // 선택 채널이 바뀌면 hoveredThumbnailTime 초기화
            hoveredThumbnailTime = 0
            if(!focusedItemIsCamera) {
                thumbnailModel.refreshHoveredThumbnail("", "", 0, 0, false)
            }
        }
    }

    onHoveredThumbnailTimeChanged: {
        if(!focusedItemIsCamera)
            return

        //console.log("onHoveredThumbnailTimeChanged", hoveredThumbnailTime)
        var refreshTime = Date.now()
        if(refreshTime - lastRefreshTime > 100) {
            // 100ms 이내에는 재요청 하지 않음
            lastRefreshTime = refreshTime
            refreshHoveredThumbnail()
        }

        hoveredThumbnailRefreshTimer.restart()
    }

    MouseArea {
        id: hoveredThumbnailMouseArea
        width: parent.width
        height: graphHeight*6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -graphHeight*2
        acceptedButtons: Qt.NoButton
        cursorShape: timelineControlView.dragging ? Qt.ClosedHandCursor : Qt.ArrowCursor
        hoverEnabled: true

        onMouseXChanged: {
            if(isThumbnailMode && focusedItemIsCamera)
                hoveredThumbnailTime = convertPositionToMsec(mouseX)
            timelineControlView.zoomX = mouseX
        }
    }

    Timer {
        id: hoveredThumbnailRefreshTimer
        interval: 100
        onTriggered: {
            // 마우스가 멈추고 100ms 지나면 마지막 좌표값으로 섬네일을 한번 더 요청
            refreshHoveredThumbnail()
        }
    }

    Rectangle {
        id: hoveredThumbnailRect
        width: thumbnailModel.thumbnailWidth + border.width*2
        height: thumbnailModel.thumbnailHeight + border.width*2
        x: convertMsecToX(hoveredThumbnailTime) - width/2
        z: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 21
        visible: thumbnailModel.hoveredThumbnailVisible && thumbnailHovered
        color: WisenetGui.transparent
        border.width: 2
        border.color: WisenetGui.color_primary
        opacity: visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo; }
        }

        Rectangle {
            x: parent.border.width
            y: parent.border.width
            width: thumbnailModel.thumbnailWidth
            height: thumbnailModel.thumbnailHeight
            color: WisenetGui.transparent

            ThumbnailImage {
                id: hoveredThumbnail
                anchors.fill: parent
                image: thumbnailModel.hoveredThumbnail
                stretch: true
            }

            Rectangle {
                width: parent.width
                height: dateTimeText.height + graphHeight
                color: WisenetGui.contrast_12_black
                opacity: 0.5
                anchors.bottom: parent.bottom
            }

            Text {
                id: dateTimeText
                width: parent.width
                font.pixelSize: 12
                font.bold: true
                color: "#D8D8D8"
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                text: {
                    var dateText = localeManager.getDateFromMsec(hoveredThumbnailTime)
                    var timeText = new Date(hoveredThumbnailTime).toLocaleTimeString(Qt.locale(), "hh:mm:ss")
                    return dateText + "  " + timeText
                }
            }
        }

        MouseArea {
            id: fixedPositionMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true
            enabled: isThumbnailMode && focusedItemIsCamera

            onClicked: {
                singleClicked(convertMsecToX(hoveredThumbnailTime))
            }
        }
    }

    Rectangle {
        width: hoveredThumbnailRect.border.width
        height: 21
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: hoveredThumbnailRect.horizontalCenter
        color: WisenetGui.color_primary
        visible: hoveredThumbnailRect.visible
    }
    //********************


    function redraw(startTime, endTime) {
        graphAreaModel.refreshGraphDataList(startTime, endTime, graphWidth, mediaControlModel.selectedOverlappedID)
        if(isSmartSearchMode)
            smartSearchGraphModel.refreshGraphDataList(startTime, endTime, graphWidth, mediaControlModel.selectedOverlappedID)

        graphCanvas.requestPaint()
        overlappedRepeater.model = 0
        overlappedRepeater.model = graphAreaModel.overlappedDataCount

        bookmarkModel.refreshBookmarkDataList(startTime, endTime, graphWidth)

        if(isThumbnailMode && focusedItemIsCamera) {
            var item = focusedViewingGridItem
            thumbnailModel.refreshTimelineThumbnail(item.mediaParam.deviceId, item.mediaParam.channelId, mediaControlModel.selectedOverlappedID,
                                                    startTime, endTime, graphWidth, additionalHeight + 4)
        }
    }

    function convertMsecToX(time) {
        var visibleTime = timelineControlView.timelineEndTime - timelineControlView.timelineStartTime
        var timeGap = time - timelineControlView.timelineStartTime
        var x = timeGap / visibleTime * graphWidth
        return x
    }

    function refreshHoveredThumbnail() {
        if(!focusedItemIsCamera)
            return

        var item = focusedViewingGridItem
        var ovelappedId = getValidOverlappedId(hoveredThumbnailTime)

        thumbnailModel.refreshHoveredThumbnail(item.mediaParam.deviceId, item.mediaParam.channelId, ovelappedId,
                                               hoveredThumbnailTime, isThumbnailMode)
    }
}
