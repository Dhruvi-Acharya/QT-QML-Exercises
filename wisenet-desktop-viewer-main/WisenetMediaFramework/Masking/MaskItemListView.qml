import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import Wisenet.ImageItem 1.0 as WisenetImageItem
import QtQml.Models 2.15
import Wisenet.Define 1.0
import WisenetMediaFramework 1.0
import "qrc:/"

Item {
    id: maskItemListView

    property var listModel
    property var timelineStartTime
    property var timelineEndTime
    property bool autoOn : false
    property bool manualOn : false
    property bool fixedOn : false
    property double selectedStartTime: 0
    property double selectedEndTime: 0

    signal selectedMaskItemChanged(var maskId, var startTime, var endTime)
    signal maskTypeChanged(var maskType)
    signal maskItemVisibleChanged(var maskId, var isVisible)
    signal maskItemNameChanged(var maskId, var maskName)
    signal maskItemRemoved(var maskId)
    signal requestSeek(var time)

    function selectMaskItem(maskId){
        console.log("maskItemListView.selectMaskItem", maskId)
        for(var i = 0; i < maskItemListDelegateModel.items.count; i++ ){
            var item = maskItemListDelegateModel.items.get(i)

            if(item.model.maskId === maskId && maskList.currentIndex !== item.itemsIndex){
                maskList.currentIndex = item.itemsIndex
                break
            }
        }
    }

    function startAutoMasking(isOn){
        maskItemListView.autoOn = isOn
        maskItemListView.fixedOn = false
        maskItemListView.manualOn = false
    }

    function startFixedMasking(isOn){
        maskItemListView.autoOn = false
        maskItemListView.fixedOn = isOn
        maskItemListView.manualOn = false
    }

    function startManualMasking(isOn){
        maskItemListView.autoOn = false
        maskItemListView.fixedOn = false
        maskItemListView.manualOn = isOn
    }

    Connections{
        target: maskItemListView.listModel
        function onItemAdded(){
            maskList.currentIndex = maskList.count - 1
        }
    }

    ListView{
        id: maskList
        anchors.fill: parent
        anchors.leftMargin: 24
        anchors.rightMargin: 8
        anchors.topMargin: 8
        anchors.bottomMargin: 8
        clip: true

        model: DelegateModel{
            id: maskItemListDelegateModel

            model: maskItemListView.listModel

            delegate: Rectangle {
                id: maskItem

                property int visualIndex: DelegateModel.itemsIndex
                property var maskItemId: model.maskId
                property var maskType: model.maskType
                property bool itemSelected

                width: 196
                height: 211

                color: "transparent"

                property var start: model.startTime
                property var end: model.endTime

                WisenetImageItem.ImageItem{
                    id: thumbnail

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.topMargin: 13

                    height: 119

                    image: model.cropImage
                }

                Rectangle{
                    id: thumbnailRect

                    anchors.fill: thumbnail
                    color: "transparent"

                    border.color: WisenetGui.color_primary
                    border.width: 2
                    visible: maskList.currentIndex === visualIndex
                }

                MouseArea{
                    id: thumnailMouseArea

                    anchors.fill: thumbnailRect
                    cursorShape: Qt.PointingHandCursor

                    onPressed: {
                        maskList.currentIndex = visualIndex
                    }

                    onDoubleClicked: {
                        maskItemListView.requestSeek(start)
                    }
                }

                WisenetMediaTextField{
                    id: nameTextField

                    anchors.top: thumbnailRect.bottom
                    anchors.topMargin: 9
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: 23
                    enterAfterFocusChanged: true

                    text: model.maskName

                    onEntered: {
                        model.maskName = text
                        maskItemListView.maskItemNameChanged(model.maskId, model.maskName)
                    }

                    onCanceled: {
                        text = model.maskName
                    }
                }

                WisenetTimeEditor{
                    id: startTimeEditor
                    anchors.top: nameTextField.bottom
                    anchors.topMargin: 10
                    anchors.left: parent.left
                    leftPadding: 2

                    width: 47
                    height: 21

                    buttonVisible: false
                    textPixelSize: 11

                    onControlFocusChanged:{
                        if(controlFocus){
                            console.log("startTimeEditor.onControlFocusChanged return", controlFocus)
                            return
                        }

                        var startDateTime = new Date(start)

                        startDateTime.setHours(startTimeEditor.hour)
                        startDateTime.setMinutes(startTimeEditor.minute)
                        startDateTime.setSeconds(startTimeEditor.second)

                        console.log("startTimeEditor.onControlFocusChanged", startDateTime.getTime())
                        var changedTime = startDateTime.getTime()

                        if(changedTime < timelineStartTime){
                            console.log("startTimeEditor.onControlFocusChanged entered time is smaller than start of timeline : time will be", new Date(timelineStartTime))
                            changedTime = timelineStartTime
                        }

                        if(changedTime > maskItem.end){
                            console.log("startTimeEditor.onControlFocusChanged entered time is bigger than end time : time will be", new Date(maskItem.end))
                            changedTime = maskItem.end
                        }

                        if(model.startTime !== changedTime){
                            console.log("startTimeEditor.onControlFocusChanged changed from", model.startTime, "to", changedTime)
                            model.startTime = changedTime
                            maskItemListView.requestSeek(changedTime)
                        }
                    }
                }

                Text{
                    id: tilde
                    anchors.top: nameTextField.bottom
                    anchors.topMargin: 6
                    anchors.left: startTimeEditor.right
                    anchors.leftMargin: 4
                    height:21

                    text: "~"
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                WisenetTimeEditor{
                    id: endTimeEditor
                    anchors.top: nameTextField.bottom
                    anchors.topMargin: 10
                    anchors.left: tilde.right
                    anchors.leftMargin: 4
                    leftPadding: 2

                    width: 47
                    height: 21

                    buttonVisible: false
                    textPixelSize: 11

                    onControlFocusChanged:{
                        if(controlFocus){
                            console.log("endTimeEditor.onControlFocusChanged return", controlFocus)
                            return
                        }

                        var endDateTime = new Date(end)

                        endDateTime.setHours(endTimeEditor.hour)
                        endDateTime.setMinutes(endTimeEditor.minute)
                        endDateTime.setSeconds(endTimeEditor.second)

                        console.log("endTimeEditor.onControlFocusChanged", endDateTime.getTime())
                        var changedTime = endDateTime.getTime()

                        if(changedTime > timelineEndTime){
                            console.log("endTimeEditor.onControlFocusChanged entered time is bigger than end of timeline : time will be ", new Date(timelineEndTime))
                            changedTime = timelineEndTime
                        }

                        if(changedTime < maskItem.start){
                            console.log("endTimeEditor.onControlFocusChanged entered time is smaller than start time : time will be", new Date(maskItem.start))
                            changedTime = maskItem.start
                        }

                        if(model.endTime !== changedTime){
                            console.log("endTimeEditor.onControlFocusChanged changed from", model.endTime, "to", changedTime)
                            model.endTime = changedTime
                            maskItemListView.requestSeek(changedTime)
                        }
                    }
                }

                Image{
                    id: typeImage

                    anchors.top: nameTextField.bottom
                    anchors.right: showButton.left
                    anchors.topMargin: 9
                    anchors.rightMargin: 4

                    width: 24
                    height: 24
                    sourceSize.width: 24
                    sourceSize.height: 24

                    source: model.maskType === MaskType.Auto ? "qrc:/images/Masking/auto_mask_normal.svg" :
                                                               model.maskType === MaskType.Fixed ? "qrc:/images/Masking/fixed_mask_normal.svg" :
                                                                                                   "qrc:/images/Masking/manual_mask_normal.svg"

                }

                WisenetMediaToggleImageButtonItem{
                    id: showButton

                    anchors.top: nameTextField.bottom
                    anchors.right: deleteButton.left
                    anchors.topMargin: 9
                    anchors.rightMargin: 4

                    width: 24
                    height: 24
                    imgWidth: 24
                    imgHeight: 24
                    imgSourceSize: "24x24"

                    toggleOn: model.visible

                    normalSource: "qrc:/images/Masking/mask_hide_normal.svg"
                    normalOnSource: "qrc:/images/Masking/mask_show_normal.svg"
                    pressSource: "qrc:/images/Masking/mask_hide_normal.svg"
                    pressOnSource: "qrc:/images/Masking/mask_show_normal.svg"
                    hoverSource: "qrc:/images/Masking/mask_hide_hover.svg"
                    hoverOnSource: "qrc:/images/Masking/mask_show_hover.svg"

                    onMouseClicked: {
                        model.visible = isOn
                        maskItemListView.maskItemVisibleChanged(maskItem.maskItemId, isOn)
                    }
                }

                WisenetMediaButtonItem{
                    id: deleteButton

                    anchors.top: nameTextField.bottom
                    anchors.topMargin: 9
                    anchors.right: parent.right

                    width: 24
                    height: 24
                    imgWidth: 24
                    imgHeight: 24
                    imgSourceSize: "24x24"

                    normalSource: "qrc:/images/Masking/delete_normal.svg"
                    pressSource: "qrc:/images/Masking/delete_normal.svg"
                    hoverSource: "qrc:/images/Masking/delete_hover.svg"

                    onMouseClicked: {
                        // Only send signal, removeByMaskId() in model will be called on mediaPlayer.
                        maskItemListView.maskItemRemoved(maskItem.maskItemId)
                    }
                }

                Rectangle{
                    id: bottomLine

                    height:1
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    color: WisenetGui.contrast_07_grey
                }

                onStartChanged: {
                    var startDateTime = new Date(start)
                    startTimeEditor.setTimeText(startDateTime.getHours(), startDateTime.getMinutes(), startDateTime.getSeconds())
                    if(model.selected)
                        selectedStartTime = start
                }

                onEndChanged: {
                    var endDateTime = new Date(end)
                    endTimeEditor.setTimeText(endDateTime.getHours(), endDateTime.getMinutes(), endDateTime.getSeconds())
                    if(model.selected)
                        selectedEndTime = end
                }

                onItemSelectedChanged:{
                    model.selected = itemSelected
                }
            }
        }

        onCurrentItemChanged: {
            if(maskList.currentItem != null){
                console.log("maskItemListView.onCurrentIndexChanged", maskList.currentItem.maskItemId)
                listModel.setSelectedTrueByIndex(maskList.currentItem.visualIndex)

                maskItemListView.selectedMaskItemChanged(maskList.currentItem.maskItemId,
                                                         maskList.currentItem.start,
                                                         maskList.currentItem.end)

                maskItemListView.maskTypeChanged(maskList.currentItem.maskType)

                selectedStartTime = maskList.currentItem.start
                selectedEndTime = maskList.currentItem.end
            }else{
                listModel.setSelectedFalseAll()
                selectedStartTime = 0
                selectedEndTime = 0
            }

            for(var i = 0; i < maskItemListDelegateModel.items.count; i++){
                var item = maskItemListDelegateModel.items.get(i)
                console.log("maskItemListView.onCurrentIndexChanged idx:", item.itemsIndex, "id:", item.model.maskId, "selected:", item.model.selected)
            }
        }

        ScrollBar.vertical: WisenetMediaScrollBar {}
    }

    onAutoOnChanged:{
        if(maskList.currentItem == null)
            return;

        if(autoOn){
            if(maskList.currentItem.maskType !== MaskType.Auto)
                maskList.currentIndex = -1
        }
        else{
            if(maskList.currentItem.maskType === MaskType.Auto)
                maskList.currentIndex = -1
        }
    }

    onManualOnChanged:{
        if(maskList.currentItem == null)
            return;

        if(manualOn){
            if(maskList.currentItem.maskType !== MaskType.Manual)
                maskList.currentIndex = -1
        }
        else{
            if(maskList.currentItem.maskType === MaskType.Manual)
                maskList.currentIndex = -1
        }
    }

    onFixedOnChanged:{
        if(maskList.currentItem == null)
            return;

        if(fixedOn){
            if(maskList.currentItem.maskType !== MaskType.Fixed)
                maskList.currentIndex = -1
        }
        else{
            if(maskList.currentItem.maskType === MaskType.Fixed)
                maskList.currentIndex = -1
        }
    }
}
