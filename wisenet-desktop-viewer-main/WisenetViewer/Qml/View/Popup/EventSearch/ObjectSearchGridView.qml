import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetLanguage 1.0
import Qt.labs.qmlmodels 1.0

import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.DeviceSelectionTreeSourceModel 1.0
import Wisenet.DeviceFilterTreeSourceModel 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.ImageItem 1.0


Rectangle{
    id: gridListView
    visible: true
    color: backgroundColor

    property color backgroundColor: WisenetGui.contrast_09_dark
    property color tableBorderColor: WisenetGui.contrast_07_grey
    property color tableLowBorderColor: WisenetGui.contrast_08_dark_grey

    property var gridListViewModel: objectSearchViewModel.gridListViewModel
    property var bestShotViewModel: objectSearchViewModel.bestShotViewModel
    property bool supportPlayback: false
    property bool supportBookmark: false
    property bool isPopupAnchored: false
    property alias listVerticalScrollBarPos: objectSearchResultTable.verticalScrollPosition

    function setPopupPosition(isAnchored,x,y) {
//        console.log("gridListView setPopupPosition : isAnchored", isAnchored)
//        console.log("objectSearchResultTable.tableContentY",objectSearchResultTable.tableContentY)
        gridListView.isPopupAnchored = isAnchored

        anchoredPopup.x = x
        anchoredPopup.y = y + objectSearchResultTable.tableViewY - objectSearchResultTable.tableContentY
        anchoredPopup.visible = true
        console.log("gridListView onAnchoredChanged : anchoredPopup.x", anchoredPopup.x)
        console.log("gridListView onAnchoredChanged : anchoredPopup.y", anchoredPopup.y)
        console.log("gridListView onAnchoredChanged : anchoredPopup.visible", anchoredPopup.visible)
    }

    function temporaryCloseAnchoredPopup(){
        anchoredPopup.close()
    }

    function openAnchoredPopup(){
        anchoredPopup.visible = true
    }

    function closeAnchoredPopup(){
        isPopupAnchored = false
        anchoredPopup.close()
    }

    signal rowDoubleClicked(var deviceId, var channelId, var deviceTime)
    signal addBookmark(var deviceId, var channelId, var deviceTime)
    signal exportButtonClicked()

    //Table 모델 및 Delegate
    ListModel{
        id: objectSearchColumns
        //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
        //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
        Component.onCompleted: {
            var bookmarkColumnWidth = supportBookmark ? 40 : 0
            var textColumnWidth = supportBookmark ? 220 : 239
            append({ "columnWidth": bookmarkColumnWidth, "title": "", "sort": 3, "splitter": 1, "filter":0})
            append({ "columnWidth": textColumnWidth, "title": WisenetLinguist.dateAndTime, "sort": 1, "splitter": 1, "filter":0})
            append({ "columnWidth": textColumnWidth, "title": WisenetLinguist.channelName, "sort": 0, "splitter": 1, "filter":1})
            append({ "columnWidth": bookmarkColumnWidth*2, "title": WisenetLinguist.bestShot, "sort": 3, "splitter": 1, "filter":0})
            append({ "columnWidth": textColumnWidth*2, "title": WisenetLinguist.objectProperties, "sort": 3, "splitter": 1, "filter":0})
        }
    }

    DelegateChooser {
        id: objectSearchTableDelegate
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
                        gridListViewModel.setHoveredRow(row, containsMouse);
                    }
                }

                Button {
                    id: bookmarkButton
                    flat: true
                    width: 21
                    height: 20
                    anchors.centerIn: parent
                    hoverEnabled: true
                    background: Rectangle {
                        color: WisenetGui.transparent
                    }

                    property bool bookmarkChecked: bookmarkIdRole != ""

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
                        var deviceId = deviceIdRole
                        var channelId = channelIdRole
                        var deviceTime = deviceTimeRole
//                        console.log("deviceId=",deviceId)
//                        console.log("channelId=",channelId)
//                        console.log("deviceTime=",deviceTime)
                        gridListView.addBookmark(deviceId, channelId, deviceTime)
                    }
                }

                Rectangle{                    
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
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

                MouseArea {
                    acceptedButtons: Qt.LeftButton
                    hoverEnabled: true
                    anchors.fill: parent
                    onContainsMouseChanged: {
                        gridListViewModel.setHoveredRow(row, containsMouse);
                    }
                    onDoubleClicked: {
                        var deviceId = deviceIdRole
                        var channelId = channelIdRole
                        var deviceTime = deviceTimeRole

                        gridListView.rowDoubleClicked(deviceId, channelId, deviceTime)
                    }
                    onPressed: {

                        var deviceId = deviceIdRole
                        var imageUrl = imageUrlRole
//                        console.log("cellRect onPressed : deviceId=",deviceId," imageUrl=",imageUrl)
                        gridListViewModel.getBestShotImage(deviceId,imageUrl)

                        if(!isPopupAnchored)
                        {
                            bestShotTooltip.x = cellRect.width
                            bestShotTooltip.y = cellRect.height/2

                            bestShotTooltip.visible = true
                            bestShotTooltip.closeY = true

//                            console.log("objectSearchResultTable.tableContentY=",objectSearchResultTable.tableContentY)
//                            console.log("cellRect onPressed : bestShotTooltip.x=",bestShotTooltip.x," bestShotTooltip.y=",bestShotTooltip.y)
//                            console.log("cellRect onPressed : bestShotTooltip.visible=",bestShotTooltip.visible," bestShotTooltip.closeY=",bestShotTooltip.closeY)
                        }
                    }
                }

                // Popup으로 하면 끝부분 아이템을 클릭했을 때 위치가 자동으로 안잡힘.
                ToolTip{
                    id: bestShotTooltip
                    height: 150 + attributePopupFixButton.height + 10
                    width: 150*gridListViewModel.imageWidth/gridListViewModel.imageHeight

                    property bool closeY : false
                    x: cellRect.width
                    y: cellRect.height/2

                    leftPadding: 0
                    rightPadding: 0
                    topPadding: 0
                    bottomPadding: 0

                    property int propertiesFontSize : 10

                    Rectangle{
                        id: bestShotImageRect

                        anchors.fill: parent
                        color: WisenetGui.contrast_09_dark
                        border.color: WisenetGui.contrast_06_grey
                        border.width: 1

                        WisenetImageButton {
                            id: bestShotpopupFixButton
                            width:20
                            height:20
                            sourceWidth: 20
                            sourceHeight: 20
                            anchors.right: parent.right
                            anchors.rightMargin: 8
                            anchors.top: parent.top
                            anchors.topMargin: 8

                            imageNormal: WisenetImage.object_search_unfixed_default
                            imageHover: WisenetImage.object_search_unfixed_hover

                            onClicked: {
//                                console.log("bestShotpopupFixButton onClicked", bestShotpopupFixButton.clicked)
            //                                bestShotItem.anchoredChanged(true, bestShotTooltip.x + bestShotItem.x, bestShotTooltip.y + bestShotItem.y)
//                                console.log("objectSearchResultTable.tableContentY=",objectSearchResultTable.tableContentY, " bestShotTooltip.y=",bestShotTooltip.y, " cellRect.y=",cellRect.y)
                                setPopupPosition(true, bestShotTooltip.x + cellRect.x + 1, bestShotTooltip.y + cellRect.y)
                                bestShotTooltip.close()
                            }
                        }

                        ImageItem{
                            id: bestShotTooltipImage
                            anchors.top: bestShotpopupFixButton.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.topMargin: 8
                            anchors.leftMargin: 6
                            anchors.rightMargin: 6
                            anchors.bottomMargin: 7
                            image: gridListViewModel.image
                        }
                    }

                    onYChanged: {
                        if(closeY)
                            close()
                    }

                    onClosed: {
                        closeY = false
                    }

                }

                Text {
                    // 이벤트 종류를 제외한 나머지 Column Text
                    visible: true
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: displayRole
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                Rectangle{                    
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    height: 1
                    width: parent.width
                    color: tableLowBorderColor
                }
            }
        }
    }

    Connections {
        target: gridListViewModel

        onSearchFinished: {
            console.debug("gridListViewModel onSearchFinished")
            deviceFilterTreeSourceModel.setDevices(cameraList)
            deviceFilterTree.reset()
        }

        onChannelFilterChanged: {
            console.debug("gridListViewModel onChannelFilterChanged")
            deviceFilterTreeSourceModel.setDevices(cameraList)
            deviceFilterTree.reset()
        }
    }

    WisenetTable {
        id: objectSearchResultTable
        anchors.fill: parent
        anchors.bottomMargin: 77

        columns: objectSearchColumns
        cellDelegate: objectSearchTableDelegate
        tableModel : gridListViewModel
        lastSortIndex : 1
        popupDepth : true
        headerBackgroundColor: WisenetGui.contrast_08_dark_grey

        borderColor: tableBorderColor

        onFilterImageClicked: {
            if(index === 2){
                deviceFilterView.x = x
                deviceFilterView.y = y
                deviceFilterView.open()
            }
        }
    }

    Rectangle {
        id: totalresult
        anchors.left: parent.left
        anchors.top: objectSearchResultTable.bottom
        anchors.topMargin: 10
        height: 16
        width: parent.width/3
        color: WisenetGui.transparent

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            text: gridListViewModel.totalCount.toLocaleString(Qt.locale('en_US'),'f',0) + " " + WisenetLinguist.results
            color: WisenetGui.contrast_04_light_grey
        }
    }

    WisenetImageAndTextButton{
        id: exportLogButton
        anchors.right: parent.right
        anchors.top: objectSearchResultTable.bottom
        anchors.topMargin: 10
        height: 14
        width: 60
        visible: false

        onButtonClicked: {
            exportButtonClicked()
        }
    }

    function filterAll(){
        var selectedChannels = []
        if(deviceFilterTree.selectedDeviceValue !== null){
            deviceFilterTree.selectedDeviceValue.forEach(function(item){
                selectedChannels.push(item.uuid)
                console.log("item.uuid=",item.uuid)
            })
        }
        gridListViewModel.filter(selectedChannels)
    }

	// Tooltip으로 하면 mouse를 창밖에서 release하면 없어짐.
    Popup{
        id: anchoredPopup
        height: 150 + attributePopupFixButton.height + 10
        width: 150*gridListViewModel.imageWidth/gridListViewModel.imageHeight

        closePolicy: Popup.NoAutoClose

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        property int previousMouseX
        property int previousMouseY

        property int propertiesFontSize : 10

        property string deviceId
        property string channelId
        property var deviceTime

        MouseArea{
            id: popupArea

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            onPressed: {
                anchoredPopup.previousMouseX = mouseX
                anchoredPopup.previousMouseY = mouseY
            }

            onMouseXChanged: {
//                console.log("onMouseXChanged before: anchoredPopup.x=",anchoredPopup.x, " anchoredPopup.y=",anchoredPopup.y)
                var dx = mouseX - anchoredPopup.previousMouseX
                var movedX = anchoredPopup.x + dx

                if(movedX <= 5)
                    anchoredPopup.x = 5
                else if(movedX + anchoredPopup.width + 5 >= gridListView.width)
                    anchoredPopup.x = gridListView.width - anchoredPopup.width - 5
                else
                    anchoredPopup.x = anchoredPopup.x + dx

//                console.log("onMouseXChanged after: anchoredPopup.x=",anchoredPopup.x, " anchoredPopup.y=",anchoredPopup.y)
            }

            onMouseYChanged: {
//                console.log("onMouseYChanged before: anchoredPopup.x=",anchoredPopup.x, " anchoredPopup.y=",anchoredPopup.y)
                var dy = mouseY - anchoredPopup.previousMouseY
                var movedY = anchoredPopup.y + dy

                if(movedY <= 5)
                    anchoredPopup.y = 5
                else if(movedY + anchoredPopup.height + 10 >= gridListView.height)
                    anchoredPopup.y = gridListView.height - anchoredPopup.height - 10
                else
                    anchoredPopup.y = anchoredPopup.y + dy

//                console.log("onMouseYChanged after: anchoredPopup.x=",anchoredPopup.x, " anchoredPopup.y=",anchoredPopup.y)
            }
        }

        Rectangle{
            id: bestShotProperties

            anchors.fill: parent
            color: WisenetGui.contrast_08_dark_grey
            border.color: WisenetGui.contrast_06_grey
            border.width: 1

            WisenetImageButton {
                id: attributePopupFixButton
                width:20
                height:20
                sourceWidth: 20
                sourceHeight: 20
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.top: parent.top
                anchors.topMargin: 8

                imageNormal: WisenetImage.object_search_fixed_default
                imageHover: WisenetImage.object_search_fixed_hover

                onClicked: {
                    gridListView.isPopupAnchored = false
//                    console.log("bestShotProperties.attributePopupFixButton : gridListView.isPopupAnchored=",gridListView.isPopupAnchored)
                    anchoredPopup.close()
                }
            }

            ImageItem{
                id: bestShotPopupImage
                anchors.top: attributePopupFixButton.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.topMargin: 8
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                anchors.bottomMargin: 7
                image: gridListViewModel.image
            }
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
//                console.debug("ObjectSearch table filter onSelectedDeviceChanged")
                gridListView.filterAll()
            }
        }

        onOpenedChanged: {
            if(opened){
                deviceFilterTree.expandAll()
            }
        }
    }
}
