import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

Rectangle {
    id: objectSearchContentView
    color: "transparent"

    property var aiSearchFilterViewState : objectSearchFilterView.state
    property int previousLeftBarX
    property int minimunResultViewWidth : 490
    property int minimumPlaybackViewWidth : 220
    property alias currentIndex : objectSearchResultView.currentIndex

    
    signal bestShotHeightChanged(var height)
    function setBestShotHeight(height){
        objectSearchResultView.setBestShotHeight(height)
    }

    function closeAnchoredPopup(){
        objectSearchResultView.closeAnchoredPopup()
    }

    function clear(){
        objectSearchViewModel.clear()
        objectSearchFilterView.reset()
        objectSearchPlaybackView.viewingGrid.cleanUp()
    }

    function initializeTree() {
        objectSearchFilterView.initializeTree()
    }

    ObjectSearchFilterView{
        id: objectSearchFilterView
        state: "OPEN"

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        width: WisenetGui.eventSearchFilterDefaultWidth

        function show()
        {
            objectSearchFilterView.state = "OPEN"
            showHideAnimation.to = WisenetGui.eventSearchFilterDefaultWidth
            objectSearchFilterView.visible = true;
            showHideAnimation.restart();
        }
        function hide()
        {
            objectSearchFilterView.state = "CLOSE"
            showHideAnimation.to = 0
            showHideAnimation.restart();
        }

        NumberAnimation {
            id: showHideAnimation
            target: objectSearchFilterView
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                objectSearchFilterView.width = objectSearchFilterView.state === "OPEN" ? WisenetGui.eventSearchFilterDefaultWidth : 0;
                objectSearchFilterView.visible = objectSearchFilterView.state === "OPEN" ? true : false;
            }
        }

        onResetListPosition: {
            objectSearchResultView.objectSearchGridView.listVerticalScrollBarPos = 0
            objectSearchPlaybackView.viewingGrid.cleanUp()
        }
    }

    Rectangle {
        id: filterSpliteBar
        anchors.left: objectSearchFilterView.right
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
                    if (objectSearchFilterView.state === "OPEN") {
                        return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-left-hover.svg" : "qrc:/WisenetStyle/Icon/putin-left-normal.svg"
                    }
                    return putinButton.hovered ? "qrc:/WisenetStyle/Icon/putin-right-hover.svg" : "qrc:/WisenetStyle/Icon/putin-right-normal.svg"
                }
                sourceSize: "7x24"
            }

            onClicked: {
                if(objectSearchFilterView.state === "OPEN"){
                    objectSearchFilterView.hide();
                }else {
                    var newResultWidth = objectSearchResultView.width - WisenetGui.eventSearchFilterDefaultWidth;
                    var calcPlaybackWidth = WisenetGui.eventSearchFilterDefaultWidth + minimumPlaybackViewWidth;
                    // 뷰잉그리드가 큰 상태에서 접었다 펼친 경우 보정
                    if (newResultWidth < minimunResultViewWidth && objectSearchPlaybackArea.width > calcPlaybackWidth) {
                        objectSearchPlaybackArea.width = objectSearchPlaybackArea.width - WisenetGui.eventSearchFilterDefaultWidth;
                    }
                    objectSearchFilterView.show();
                }
            }
        }
    }

    onWidthChanged: {
        if (objectSearchResultView.width < minimunResultViewWidth && objectSearchPlaybackArea.width > 0) {
            var reCalcWidth = minimunResultViewWidth - objectSearchResultView.width;
            objectSearchPlaybackArea.width -= reCalcWidth
            if (objectSearchPlaybackArea.width < minimumPlaybackViewWidth) {
                objectSearchPlaybackArea.width = minimumPlaybackViewWidth;
            }
        }
    }


    ObjectSearchResultView{
        id: objectSearchResultView

        anchors.left: filterSpliteBar.right
        anchors.top: parent.top
        anchors.right: resultSpliteBar.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 25
        color: WisenetGui.contrast_09_dark

        onBestShotHeightChanged: objectSearchContentView.bestShotHeightChanged(height)
        onCloseLoading: pulseView.close()

        onBestshotDoubleClicked:{
            console.log("objectSearchResultView.BestshotDoubleClicked", deviceId, channelId, deviceTime)
            var playbackTime = deviceTime - 5000
            objectSearchPlaybackView.viewingGrid.checkNewVideoChannel(deviceId, channelId, true, playbackTime, -1, true, "", false)
        }

        onAddBookmark:{
            objectSearchPlaybackView.addBookmark(deviceId, channelId, deviceTime)
        }
    }

    Text{
        id: bestShotCount
        anchors.right: resultSpliteBar.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5

        width: 60
        height: 10
        color: "white"
        font.pixelSize: 11
        visible: eventListViewModel.eventTestVisible

        text: objectSearchViewModel.bestShotViewModel.bestShotCount

        horizontalAlignment: Text.AlignHCenter
    }

    Popup {
        id: pulseView

        width: 100
        height: 185
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose
        visible: false

        background: Rectangle{
            color: "transparent"
        }

        contentItem: Rectangle{
            width: 100
            height: 180
            color: "transparent"

            WisenetMediaBusyIndicator {
                id: processingIndicator

                width: 100
                height: 100
                sourceWidth: 100
                sourceHeight: 100
                visible:true
                running:true
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle{
                id: countRect
                width: received.width + slashText.width + total.width
                height: 30
                anchors.top: processingIndicator.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter

                color: "transparent"

                Text{
                    id: received
                    anchors.top: parent.top
                    anchors.left: parent.left
                    color: WisenetGui.contrast_01_light_grey
                    text: objectSearchViewModel.receivedMetaCount
                }

                Text{
                    id: slashText
                    anchors.top: parent.top
                    anchors.left: received.right
                    color: WisenetGui.contrast_01_light_grey
                    text: "/"
                }

                Text{
                    id: total
                    anchors.top: parent.top
                    anchors.left: slashText.right
                    color: WisenetGui.contrast_01_light_grey
                    text: objectSearchViewModel.totalMetaCount
                }
            }

            WisenetButton2{
                id: cancelButton

                anchors.top: countRect.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                width: 120
                height: 25

                text: WisenetLinguist.cancel

                onClicked: objectSearchViewModel.cancel()
            }
        }
    }

    Popup {
        id: bestshotPulseView

        width: 100
        height: 100
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose
        visible: false

        background: Rectangle{
            color: "transparent"
        }

        contentItem: WisenetMediaBusyIndicator {
            id: bestshotProcessingIndicator

            width: 100
            height: 100
            sourceWidth: 100
            sourceHeight: 100

            visible:true
            running:true
            anchors.fill: parent
        }
    }

    Connections{
        target: objectSearchViewModel

        function onOpenLoading(){
            pulseView.open()
        }

        function onCloseLoading(){
            pulseView.close()
        }

        function onPopupNoResult(){
            noResultDialog.show()
        }
    }

    Connections{
        target: objectSearchViewModel.bestShotViewModel

        function onOpenLoading(){
            bestshotPulseView.open()
        }

        function onCloseLoading(){
            bestshotPulseView.close()
            pulseView.close()
        }
    }


    Rectangle {
        id: resultSpliteBar
        //anchors.left: eventSearchResultView.right
        anchors.right: objectSearchPlaybackArea.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        visible: objectSearchPlaybackArea.width != 0
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
                    var newPlaybackWidth = objectSearchPlaybackArea.width + dx
                    var newResultWidth = objectSearchResultView.width - dx

                    if(newPlaybackWidth < minimumPlaybackViewWidth || newResultWidth < minimunResultViewWidth)
                        return;

                    objectSearchPlaybackArea.width = newPlaybackWidth
                }
            }

            onReleased: {
                //eventSearchResultViewWidthBehavior.enabled = true
            }
        }
    }

    Rectangle{
        id: objectSearchPlaybackArea
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
            id: objectSearchPlaybackView
            anchors.fill : parent
            anchors.margins: 1
            visible: false
            onViewItemCountChanged: {
                if (viewItemCount > 0) {
                    visible = true
                    if (objectSearchPlaybackArea.width < minimumPlaybackViewWidth) {
                        widthChangeAnimation.to = minimumPlaybackViewWidth
                        widthChangeAnimation.restart();
                    }
                    else if (objectSearchPlaybackArea.width === minimumPlaybackViewWidth && viewItemCount > 1) {
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

            function addBookmark(deviceId, channelId, deviceTime) {
                viewingGrid.checkNewVideoChannel(deviceId, channelId, true, deviceTime - 5000, -1, true, "", false)
                mediaControllerView.addEventBookmark(deviceId, channelId, "", deviceTime - 5000, parseInt(deviceTime) + 30000)
            }
        }

        NumberAnimation {
            id: widthChangeAnimation
            target: objectSearchPlaybackArea
            properties: "width"
            duration: 300
            easing.type: Easing.OutQuad
            onStopped: {
                objectSearchPlaybackView.visible = objectSearchPlaybackArea.width > 0
            }
        }
    }

    WisenetMessageDialog {
        id: noResultDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        message: WisenetLinguist.noResultMessage
    }
}
