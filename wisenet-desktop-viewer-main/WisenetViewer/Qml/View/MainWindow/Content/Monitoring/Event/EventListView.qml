import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetMediaFramework 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import WisenetLanguage 1.0
import "qrc:/"

Rectangle{
    id: eventListView

    color:  WisenetGui.contrast_09_dark

    property var selectedViewingGrid: null
    property var gridItems : (selectedViewingGrid && selectedViewingGrid.gridItems) ? selectedViewingGrid.gridItems : null
    property var focusedGridItem : (selectedViewingGrid && selectedViewingGrid.focusedItem) ? selectedViewingGrid.focusedItem : null

    Connections{
        target: selectedViewingGrid ? selectedViewingGrid : null

        onItemsAdded: {
            console.log("eventListView.onItemAdded");
            gridItemUpdated()
        }

        onItemsRemoved: {
            console.log("eventListView.onItemsRemoved");
            gridItemUpdated()
        }
    }

    function gridItemUpdated(){
        console.log("eventListView.gridItemUpdated", gridItems);

        if(eventListView.cameraFilterMode === EventListView.CameraFilterMode.OnLayout){
            applyGridItems();
        }
    }

    function applyGridItems(){

        var itemList = []

        for (var i = 0 ; i < gridItems.length ; i++) {
            var targetItem = gridItems[i];

            if(targetItem.isCamera)
            {
                console.log("eventListView.applyGridItems ", targetItem.itemModel.deviceId, targetItem.itemModel.channelId)
                var id = targetItem.itemModel.deviceId + "_" + targetItem.itemModel.channelId
                itemList.push(id)
            }
        }

        eventListViewModel.eventListFilterProxyModel.cameraFilterEnabled = true
        eventListViewModel.eventListFilterProxyModel.setChannelList(itemList)
    }

    function applyFocusedItem(){

        if(focusedGridItem.isCamera){
            console.log("eventListView.applyFocusedItem ", focusedGridItem.itemModel.deviceId, focusedGridItem.itemModel.channelId)
        }

        var id = focusedGridItem.itemModel.deviceId + "_" + focusedGridItem.itemModel.channelId
        var itemList = []
        itemList.push(id)

        eventListViewModel.eventListFilterProxyModel.cameraFilterEnabled = true;
        eventListViewModel.eventListFilterProxyModel.setChannelList(itemList);
    }

    function getResourceIcon(name){
        if(name === "MotionDetection"){
            return WisenetImage.motionDetection
        }else if(name === "FaceDetection"){
            return WisenetImage.faceDetection
        }else if(name === "Tampering"){
            return WisenetImage.tamperingDetection
        }else if(name === "AudioDetection"){
            return WisenetImage.audioDetection
        }else if(name === "VideoAnalytics"){
            return WisenetImage.iva
        }else if(name === "Tracking"){
            return WisenetImage.autoTracking
        }else if(name === "Tracking.Start"){
            return WisenetImage.autoTracking
        }else if(name === "Tracking.End"){
            return WisenetImage.autoTracking
        }else if(name === "DefocusDetection"){
            return WisenetImage.defocusDetection
        }else if(name === "FogDetection"){
            return WisenetImage.fogDetection
        }else if(name === "AudioAnalytics"){
            return WisenetImage.soundClassification
        }else if(name === "AlarmInput"){
            return WisenetImage.alarmInput
        }else if(name === "Videoloss"){
            return WisenetImage.videoloss
        }else if(name === "Videoloss.Start"){
            return WisenetImage.videoloss
        }else if(name === "Videoloss.End"){
            return WisenetImage.videoloss
        }else if(name === "VideoAnalytics.Passing"){
            return WisenetImage.videoAnalyticsPassing
        }else if(name === "VideoAnalytics.Intrusion"){
            return WisenetImage.videoAnalyticsIntrusion
        }else if(name === "VideoAnalytics.Entering"){
            return WisenetImage.videoAnalyticsEntering
        }else if(name === "VideoAnalytics.Exiting"){
            return WisenetImage.videoAnalyticsExiting
        }else if(name === "VideoAnalytics.Appearing"){
            return WisenetImage.videoAnalyticsAppearing
        }else if(name === "VideoAnalytics.Disappearing"){
            return WisenetImage.videoAnalyticsDisappearing
        }else if(name === "VideoAnalytics.Loitering"){
            return WisenetImage.videoAnalyticsLoitering
        }else if(name === "AudioAnalytics.Scream"){
            return WisenetImage.audioAnalyticsScream
        }else if(name === "AudioAnalytics.Gunshot"){
            return WisenetImage.audioAnalyticsGunshot
        }else if(name === "AudioAnalytics.Explosion"){
            return WisenetImage.audioAnalyticsExplosion
        }else if(name === "AudioAnalytics.GlassBreak"){
            return WisenetImage.audioAnalyticsGlassBreak
        }else if(name === "NetworkAlarmInput"){
            return WisenetImage.networkAlarmInput
        }

        return WisenetImage.dynamicEvent
    }

    function openEventSearch(cameraId){
        windowService.eventSearchView_SetSearchMenu(0)
        windowService.eventSearchView_SetVisible(true)
        windowService.eventSearchView_ResetFilter()

        console.log("openEventSearch", cameraId)

        var channelGuids = []

        if(cameraId !== ""){
            channelGuids.push(cameraId)
            windowService.eventSearchView_SetChannelFilter(channelGuids)
        }

        windowService.eventSearchView_SearchByFilter()
    }

    function closeCameraFilter(){
        if(cameraFilterMenu.opened)
            cameraFilterMenu.close()
    }

    function closeEventFilter(){
        if(eventTypeViewPopup.opened)
            eventTypeViewPopup.close()
    }

    onFocusedGridItemChanged: {
        if(eventListView.cameraFilterMode === EventListView.CameraFilterMode.SelectedCamera){
            applyFocusedItem()
        }
    }

    property int cameraFilterMode : EventListView.CameraFilterMode.AllCameras
    enum CameraFilterMode {
        AllCameras,
        OnLayout,
        SelectedCamera
    }

    Rectangle{
        id: buttonArea

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 37
        color: "transparent"

        Rectangle{
            id: upperBorder
            height: 1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: WisenetGui.contrast_08_dark_grey
        }

        WisenetSearchBox{
            id: searchTextBox

            placeholderText: WisenetLinguist.search
            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.right: eventCount.visible ? eventCount.left : cameraFilterButton.left
            anchors.rightMargin: 5
            height: 31
            bgColor: WisenetGui.contrast_09_dark

            onSearchTextChanged:{
                console.log("Event list - onSearchTextChanged :" + text)
                eventListViewModel.eventListFilterProxyModel.setFilterText(text)
            }
        }

        Text{
            id: eventCount

            visible: eventListViewModel.eventTestVisible
            text: eventListViewModel.eventListModel.count

            font.pixelSize: 14
            color: WisenetGui.contrast_00_white

            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.right: cameraFilterButton.left
            anchors.rightMargin: 10

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        WisenetImageButton{
            id: cameraFilterButton

            width: 20
            height: 20
            sourceWidth: 20
            sourceHeight: 20

            checkable: true
            checked: cameraFilterMenu.opened

            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.right: eventFilterButton.left
            anchors.rightMargin: 10

            imageNormal: "qrc:/Assets/Icon/Monitoring/RightPanel/camera_filter_normal.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/RightPanel/camera_filter_opened.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/RightPanel/camera_filter_hover.svg"

            onCheckedChanged: {
                if(checked)
                {
                    closeEventFilter()
                    cameraFilterMenu.open()
                }
                else
                    cameraFilterMenu.close()
            }

            WisenetMediaToolTip {
                delay: 1000
                visible: cameraFilterButton.hovered
                text: WisenetLinguist.channels
            }
        }

        WisenetImageButton{
            id:  eventFilterButton

            width: 20
            height: 20
            sourceWidth: 20
            sourceHeight: 20
            checkable: true
            checked: eventTypeViewPopup.opened

            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 15

            imageNormal: "qrc:/Assets/Icon/Monitoring/RightPanel/event_type_filter_normal.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/RightPanel/event_type_filter_opened.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/RightPanel/event_type_filter_hover.svg"

            onCheckedChanged: {
                if(checked){
                    closeCameraFilter()
                    eventSearchFilterTreeSourceModel.updateAllEventData()
                    eventTypeViewPopup.open()
                }
                else
                    eventTypeViewPopup.close()
            }

            WisenetMediaToolTip {
                delay: 1000
                visible: eventFilterButton.hovered
                text: WisenetLinguist.event
            }
        }

        Rectangle{
            id: lowerBorder
            height: 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 15
            anchors.rightMargin: 14
            color: WisenetGui.contrast_08_dark_grey
        }
    }

    MouseArea{
        anchors.fill: eventList

        acceptedButtons: Qt.RightButton

        onPressed: {

            if (mouse.button == Qt.RightButton)
            {
                contextMenu.cameraId = ""
                contextMenu.popup()
            }
        }
    }

    ListView{
        id: eventList
        anchors.top: buttonArea.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        clip: true

        model: eventListViewModel.eventListFilterProxyModel
        delegate: Rectangle {
            id: alarmArea

            width: eventList.width
            height: 80 + sourceNameArea.height - 16
            color: {
                if(alarmMouseArea.containsMouse)
                    return WisenetGui.contrast_08_dark_grey
                else
                    return WisenetGui.contrast_09_dark
            }

            property var itemId : sourceId
            property var isPriority : model.isPriorityAlarm

            Image{
                id: priorityAlarmMark

                visible: model.isPriorityAlarm && !model.played

                anchors.top: parent.top
                anchors.left: parent.left
                width:9
                height:9

                sourceSize.width: 9
                sourceSize.height: 9

                source: "qrc:/Assets/Icon/Monitoring/RightPanel/priority_alarm.svg"
            }

            Rectangle {
                id: alarmLayout

                anchors.fill: parent
                anchors.topMargin: 13
                anchors.leftMargin: 15
                anchors.rightMargin: 14
                anchors.bottomMargin: 13

                color: "transparent"

                Text{
                    id: sourceNameArea
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //height: 16

                    text: sourceName
                    font.pixelSize: 14
                    color: WisenetGui.contrast_00_white

                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Image{
                    id: alarmIcon
                    width: 16
                    height: 16
                    anchors.top: sourceNameArea.bottom
                    anchors.topMargin: 7
                    anchors.left: parent.left

                    sourceSize.width: 16
                    sourceSize.height: 16

                    source: eventListView.getResourceIcon(alarmType)
                }

                Item{
                    id: alarmTypeArea
                    width: parent.width
                    height: 14
                    anchors.top: sourceNameArea.bottom
                    anchors.topMargin: 8
                    anchors.left: alarmIcon.right
                    anchors.leftMargin: 4
                    anchors.right: parent.right

                    Text {
                        id: alarmTypeText

                        anchors.fill: parent
                        width: parent.width
                        height: parent.height

                        text: alarmName
                        font.pixelSize: 12
                        color: WisenetGui.contrast_04_light_grey

                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter

                        elide: Qt.ElideRight
                    }
                }

                Item{
                    id: timeArea
                    width: parent.width
                    height: 12
                    anchors.top: alarmTypeArea.bottom
                    anchors.topMargin: 4
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.right: parent.right

                    Text {
                        id: occurrenceTimeText
                        anchors.fill: parent
                        width: parent.width
                        height: parent.height

                        text: occurrenceTime
                        font.pixelSize: 10
                        color: WisenetGui.contrast_04_light_grey

                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            Rectangle{
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 15
                anchors.rightMargin: 14
                height: 1
                color: WisenetGui.contrast_08_dark_grey
            }

            MouseArea
            {
                id: alarmMouseArea
                hoverEnabled: true
                anchors.fill: parent

                acceptedButtons: Qt.RightButton | Qt.LeftButton

                onDoubleClicked: {
                    if(!userGroupModel.playback)
                        return

                    var id = model.sourceId
                    var splitId = id.split('_')

                    // deviceTime 기준으로 Playback 하도록 수정
                    var playTime = deviceTime
                    if(Date.now() - utcTime < 30000)
                        playTime -= 31000   // 발생 30초 이내의 이벤트는 녹화 시간을 고려하여, 발생 30초 전 영상 재생
                    else
                        playTime -= 6000    // 장비의 이벤트 발생 5초 전 영상 재생

                    console.log("EventListView double clicked device:" + splitId[0] + ", camera:" + splitId[1] + ", startTime:" + utcTime + ", playTime:" + playTime)

                    selectedViewingGrid.checkNewVideoChannel(splitId[0], splitId[1], true, playTime, -1, true, "", false)

                    if(model.isPriorityAlarm && !model.played)
                    {
                        eventList.currentIndex = 0
                        model.played = true
                    }
                }

                onPressed:{
                    if (mouse.button == Qt.RightButton){
                        console.log("EventListView item onPressed", sourceName, model.sourceId, isPriority)

                        contextMenu.cameraId = model.sourceId
                        contextMenu.setEventSearchVisible(model.isPriority)
                        contextMenu.popup()
                    }
                }
            }
        }

        //ScrollBar.vertical: ScrollBar {}
        ScrollBar.vertical: WisenetScrollBar {
            id: verticalScrollBar
        }

        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
            //   NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
        }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutQuad }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }
    }

    WisenetImageButton{
        id: moveTopButton

        width: 48
        height: 48
        sourceWidth: 48
        sourceHeight: 48

        anchors.bottom: eventList.bottom
        anchors.right: eventList.right
        anchors.bottomMargin: 10
        anchors.rightMargin: 10

        imageNormal: "qrc:/Assets/Icon/Monitoring/RightPanel/go_to_top_normal.svg"
        imageHover: "qrc:/Assets/Icon/Monitoring/RightPanel/go_to_top_hover.svg"

        visible: verticalScrollBar.position > 0

        onClicked: {
            verticalScrollBar.decrease()
            moveTopAnimation.start()
        }
    }

    NumberAnimation{
        id: moveTopAnimation
        target: verticalScrollBar
        properties: "position"
        to:0
        duration:300
    }

    Popup {
        id: eventTypeViewPopup

        closePolicy: Popup.CloseOnEscape

        x: eventList.x
        y: eventList.y - 1
        width: eventList.width
        height: eventList.height
        //modal: true
        focus: true

        ResourceTreeProxyModel{
            id: eventSearchFilterTreeProxyModel
            sourceModel: EventFilterTreeSourceModel{
                id: eventSearchFilterTreeSourceModel
                Component.onCompleted: {
                    //setAlertAlarm()
                    setAllEventData()
                }
            }
        }


        contentItem: WisenetEventFilterTree{
            id: eventSearchFilterTree
            anchors.fill: parent
            treeModel: eventSearchFilterTreeProxyModel

            onSelectedEventChanged: {
                var emptyList = []

                if(allEvent === true){
                    console.log("all")
                    eventListViewModel.eventListFilterProxyModel.eventTypeFilterEnabled = false
                    eventListViewModel.eventListFilterProxyModel.setEventTypeFilter(emptyList)
                }
                else if(selectedEventValue === null || selectedEventValue.length === 0){
                    console.log("none")
                    eventListViewModel.eventListFilterProxyModel.eventTypeFilterEnabled = true
                    eventListViewModel.eventListFilterProxyModel.setEventTypeFilter(emptyList)
                }
                else {
                    console.log("selected")

                    eventListViewModel.eventListFilterProxyModel.eventTypeFilterEnabled = true
                    var eventFilterList = []
                    selectedEventValue.forEach(function(item) {
                        console.log("event display name",item.uuid)
                        eventFilterList.push(item.uuid)
                    })

                    eventListViewModel.eventListFilterProxyModel.setEventTypeFilter(eventFilterList)
                }
            }
        }

        onOpenedChanged: {
            if(opened){
                eventSearchFilterTree.expandAll()
            }
        }
    }


    WisenetMediaMenu {
        id: cameraFilterMenu

        closePolicy: Popup.CloseOnEscape
        rightMargin: 0

        x: eventList.x
        y: eventList.y - 1
        width: eventListView.width

        Action {
            id: allCamera
            text: WisenetLinguist.allCameras
            checkable: true
            checked: eventListView.cameraFilterMode === EventListView.CameraFilterMode.AllCameras
            onTriggered: {
                eventListView.cameraFilterMode = EventListView.CameraFilterMode.AllCameras
                eventListViewModel.eventListFilterProxyModel.cameraFilterEnabled = false;
                eventListViewModel.eventListFilterProxyModel.updateFilter()
            }
        }

        Action {
            id: onLayout
            text: WisenetLinguist.onLayout_
            checkable: true
            checked: cameraFilterMode === EventListView.CameraFilterMode.OnLayout
            onTriggered: {
                eventListView.cameraFilterMode = EventListView.CameraFilterMode.OnLayout
                eventListView.applyGridItems()
            }
        }

        Action {
            id: selectedCamera
            text: WisenetLinguist.selectedCamera
            checkable: true
            checked: cameraFilterMode === EventListView.CameraFilterMode.SelectedCamera
            onTriggered: {
                eventListView.cameraFilterMode = EventListView.CameraFilterMode.SelectedCamera
                eventListView.applyFocusedItem()
            }
        }
    }

    // event search, notifications(settings), clear
    WisenetMediaMenu{
        id: contextMenu

        property string cameraId : ""

        function setEventSearchVisible(isPriority){
            eventSearchMenu.visible = !isPriority
        }

        WisenetMediaMenuItem{
            id: eventSearchMenu
            text: WisenetLinguist.eventSearch
            onTriggered: eventListView.openEventSearch(contextMenu.cameraId)
        }

        Action{
            text: WisenetLinguist.clear
            onTriggered: {
                eventListViewModel.eventListModel.removeAll()
            }
        }
    }
}

