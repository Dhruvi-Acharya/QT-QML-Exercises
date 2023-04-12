import QtQuick 2.15
import QtQuick.Shapes 1.15

Item {
    id: rootItem

    property var lineList: []
    property var currentLine: null

    signal maxCountOverflowed()

    function clearAll() {
        lineList.forEach(function(line) {
            line.destroy()
        });
        lineList = []
        currentLine = null
    }

    function selectItem(item) {
        lineList.forEach(function(line) {
            if(line === item) {
                line.selected = true
                line.z = 1
            }
            else {
                line.selected = false
                line.z = -1
            }
        });
    }

    function deleteItem(item) {
        for(var i=0 ; i<lineList.length ; i++) {
            if(lineList[i] === item) {
                lineList.splice(i, 1)
                item.destroy()
                break
            }
        }
    }

    onEnabledChanged: {
        // 컨트롤이 disable 된 경우
        if(!enabled){
            if(currentLine != null) {
                // 그리고 있던 line을 지움
                lineList.pop()
                currentLine.destroy()
                currentLine = null
            }

            // 모든 select를 해제
            selectItem(null)
        }
    }

    Component {
        id: lineComponent
        VirtualLine {
            id: componentItem
            anchors.fill: parent
            onDeleteButtonClicked: deleteItem(componentItem)
        }
    }

    MouseArea {
        z: -2
        anchors.fill: parent
        anchors.margins: blockAeraWidth
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true

        onPressedChanged: {
            if(pressedButtons & Qt.RightButton)
                return

            if(pressed && currentLine == null) {
                // 3개 까지 그리기 제한
                if(lineList.length == 3) {
                    maxCountOverflowed()
                    return
                }

                // 신규 line 생성
                selectItem(null)
                currentLine = lineComponent.createObject(rootItem);
                lineList.push(currentLine)
                currentLine.setStartPoint(mouseX + blockAeraWidth, mouseY + blockAeraWidth)
                currentLine.setEndPoint(mouseX + blockAeraWidth, mouseY + blockAeraWidth)
            }
        }

        onClicked: {
            if(mouse.button == Qt.RightButton) {
                if(currentLine) {
                    deleteItem(currentLine)
                    currentLine = null
                }
                return
            }

            if(currentLine != null && (currentLine.lineShapeWidth > 14 || currentLine.lineShapeHeight > 14)) {
                selectItem(currentLine)
                currentLine = null  // Line을 일정 길이 이상 그렸으면 그리기 종료
            }
        }

        onPositionChanged: {
            if(pressedButtons & Qt.RightButton)
                return

            if(currentLine) {
                // 현재 그리고 있는 line의 끝점 좌표 업데이트
                currentLine.setEndPoint(mouseX + blockAeraWidth, mouseY + blockAeraWidth)
            }
        }
    }
}


