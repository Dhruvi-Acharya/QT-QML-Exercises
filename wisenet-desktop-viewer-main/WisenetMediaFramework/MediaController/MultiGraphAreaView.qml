import QtQuick 2.15
import QtQuick.Controls 2.15
import Wisenet.MediaController 1.0
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"
import "qrc:/"

// Multi 타임라인 Record Graph 표시 영역
Item {
    id: itemRoot
    property real graphHeight: 10

    Component.onCompleted: {
        multiGraphAreaModel.selectedDateChanged(calendarControl.calendar.selectedDate)
        redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
    }

    Connections {
        target: mediaControlModel

        function onIsMultiTimelineModeChanged() {
            if(mediaControlModel.isMultiTimelineMode) {
                // multi timeline으로 전환 시 전체 아이템 갱신
                refreshLayoutItems()
            }
        }

        function onCalendarSelectedDateChanged() {
            // 선택 한 날짜를 전달하여 타임라인 추가 로드
            multiGraphAreaModel.selectedDateChanged(mediaControlModel.calendarSelectedDate)
        }

        function onRecordingTypeFilterChanged(filter) {
            multiGraphAreaModel.recordingTypeFilterChanged(filter)
            redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
        }
    }

    Connections {
        target: mediaController

        function onSelectedViewingGridChanged() {
            refreshLayoutItems()
        }

        function onFocusedViewingGridItemChanged() {
            if(!focusedItemIsCamera)
                return;

            // 선택 채널 변경 시, 스크롤 할 index를 찾기 위해 model로 전달
            multiGraphAreaModel.focusedItemChanged(focusedViewingGridItem.mediaParam.deviceId, focusedViewingGridItem.mediaParam.channelId)
        }
    }

    Connections {
        target: selectedViewingGrid

        function onItemsAdded(addedItems) {
            // viewingGrid Item 추가 시 멀티 타임라인에 추가
            //console.log("MultiGraphAreaView.onItemsAdded() count:", addedItems.length)
            //multiGraphAreaModel.addLayoutItems(addedItems, calendarControl.calendar.selectedDate)
            //refreshCalendar(false)
            refreshLayoutItems()
        }

        function onItemsRemoved(removedItemIds) {
            // viewingGrid Item 삭제 시 멀티 타임라인에서 삭제
            //console.log("MultiGraphAreaView.onItemsRemoved()", removedItemIds)
            //multiGraphAreaModel.removeLayoutItems(removedItemIds)
            //refreshCalendar(false)
            refreshLayoutItems()
        }

        function onSavedLayoutLoaded() {
            // 저장 된 레이아웃 로드 signal -> 전체 아이템 갱신
            refreshLayoutItems()
        }
    }

    Connections {
        target: multiGraphAreaModel

        function onTimelineUpdated(deviceId, channelIdList) {
            // 신규 타임라인 획득 완료 signal -> 해당 채널의 타임라인을 다시 그림
            var overlappedIdMap = Object() // item 별 overlappedId Map
            if(selectedViewingGrid) {
                for(var gridItem of selectedViewingGrid.gridItems) {
                    if(gridItem.isCamera)
                        overlappedIdMap[gridItem.itemId] = gridItem.mediaParam.trackId
                }
            }

            multiGraphAreaModel.refreshGraphDataList(timelineStartTime, timelineEndTime, graphWidth, deviceId, channelIdList, overlappedIdMap)
        }

        function onFocusedItemIndexChanged(index) {
            // index 위치의 타임라인이 화면에 표시되도록 타임라인 리스트 스크롤
            //console.log("multiGraphAreaModel.onFocusedItemIndexChanged() index :", index)
            var newContentY = index * (graphHeight + multiTimelineList.spacing)

            if(newContentY >= multiTimelineList.contentY &&
                    newContentY + graphHeight + multiTimelineList.spacing <= multiTimelineList.contentY + multiTimelineList.height) {
                return  // 현재 화면에 index의 타임라인이 이미 표시중인 경우 스크롤 하지 않고 리턴
            }

            if(newContentY >= multiTimelineList.contentHeight - multiTimelineList.height)
                newContentY = multiTimelineList.contentHeight - multiTimelineList.height    // 최대 스크롤을 넘어가지 않도록

            scrollAimation.enabled = true
            multiTimelineList.contentY = newContentY    // 스크롤 수행
            scrollAimation.enabled = false
        }
    }

    ListView {
        id: multiTimelineList
        anchors.fill: parent
        anchors.topMargin: 1
        clip: true
        spacing: 1
        interactive: false
        ScrollBar.vertical: WisenetScrollBar {}

        model: multiGraphAreaModel
        delegate: Rectangle {
            id: backgroundRect
            width: multiTimelineList.width - sideMargin
            height: graphHeight
            color: WisenetGui.contrast_08_dark_grey

            property string deviceId: model.deviceId
            property string channelId: model.channelId
            property bool isfocusedItem: focusedItemIsCamera
                                         && focusedViewingGridItem.mediaParam.deviceId === deviceId
                                         && focusedViewingGridItem.mediaParam.channelId === channelId
            property var graphImage: model.graphImage
            property int overlapCount: model.overlappedBlockCount
            property int modelIndex: index

            onGraphImageChanged: {
                // graphImage가 바뀌면 overlap 리스트를 다시 그림
                overlapCount = 0
                overlapCount = model.overlappedBlockCount
            }

            ThumbnailImage {
                anchors.fill: parent
                stretch: false
                image: model.graphImage
            }

            Repeater {
                model: backgroundRect.overlapCount
                Image {
                    width: multiGraphAreaModel.getOverlappedBlockWidth(backgroundRect.modelIndex, index)
                    height: parent.height
                    x: multiGraphAreaModel.getOverlappedBlockPos(backgroundRect.modelIndex, index)
                    sourceSize: Qt.size(4, 4)
                    source: WisenetImage.mediaController_overlappedMask
                    fillMode: Image.Tile
                    horizontalAlignment: Image.AlignLeft
                    verticalAlignment: Image.AlignTop
                }
            }

            Rectangle {
                width: 150
                height: parent.height
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: "#99000000" }
                    //GradientStop { position: 0.25; color: WisenetGui.contrast_08_dark_grey }
                    GradientStop { position: 1.0; color: WisenetGui.transparent }
                }

                Text {
                    text: model.channelName
                    color: WisenetGui.contrast_00_white
                    x: graphHeight
                    width: 110
                    height: parent.height
                    font.pixelSize: 10
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight

                    Rectangle {
                        width: parent.contentWidth
                        height: parent.height
                        color: WisenetGui.transparent

                        WisenetMediaToolTip {
                            visible: tooltipMouseArea.containsMouse
                            text: model.channelName
                        }

                        MouseArea {
                            id: tooltipMouseArea
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton
                            hoverEnabled: true
                        }
                    }
                }
            }

            Rectangle {
                anchors.fill: parent
                anchors.topMargin: -1
                anchors.bottomMargin: -1
                visible: parent.isfocusedItem
                color: WisenetGui.transparent
                border.width: 2
                border.color: WisenetGui.color_primary
            }
        }

        Behavior on contentY {
            id: scrollAimation
            enabled: false
            NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo; }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: WisenetGui.contrast_12_black
        opacity: multiGraphAreaModel.isLoading ? 0.8 : 0

        Behavior on opacity {
            NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo; }
        }
    }

    WisenetMediaBusyIndicator {
        visible: multiGraphAreaModel.isLoading && parent.height >= 100
        running: true
        anchors.centerIn: parent
        sourceWidth: 100
        sourceHeight: 100
        width: 100
        height: 100
    }

    function redraw(startTime, endTime) {
        // 전체 채널 타임라인 redraw
        var overlappedIdMap = Object() // item 별 overlappedId Map
        if(selectedViewingGrid) {
            for(var gridItem of selectedViewingGrid.gridItems) {
                if(gridItem.isCamera)
                    overlappedIdMap[gridItem.itemId] = gridItem.mediaParam.trackId
            }
        }

        multiGraphAreaModel.refreshGraphDataList(startTime, endTime, graphWidth, "", [], overlappedIdMap)
    }

    function refreshLayoutItems() {
        // 전체 아이템 갱신 함수
        if(!selectedViewingGrid || !mediaControlModel.isMultiTimelineMode)
            return

        var layoutItemList = []

        for(var gridItem of selectedViewingGrid.gridItems) {
            if(!gridItem.isCamera)
                continue    // gridItem이 카메라가 아닌 경우

            layoutItemList.push(gridItem.itemModel)
        }

        multiGraphAreaModel.refreshLayoutItems(layoutItemList, calendarControl.calendar.selectedDate)
        refreshCalendar(false)
    }


    function mouseClicked(mouseY) {
        // 해당 좌표의 타임라인을 찾아 선택 및 Focuse이동
        var y = mouseY - multiTimelineList.anchors.topMargin + multiTimelineList.contentY
        var item = multiTimelineList.itemAt(0, y)
        if(item) {
            //console.log("multitimeline.mouseClicked() deviceId :",item.deviceId, "channelId :", item.channelId)
            selectedViewingGrid.changeFocusedItem(item.deviceId, item.channelId)
        }
    }

    function convertMsecToX(time) {
        var visibleTime = timelineControlView.timelineEndTime - timelineControlView.timelineStartTime
        var timeGap = time - timelineControlView.timelineStartTime
        var x = timeGap / visibleTime * graphWidth
        return x
    }
}
