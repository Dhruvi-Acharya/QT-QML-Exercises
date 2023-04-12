import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import Wisenet.EventLogViewModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import Wisenet.DeviceFilterTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import WisenetLanguage 1.0

import "qrc:/"
import WisenetMediaFramework 1.0

Rectangle{
    id: eventSearchResultView
    visible: true
    color: backgroundColor

    property color backgroundColor: WisenetGui.contrast_09_dark
    property color tableBorderColor: WisenetGui.contrast_07_grey
    property color tableLowBorderColor: WisenetGui.contrast_08_dark_grey

    property var tableModel: eventSearchEventLogViewModel
    property bool supportPlayback: false
    property bool supportBookmark: false
    property alias listVerticalScrollBarPos: eventSearchResultTable.verticalScrollPosition

    function search(from, to, allDevice, devices, allEvent, events){
        eventSearchEventLogViewModel.search(from, to, allDevice, devices, allEvent, events)
        eventSearchResultTable.initializeSortState(1)
    }

    function useExportButton(visible){
        exportLogButton.visible = visible
    }

    signal exportButtonClicked()

    //Table 모델 및 Delegate
    //ListModel{
    //    id: eventSearchColumns
    //    //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
    //    //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
    //    ListElement { columnWidth: 245; title: WisenetLinguist.dateAndTime; sort: 1; splitter: 1; filter:0}
    //    ListElement { columnWidth: 245; title: WisenetLinguist.deviceName; sort: 0; splitter: 1; filter:1}
    //    ListElement { columnWidth: 245; title: WisenetLinguist.eventType; sort: 0; splitter: 1; filter:1}
    //    ListElement { columnWidth: 247; title: WisenetLinguist.description; sort: 3; splitter: 1; filter:2}
    //}
    ListModel{
        id: eventSearchColumns
        //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
        //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
        Component.onCompleted: {
            var bookmarkColumnWidth = supportBookmark ? 40 : 0
            var textColumnWidth = supportBookmark ? 220 : 239
            append({ "columnWidth": bookmarkColumnWidth, "title": "", "sort": 0, "splitter": 0, "filter":0})
            append({ "columnWidth": textColumnWidth, "title": WisenetLinguist.dateAndTime, "sort": 1, "splitter": 1, "filter":0})
            append({ "columnWidth": textColumnWidth, "title": WisenetLinguist.deviceName, "sort": 0, "splitter": 1, "filter":1})
            append({ "columnWidth": textColumnWidth, "title": WisenetLinguist.eventType, "sort": 0, "splitter": 1, "filter":1})
            append({ "columnWidth": textColumnWidth, "title": WisenetLinguist.description, "sort": 3, "splitter": 1, "filter":2})
        }
    }

    DelegateChooser {
        id: eventSearchTableDelegate
        DelegateChoice {
            // Bookmark Column의 delegate
            column: 0
            delegate: Rectangle {
                implicitHeight: 30
                color: hoveredRole ? WisenetGui.contrast_08_dark_grey : backgroundColor

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onContainsMouseChanged: {
                        eventSearchEventLogViewModel.setHoveredRow(row, containsMouse);
                    }
                }

                Button {
                    id: bookmarkButton
                    flat: true
                    width: 21
                    height: 20
                    anchors.centerIn: parent
                    enabled: !isDeletedChannelRole  // 삭제 된 채널이면 비활성화
                    hoverEnabled: true
                    background: Rectangle {
                        color: WisenetGui.transparent
                    }

                    property bool bookmarkChecked: bookmarkIdRole !== ""

                    WisenetMediaToolTip {
                        delay: 1000
                        visible: parent.hovered
                        text: parent.bookmarkChecked ? WisenetLinguist.deleteBookmark : WisenetLinguist.addBookmark
                    }

                    Image {
                        anchors.fill: parent
                        sourceSize: Qt.size(width, height)
                        source: {
                            if(parent.bookmarkChecked) {
                                if(parent.hovered)
                                    return WisenetImage.eventBookmark_checked_hover
                                else
                                    return WisenetImage.eventBookmark_checked_default
                            }
                            else {
                                if(parent.hovered)
                                    return WisenetImage.eventBookmark_unchecked_hover
                                else
                                    return WisenetImage.eventBookmark_unchecked_default
                            }
                        }
                    }

                    onClicked: {
                        if(!eventSearchEventLogViewModel.isChannelExist(deviceIdRole, channelIdRole))
                            return  // 삭제 된 채널이면 리턴

                        if(!bookmarkChecked) {
                            var deviceId = deviceIdRole
                            var channelId = channelIdRole
                            var deviceTime = deviceTimeRole
                            var eventLogId = eventLogIdRole
                            eventSearchPlaybackView.addBookmark(deviceId, channelId, deviceTime, eventLogId);
                        }
                        else {
                            var bookmarkId = bookmarkIdRole
                            eventSearchPlaybackView.deleteBookmark(bookmarkId);
                        }
                    }
                }

                Rectangle{
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    height: 1
                    width: parent.width
                    color: tableLowBorderColor
                }
            }
        }
        DelegateChoice {
            // Bookmark Column 제외한 나머지의 delegate
            delegate: Rectangle {
                id: cellRect
                implicitHeight: 30
                color: hoveredRole ? WisenetGui.contrast_08_dark_grey : backgroundColor
                //clip: true

                MouseArea {
                    acceptedButtons: Qt.LeftButton
                    hoverEnabled: true
                    anchors.fill: parent
                    onContainsMouseChanged: {
                        eventSearchEventLogViewModel.setHoveredRow(row, containsMouse);
                    }
                    onDoubleClicked: {
                        if(isDeletedChannelRole)
                            return  // 삭제 된 채널이면 리턴

                        var deviceId = deviceIdRole
                        var channelId = channelIdRole
                        var serviceTime = serviceTimeRole
                        var deviceTime = deviceTimeRole

                        if(supportPlayback && userGroupModel.playback && deviceId  && channelId && deviceTime ) {
                            if(serviceTime && Date.now() - serviceTime < 30000)
                                deviceTime -= 31000 // 발생 30초 이내의 이벤트는 녹화 시간을 고려하여, 발생 30초 전 영상 재생
                            else
                                deviceTime -= 6000  // 장비의 이벤트 발생 5초 전 영상 재생

                            eventSearchPlaybackView.viewingGrid.checkNewVideoChannel(deviceId, channelId, true, deviceTime, -1, true, "", false)
                        }
                    }
                }

                Rectangle{
                    // 이벤트 종류 (Image + Text)
                    visible: column == 3
                    width: eventImage.width + dummyEventText.contentWidth + 6 > parent.width ? parent.width : eventImage.width + dummyEventText.contentWidth + 6
                    height: parent.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: WisenetGui.transparent

                    Image{
                        id: eventImage
                        width: 16
                        height: 16
                        anchors.verticalCenter: parent.verticalCenter
                        sourceSize: Qt.size(16, 16)
                        source: eventFilterTree.getResourceIcon(EventImageRole)
                    }

                    Text {
                        id: eventText
                        anchors.left: eventImage.right
                        anchors.leftMargin: 6
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter

                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: displayRole
                        color: WisenetGui.contrast_01_light_grey
                        font.pixelSize: 12
                    }

                    Text {
                        id: dummyEventText
                        text: eventText.text
                        color: WisenetGui.transparent
                        font.pixelSize: eventText.font.pixelSize
                    }
                }

                Text {
                    // 이벤트 종류를 제외한 나머지 Column Text
                    visible: column != 3
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: displayRole
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Rectangle{
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    height: 1
                    width: parent.width
                    color: tableLowBorderColor
                }
            }
        }
    }

    EventLogViewModel {
        id: eventSearchEventLogViewModel

        onSearchStarted: {
            console.debug("searchStarted")
            resultTimer.restart();
        }

        onSearchFinished: {
            deviceFilterTreeSourceModel.setDevices(cameraList)
            eventFilterTreeSourceModel.setEventData(eventList)
            deviceFilterTree.reset()
            eventFilterTree.reset()
            descriptionFilterSearchBox.reset()
            resultTimer.stop()
            pulseView.close()
        }
    }

    // 200ms 동안 결과를 못받은 경우, timer 구동
    Timer {
        id: resultTimer
        interval: 200
        repeat: false
        onTriggered: {
            pulseView.open();
        }
    }


    WisenetTable {
        id: eventSearchResultTable
        anchors.fill: parent
        anchors.bottomMargin: 77

        columns: eventSearchColumns
        cellDelegate: eventSearchTableDelegate
        tableModel : eventSearchEventLogViewModel
        lastSortIndex : 1
        popupDepth : true
        headerBackgroundColor: WisenetGui.contrast_08_dark_grey

        borderColor: tableBorderColor

        onFilterImageClicked: {
            if(2 === index){
                deviceFilterView.x = x
                deviceFilterView.y = y
                deviceFilterView.open()
            }else if(3 === index){
                eventFilterView.x = x
                eventFilterView.y = y
                eventFilterView.open()
            }else if(4 === index){
                descriptionFilterView.x = x
                descriptionFilterView.y = y
                descriptionFilterView.open()
            }
        }
    }

    Rectangle {
        id: totalresult
        anchors.left: parent.left
        anchors.top: eventSearchResultTable.bottom
        anchors.topMargin: 10
        height: 16
        width: parent.width/3
        color: WisenetGui.transparent

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            text: eventSearchEventLogViewModel.totalCount.toLocaleString(Qt.locale('en_US'),'f',0) + " " + WisenetLinguist.results
            color: WisenetGui.contrast_04_light_grey
        }
    }

    WisenetImageAndTextButton{
        id: exportLogButton
        anchors.right: parent.right
        anchors.top: eventSearchResultTable.bottom
        anchors.topMargin: 10
        height: 14
        width: 60
        visible: false

        onButtonClicked: {
            exportButtonClicked()
        }
    }

    function filterAll(){
        var selectedChannels = [];
        if(deviceFilterTree.selectedDeviceValue !== null){
            deviceFilterTree.selectedDeviceValue.forEach(function(item){
                selectedChannels.push(item.uuid)
            })
        }

        var selectedEvents = [];
        if(eventFilterTree.selectedEventValue !== null){
            eventFilterTree.selectedEventValue.forEach(function(item){
                selectedEvents.push(item.uuid)
            })
        }

        eventSearchEventLogViewModel.filter(selectedChannels,selectedEvents,descriptionFilterSearchBox.lastDisplayText)
    }

    //검색 중 표시
    Popup {
        id: pulseView

        width: 100
        height: 100
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose

        background: Rectangle{
            color: WisenetGui.transparent
        }

        contentItem: WisenetMediaBusyIndicator {
            id: processingIndicator
            visible:true
            running:true
            anchors.centerIn: parent

            sourceWidth: 100
            sourceHeight: 100
            width: 100
            height: 100
        }
    }

    //장치 필터
    Popup {
        id: deviceFilterView
        x: 0
        y: 0
        width: 300
        height: 500
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: deviceFilterDeviceTreeProxyModel
            sourceModel: DeviceFilterTreeSourceModel{
                id: deviceFilterTreeSourceModel
            }

            Component.onCompleted: {
                doSort(Qt.AscendingOrder)
            }

        }

        contentItem: WisenetDeviceSelectionTree{
            id: deviceFilterTree
            anchors.fill: parent
            treeModel: deviceFilterDeviceTreeProxyModel
            onSelectedDeviceChanged: {
                console.debug("EventSearch table filter onSelectedDeviceChanged")
                eventSearchResultView.filterAll()
            }
        }

        onOpenedChanged: {
            if(opened){
                deviceFilterTree.expandAll()
            }
        }

    }

    //이벤트 필터
    Popup {
        id: eventFilterView
        x: 0
        y: 0
        width: 300
        height: 500
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: eventFilterTreeProxyModel
            sourceModel: EventFilterTreeSourceModel{
                id: eventFilterTreeSourceModel
            }
        }


        contentItem: WisenetEventFilterTree{
            id: eventFilterTree
            anchors.fill: parent
            treeModel: eventFilterTreeProxyModel

            onSelectedEventChanged: {
                console.debug("EventSearch table filter onSelectedEventChanged .....")
                eventSearchResultView.filterAll()
            }
        }

        onOpenedChanged: {
            if(opened){
                eventFilterTree.expandAll()
            }
        }
    }

    //Description 필터
    Popup {
        id: descriptionFilterView
        x: 0
        y: 0
        width: 220
        height: 31
        modal: true
        focus: true

        contentItem: WisenetSearchFilter{
            id: descriptionFilterSearchBox
            //focus: true
            anchors.fill: parent

            onSearchFilterTextChanged: {
                console.debug("EventSearch table filter descriptionFilter Search Text: " + text)
                eventSearchResultView.filterAll()
            }

        }

    }



}
