import QtQuick 2.15
import QtQuick.Controls 2.15

// 타임라인의 ContextMenu
Item {
    property double mouseX: 0

    function showMenu(xPos) {
        mouseX = xPos

        markSelectionStartMenu.visible = false
        markSelectionEndMenu.visible = false
        clearSelectionEndMenu.visible = true
        zoomToSelectionMenu.visible = false

        if(selectionStartTime == 0) {
            markSelectionStartMenu.visible = true
            clearSelectionEndMenu.visible = false
        }
        else if(selectionEndTime == 0) {
            markSelectionEndMenu.visible = true
        }
        else {
            zoomToSelectionMenu.visible = true
        }

        contextMenu.popup();
    }

    Menu {
        id: contextMenu

        MenuItem {
            id: markSelectionStartMenu
            text: "Mark Selection Start"
            visible: false
            height: {
                if(visible) return 20
                else return 0
            }

            onTriggered: {
                selectionStartTime = convertPositionToMsec(mouseX)
            }
        }

        MenuItem {
            id: markSelectionEndMenu
            text: "Mark Selection End"
            visible: false
            height: {
                if(visible) return 20
                else return 0
            }
            onTriggered: {
                var time = convertPositionToMsec(mouseX)
                if(time < selectionStartTime) {
                    selectionEndTime = selectionStartTime
                    selectionStartTime = time
                }
                else {
                    selectionEndTime = time
                }
            }
        }

        MenuItem {
            id: clearSelectionEndMenu
            text: "Clear Selection"
            visible: false
            height: {
                if(visible) return 20
                else return 0
            }
            onTriggered: {
                selectionStartTime = 0
            }
        }

        MenuItem {
            id: zoomToSelectionMenu
            text: "Zoom to Selection"
            visible: false
            height: {
                if(visible) return 20
                else return 0
            }
            onTriggered: {
                var rangeDelta = selectionEndTime - selectionStartTime - mediaControlModel.visibleTimeRange  // 표시 범위 변화량
                var startTimeDelta = selectionStartTime - mediaControlModel.visibleStartTime    // 표시 시작 시간 변화량

                if(mediaControlModel.visibleTimeRange + rangeDelta <= 1500) {
                    rangeDelta = 1500 - mediaControlModel.visibleTimeRange
                    startTimeDelta = selectionStartTime - (1500 - selectionEndTime + selectionStartTime) / 2 - mediaControlModel.visibleStartTime
                }

                timelineControlView.animationDuration = 500;
                mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)
            }
        }
    }
}
