import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import Wisenet.DeviceFilterTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import WisenetLanguage 1.0

import "qrc:/"
import WisenetMediaFramework 1.0

Rectangle{
    id: searchResultView
    visible: true
    color: backgroundColor

    property color backgroundColor: WisenetGui.contrast_09_dark
    property color tableBorderColor: WisenetGui.contrast_07_grey
    property color tableLowBorderColor: WisenetGui.contrast_08_dark_grey

    property alias listVerticalScrollBarPos: searchResultTable.verticalScrollPosition

    ListModel{
        id: searchColumns
        //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
        //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
        Component.onCompleted: {
            append({ "columnWidth": 200, "title": WisenetLinguist.dateAndTime, "sort": 1, "splitter": 1, "filter":0})
            append({ "columnWidth": 200, "title": WisenetLinguist.eventType, "sort": 0, "splitter": 1, "filter":1})
        }
    }

    DelegateChooser {
        id: smartSearchTableDelegate

        DelegateChoice {
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
                        smartSearchViewModel.setHoveredRow(row, containsMouse);
                    }
                    onDoubleClicked: {
                        var deviceId = searchPlaybackView.viewingGrid.focusedItem.mediaParam.deviceId
                        var channelId = searchPlaybackView.viewingGrid.focusedItem.mediaParam.channelId
                        var deviceTime = deviceTimeRole

                        if(userGroupModel.playback && deviceId  && channelId && deviceTime) {
                            deviceTime -= 5000  // 장비의 이벤트 발생 5초 전 영상 재생
                            searchPlaybackView.viewingGrid.checkNewVideoChannel(deviceId, channelId, true, deviceTime, -1 , true)
                        }
                    }
                }

                Rectangle{
                    // 이벤트 종류 (Image + Text)
                    visible: column === 1
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
                    visible: column !== 1
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

    Text {
        id: channelInfoText
        height: 16
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: 12
        text: searchPlaybackView.viewingGrid.focusedItem && searchPlaybackView.viewingGrid.focusedItem.isCamera ?
                  searchPlaybackView.viewingGrid.focusedItem.mediaName : ""
        color: WisenetGui.contrast_04_light_grey
    }

    WisenetTable {
        id: searchResultTable
        anchors.fill: parent
        anchors.topMargin: channelInfoText.height + 10
        anchors.bottomMargin: totalresult.height + 10

        columns: searchColumns
        cellDelegate: smartSearchTableDelegate
        tableModel : smartSearchViewModel
        lastSortIndex : 0
        popupDepth : true
        headerBackgroundColor: WisenetGui.contrast_08_dark_grey

        borderColor: tableBorderColor

        onFilterImageClicked: {
            if(1 === index) {
                typeFilterView.x = 0
                if(typeFilterView.width < searchResultTable.width)
                    typeFilterView.x = (searchResultTable.width - typeFilterView.width) / 2 // x는 테이블 중앙
                typeFilterView.open()
            }
        }
    }

    Rectangle {
        id: totalresult
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        height: 16
        width: parent.width/3
        color: WisenetGui.transparent

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 12
            text: smartSearchViewModel.totalCount.toLocaleString(Qt.locale('en_US'),'f',0) + " " + WisenetLinguist.results
            color: WisenetGui.contrast_04_light_grey
        }
    }

    function filterAll(){
        var selectedEvents = [];
        if(eventFilterTree.selectedEventValue !== null) {
            eventFilterTree.selectedEventValue.forEach(function(item){
                selectedEvents.push(item.uuid)
            })
        }
        console.log("SmartSearchFilter,", selectedEvents)

        smartSearchViewModel.filter(selectedEvents)
    }

    //이벤트 필터
    Popup {
        id: typeFilterView
        x: 0
        y: 63
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
                searchResultView.filterAll()
            }
        }

        onOpenedChanged: {
            if(opened){
                eventFilterTree.expandAll()
            }
        }
    }

    Connections {
        target: smartSearchViewModel
        function onResultUpdated(typeSet) {
            // 스크롤과 정렬 초기화
            searchResultTable.verticalScrollPosition = 0
            searchResultTable.initializeSortState(0)

            // 타입필터 초기화
            eventFilterTreeSourceModel.setEventData(typeSet)
            eventFilterTree.reset()
        }
    }
}
