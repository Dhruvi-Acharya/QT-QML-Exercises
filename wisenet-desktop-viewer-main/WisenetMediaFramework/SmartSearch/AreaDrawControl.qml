import QtQuick 2.15
import QtQuick.Shapes 1.15
import WisenetStyle 1.0
import Wisenet.Define 1.0

Item {
    id: rootItem

    property int areaType: SmartSearchAreaType.Inside // Inside(roi) or Outside(exclude)
    property var areaList: []
    property var currentArea: null
    property bool aiFilterVisible: false

    signal maxCountOverflowed()

    function clearAll() {
        areaList.forEach(function(area) {
            area.destroy()
        });
        areaList = []
        currentArea = null
    }

    function selectItem(item) {
        areaList.forEach(function(area) {
            if(area === item) {
                area.selected = true
                area.z = 1
            }
            else {
                area.selected = false
                area.z = -1
            }
        });
    }

    function deleteItem(item) {
        for(var i=0 ; i<areaList.length ; i++) {
            if(areaList[i] === item) {
                areaList.splice(i, 1)
                item.destroy()
                break
            }
        }
    }

    onEnabledChanged: {
        // 컨트롤이 disable 된 경우
        if(!enabled) {
            if(currentArea != null) {
                // 그리고 있던 area를 지움
                areaList.pop()
                currentArea.destroy()
                currentArea = null
            }

            // 모든 select를 해제
            selectItem(null)
        }
    }

    Component {
        id: areaComponent
        VirtualArea {
            id: componentItem
            anchors.fill: parent
            aiFilterVisible : rootItem.aiFilterVisible
            onDeleteButtonClicked: deleteItem(componentItem)
        }
    }

    MouseArea {
        id: drawMouseArea
        z: -2
        anchors.fill: parent
        anchors.margins: blockAeraWidth
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
                if(currentArea) {
                    deleteItem(currentArea)
                    currentArea = null
                }
                return
            }

            if(areaList.length == 3 && currentArea == null) {
                // 3개 까지 그리기 제한
                maxCountOverflowed()
                return
            }

            if(areaList.length < 3 && !currentArea) {
                // 신규 area 생성
                selectItem(null)
                currentArea = areaComponent.createObject(rootItem)
                currentArea.areaType = rootItem.areaType
                areaList.push(currentArea)
            }

            if(currentArea) {
                if(currentArea.getPointsCount() === 4) {
                    selectItem(currentArea)
                    currentArea = null
                }
                else {
                    currentArea.addPoint(mouseX + blockAeraWidth, mouseY + blockAeraWidth)
                }
            }
        }

        onPressedChanged: {
            if(pressedButtons & Qt.RightButton)
                return

            if(dragging) {
                // drag로 그린 area를 실제로 생성
                if(drawRect.width > 14 || drawRect.height > 14) {
                    currentArea = areaComponent.createObject(rootItem)
                    currentArea.areaType = rootItem.areaType
                    currentArea.addRectangle(drawRect.x, drawRect.y, drawRect.x + drawRect.width, drawRect.y + drawRect.height)
                    areaList.push(currentArea)
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

            if(areaList.length == 3 && currentArea == null)
                return  // 최대 수 초과 시 리턴

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
                if(currentArea) {
                    // 현재 그리고 있는 line의 끝점 좌표 업데이트
                    currentArea.updateCurrentPoint(mouseX + blockAeraWidth, mouseY + blockAeraWidth)
                }
            }
        }
    }

    Rectangle {
        id: drawRect
        visible: drawMouseArea.dragging
        x: Math.min(drawMouseArea.pressedX, drawMouseArea.blockedX) + blockAeraWidth
        y: Math.min(drawMouseArea.pressedY, drawMouseArea.blockedY) + blockAeraWidth
        width: Math.abs(drawMouseArea.pressedX - drawMouseArea.blockedX)
        height: Math.abs(drawMouseArea.pressedY - drawMouseArea.blockedY)
        color: areaType == SmartSearchAreaType.Inside ? WisenetGui.color_smartSearch_roi_30 : WisenetGui.color_smartSearch_exclude_30
        border.color: areaType == SmartSearchAreaType.Inside ? WisenetGui.color_smartSearch_roi : WisenetGui.color_smartSearch_exclude
        border.width: 2
    }
}


