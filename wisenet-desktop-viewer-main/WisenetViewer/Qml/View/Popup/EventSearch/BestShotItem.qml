import QtQuick 2.15
import QtQuick.Controls 2.15
import Wisenet.ImageItem 1.0
import WisenetLanguage 1.0
import WisenetStyle 1.0

Rectangle{
    id: bestShotItem

    property BestShotFilterTooltip filterTooltip
    property bool toolTipVisible : false
    property var itemModel
    property var viewModel
    property bool isPopupAnchored

    // used when calculate filter popup position
    property int scrollYPosition
    property int parentHeight

    signal anchoredChanged(var isAnchored, var x, var y)
    signal bestshotDoubleClicked(var deviceId, var channelId, var deviceTime)
    signal addBookmark(var deviceId, var channelId, var deviceTime)
    //signal FilterPopupAttributes()

    x: itemModel.pointX
    y: itemModel.pointY
    width: itemModel.bestShotWidth
    height: itemModel.bestShotHeight

    border.color: "white"
    border.width: itemModel.sequence === viewModel.selectedIndex ? 1 : 0
    visible: itemModel.visible

    color: "transparent"

    Connections{
        id: closeConnection
        target: filterTooltip

        onClosed: {
            if(filterTooltip.parent == bestShotItem){
                console.log("tooltip closed")
                toolTipVisible = false
            }
        }
        onResetIcon: toolTipVisible = false
    }

    /*
    Text{
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.leftMargin: 5

        text: itemModel.sequence
        color: "white"
    }*/
    ImageItem{
        id: imageArea
        anchors.fill: parent
        anchors.margins: 1
        image: itemModel.image
    }

    // MouseArea Rectangle 순서 매우 중요 변경 시 빠르게 마우스 이동하면 button이 사라지지 않는 경우 발생함.
    MouseArea{
        id: bestShotMouseArea

        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton

        onPressed: {
            viewModel.selectedIndex = itemModel.sequence
            console.log("bestShotMouseArea.onPressed")
            //mouse.accepted = false

            if(!isPopupAnchored)
            {
                bestShotDetailTooltip.visible = true
                bestShotDetailTooltip.closeY = true
            }
        }

        // 체크 해제시에 버튼 visible update를 위함.
        onExited: flagIcon.update()

        onDoubleClicked: {
            console.log("bestShotMouseArea.onDoubleClicked")
            bestShotItem.bestshotDoubleClicked(itemModel.deviceId, itemModel.channelId, itemModel.deviceTime)
        }
    }

    WisenetImageButton{
        id: filterIcon

        width: 28
        height: 28
        sourceWidth: 20
        sourceHeight: 20

        hoverEnabled: true
        checked: toolTipVisible
        imageNormal: WisenetImage.object_search_filter_button_default
        imageSelected: WisenetImage.object_search_filter_button_selected
        imageHover: WisenetImage.object_search_filter_button_hover
        visible: (bestShotMouseArea.containsMouse || filterIcon.hovered || flagIcon.hovered || toolTipVisible) && itemModel.attributeType !== 3

        anchors.bottom: bestShotItem.width > filterIcon.width + flagIcon.width + 10 ? parent.bottom : flagIcon.top
        anchors.right: bestShotItem.width > filterIcon.width + flagIcon.width + 10 ? flagIcon.left : parent.right
        //anchors.bottomMargin: bestShotItem.width > filterIcon.width + flagIcon.width + 10 ? 5 : 0
        //anchors.rightMargin: bestShotItem.width > filterIcon.width + flagIcon.width + 10 ? 0 : 5

        background: Rectangle {
            implicitWidth: image && image.width ? image.width : 0
            implicitHeight: image && image.width ? image.width : 0
            color: WisenetGui.color_live_control_bg
            opacity: 0.7
        }

        onClicked:  {
            console.log("filterButtonArea.onReleased")
            filterTooltip.parent = bestShotItem

            filterTooltip.resetIcon()
            filterTooltip.setFilterVisible(itemModel.metaAttr)
            objectSearchViewModel.updateBestshotTooltipChecked()

            filterTooltip.x = bestShotItem.x + filterIcon.x + filterTooltip.width <= bestShotViewModel.viewWidth ? filterIcon.x : filterIcon.x - filterTooltip.width + filterIcon.width
            filterTooltip.y = bestShotItem.y - bestShotItem.scrollYPosition + bestShotItem.height + filterTooltip.height <= bestShotItem.parentHeight ? filterIcon.y + filterIcon.height + 5 : filterIcon.y - filterTooltip.height - 5
            filterTooltip.closeY = true

            console.log("filterButtonArea.onReleased", filterTooltip.width, filterTooltip.height, bestShotItem.x, bestShotItem.y, bestShotItem.width, bestShotItem.height, visible, filterIcon.x, filterIcon.y, bestShotItem.scrollYPosition, bestShotItem.parentHeight)
            toolTipVisible = true
            filterTooltip.open()

            if(bestShotDetailTooltip.visible === true)
                bestShotDetailTooltip.close()
        }
    }

    WisenetImageButton{
        id: flagIcon

        width: 28
        height: 28
        sourceWidth: 20
        sourceHeight: 20

        hoverEnabled: true
        checked: itemModel.flagChecked
        checkable: true
        imageNormal: WisenetImage.object_search_flag_filled_default
        imageSelected: WisenetImage.object_search_flag_filled_selected
        imageHover: flagIcon.checked ? WisenetImage.object_search_flag_filled_selected : WisenetImage.object_search_flag_filled_hover

        visible : bestShotMouseArea.containsMouse || filterIcon.hovered || flagIcon.hovered || itemModel.flagChecked === true
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        //anchors.bottomMargin: 5
        //anchors.rightMargin: 5

        background: Rectangle {
            implicitWidth: image && image.width ? image.width : 0
            implicitHeight: image && image.width ? image.width : 0
            color: WisenetGui.color_live_control_bg
            opacity: 0.7
        }

        onClicked: {
            itemModel.flagChecked = !itemModel.flagChecked
        }
    }

    /*
    BestShotFilterTooltip{
        id: filterTooltip

        filterModel: objectSearchViewModel.bestshotFilterTooltipModel

        x: bestShotItem.x + filterIcon.x + filterTooltip.width <= bestShotViewModel.viewWidth ? filterIcon.x : filterIcon.x - filterTooltip.width + filterIcon.width
        y: bestShotItem.y - bestShotItem.scrollYPosition + bestShotItem.height + filterTooltip.height <= bestShotItem.parentHeight ? filterIcon.y + filterIcon.height + 5 : filterIcon.y - filterTooltip.height - 5

        onVisibleChanged: {
            if(visible){
                console.log("filterTooltip.onVisibleChanged", filterTooltip.width, filterTooltip.height, bestShotItem.x, bestShotItem.y, bestShotItem.width, bestShotItem.height, visible, filterIcon.x, filterIcon.y, bestShotItem.scrollYPosition, bestShotItem.parentHeight)
            }
        }
    }*/

    // Popup으로 하면 끝부분 아이템을 클릭했을 때 위치가 자동으로 안잡힘.
    ToolTip{
        id: bestShotDetailTooltip
        height: 12 + deviceNameText.height + 4 + datetimeText.height + 13 + propertyTitleText.height + 6 + (personProperties.visible ? personProperties.height : faceProperties.visible ? faceProperties.height : vehicleProperties.visible ? vehicleProperties.height : ocrProperties.height) + 24
        width: 250

        property bool closeY : false
        x: bestShotItem.width
        y: bestShotItem.height/2

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        property int propertiesFontSize : 10

        Rectangle{
            id: bestShotProperties

            anchors.fill: parent
            color: WisenetGui.contrast_09_dark
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

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: personClothingTopText

                    anchors.top: personGenderText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 9

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: personClothingBottomText

                    anchors.top: personClothingTopText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 9

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: personBagText

                    anchors.top: personClothingBottomText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 9

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
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

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: faceAgeText

                    anchors.top: faceGenderText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 9

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: faceGlassesText

                    anchors.top: faceAgeText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 9

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: faceMaskText

                    anchors.top: faceGlassesText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 9

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
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

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
                    color: WisenetGui.contrast_01_light_grey

                    text: " "
                }

                Text{
                    id: vehicleColorText

                    anchors.top: vehicleTypeText.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 9

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
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

                    font.pixelSize: bestShotDetailTooltip.propertiesFontSize
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
                anchors.topMargin: 10

                imageNormal: WisenetImage.object_search_unfixed_default
                imageHover: WisenetImage.object_search_unfixed_hover

                onClicked: {
                    bestShotItem.anchoredChanged(true, bestShotDetailTooltip.x + bestShotItem.x, bestShotDetailTooltip.y + bestShotItem.y)
                    bestShotDetailTooltip.close()
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

                onClicked: bestShotItem.addBookmark(itemModel.deviceId, itemModel.channelId, itemModel.deviceTime)
            }
        }

        onVisibleChanged: {
            if(visible){
                if(filterTooltip.opened)
                    filterTooltip.close()

                console.log("bestShotDetailTooltip.onVisibleChanged", visible, itemModel.attributeType)
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

        onYChanged: {
            if(closeY)
                close()
        }

        onClosed: {
            closeY = false
        }

    }
}
