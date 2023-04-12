import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import Wisenet.TextSearchViewModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import Wisenet.DeviceFilterTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import WisenetLanguage 1.0

import "qrc:/"
import WisenetMediaFramework 1.0

Rectangle{
    id: textSearchGridView
    visible: true
    color: backgroundColor

    property color backgroundColor: WisenetGui.contrast_09_dark
    property color tableBorderColor: WisenetGui.contrast_07_grey
    property color tableLowBorderColor: WisenetGui.contrast_08_dark_grey

    property var listModel: textSearchViewModel.textItemViewModel
    property var tableModel: textSearchViewModel
    property bool supportPlayback: false
    property bool supportBookmark: false
    property bool enablePopup: false
    property alias listVerticalScrollBarPos: textSearchResultTable.verticalScrollPosition

    function initialize() {
        textSearchViewModel.initialize()
    }

    function search(allDevice, devices, keyword, from, to, isWholeWord, isCaseSensitive){
        textSearchViewModel.search(allDevice, devices, keyword, from, to, isWholeWord, isCaseSensitive)
        textSearchResultTable.initializeSortState(1)
    }

    onVisibleChanged: {
        this.initialize()
    }

    Popup {
        id: textItemPopup
        property var defaultWidth: 260
        property var defaultHeight: 340
        property var textDataItem: ""

        // 팝업 크기
        width: textData.width + (textData.anchors.leftMargin * 2)
        height: textData.height + (textData.anchors.topMargin * 2)

        // 가운데 정렬
        x: (parent.width - defaultWidth) / 2
        y: (parent.height - defaultHeight) / 2

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        Flickable{
            id: scrollArea

            anchors.fill: parent

            contentWidth: textData.width + (textData.anchors.leftMargin * 2)
            contentHeight: textData.height + (textData.anchors.topMargin * 2)
            interactive: false

            ScrollBar.vertical: WisenetScrollBar {
                id: verticalScrollBar

                parent: scrollArea

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                stepSize: verticalScrollBar.visualSize / 4
            }

            clip: true

            Rectangle{
                id: textItem
                anchors.fill: parent
                color: "white" //WisenetGui.contrast_08_dark_grey
                border.width: 1

                WisenetText{
                    id: textData
                    anchors.top: textItem.top
                    anchors.topMargin: 15
                    anchors.left: textItem.left
                    anchors.leftMargin: 15
                    text: textItemPopup.textDataItem
                    color: "black"
                }
            }
        }
    }

    ListModel{
        id: textSearchColumns
        //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
        //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
        Component.onCompleted: {
            var bookmarkColumnWidth = supportBookmark ? 40 : 0
            var textColumnWidth = supportBookmark ? 220 : 239
            append({ "columnWidth": bookmarkColumnWidth, "title": "", "sort": 0, "splitter": 0, "filter":0})
            append({ "columnWidth": 130, "title": WisenetLinguist.dateAndTime, "sort": 0, "splitter": 1, "filter":0})
            append({ "columnWidth": 100, "title": WisenetLinguist.deviceName, "sort": 1, "splitter": 1, "filter":0})
            append({ "columnWidth": 100, "title": WisenetLinguist.posName, "sort": 1, "splitter": 1, "filter":0})
            append({ "columnWidth": 120, "title": WisenetLinguist.channel, "sort": 1, "splitter": 1, "filter":0})
            append({ "columnWidth": 120, "title": WisenetLinguist.text, "sort": 3, "splitter": 1, "filter":0})
        }
    }

    DelegateChooser {
        id: textSearchTableDelegate
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
                        textSearchViewModel.setHoveredRow(row, containsMouse);
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

                    property bool bookmarkChecked: bookmarkIdRole === ""

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
                        if (!bookmarkChecked) {
                            var dateTimeId = dateTimeIdRole
                            var deviceNameId = deviceNameIdRole
                            var channelId = channelIdRole

                            var deviceUUID = textSearchViewModel.getDeviceID(deviceNameId)
                            var serviceTime = textSearchViewModel.getServiceTime(dateTimeId)

                            var chIDList = channelId.split(',')
                            textSearchPlaybackView.addBookmark(deviceUUID, parseInt(chIDList[0]) + 1, serviceTime * 1000, deviceUUID);
                        }
                        else {
                            var bookmarkId = bookmarkIdRole
                            textSearchPlaybackView.deleteBookmark(bookmarkId);
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
            delegate: Rectangle {
                id: cellRect
                implicitHeight: 30
                color: hoveredRole ? WisenetGui.contrast_08_dark_grey : backgroundColor

                MouseArea {
                    acceptedButtons: Qt.LeftButton
                    hoverEnabled: true
                    anchors.fill: parent
                    onContainsMouseChanged: {
                        textSearchViewModel.setHoveredRow(row, containsMouse);
                    }

                    onEntered: {
                        if (!enablePopup) {
                            enablePopup = true
                            //console.info("=====> enter")
                            textItemPopup.visible = true
                            textItemPopup.textDataItem = textDataIdRole
                        }
                    }

                    onExited: {
                        if (enablePopup) {
                            enablePopup = false
                            //console.info("=====> exit")
                            textItemPopup.visible = false
                            textItemPopup.textDataItem = ""
                        }
                    }

                    onDoubleClicked: {
                        var dateTimeId = dateTimeIdRole
                        var deviceNameId = deviceNameIdRole
                        var posNameId = posNameIdRole
                        var channelId = channelIdRole
                        var textDataId = textDataIdRole

                        var deviceUUID = textSearchViewModel.getDeviceID(deviceNameId)
                        var serviceTime = textSearchViewModel.getServiceTime(dateTimeId)

                        if (deviceNameId  && channelId && dateTimeId ) {
                            var chIDList = channelId.split(',')
                            var chID = (parseInt(chIDList[0]) + 1).toString()

                            textSearchPlaybackView.viewingGrid.checkNewVideoChannel(deviceUUID, chID, true, serviceTime * 1000, -1, true, chID, true)
                        }
                    }
                }

                Text {
                    visible: column === 1
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: dateTimeIdRole
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Text {
                    visible: column === 2
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: deviceNameIdRole
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Text {
                    visible: column === 3
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: posNameIdRole
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Text {
                    visible: column === 4
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: channelIdRole
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Text {
                    visible: column === 5
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: textDataIdRole
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

    TextSearchViewModel {
        id: textSearchViewModel

        onSearchStarted: {
            resultTimer.restart();
        }

        onTotalCountChanged: {
            //console.info("=====> total count: ", textSearchViewModel.totalCount)
            if (textSearchViewModel.totalCount < 1) {
                //noResultDialog.show()

            }
        }

        onSearchFinished: {
            resultTimer.stop()
            pulseView.close()
        }
    }

    Timer {
        id: resultTimer
        interval: 200
        repeat: false
        onTriggered: {
            pulseView.open();
        }
    }

    WisenetMessageDialog {
        id: noResultDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        message: WisenetLinguist.noResultMessage
    }


    WisenetTable {
        id: textSearchResultTable
        anchors.fill: parent
        anchors.bottomMargin: 77

        columns: textSearchColumns
        cellDelegate: textSearchTableDelegate
        tableModel : textSearchViewModel
        lastSortIndex : 1
        popupDepth : true
        headerBackgroundColor: WisenetGui.contrast_08_dark_grey

        borderColor: tableBorderColor
    }

    Rectangle {
        id: totalresult
        anchors.left: parent.left
        anchors.top: textSearchResultTable.bottom
        anchors.topMargin: 10
        height: 16
        width: parent.width/3
        color: WisenetGui.transparent

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            text: textSearchViewModel.totalCount.toLocaleString(Qt.locale('en_US'),'f',0) + " " + WisenetLinguist.results
            color: WisenetGui.contrast_04_light_grey
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
}
