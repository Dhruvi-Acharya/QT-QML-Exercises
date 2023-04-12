import QtQuick 2.15
import QtQuick.Controls 2.15
import Wisenet.ImageItem 1.0
import WisenetLanguage 1.0
import WisenetStyle 1.0
import Wisenet.TextSearchViewModel 1.0

MouseArea{
    id: textItemArea

    width: scrollArea.contentWidth
    height: scrollArea.contentHeight

    property var viewModel

    property int zoomInOutStepValue: 0
    property bool increaseDecision: false    
    property bool enablePopup: false

    hoverEnabled: true

    TextSearchViewModel {
        id: textItemViewModel
    }

    Flickable{
        id: scrollArea
        anchors.fill: parent
        interactive: false
        contentWidth: textItemArea.increaseDecision === true ? (textData.width + (textData.anchors.leftMargin * 2)) + textItemArea.zoomInOutStepValue
                                                             : (textData.width + (textData.anchors.leftMargin * 2)) + textItemArea.zoomInOutStepValue
        contentHeight: textItemArea.increaseDecision === true ? (textData.height + (textData.anchors.bottomMargin * 2)) + textItemArea.zoomInOutStepValue
                                                              : (textData.height + (textData.anchors.bottomMargin * 2)) + textItemArea.zoomInOutStepValue
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
            color: "white"
            border.width: 1

            WisenetText{
                id: textData
                anchors.top: textItem.top
                anchors.topMargin: 15
                anchors.left: textItem.left
                anchors.leftMargin: 15
                anchors.bottomMargin: 15
                font.pixelSize: 10
                text: textDataIdRole
                color: "black"
            }

            WisenetText{
                id: posName
                anchors.top: textItem.bottom
                anchors.topMargin: 5
                font.bold: true
                text: posNameIdRole
                color: "black"
                visible: false
            }

            WisenetText{
                id: eventTime
                anchors.top: posName.bottom
                text: dateTimeIdRole
                color: "black"
                visible: false
            }

            WisenetText{
                id: eventChannel
                anchors.top: eventTime.bottom
                text: channelIdRole + " " + WisenetLinguist.channel
                color: "black"
                visible: false
            }

            WisenetText{
                id: deviceName
                anchors.top: eventChannel.bottom
                text: deviceNameIdRole
                color: "black"
                visible: false
            }

            WisenetText{
                id: eventChannels
                anchors.top: deviceName.bottom
                text: channelIdsRole
                color: "black"
                visible: false
            }
        }
    }

    Popup {
        id: popupTextItem
        property var defaultWidth: 260
        property var defaultHeight: 340
        property var posName: ""
        property var nvrName: ""
        property var chId: ""
        property var serviceTime: ""
        property var chNumber: ""

        // 팝업 크기
        width: popupServiceTime.width + (popupServiceTime.anchors.leftMargin * 4)
        height: popupPosName.height + popupNvrName.height + popupCHId.height + popupCHNumber.height + popupServiceTime.height + (15 * 2)

        // 가운데 정렬
        x: (textItemArea.width) / 2
        y: (textItemArea.height) / 2

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        Flickable{
            id: popupScrollArea

            anchors.fill: parent

            contentWidth: popupServiceTime.width + (popupServiceTime.anchors.leftMargin * 4)
            contentHeight: popupPosName.height + popupNvrName.height + popupCHId.height + popupCHNumber.height + popupServiceTime.height + (15 * 2)
            interactive: false

            ScrollBar.vertical: WisenetScrollBar {
                id: popupVerticalScrollBar

                parent: popupScrollArea

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                stepSize: popupVerticalScrollBar.visualSize / 4
            }

            clip: true

            Rectangle{
                id: popupTextRect
                anchors.fill: parent
                color: "white" //WisenetGui.contrast_08_dark_grey
                border.width: 1

                WisenetText{
                    id: popupPosName
                    anchors.top: popupTextRect.top
                    anchors.topMargin: 15
                    anchors.left: popupTextRect.left
                    anchors.leftMargin: 15
                    font.pixelSize: 10
                    text: popupTextItem.posName
                    color: "black"
                }

                WisenetText{
                    id: popupNvrName
                    anchors.top: popupPosName.bottom
                    anchors.left: popupTextRect.left
                    anchors.leftMargin: 15
                    font.pixelSize: 10
                    text: popupTextItem.nvrName
                    color: "black"
                }

                WisenetText{
                    id: popupCHId
                    anchors.top: popupNvrName.bottom
                    anchors.left: popupTextRect.left
                    anchors.leftMargin: 15
                    font.pixelSize: 10
                    text: popupTextItem.chId
                    color: "black"
                }

                WisenetText{
                    id: popupServiceTime
                    anchors.top: popupCHId.bottom
                    anchors.left: popupTextRect.left
                    anchors.leftMargin: 15
                    anchors.bottomMargin: 15
                    font.pixelSize: 10
                    text: popupTextItem.serviceTime
                    color: "black"
                }

                WisenetText{
                    id: popupCHNumber
                    anchors.top: popupServiceTime.bottom
                    anchors.left: popupTextRect.left
                    anchors.leftMargin: 15
                    font.pixelSize: 10
                    text: popupTextItem.chNumber
                    color: "black"
                }
            }
        }
    }

    onEntered: {
        if (!enablePopup) {
            enablePopup = true
            popupTextItem.visible = true
            popupTextItem.posName = posNameIdRole
            popupTextItem.nvrName = deviceNameIdRole
            popupTextItem.serviceTime = viewModel.convertTimestampToDatetime(viewModel.getServiceTime(dateTimeIdRole))

            var channelIds = channelIdsRole
            popupTextItem.chNumber = channelIds + " " + WisenetLinguist.channels

            var channelId = channelIdRole
            var chIDList = channelId.split(',')
            popupTextItem.chId = (parseInt(chIDList[0]) + 1).toString() + " " + WisenetLinguist.channel
        }
    }

    onExited: {
        if (enablePopup) {
            enablePopup = false
            popupTextItem.visible = false
        }
    }

    onDoubleClicked: {
        var dateTimeId = dateTimeIdRole
        var deviceNameId = deviceNameIdRole
        var posNameId = posNameIdRole
        var channelId = channelIdRole
        var textDataId = textDataIdRole

        var deviceUUID = viewModel.getDeviceID(deviceNameId)
        var serviceTime = viewModel.getServiceTime(dateTimeId)

        if (deviceNameId  && channelId && dateTimeId ) {
            var chIDList = channelId.split(',')
            var chID = (parseInt(chIDList[0]) + 1).toString()

            textSearchPlaybackView.viewingGrid.checkNewVideoChannel(deviceUUID, chID, true, serviceTime * 1000, -1, true, chID, true)
        }
    }
}

