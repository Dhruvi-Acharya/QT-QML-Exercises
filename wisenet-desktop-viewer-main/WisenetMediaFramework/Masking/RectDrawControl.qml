import QtQuick 2.15
import QtQuick.Shapes 1.15
import WisenetStyle 1.0
import Wisenet.Define 1.0

Item {
    id: rootItem

    property int maskType: MaskType.Fixed
    property var rectList: []
    property var currentRect: null
    property int minimunSize: 20

    signal maxCountOverflowed()
    signal drawFinished(var maskId, var rectCoordinates)
    signal maskRemoved(var maskId)
    signal updateFinished(var maskId, var rectCoordinates)
    signal manualBoxUpdated(var maskId, var rectCoordinates)
    signal selectedMaskItemChanged(var maskItem)

    function clearAll() {
        rectList.forEach(function(rect) {
            maskRemoved(rect.maskId)
            rect.destroy()
        });
        rectList = []
        currentRect = null
    }

    function selectItem(item) {
        //console.log("rootItem.selectItem", item)
        rectList.forEach(function(rect) {
            if(rect === item) {
                rect.selected = true
                rect.z = 1
            }
            else {
                rect.selected = false
                rect.z = -1
            }
        });

        if(item !== null){
            //console.log("rootItem.selectItem id", item.maskId)
            rootItem.selectedMaskItemChanged(item)
        }
    }

    function deleteItem(item) {
        for(var i=0 ; i<rectList.length ; i++) {
            if(rectList[i] === item) {
                rectList.splice(i, 1)
                maskRemoved(item.maskId)
                item.destroy()
                break
            }
        }
    }

    function selectMaskItem(maskId){
        var selectedMaskRectangle = null;

        for(var i=0 ; i<rectList.length ; i++) {
            if(rectList[i].maskId === maskId) {
                rectList[i].selected = true
                selectedMaskRectangle = rectList[i]
            }
            else {
                rectList[i].selected = false
            }
            //console.log("selectMaskItem", rectList[i].maskId, rectList[i].selected)
        }

        return selectedMaskRectangle
    }

    function changeMaskItemVisible(maskId, isVisible){
        for(var i=0 ; i<rectList.length ; i++) {
            if(rectList[i].maskId === maskId)
                rectList[i].visible = isVisible

            //console.log("changeMaskItemVisible", rectList[i].maskId, rectList[i].visible)
        }
    }

    function changeMaskItemName(maskId, maskName){
        for(var i=0 ; i<rectList.length ; i++) {
            if(rectList[i].maskId === maskId)
                rectList[i].maskName = maskName

            //console.log("changeMaskItemName", rectList[i].maskId, rectList[i].maskName)
        }
    }

    function removeMaskItemById(maskId){
        for(var i=0 ; i<rectList.length ; i++) {
            if(rectList[i].maskId === maskId) {
                var item = rectList[i]
                rectList.splice(i, 1)
                maskRemoved(maskId)
                item.destroy()
                break
            }
        }
    }

    function updateMaskRectPosition(maskRectMap) {
        // update all mask rect position in maskRectMap
        rectList.forEach(function(rect) {
            if(rect.drawing || rect.editing)
                return

            if(maskRectMap[rect.maskId]) {
                rect.maskRectangle.visible = true
                rect.updateNormalRectangle(maskRectMap[rect.maskId])
            }
            else {
                rect.maskRectangle.visible = false
            }
        })
    }

    onEnabledChanged: {
        // 컨트롤이 disable 된 경우
        if(!enabled) {
            if(currentRect != null) {
                // 그리고 있던 area를 지움
                rectList.pop()
                currentRect.destroy()
                currentRect = null
            }

            // 모든 select를 해제
            selectItem(null)
        }
    }

    Component {
        id: rectComponent
        MaskRectangle {
            id: componentItem
            anchors.fill: parent
            Component.onCompleted: {
                maskId = maskOsdControl.newMaskId++
                maskName = "Mask " + maskId
                if(rootItem.maskType == MaskType.Fixed)
                    maskName += " (Fixed)"
                else if(rootItem.maskType == MaskType.Manual)
                    maskName += " (Manual)"
                else if(rootItem.maskType == MaskType.Auto)
                    maskName += " (Auto)"
            }
            onDeleteButtonClicked: deleteItem(componentItem)            
        }
    }

    MouseArea {
        id: drawMouseArea
        z: -2
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true

        property real pressedX
        property real pressedY
        property bool dragging

        // MouseArea를 벗어나지 않는 좌표
        property real blockedX
        property real blockedY

        onClicked: {
            if(mouse.button == Qt.RightButton) {
                if(currentRect) {
                    deleteItem(currentRect)
                    currentRect = null
                }
                return
            }

            /*
            if(rectList.length == 3 && currentRect == null) {
                // 3개 까지 그리기 제한
                maxCountOverflowed()
                return
            }
            */

            if(/*rectList.length < 3 && */!currentRect) {
                // 신규 rect 생성
                selectItem(null)
                currentRect = rectComponent.createObject(rootItem)
                currentRect.drawing = true
                currentRect.maskType = rootItem.maskType
                currentRect.updateRectangle(drawRect.x, drawRect.y, drawRect.width, drawRect.height)
                rectList.push(currentRect)
            }
            else if(drawRect.width >= minimunSize && drawRect.height >= minimunSize) {
                console.log("drawRect.width",drawRect.width,"drawRect.height", drawRect.height)
                selectItem(currentRect)
                currentRect.drawing = false
                var maskId = currentRect.maskId
                var rectCoordinates = currentRect.getRectCoordinates()
                currentRect = null  // make currentRect to null before drawFinished()
                drawFinished(maskId, rectCoordinates)
            }
        }

        onPressedChanged: {
            if(pressedButtons & Qt.RightButton)
                return

            if(dragging) {
                // drag로 그린 rect를 실제로 생성
                if(drawRect.width >= minimunSize && drawRect.height >= minimunSize) {
                    currentRect = rectComponent.createObject(rootItem)
                    currentRect.maskType = rootItem.maskType
                    currentRect.updateRectangle(drawRect.x, drawRect.y, drawRect.width, drawRect.height)
                    rectList.push(currentRect)
                }
            }

            if(pressed) {
                pressedX = mouseX
                pressedY = mouseY
            }

            dragging = false
        }

        onPositionChanged: {
            if(pressedButtons & Qt.RightButton)
                return

            /*
            if(rectList.length == 3 && currentRect == null)
                return  // 최대 수 초과 시 리턴
            */

            blockedX = mouseX<0 ? 0 : mouseX>drawMouseArea.width ? drawMouseArea.width : mouseX
            blockedY = mouseY<0 ? 0 : mouseY>drawMouseArea.height ? drawMouseArea.height : mouseY

            if(pressed) {
                var dx = mouseX - pressedX
                var dy = mouseY - pressedY

                if(Math.abs(dx) >= 2 && Math.abs(dy) >= 2) {
                    selectItem(null)
                    dragging = true
                }
            }
            else {
                if(currentRect && currentRect.drawing) {
                    // 현재 그리고 있는 rect의 좌표 업데이트
                    currentRect.updateRectangle(drawRect.x, drawRect.y, drawRect.width, drawRect.height)
                }
            }
        }
    }

    Rectangle {
        id: drawRect
        visible: drawMouseArea.dragging
        x: Math.min(drawMouseArea.pressedX, drawMouseArea.blockedX)
        y: Math.min(drawMouseArea.pressedY, drawMouseArea.blockedY)
        width: Math.abs(drawMouseArea.pressedX - drawMouseArea.blockedX)
        height: Math.abs(drawMouseArea.pressedY - drawMouseArea.blockedY)
        color: WisenetGui.transparent
        border.color: maskType === MaskType.Fixed ? WisenetGui.color_masking_fixed :
                      maskType === MaskType.Manual ? WisenetGui.color_masking_manual : WisenetGui.color_masking_auto
        border.width: 2
    }
}


