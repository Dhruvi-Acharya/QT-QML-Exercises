import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import WisenetStyle 1.0
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0
import "qrc:/"

Rectangle {
    id: bookmarkListView
    color: WisenetGui.contrast_09_dark

    signal openBookmarkPopup(var bookmarkId)
    signal bookmarkSelected(var bookmarkId)
    signal openBookmarkExportVideoPopup(var bookmarkId)

    property var selectedViewingGrid: null
    property var gridItems : (selectedViewingGrid && selectedViewingGrid.gridItems) ? selectedViewingGrid.gridItems : null
    property var focusedGridItem : (selectedViewingGrid && selectedViewingGrid.focusedItem) ? selectedViewingGrid.focusedItem : null

    property var timeLineStart
    property var timeLineEnd

    Connections{
        target: selectedViewingGrid ? selectedViewingGrid : null

        onItemsAdded: {
            console.log("bookmarkListView.onItemAdded");
            gridItemUpdated()
        }

        onItemsRemoved: {
            console.log("bookmarkListView.onItemsRemoved");
            gridItemUpdated()
        }
    }

    // Set focus when bookmark added
    Connections{
        target: (bookmarkListViewModel && bookmarkListViewModel.bookmarkListModel) ? bookmarkListViewModel.bookmarkListModel : null

        onSelectedBookmarkChanged:{
            bookmarkListView.setSelectedBookmarkId(bookmarkId)
        }
    }

    function gridItemUpdated(){
        console.log("bookmarkListView.gridItemUpdated", gridItems);

        if(bookmarkListView.cameraFilterMode === BookmarkListView.CameraFilterMode.OnLayout){
            applyGridItems();
        }
    }

    function setVisibleTime(startTime, endTime){
        timeLineStart = startTime
        timeLineEnd = endTime

        if(bookmarkListView.timeFilterMode === BookmarkListView.TimeFilterMode.SelectedOnTimeline){
            bookmarkListViewModel.bookmarkListSortFilterProxyModel.setUtcFilter(timeLineStart, timeLineEnd)
        }
    }

    function applyGridItems(){

        var itemList = []

        for (var i = 0 ; i < gridItems.length ; i++) {
            var targetItem = gridItems[i];

            if(targetItem.isCamera)
            {
                console.log("bookmarkListView.applyGridItems ", targetItem.itemModel.deviceId, targetItem.itemModel.channelId)
                var id = targetItem.itemModel.deviceId + "_" + targetItem.itemModel.channelId
                itemList.push(id)
            }
        }

        bookmarkListViewModel.bookmarkListSortFilterProxyModel.cameraFilterEnabled = true
        bookmarkListViewModel.bookmarkListSortFilterProxyModel.setChannelList(itemList)
    }

    function applyFocusedItem(){

        if(focusedGridItem.isCamera){
            console.log("bookmarkListView.applyFocusedItem ", focusedGridItem.itemModel.deviceId, focusedGridItem.itemModel.channelId)
        }

        var id = focusedGridItem.itemModel.deviceId + "_" + focusedGridItem.itemModel.channelId
        var itemList = []
        itemList.push(id)

        bookmarkListViewModel.bookmarkListSortFilterProxyModel.cameraFilterEnabled = true;
        bookmarkListViewModel.bookmarkListSortFilterProxyModel.setChannelList(itemList);
    }

    function applyTime(days){
        bookmarkListViewModel.bookmarkListSortFilterProxyModel.datetimeFilterEnabled = true
        var today = new Date()
        today.setHours(0)
        today.setMinutes(0)
        today.setSeconds(0)

        var todayTimeStamp = today.getTime()
        var startTime = todayTimeStamp - (days * 24 * 60 * 60 * 1000);

        console.log("bookmarkListView.applyTime ", days, todayTimeStamp, startTime)

        bookmarkListViewModel.bookmarkListSortFilterProxyModel.setUtcFilter(startTime, 0)
    }

    function closeCameraFilter(){
        if(cameraFilterMenu.opened)
            cameraFilterMenu.close()
    }

    function closeTimeFilter(){
        if(timeFilterMenu.opened)
            timeFilterMenu.close()
    }

    // from Media controller
    function setSelectedBookmarkId(bookmarkId){
        console.log("bookmarkListView.setSelectedBookmarkId()", bookmarkList.count, bookmarkId)

        // bookmarkList.itemAtIndex(i) sometime returns null
        for(var j=0; j<visualModel.items.count; j++){
            var item = visualModel.items.get(j)
            //console.log("bookmarkListView.setSelectedBookmarkId() visualModel", item, item.model.bookmarkId, item.model.bookmarkName, j);
            if(item.model.bookmarkId === bookmarkId){
                bookmarkList.currentIndex = j
                break
            }
        }

    }

    onFocusedGridItemChanged: {
        if(bookmarkListView.cameraFilterMode === BookmarkListView.CameraFilterMode.SelectedCamera){
            applyFocusedItem();
        }
    }

    property int cameraFilterMode : BookmarkListView.CameraFilterMode.AllCameras
    enum CameraFilterMode {
        AllCameras,
        OnLayout,
        SelectedCamera
    }

    property int timeFilterMode : BookmarkListView.TimeFilterMode.AnyTime
    enum TimeFilterMode{
        AnyTime,
        LastDay,
        Last7Days,
        Last30Days,
        SelectedOnTimeline
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

            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.right: timeFilterButton.left
            anchors.rightMargin: 5
            height: 31
            bgColor: WisenetGui.contrast_09_dark

            onSearchTextChanged:{
                console.log("Bookmark list - onSearchTextChanged :" + text)
                bookmarkListViewModel.bookmarkListSortFilterProxyModel.setFilterText(text)
            }
        }

        WisenetImageButton{
            id: timeFilterButton

            width: 20
            height: 20
            sourceWidth: 20
            sourceHeight: 20

            checkable: true
            checked: timeFilterMenu.visible

            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.right: cameraFilterButton.left
            anchors.rightMargin: 10

            imageNormal: "qrc:/Assets/Icon/Monitoring/RightPanel/calendar_normal.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/RightPanel/calendar_selected.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/RightPanel/calendar_hover.svg"

            onCheckedChanged: {
                if(checked)
                {
                    closeCameraFilter()
                    timeFilterMenu.open()
                }
                else
                    timeFilterMenu.close()
            }

            WisenetMediaToolTip {
                delay: 1000
                visible: timeFilterButton.hovered
                text: WisenetLinguist.time
            }
        }

        WisenetImageButton{
            id: cameraFilterButton

            width: 20
            height: 20

            sourceWidth: 20
            sourceHeight: 20

            checkable: true
            checked: cameraFilterMenu.visible

            imageNormal: "qrc:/Assets/Icon/Monitoring/RightPanel/camera_filter_normal.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/RightPanel/camera_filter_opened.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/RightPanel/camera_filter_hover.svg"

            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 15

            onCheckedChanged: {
                if(checked)
                {
                    closeTimeFilter()
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

    ListView{
        id: bookmarkList

        anchors.top: buttonArea.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        clip: true
        focus: true

        model: DelegateModel{
            id: visualModel

            model: (bookmarkListViewModel && bookmarkListViewModel.bookmarkListSortFilterProxyModel) ? bookmarkListViewModel.bookmarkListSortFilterProxyModel : null

            delegate: BookmarkItem{

                property int visualIndex: DelegateModel.itemsIndex

                bookmarkId: model.bookmarkId
                bookmarkName: model.bookmarkName
                cameraName: model.cameraName
                displayTime: model.displayTime
                description: model.description
                image: model.image
                imageWidth : model.imageWidth
                imageHeight : model.imageHeight
                isCurrentItem : bookmarkList.currentIndex === visualIndex

                width: bookmarkList.width// - 14
                //anchors.rightMargin: 14

                onDoubleClicked: {
                    var id = model.sourceId.split('_')
                    console.log("Bookmark item double clicked device:" + id[0] + ", camera:" + id[1] + ", startTime:" + model.utcStartTime)
                    selectedViewingGrid.checkNewVideoChannel(id[0], id[1], true, model.utcStartTime, -1, true, "", false)
                }

                onPressed: {
                    if (mouse.button === Qt.RightButton)
                    {
                        var id = model.sourceId.split('_')

                        console.log("Bookmark item onPressed device:" + id[0] + ", camera:" + id[1] + ", startTime:" + model.utcStartTime)
                        contextMenu.deviceId = id[0]
                        contextMenu.cameraId = id[1]
                        contextMenu.utcStartTime = model.utcStartTime
                        contextMenu.bookmarkId = model.bookmarkId

                        contextMenu.popup()
                    }

                    bookmarkList.currentIndex = visualIndex

                    // to Media controller
                    var bookmarkItem = bookmarkList.currentItem
                    bookmarkListView.bookmarkSelected(bookmarkItem.bookmarkId)
                }
            }
        }

        onCurrentItemChanged:{
            console.log("bookmarkList.onCurrentItemChanged", currentItem)

        }

        onCurrentIndexChanged:{
            console.log("bookmarkList.onCurrentItemChanged", currentIndex)
        }

        ScrollBar.vertical: WisenetScrollBar {}
    }

    WisenetMediaMenu{
        id: timeFilterMenu

        closePolicy: Popup.CloseOnEscape
        rightMargin: 0

        x: bookmarkList.x
        y: bookmarkList.y - 1
        width: bookmarkListView.width

        Action {
            id: anyTime
            text: WisenetLinguist.anyTime
            checkable: true
            checked: bookmarkListView.timeFilterMode === BookmarkListView.TimeFilterMode.AnyTime
            onTriggered: {
                bookmarkListView.timeFilterMode = BookmarkListView.TimeFilterMode.AnyTime
                bookmarkListViewModel.bookmarkListSortFilterProxyModel.datetimeFilterEnabled = false
                bookmarkListViewModel.bookmarkListSortFilterProxyModel.updateFilter()

            }
        }

        Action {
            id: lastDay
            text: WisenetLinguist.lastDay
            checkable: true
            checked: bookmarkListView.timeFilterMode === BookmarkListView.TimeFilterMode.LastDay
            onTriggered: {
                bookmarkListView.timeFilterMode = BookmarkListView.TimeFilterMode.LastDay
                bookmarkListView.applyTime(1)
            }
        }

        Action {
            id: last7Days
            text: WisenetLinguist.last7Days
            checkable: true
            checked: bookmarkListView.timeFilterMode === BookmarkListView.TimeFilterMode.Last7Days
            onTriggered: {
                bookmarkListView.timeFilterMode = BookmarkListView.TimeFilterMode.Last7Days
                bookmarkListView.applyTime(7)
            }
        }

        Action {
            id: last30Days
            text: WisenetLinguist.last30Days
            checkable: true
            checked: bookmarkListView.timeFilterMode === BookmarkListView.TimeFilterMode.Last30Days
            onTriggered: {
                bookmarkListView.timeFilterMode = BookmarkListView.TimeFilterMode.Last30Days
                bookmarkListView.applyTime(30)
            }
        }

        Action {
            id: selectedOnTimeline
            text: WisenetLinguist.selectedOnTimeline
            checkable: true
            checked: bookmarkListView.timeFilterMode === BookmarkListView.TimeFilterMode.SelectedOnTimeline
            onTriggered: {
                bookmarkListView.timeFilterMode = BookmarkListView.TimeFilterMode.SelectedOnTimeline
                bookmarkListViewModel.bookmarkListSortFilterProxyModel.datetimeFilterEnabled = true
                bookmarkListViewModel.bookmarkListSortFilterProxyModel.setUtcFilter(timeLineStart, timeLineEnd)
            }
        }

    }

    WisenetMediaMenu {
        id: cameraFilterMenu

        closePolicy: Popup.CloseOnEscape
        rightMargin: 0

        x: bookmarkList.x
        y: bookmarkList.y - 1
        width: bookmarkListView.width

        Action {
            id: allCamera
            text: WisenetLinguist.allCameras
            checkable: true
            checked: cameraFilterMode === BookmarkListView.CameraFilterMode.AllCameras
            onTriggered: {
                bookmarkListView.cameraFilterMode = BookmarkListView.CameraFilterMode.AllCameras
                bookmarkListViewModel.bookmarkListSortFilterProxyModel.cameraFilterEnabled = false;
                bookmarkListViewModel.bookmarkListSortFilterProxyModel.updateFilter()
            }
        }

        Action {
            id: onLayout
            text: WisenetLinguist.onLayout_
            checkable: true
            checked: cameraFilterMode === BookmarkListView.CameraFilterMode.OnLayout
            onTriggered: {
                bookmarkListView.cameraFilterMode = BookmarkListView.CameraFilterMode.OnLayout
                bookmarkListView.applyGridItems()
            }
        }

        Action {
            id: selectedCamera
            text: WisenetLinguist.selectedCamera
            checkable: true
            checked: cameraFilterMode === BookmarkListView.CameraFilterMode.SelectedCamera
            onTriggered: {
                bookmarkListView.cameraFilterMode = BookmarkListView.CameraFilterMode.SelectedCamera
                bookmarkListView.applyFocusedItem()
            }
        }
    }

    WisenetMediaMenu{
        id: contextMenu

        property var deviceId
        property var cameraId
        property var utcStartTime
        property var bookmarkId

        Action{
            text: WisenetLinguist.open
            onTriggered: {
                selectedViewingGrid.checkNewVideoChannel(contextMenu.deviceId, contextMenu.cameraId, true, contextMenu.utcStartTime, -1, true, "", false)
            }
        }

        Action{
            text: WisenetLinguist.deleteText
            enabled: userGroupModel && userGroupModel.playback
            onTriggered: {
                console.log("Bookmark delete")
                bookmarkListViewModel.removeBookmarkItem(contextMenu.bookmarkId)
            }
        }

        Action{
            text: WisenetLinguist.edit
            enabled: userGroupModel && userGroupModel.playback
            onTriggered: {
                console.log("Bookmark edit")
                openBookmarkPopup(contextMenu.bookmarkId)
            }
        }

        Action{
            text: WisenetLinguist.exportVideo
            enabled: userGroupModel && userGroupModel.exportVideo
            onTriggered: {
                console.log("Bookmark export video")
                openBookmarkExportVideoPopup(contextMenu.bookmarkId)
            }
        }
    }
}
