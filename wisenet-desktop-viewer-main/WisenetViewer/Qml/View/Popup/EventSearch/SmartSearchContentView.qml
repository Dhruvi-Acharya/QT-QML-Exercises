import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import "qrc:/"

Rectangle{
    id: smartSearchContentView
    visible: true
    color: "transparent"

    property int smartSearchFilterWidth : searchFilterView.width
    property string smartSearchFilterViewState : searchFilterView.state
    property int previousRightBarX
    property int minimunResultViewWidth : 200
    property int maximunResultViewWidth : 400

    property alias isFocusedWindow : searchPlaybackView.isFocusedWindow

    /*
    onVisibleChanged: {
        if(visible)
            searchFilterView.expandAll()
    }
    */

    function resetFilter()
    {
        searchFilterView.reset()
    }

    function initializeTree()
    {
        searchFilterView.initializeTree()
    }

    function clear(){
        smartSearchViewModel.clear()
        searchPlaybackView.viewingGrid.cleanUp()
        searchResultView.hide()
    }

    //Filter
    SmartSearchFilterView {
        id: searchFilterView
        state: "OPEN"
        x: 0
        y: 0
        width: WisenetGui.eventSearchFilterDefaultWidth
        height: parent.height

        onSearch: {
            searchResultView.search(from, to, allChannel,channels, allEvent, events);
        }
        onResetListPosition : {
            searchResultView.listVerticalScrollBarPos = 0
        }

        function show()
        {
            searchFilterView.state = "OPEN"
            showHideAnimation.to = WisenetGui.eventSearchFilterDefaultWidth
            searchFilterView.visible = true;
            showHideAnimation.restart();
        }
        function hide()
        {
            searchFilterView.state = "CLOSE"
            showHideAnimation.to = 0
            showHideAnimation.restart();
        }

        NumberAnimation {
            id: showHideAnimation
            target: searchFilterView
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                searchFilterView.width = searchFilterView.state === "OPEN" ? WisenetGui.eventSearchFilterDefaultWidth : 0;
                searchFilterView.visible = searchFilterView.state === "OPEN" ? true : false;
            }
        }
    }

    //Splite
    Rectangle {
        id: filterSpliteBar
        anchors.left: searchFilterView.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 30
        color: WisenetGui.contrast_09_dark

        Button{
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
                    if (searchFilterView.state === "OPEN") {
                        return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-left-hover.svg" : "qrc:/WisenetStyle/Icon/putin-left-normal.svg"
                    }
                    return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-right-hover.svg" : "qrc:/WisenetStyle/Icon/putin-right-normal.svg"
                }
                sourceSize: "7x24"
            }

            onClicked: {
                if(searchFilterView.state === "OPEN") {
                    searchFilterView.hide();
                }
                else {
                    searchFilterView.show();
                }
            }
        }
    }

    //ViewingGrid
    Rectangle{
        id: searchPlaybackArea
        anchors.left: filterSpliteBar.right
        anchors.top: parent.top
        anchors.right: resultSpliteBar.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        color: WisenetGui.contrast_11_bg
        border.width: 1
        border.color: WisenetGui.contrast_08_dark_grey
        clip: true

        EventSearchPlaybackView {
            id: searchPlaybackView
            anchors.fill : parent
            anchors.margins: 1

            viewingGrid.isSmartSearchMode: true
            mediaControllerView.isSmartSearchMode: true

            onFocusedViewingGridItemChanged: {
                // 뷰잉그리드의 focusedItem이 변경 된 경우 (선택 채널 변경 처리)
                selectSmartSearchResultChannel(-1)
            }

            onSelectedTrackIdChanged: {
                // 타임라인에서 TrackId 선택을 변경 한 경우
                selectSmartSearchResultChannel(trackId)
            }

            function selectSmartSearchResultChannel(trackId) {
                var videoItem = viewingGrid.focusedItem

                if(!videoItem || !videoItem.isCamera) {
                    smartSearchViewModel.selectChannel("")
                    return
                }
                else {
                    var deviceChanneId = videoItem.mediaParam.deviceId + "_" + videoItem.mediaParam.channelId
                    if(trackId === -1)
                        trackId = videoItem.mediaParam.trackId
                    if(trackId === -1)
                        trackId = mediaControllerView.getDefaultOverlappedId(videoItem.mediaParam.deviceId, videoItem.mediaParam.channelId)

                    // select channel Key = deviceId_channelId_trackId
                    smartSearchViewModel.selectChannel(deviceChanneId + "_" + trackId)
                    console.log("smartSearchViewModel.selectChannel", deviceChanneId + "_" + trackId)
                }
            }

            onSmartSearchRequest: {
                // 선택 타일 기준으로 스마트검색 요청
                var videoItem = viewingGrid.focusedItem

                // 검색 범위 체크
                if(mediaControllerView.selectionEndTime == 0) {
                    // 검색 범위 미설정
                    messageDialog.message = WisenetLinguist.searchPeroidWarn
                    messageDialog.show()
                    return
                }
                else {
                    mediaControllerView.checkSelectionRangeLimit(24 * 3600 * 1000)  // 검색 범위 1일 제한
                }

                // 검색 영역 체크
                var lineList = videoItem.getVirtualLineList()
                var areaList = videoItem.getVirtualAreaList()

                if(lineList.length === 0 && areaList.length === 0) {
                    // 검색 영역 미설정
                    messageDialog.message = WisenetLinguist.searchAreaWarn
                    messageDialog.show()
                    return
                }

                // 검색 조건 초기화
                smartSearchViewModel.clearRequest()

                // 검색 파라미터 설정 (검색범위, trackID, channelId)
                var deviceChannelId = videoItem.mediaParam.deviceId + "_" + videoItem.mediaParam.channelId
                var trackId = videoItem.mediaParam.trackId
                if(trackId === -1)
                    trackId = mediaControllerView.getDefaultOverlappedId(videoItem.mediaParam.deviceId, videoItem.mediaParam.channelId)
                console.log("onSmartSearchRequest, trackId:", trackId, "deviceChannelId:", deviceChannelId)

                smartSearchViewModel.setSearchParam(mediaControllerView.selectionStartTime, mediaControllerView.selectionEndTime,
                                                    trackId, deviceChannelId)

                //console.log("onSmartSearchRequest", videoItem.mediaParam.trackId, deviceChannelId)

                lineList.forEach(function(lineData) {
                    // lineData[0]:방향, lineData[1]:좌표 array
                    if(lineData.length === 2) {
                        console.log("onSmartSearchRequest, line", lineData[0], lineData[1])
                        smartSearchViewModel.addLine(lineData[0], lineData[1])
                    }
                });

                areaList.forEach(function(areaData) {
                    // areaData[0]:areaType, areaData[1]:coordinates(array), areaData[2]:eventTypes(array), areaData[3]:aiTypes(array)
                    if(areaData.length === 4) {
                        console.log("onSmartSearchRequest, area", areaData[0], areaData[1], areaData[2], areaData[3])
                        smartSearchViewModel.addArea(areaData[0], areaData[1], areaData[2], areaData[3])
                    }
                });

                pulseView.open()
                smartSearchViewModel.searchRequest()    // 장비에 검색 요청
            }

            onOpenMessageDialog: {
                messageDialog.message = message
                messageDialog.show()
            }
        }

        WisenetMessageDialog {
            id: messageDialog
            applyButtonVisible: false
            cancelButtonText: WisenetLinguist.close
        }
    }

    //Splite
    Rectangle {
        id: resultSpliteBar
        anchors.right: searchResultView.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 30
        color: WisenetGui.contrast_09_dark

        MouseArea{
            anchors.fill: parent
            cursorShape: searchResultView.state === "OPEN" ? Qt.SizeHorCursor : Qt.ArrowCursor
            hoverEnabled: true
            enabled: searchResultView.state === "OPEN"

            onPressed: {
                previousRightBarX = mouseX
            }

            onMouseXChanged: {
                if(pressed){
                    var dx = previousRightBarX - mouseX
                    var newResultWidth = searchResultView.width + dx

                    if(newResultWidth < minimunResultViewWidth)
                        newResultWidth = minimunResultViewWidth
                    if(newResultWidth > maximunResultViewWidth)
                        newResultWidth = maximunResultViewWidth

                    searchResultView.width = newResultWidth
                }
            }

            onReleased: {
                //eventSearchResultViewWidthBehavior.enabled = true
            }
        }

        Button{
            anchors.verticalCenter: resultSpliteBar.verticalCenter
            anchors.horizontalCenter: resultSpliteBar.horizontalCenter
            width: 7
            height: 24
            hoverEnabled: true

            background: Rectangle{
                color:{
                    return WisenetGui.transparent
                }
            }

            Image {
                x: 0
                y: 0
                width: 7
                height: 24
                source: {
                    if (searchResultView.state === "OPEN")
                        return parent.hovered ? "qrc:/WisenetStyle/Icon/putin-right-hover.svg" : "qrc:/WisenetStyle/Icon/putin-right-normal.svg"
                    else
                        return parent.hovered ? "qrc:/WisenetStyle/Icon/putin-left-hover.svg" : "qrc:/WisenetStyle/Icon/putin-left-normal.svg"
                }
                sourceSize: "7x24"
            }

            onClicked: {
                if(searchResultView.state === "OPEN"){
                    searchResultView.hide();
                }
                else {
                    searchResultView.show();
                }
            }
        }
    }

    //List
    SmartSearchResultView {
        id: searchResultView
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        width: 0
        state: "CLOSE"

        property int prevWidth: maximunResultViewWidth

        function show()
        {
            if(searchResultView.state == "OPEN")
                return

            searchResultView.anchors.rightMargin = 30
            searchResultView.state = "OPEN"
            resultViewAnimation.to = prevWidth
            searchResultView.visible = true;
            resultViewAnimation.restart();
        }

        function hide()
        {
            if(searchResultView.state == "CLOSE")
                return

            prevWidth = searchResultView.width
            searchResultView.state = "CLOSE"
            resultViewAnimation.to = 0
            resultViewAnimation.restart();
        }

        NumberAnimation {
            id: resultViewAnimation
            target: searchResultView
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                searchResultView.width = searchResultView.state === "OPEN" ? searchResultView.prevWidth : 0;
                searchResultView.visible = searchResultView.state === "OPEN" ? true : false;
                searchResultView.anchors.rightMargin = searchResultView.state === "OPEN" ? 30 : 0
            }
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

    WisenetMessageDialog{
        id: successMsgDialog
        visible: false
        width: 400
        height: 200
        message: WisenetLinguist.noData
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    Connections {
        target: smartSearchViewModel
        function onResultUpdated() {
            // 로딩화면 close
            pulseView.close()

            // 타임라인 업데이트
            var resultList = smartSearchViewModel.getSelectedChannelResult()
            searchPlaybackView.mediaControllerView.setSmartSearchResult(resultList)
        }

        function onSmartSearchFinished() {
            // 검색 완료 signal 수신
            if(searchResultView.state !== "OPEN")
                searchResultView.show();    // 결과 테이블 open
        }

        function onSmartSearchNoData(isSuccess){
            console.log("onSmartSearchNoData" + isSuccess)

            if(isSuccess === true)
                successMsgDialog.message = WisenetLinguist.noData
            else
                successMsgDialog.message = WisenetLinguist.smartSearchNoMetadataFound

            successMsgDialog.visible = true
        }
    }
}
