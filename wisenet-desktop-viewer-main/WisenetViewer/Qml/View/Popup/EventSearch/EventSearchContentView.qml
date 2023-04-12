import QtQuick 2.15
import QtQuick.Controls 2.15 as Control2
import WisenetStyle 1.0

import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0

Rectangle{
    id: eventSearchContentView
    visible: true
    color: "transparent"

    property int eventSearchFilterWidth : eventSearchFilterView.width
    property var eventSearchFilterViewState : eventSearchFilterView.state
    property int previousLeftBarX
    property int minimunResultViewWidth : 465
    property int minimumPlaybackViewWidth : 220

    property alias isFocusedWindow : eventSearchPlaybackView.isFocusedWindow

    function resetFilter()
    {
        eventSearchFilterView.reset()
    }

    function setChannelFilter(devices)
    {
        eventSearchFilterView.setChannelFilter(devices)
    }

    function setDateFilter(from, to)
    {
        eventSearchFilterView.setDateFilter(from, to)
    }

    function searchByFilter()
    {
        eventSearchFilterView.searchByFilter()
    }

    function initializeTree()
    {
        eventSearchFilterView.initializeTree()
    }

    Connections {
        target: eventSearch
        function onClosing(close) {
            eventSearchPlaybackView.viewingGrid.cleanUp()
        }
    }

    //Filter
    EventSearchFilterView {
        id: eventSearchFilterView
        state: "OPEN"
        x: 0
        y: 0
        width: WisenetGui.eventSearchFilterDefaultWidth
        height: parent.height

        onSearch: {
            eventSearchResultView.search(from, to, allChannel,channels, allEvent, events);
        }
        onResetListPosition : {
            eventSearchResultView.listVerticalScrollBarPos = 0
        }

        function show()
        {
            eventSearchFilterView.state = "OPEN"
            showHideAnimation.to = WisenetGui.eventSearchFilterDefaultWidth
            eventSearchFilterView.visible = true;
            showHideAnimation.restart();
        }
        function hide()
        {
            eventSearchFilterView.state = "CLOSE"
            showHideAnimation.to = 0
            showHideAnimation.restart();
        }

        NumberAnimation {
            id: showHideAnimation
            target: eventSearchFilterView
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                eventSearchFilterView.width = eventSearchFilterView.state === "OPEN" ? WisenetGui.eventSearchFilterDefaultWidth : 0;
                eventSearchFilterView.visible = eventSearchFilterView.state === "OPEN" ? true : false;
            }
        }
    }

    //Splite
    Rectangle {
        id: filterSpliteBar
        anchors.left: eventSearchFilterView.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 30
        color: WisenetGui.contrast_09_dark

        Control2.Button{
            id: putinButton
            anchors.verticalCenter: filterSpliteBar.verticalCenter
            anchors.horizontalCenter: filterSpliteBar.horizontalCenter
            width: 7
            height: 24
            hoverEnabled: true

            background: Rectangle{
                color:{
                    return WisenetGui.transparent
                }
            }

            Image {
                id: putinButtonImage
                x: 0
                y: 0
                width: 7
                height: 24
                source: {
                    if (eventSearchFilterView.state === "OPEN") {
                        return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-left-hover.svg" : "qrc:/WisenetStyle/Icon/putin-left-normal.svg"
                    }
                    return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-right-hover.svg" : "qrc:/WisenetStyle/Icon/putin-right-normal.svg"
                }
                sourceSize: "7x24"
            }

            onClicked: {
                if(eventSearchFilterView.state === "OPEN"){
                    eventSearchFilterView.hide();
                }else {
                    var newResultWidth = eventSearchResultView.width - WisenetGui.eventSearchFilterDefaultWidth;
                    var calcPlaybackWidth = WisenetGui.eventSearchFilterDefaultWidth + minimumPlaybackViewWidth;
                    // 뷰잉그리드가 큰 상태에서 접었다 펼친 경우 보정
                    if (newResultWidth < minimunResultViewWidth && eventSearchPlaybackArea.width > calcPlaybackWidth) {
                        eventSearchPlaybackArea.width = eventSearchPlaybackArea.width - WisenetGui.eventSearchFilterDefaultWidth;
                    }
                    eventSearchFilterView.show();
                }
            }
        }
    }

    onWidthChanged: {
        if (eventSearchResultView.width < minimunResultViewWidth && eventSearchPlaybackArea.width > 0) {
            var reCalcWidth = minimunResultViewWidth - eventSearchResultView.width;
            eventSearchPlaybackArea.width -= reCalcWidth
            if (eventSearchPlaybackArea.width < minimumPlaybackViewWidth) {
                 eventSearchPlaybackArea.width = minimumPlaybackViewWidth;
            }
        }
    }

    //List
    EventSearchResultView {
        id: eventSearchResultView
        anchors.left: filterSpliteBar.right
        anchors.top: parent.top
        anchors.right: resultSpliteBar.left
        //width: parent.width - filterSpliteBar.width - eventSearchFilterView.width - resultSpliteBar.width
        height: parent.height
        supportPlayback: true
        supportBookmark: true
    }

    //Splite
    Rectangle {
        id: resultSpliteBar
        //anchors.left: eventSearchResultView.right
        anchors.right: eventSearchPlaybackArea.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        visible: eventSearchPlaybackArea.width != 0
        width: visible ? 25 : 0
        color: WisenetGui.contrast_09_dark

        Image{
            id: resultSpliteImage
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 4
            height: 22
            sourceSize: Qt.size(width, height)
            source: "qrc:/WisenetStyle/Icon/view-split-normal.svg"
        }

        MouseArea{
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            hoverEnabled: true
            onPressed: {
                //eventSearchResultViewWidthBehavior.enabled = false
                previousLeftBarX = mouseX
            }

            onEntered: {
                resultSpliteImage.source = "qrc:/WisenetStyle/Icon/view-split-hover.svg"
            }
            onExited: {
                resultSpliteImage.source = "qrc:/WisenetStyle/Icon/view-split-normal.svg"
            }

            onMouseXChanged: {
                if(pressed){
                    var dx = previousLeftBarX - mouseX
                    var newPlaybackWidth = eventSearchPlaybackArea.width + dx
                    var newResultWidth = eventSearchResultView.width - dx

                    if(newPlaybackWidth < minimumPlaybackViewWidth)
                        newPlaybackWidth = minimumPlaybackViewWidth

                    if(newResultWidth < minimunResultViewWidth)
                        newPlaybackWidth = eventSearchContentView.width - filterSpliteBar.width - eventSearchFilterView.width
                                - resultSpliteBar.width - minimunResultViewWidth - eventSearchPlaybackArea.anchors.rightMargin

                    eventSearchPlaybackArea.width = newPlaybackWidth
                }
            }

            onReleased: {
                //eventSearchResultViewWidthBehavior.enabled = true
            }
        }
    }

    //ViewingGrid
    Rectangle{
        id: eventSearchPlaybackArea
        //anchors.left: resultSpliteBar.right
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 77
        width: 0
        color: WisenetGui.contrast_11_bg
        border.width: 1
        border.color: WisenetGui.contrast_08_dark_grey
        clip: true

        EventSearchPlaybackView{
            id: eventSearchPlaybackView
            anchors.fill : parent
            anchors.margins: 1
            visible: false
            onViewItemCountChanged: {
                if (viewItemCount > 0) {
                    visible = true
                    if (eventSearchPlaybackArea.width < minimumPlaybackViewWidth) {
                        widthChangeAnimation.to = minimumPlaybackViewWidth
                        widthChangeAnimation.restart();
                    }
                    else if (eventSearchPlaybackArea.width === minimumPlaybackViewWidth && viewItemCount > 1) {
                        widthChangeAnimation.to = minimumPlaybackViewWidth*2
                        widthChangeAnimation.restart();
                    }
                }
                else {
                    visible = false
                    widthChangeAnimation.to = 0
                    widthChangeAnimation.restart();
                }
            }

            function addBookmark(deviceId, channelId, deviceTime, eventLogId) {
                viewingGrid.checkNewVideoChannel(deviceId, channelId, true, deviceTime - 5000, -1, true, "", false)
                mediaControllerView.addEventBookmark(deviceId, channelId, eventLogId, deviceTime - 5000, parseInt(deviceTime) + 30000)
            }

            function deleteBookmark(bookmarkID) {
                mediaControllerView.deleteBookmark(bookmarkID)
            }
        }

        NumberAnimation {
            id: widthChangeAnimation
            target: eventSearchPlaybackArea
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                eventSearchPlaybackView.visible = eventSearchPlaybackArea.width > 0
            }
        }
    }
}
