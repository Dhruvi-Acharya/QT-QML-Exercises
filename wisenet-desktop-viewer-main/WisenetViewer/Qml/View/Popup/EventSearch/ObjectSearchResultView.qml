import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetLanguage 1.0

Rectangle {
    id: objectSearchResultView

    property int scrollRightMargin : 15
    property int zoomStep : 10
    property int bestShotViewTopMargin : 18
    property var bestShotViewModel : objectSearchViewModel.bestShotViewModel
    property var flagViewModel : objectSearchViewModel.flagViewModel
    property alias currentIndex : stackView.currentIndex
    property var objectSearchGridView: gridListView

    signal bestShotHeightChanged(var height)
    function setBestShotHeight(height){
        bestShotViewModel.bestShotHeight = height
        flagViewModel.bestShotHeight = height
    }

    signal closeLoading()
    signal bestshotDoubleClicked(var deviceId, var channelId, var deviceTime)
    signal addBookmark(var deviceId, var channelId, var deviceTime)

    function closeAnchoredPopup(){
        scrollArea.isPopupAnchored = false
        anchoredBestShotPopup.close()

        gridListView.closeAnchoredPopup()
    }

    StackLayout {
        id: stackView

        anchors.fill: parent

        Rectangle{
            id: bestShotView
            color: "transparent"

            Rectangle{
                id: upperBorder

                height:1
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                color: WisenetGui.contrast_08_dark_grey

            }

            Flickable{
                id: scrollArea

                anchors.fill: parent
                anchors.topMargin: bestShotViewTopMargin

                property bool isPopupAnchored : false

                contentHeight: bestShotViewModel.viewHeight
                interactive: false

                ScrollBar.vertical: WisenetScrollBar {
                    id: verticalScrollBar

                    parent: scrollArea

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    stepSize: verticalScrollBar.visualSize/4

                    onPositionChanged: {
                        //console.log("verticalScrollBar.position", verticalScrollBar.position, "verticalScrollBar.visualSize", verticalScrollBar.visualSize)

                        if(verticalScrollBar.position + verticalScrollBar.visualSize == 1.0){
                            scrollBottomTimer.restart()
                        }
                    }
                }

                onHeightChanged: {
                    if(contentHeight < scrollArea.height && contentHeight != 0)
                    {
                        console.log("onHeightChanged2", contentHeight, scrollArea.height)
                        bestShotViewModel.loadMoreBestShots(false)
                    }
                }

                Connections{
                    target: objectSearchViewModel.bestShotViewModel

                    function onViewHeightChanged(viewHeight){
                        if(bestShotViewModel.viewHeight < scrollArea.height && bestShotViewModel.viewHeight !== 0)
                        {
                            console.log("onViewHeightChanged loadMoreBestShots()", bestShotViewModel.viewHeight, scrollArea.height)
                            bestShotViewModel.loadMoreBestShots(false)
                        }
                    }

                    function onSelectedItemChanged(itemModel){
                        anchoredBestShotPopup.setProperties(itemModel)
                    }
                }

                Connections{
                    target: objectSearchViewModel.flagViewModel

                    function onSelectedItemChanged(itemModel){
                        anchoredBestShotPopup.setProperties(itemModel)
                    }
                }

                clip: true

                // for Keyboard event
                focus: true
                Keys.onUpPressed: verticalScrollBar.decrease()
                Keys.onDownPressed: verticalScrollBar.increase()

                Repeater{
                    id: bestShotRepeater

                    //model: 3
                    model: bestShotViewModel.bestShotListModel

                    width: bestShotViewModel.viewWidth
                    height: bestShotViewModel.viewHeight

                    BestShotItem{
                        id: bestShotItem

                        filterTooltip: bestShotFilterTooltip
                        itemModel: model
                        viewModel: bestShotViewModel
                        isPopupAnchored: scrollArea.isPopupAnchored

                        scrollYPosition: scrollArea.contentY
                        parentHeight: scrollArea.height

                        onAnchoredChanged: {
                            scrollArea.isPopupAnchored = isAnchored

                            anchoredBestShotPopup.x = x
                            anchoredBestShotPopup.y = y - scrollArea.contentY + bestShotViewTopMargin
                            anchoredBestShotPopup.visible = true
                        }

                        onBestshotDoubleClicked: objectSearchResultView.bestshotDoubleClicked(deviceId, channelId, deviceTime)
                        onAddBookmark: objectSearchResultView.addBookmark(deviceId, channelId, deviceTime)
                    }
                }

                // Bestshot 확대/축소
                MouseArea{
                    anchors.fill: parent

                    onPressed: {
                        mouse.accepted = false
                    }

                    onWheel: {
                        if (wheel.modifiers & Qt.ControlModifier){
                            console.log("onWheel", wheel.pixelDelta.y, wheel.angleDelta.y)

                            if(wheel.angleDelta.y > 0)
                            {
                                if(bestShotViewModel.bestShotHeight + zoomStep <= 500)
                                    objectSearchResultView.bestShotHeightChanged(bestShotViewModel.bestShotHeight + zoomStep)
                            }
                            else
                            {
                                if(bestShotViewModel.bestShotHeight - zoomStep >= 100)
                                    objectSearchResultView.bestShotHeightChanged(bestShotViewModel.bestShotHeight - zoomStep)
                            }

                            wheel.accepted = true
                        }
                        else
                        {
                            if(wheel.angleDelta.y > 0)
                                verticalScrollBar.decrease()
                            else
                                verticalScrollBar.increase()

                            wheel.accepted = false
                        }
                    }
                }
            }

        }
        ObjectSearchGridView {
            id: gridListView
            supportPlayback: true
            supportBookmark: true

            onAddBookmark: objectSearchResultView.addBookmark(deviceId, channelId, deviceTime)
            onRowDoubleClicked: objectSearchResultView.bestshotDoubleClicked(deviceId, channelId, deviceTime)
        }

        Rectangle{
            id: flagView
            color: "transparent"

            Rectangle{
                id: flagViewUpperBorder

                height:1
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                color: WisenetGui.contrast_08_dark_grey

            }

            Flickable{
                id: flagArea

                anchors.fill: parent
                anchors.topMargin: bestShotViewTopMargin

                contentHeight: flagViewModel.viewHeight
                interactive: false

                ScrollBar.vertical: WisenetScrollBar {
                    id: flagVerticalScrollBar

                    parent: flagArea

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    stepSize: flagVerticalScrollBar.visualSize/4
                }

                clip: true
                focus: true
                Keys.onUpPressed: flagVerticalScrollBar.decrease()
                Keys.onDownPressed: flagVerticalScrollBar.increase()

                Repeater{
                    id: flagRepeater

                    //model: 3
                    model: flagViewModel.bestShotListModel

                    width: flagViewModel.viewWidth
                    height: flagViewModel.viewHeight

                    BestShotItem{
                        id: flagItem

                        filterTooltip: bestShotFilterTooltip
                        itemModel: model
                        viewModel: flagViewModel
                        isPopupAnchored: scrollArea.isPopupAnchored

                        scrollYPosition: flagArea.contentY
                        parentHeight: flagArea.height

                        onAnchoredChanged: {
                            scrollArea.isPopupAnchored = isAnchored

                            anchoredBestShotPopup.x = x
                            anchoredBestShotPopup.y = y - flagArea.contentY + bestShotViewTopMargin
                            anchoredBestShotPopup.visible = true
                        }

                        onBestshotDoubleClicked: objectSearchResultView.bestshotDoubleClicked(deviceId, channelId, deviceTime)
                        onAddBookmark: objectSearchResultView.addBookmark(deviceId, channelId, deviceTime)
                    }
                }

                // Bestshot 확대/축소
                MouseArea{
                    anchors.fill: parent
                    //hoverEnabled: true

                    onPressed: {
                        mouse.accepted = false
                    }

                    onWheel: {
                        if (wheel.modifiers & Qt.ControlModifier){
                            console.log("onWheel", wheel.pixelDelta.y, wheel.angleDelta.y)

                            if(wheel.angleDelta.y > 0)
                            {
                                bestShotViewModel.bestShotHeight += 5
                                flagViewModel.bestShotHeight += 5
                            }
                            else
                            {
                                bestShotViewModel.bestShotHeight -= 5
                                flagViewModel.bestShotHeight -= 5
                            }

                            wheel.accepted = true
                        }
                        else
                        {
                            if(wheel.angleDelta.y > 0)
                                flagVerticalScrollBar.decrease()
                            else
                                flagVerticalScrollBar.increase()

                            wheel.accepted = false
                        }
                    }
                }
            }
        }

        // Tooltip으로 하면 mouse를 창밖에서 release하면 없어짐.
        Popup{
            id: anchoredBestShotPopup
            height: 12 + deviceNameText.height + 4 + datetimeText.height + 13 + propertyTitleText.height + 6 + (personProperties.visible ? personProperties.height : faceProperties.visible ? faceProperties.height : vehicleProperties.visible ? vehicleProperties.height : ocrProperties.height) + 24
            width: 250

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
                    anchoredBestShotPopup.previousMouseX = mouseX
                    anchoredBestShotPopup.previousMouseY = mouseY
                }

                onMouseXChanged: {
                    var dx = mouseX - anchoredBestShotPopup.previousMouseX
                    var movedX = anchoredBestShotPopup.x + dx

                    if(movedX <= 1)
                        anchoredBestShotPopup.x = 1
                    else if(movedX + anchoredBestShotPopup.width + objectSearchResultView.scrollRightMargin + 1 >= bestShotView.width)
                        anchoredBestShotPopup.x = bestShotView.width - anchoredBestShotPopup.width - (objectSearchResultView.scrollRightMargin + 1)
                    else
                        anchoredBestShotPopup.x = anchoredBestShotPopup.x + dx
                }

                onMouseYChanged: {
                    var dy = mouseY - anchoredBestShotPopup.previousMouseY
                    var movedY = anchoredBestShotPopup.y + dy

                    if(movedY <= bestShotViewTopMargin + 1)
                        anchoredBestShotPopup.y = bestShotViewTopMargin + 1
                    else if(movedY + anchoredBestShotPopup.height + 1 >= bestShotView.height)
                        anchoredBestShotPopup.y = bestShotView.height - anchoredBestShotPopup.height - 1
                    else
                        anchoredBestShotPopup.y = anchoredBestShotPopup.y + dy
                }
            }

            Rectangle{
                id: bestShotProperties

                anchors.fill: parent
                color: WisenetGui.contrast_08_dark_grey
                border.color: WisenetGui.contrast_06_grey
                border.width: 1

                Text{
                    id: deviceNameText

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 12
                    anchors.leftMargin: 15

                    font.pixelSize: 14
                    font.bold: true
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: datetimeText

                    anchors.top: deviceNameText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 4
                    anchors.leftMargin: 15

                    font.pixelSize: 12
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: propertyTitleText

                    anchors.top: datetimeText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 13
                    anchors.leftMargin: 15

                    font.pixelSize: 12
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Rectangle{
                    id: personProperties

                    anchors.top: propertyTitleText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 6
                    anchors.leftMargin: 15

                    height: 30 + personGenderText.height + personClothingTopText.height + personClothingBottomText.height + personBagText.height

                    Text{
                        id: personGenderText

                        anchors.top: parent.top
                        anchors.left: parent.left

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }

                    Text{
                        id: personClothingTopText

                        anchors.top: personGenderText.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 9

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }

                    Text{
                        id: personClothingBottomText

                        anchors.top: personClothingTopText.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 9

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }

                    Text{
                        id: personBagText

                        anchors.top: personClothingBottomText.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 9

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }
                }

                Rectangle{
                    id: faceProperties

                    anchors.top: propertyTitleText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 6
                    anchors.leftMargin: 15

                    height: 30 + faceGenderText.height + faceAgeText.height + faceGlassesText.height + faceMaskText.height

                    Text{
                        id: faceGenderText

                        anchors.top: parent.top
                        anchors.left: parent.left

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }

                    Text{
                        id: faceAgeText

                        anchors.top: faceGenderText.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 9

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }

                    Text{
                        id: faceGlassesText

                        anchors.top: faceAgeText.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 9

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }

                    Text{
                        id: faceMaskText

                        anchors.top: faceGlassesText.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 9

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }
                }

                Rectangle{
                    id: vehicleProperties

                    anchors.top: propertyTitleText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 6
                    anchors.leftMargin: 15

                    height: 10 + vehicleTypeText.height + vehicleColorText.height

                    Text{
                        id: vehicleTypeText

                        anchors.top: parent.top
                        anchors.left: parent.left

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }

                    Text{
                        id: vehicleColorText

                        anchors.top: vehicleTypeText.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 9

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }
                }

                Rectangle{
                    id: ocrProperties

                    anchors.top: propertyTitleText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 6
                    anchors.leftMargin: 15

                    height: ocrNumberText.height

                    Text{
                        id: ocrNumberText

                        anchors.top: parent.top
                        anchors.left: parent.left

                        font.pixelSize: anchoredBestShotPopup.propertiesFontSize
                        color: WisenetGui.contrast_01_light_grey

                        text: " "
                    }
                }

                WisenetImageButton {
                    id: attributePopupCloseButton
                    width:20
                    height:20
                    sourceWidth: 20
                    sourceHeight: 20
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.top: parent.top
                    anchors.topMargin: 12

                    imageNormal: WisenetImage.object_search_fixed_default
                    imageHover: WisenetImage.object_search_fixed_hover

                    onClicked: {
                        objectSearchResultView.closeAnchoredPopup()
                    }
                }

                WisenetImageButton {
                    id: addBookmarkButton
                    width:20
                    height:20
                    sourceWidth: 20
                    sourceHeight: 20
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 12

                    imageNormal: WisenetImage.object_search_add_bookmark_default
                    imageHover: WisenetImage.object_search_add_bookmark_hover

                    onClicked: objectSearchResultView.addBookmark(anchoredBestShotPopup.deviceId, anchoredBestShotPopup.channelId, anchoredBestShotPopup.deviceTime)
                }
            }

            function setProperties(itemModel){
                console.log("bestShotProperties.setProperties", visible, itemModel.attributeType)
                anchoredBestShotPopup.deviceId = itemModel.deviceId
                anchoredBestShotPopup.channelId = itemModel.channelId
                anchoredBestShotPopup.deviceTime = itemModel.deviceTime
                personProperties.visible = false
                faceProperties.visible = false
                vehicleProperties.visible = false
                ocrProperties.visible = false

                deviceNameText.text = itemModel.cameraName
                datetimeText.text = itemModel.displayTime

                if(itemModel.attributeType === 0)
                {
                    personProperties.visible = true
                    propertyTitleText.text = WisenetLinguist.objectProperties + " (" + WisenetLinguist.person + ")"
                    personGenderText.text = "·  " + WisenetLinguist.gender + " : " + itemModel.personGender
                    personClothingTopText.text = "·  " + WisenetLinguist.clothing + " ("+ WisenetLinguist.top + ") : " + itemModel.personClothingTopColor
                    personClothingBottomText.text = "·  " + WisenetLinguist.clothing + " ("+ WisenetLinguist.bottom + ") : " + itemModel.personClothingBottomColor
                    personBagText.text = "·  " + WisenetLinguist.bag + " : " + itemModel.personBag
                }
                else if(itemModel.attributeType === 1)
                {
                    vehicleProperties.visible = true
                    propertyTitleText.text = WisenetLinguist.objectProperties + " (" + WisenetLinguist.vehicle + ")"
                    vehicleTypeText.text = "·  " + WisenetLinguist.vehicleType + " : " + itemModel.vehicleType
                    vehicleColorText.text = "·  " + WisenetLinguist.color + " : " + itemModel.vehicleColor
                }
                else if(itemModel.attributeType === 2)
                {
                    faceProperties.visible = true
                    propertyTitleText.text = WisenetLinguist.objectProperties + " (" + WisenetLinguist.face + ")"

                    faceGenderText.text =  "·  " + WisenetLinguist.gender + " : " + itemModel.faceGender
                    faceAgeText.text =  "·  " + WisenetLinguist.age + " : " + itemModel.faceAge
                    faceGlassesText.text =  "·  " + WisenetLinguist.glasses + " : " + itemModel.faceGlasses
                    faceMaskText.text =  "·  " + WisenetLinguist.faceMask + " : " + itemModel.faceMask
                }
                else
                {
                    ocrProperties.visible = true
                    propertyTitleText.text = WisenetLinguist.objectProperties + " (" + WisenetLinguist.ocr + ")"

                    ocrNumberText.text =  "·  " + WisenetLinguist.ocr + " : " + (itemModel.ocrNumber !== "" ? itemModel.ocrNumber : "-")
                }

                bestShotProperties.height = 10 + deviceNameText.height + 10 + datetimeText.height + 10 + propertyTitleText.height + (personProperties.visible ? personProperties.height : (faceProperties.visible ? faceProperties.height : (vehicleProperties.visible ? vehicleProperties.height : ocrProperties.height)))
            }
        }

        BestShotFilterTooltip{
            id: bestShotFilterTooltip

            filterModel: objectSearchViewModel.bestshotFilterTooltipModel

            onVisibleChanged: {
                if(visible){
                    //console.log("filterTooltip.onVisibleChanged", filterTooltip.width, filterTooltip.height, bestShotItem.x, bestShotItem.y, bestShotItem.width, bestShotItem.height, visible, filterIcon.x, filterIcon.y, bestShotItem.scrollYPosition, bestShotItem.parentHeight)
                }
            }
        }

        onCurrentIndexChanged: {
            anchoredBestShotPopup.close()
            gridListView.temporaryCloseAnchoredPopup()

            if(currentIndex == 0 || currentIndex == 2){
                if(scrollArea.isPopupAnchored === true)
                    anchoredBestShotPopup.visible = true
            }

            if(currentIndex == 1){
                if(gridListView.isPopupAnchored === true)
                    gridListView.openAnchoredPopup()
            }


            if(currentIndex == 2){
                flagViewModel.clearAll()
                flagViewModel.addFlagedItem(bestShotViewModel)
            }
        }

    }

    Connections{
        target: objectSearchViewModel

        function onRefreshWidth(){
            objectSearchViewModel.bestShotViewModel.viewWidth = scrollArea.width - objectSearchResultView.scrollRightMargin
            objectSearchViewModel.flagViewModel.viewWidth = scrollArea.width - objectSearchResultView.scrollRightMargin
        }
    }

    onWidthChanged: {
        widthChangedTimer.restart()
        // Popup 위치 조정
        if(anchoredBestShotPopup.x + anchoredBestShotPopup.width + 20 >= scrollArea.width)
            anchoredBestShotPopup.x = scrollArea.width - (anchoredBestShotPopup.width + 20)
    }

    onHeightChanged: {
        // Popup 위치 조정
        if(anchoredBestShotPopup.y + anchoredBestShotPopup.height + 10 >= scrollArea.height)
            anchoredBestShotPopup.y = scrollArea.height - (anchoredBestShotPopup.height + 10)
    }

    Timer {
        id: widthChangedTimer
        interval: 500
        onTriggered: {
            objectSearchViewModel.bestShotViewModel.viewWidth = scrollArea.width - objectSearchResultView.scrollRightMargin
            objectSearchViewModel.flagViewModel.viewWidth = scrollArea.width - objectSearchResultView.scrollRightMargin
        }
    }

    Timer {
        id: scrollBottomTimer
        interval: 50
        onTriggered: {
            console.log("SCROLL BOTTOM")
            bestShotViewModel.loadMoreBestShots()
        }
    }

}
