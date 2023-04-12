import QtQuick 2.15
import QtQuick.Controls 2.15 as Control2
import WisenetStyle 1.0

import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0

import "qrc:/"

Rectangle{
    id: textSearchContentView
    visible: true
    color: "transparent"

    property int textSearchFilterWidth : textSearchFilterView.width
    property var textSearchFilterViewState : textSearchFilterView.state
    property int previousLeftBarX
    property int minimunResultViewWidth : 420
    property int minimumPlaybackViewWidth : 300
    property alias currentIndex : textSearchResultView.currentIndex

    property alias isFocusedWindow : textSearchPlaybackView.isFocusedWindow

    signal zoomInOutStepValue(var stepValue)
    function setZoomInOutStepValue(stepValue){
        textSearchResultView.setZoomInOutStepValue(stepValue)
    }

    function resetFilter()
    {
        textSearchFilterView.reset()
    }

    function setChannelFilter(devices)
    {
        textSearchFilterView.setChannelFilter(devices)
    }

    function setDateFilter(from, to)
    {
        textSearchFilterView.setDateFilter(from, to)
    }

    function searchByFilter()
    {
        textSearchFilterView.searchByFilter()
    }

    function initializeTree()
    {
        textSearchFilterView.initializeTree()
    }

    function clear()
    {
        textSearchPlaybackView.viewingGrid.cleanUp()
    }

    onCurrentIndexChanged: {
        if (currentIndex == 0) {
            // 썸네일(=영수증)보기 일때는 뷰잉그리드 숨김
            textSearchPlaybackArea.width = 0
        }
        else {
            // 리스트뷰 보기 일때는 뷰잉그리드 보이도록..
            textSearchPlaybackArea.width = minimumPlaybackViewWidth
        }
    }

    // Filter
    TextSearchFilterView {
        id: textSearchFilterView
        state: "OPEN"
        x: 0
        y: 0
        width: WisenetGui.eventSearchFilterDefaultWidth
        height: parent.height

        onSearch: {
            textSearchResultView.search(allDevice, channels, keyword, from, to, isWholeWord, isCaseSensitive)
        }

        onResetListPosition : {
            //textSearchResultView.listVerticalScrollBarPos = 0
        }

        function show()
        {
            textSearchFilterView.state = "OPEN"
            showHideAnimation.to = WisenetGui.eventSearchFilterDefaultWidth
            textSearchFilterView.visible = true;
            showHideAnimation.restart();
        }
        function hide()
        {
            textSearchFilterView.state = "CLOSE"
            showHideAnimation.to = 0
            showHideAnimation.restart();
        }

        NumberAnimation {
            id: showHideAnimation
            target: textSearchFilterView
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                textSearchFilterView.width = textSearchFilterView.state === "OPEN" ? WisenetGui.eventSearchFilterDefaultWidth : 0;
                textSearchFilterView.visible = textSearchFilterView.state === "OPEN" ? true : false;
            }
        }
    }

    // Splite - Left
    Rectangle {
        id: filterSpliteBar
        anchors.left: textSearchFilterView.right
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
                    if (textSearchFilterView.state === "OPEN") {
                        return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-left-hover.svg" : "qrc:/WisenetStyle/Icon/putin-left-normal.svg"
                    }
                    return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-right-hover.svg" : "qrc:/WisenetStyle/Icon/putin-right-normal.svg"
                }
                sourceSize: "7x24"
            }

            onClicked: {
                if(textSearchFilterView.state === "OPEN"){
                    textSearchFilterView.hide();
                }else {
                    var newResultWidth = textSearchResultView.width - WisenetGui.eventSearchFilterDefaultWidth;
                    var calcPlaybackWidth = WisenetGui.eventSearchFilterDefaultWidth + minimumPlaybackViewWidth;
                    // 뷰잉그리드가 큰 상태에서 접었다 펼친 경우 보정
                    if (newResultWidth < minimunResultViewWidth && textSearchPlaybackArea.width > calcPlaybackWidth) {
                        textSearchPlaybackArea.width = textSearchPlaybackArea.width - WisenetGui.eventSearchFilterDefaultWidth;
                    }
                    textSearchFilterView.show();
                }
            }
        }
    }

    onWidthChanged: {
        if (textSearchResultView.width < minimunResultViewWidth && textSearchPlaybackArea.width > 0) {
            var reCalcWidth = minimunResultViewWidth - textSearchResultView.width;
            textSearchPlaybackArea.width -= reCalcWidth
            if (textSearchPlaybackArea.width < minimumPlaybackViewWidth) {
                textSearchPlaybackArea.width = minimumPlaybackViewWidth;
            }
        }
    }

    // List
    TextSearchResultView {
        id: textSearchResultView
        anchors.left: filterSpliteBar.right
        anchors.top: parent.top
        anchors.right: resultSpliteBar.left
        width: parent.width - filterSpliteBar.width - textSearchFilterView.width - resultSpliteBar.width
        height: parent.height
//        supportPlayback: true
//        supportBookmark: true
    }

    // Splite - Right
    Rectangle {
        id: resultSpliteBar
        //anchors.left: textSearchResultView.right
        anchors.right: textSearchPlaybackArea.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        visible: textSearchPlaybackArea.width != 0
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
                if (pressed){
                    var dx = previousLeftBarX - mouseX
                    var newPlaybackWidth = textSearchPlaybackArea.width + dx
                    var newResultWidth = textSearchResultView.width - dx

                    if (newPlaybackWidth < minimumPlaybackViewWidth)
                        newPlaybackWidth = minimumPlaybackViewWidth

                    if (newResultWidth < minimunResultViewWidth)
                        newPlaybackWidth = textSearchContentView.width - filterSpliteBar.width - textSearchFilterView.width
                                - resultSpliteBar.width - minimunResultViewWidth - textSearchPlaybackArea.anchors.rightMargin

                    textSearchPlaybackArea.width = newPlaybackWidth
                }
            }

            onReleased: {
                //eventSearchResultViewWidthBehavior.enabled = true
            }
        }
    }

    // ViewingGrid
    Rectangle{
        id: textSearchPlaybackArea
        //anchors.left: resultSpliteBar.right
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.bottomMargin: 77
        width: 0
        height: parent.height / 2.2
        color: WisenetGui.contrast_11_bg
        border.width: 1
        border.color: WisenetGui.contrast_08_dark_grey
        clip: true

        TextSearchPlaybackView{
            id: textSearchPlaybackView
            anchors.fill : parent
            anchors.margins: 1

            viewingGrid.isSmartSearchMode: false
            mediaControllerView.isSmartSearchMode: false

            onViewItemCountChanged: {
                if (viewItemCount > 0) {
                    visible = true
                    if (textSearchPlaybackArea.width < minimumPlaybackViewWidth) {
                        widthChangeAnimation.to = minimumPlaybackViewWidth
                        widthChangeAnimation.restart();
                    }
                    else if (textSearchPlaybackArea.width === minimumPlaybackViewWidth && viewItemCount > 1) {
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
                viewingGrid.checkNewVideoChannel(deviceId, channelId, true, deviceTime - 5000, -1, true, channelId, true)

                mediaControllerView.addEventBookmark(deviceId, channelId, eventLogId, deviceTime - 5000, parseInt(deviceTime) + 30000)
            }

            function deleteBookmark(bookmarkID) {
                mediaControllerView.deleteBookmark(bookmarkID)
            }
        }

        NumberAnimation {
            id: widthChangeAnimation
            target: textSearchPlaybackArea
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                textSearchPlaybackView.visible = textSearchPlaybackArea.width > 0
            }
        }
    }

    // TextListView
    Rectangle{
        id: textListViewArea
        anchors.top: textSearchPlaybackArea.bottom
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 25
        width: textSearchPlaybackArea.width // 뷰잉그리드 Width에 맞게 조절되게..
        height: parent.height / 2.2
        color: WisenetGui.contrast_11_bg
        border.width: 1
        border.color: WisenetGui.contrast_08_dark_grey
        clip: true
        visible: textSearchPlaybackArea.visible // 뷰잉그리드 표시 여부에 따라 함께 움직이게..

        TextListView {
            id: textListView
            anchors.fill: parent
        }
    }
}
