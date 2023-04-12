import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"

// 타임라인의 ContextMenu
Item {
    property double mouseX: 0

    function showMenu(xPos) {
        mouseX = xPos

        markSelectionStart.visible = false
        markSelectionEnd.visible = false
        clearSelection.visible = false
        zoomToSelection.visible = false

        if(selectionStartTime == 0) {
            markSelectionStart.visible = true
        }
        else {
            clearSelection.visible = true

            if(selectionEndTime == 0) {
                markSelectionEnd.visible = true
            }
            else {
                markSelectionStart.visible = true
                zoomToSelection.visible = true
            }
        }

        if(focusedItemIsCamera) {
            var overlappedIdList = graphAreaModel.getOverlappedIdList(timelineControlView.convertPositionToMsec(xPos))
            if(overlappedIdList.length >= 2) {
                overlappedDataMenu.setMenuVisible(true) // click 위치에 2개 이상의 중복구간이 있을 때만 메뉴 표시
                overlappedDataRepeater.model = 0
                overlappedDataRepeater.model = overlappedIdList
            }
            else {
                overlappedDataMenu.setMenuVisible(false)
            }
            //overlappedDataRepeater.model = mediaControlModel.getOverlappedIdList()
        }
        else {
            overlappedDataMenu.setMenuVisible(false)
        }

        contextMenu.popup()
    }

    function setDefaultSelectionRange() {
        if(selectionEndTime != 0)
            return

        selectionStartTime = timelineControlView.convertPositionToMsec(mouseX)
        var endTime = selectionStartTime + mediaControlModel.visibleTimeRange / 10
        selectionEndTime = endTime > mediaControlModel.timelineEndTime ? mediaControlModel.timelineEndTime : endTime
    }

    WisenetMediaMenu {
        id: contextMenu
        implicitWidth: {
            var textWidth = dummySelectionText.contentWidth + dummyCtrlShortcutText.contentWidth + 60
            if(textWidth < 180)
                textWidth = 180
            return textWidth
        }

        WisenetMediaMenu {
            id: overlappedDataMenu
            title: WisenetLinguist.overlappedData

            Repeater {
                id: overlappedDataRepeater
                model: {-1}

                WisenetMediaMenuItem {
                    text: model.modelData === -1 ? WisenetLinguist.noData : WisenetLinguist.overlapped + " " + index
                    checkable: true
                    checked: model.modelData === mediaControlModel.selectedOverlappedID

                    onTriggered: {
                        if(mediaControlModel.selectedOverlappedID !== model.modelData)
                        {
                            mediaControlModel.selectedOverlappedID = model.modelData
                            mediaControllerView.selectedTrackIdChanged(mediaControlModel.selectedOverlappedID)

                            // Playback 중이면 마우스 클릭 위치로 Seek 수행
                            // Overlapped ID가 변경 된 경우 requestSeek() 안에서 MediaOpen까지 재수행 함
                            if(focusedItemIsCamera && !focusedViewingGridItem.mediaParam.isLive)
                                requestSeek(timelineControlView.convertPositionToMsec(mouseX))
                        }
                    }
                }
            }
        }

        WisenetMediaMenuSeparator {
            visible: focusedItemIsCamera
        }

        Shortcut {
            id: dummyCtrlShortcut
            enabled: false
            sequence: "Ctrl+."
        }

        WisenetMediaMenuItem {
            id: markSelectionStart
            text: WisenetLinguist.markSelectionStart
            visible: false
            onTriggered: {
                selectionEndTime = 0
                selectionStartTime = timelineControlView.convertPositionToMsec(mouseX)
            }

            Text {
                id: dummySelectionText
                padding: 0
                color: WisenetGui.transparent
                font.pixelSize: 12
                text: markSelectionStart.text
            }

            Text {
                id: dummyCtrlShortcutText
                height: parent.height
                anchors.right: parent.right
                anchors.rightMargin: 15
                padding: 0
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                text: dummyCtrlShortcut.nativeText.substring(0, dummyCtrlShortcut.nativeText.length-1) + "Click"
                color: WisenetGui.contrast_00_white
                font.pixelSize: 8
            }
        }


        WisenetMediaMenuItem {
            id: markSelectionEnd
            text: WisenetLinguist.markSelectionEnd
            visible: false
            onTriggered: {
                var time = timelineControlView.convertPositionToMsec(mouseX)
                if(time < selectionStartTime) {
                    selectionEndTime = selectionStartTime
                    selectionStartTime = time
                }
                else {
                    selectionEndTime = time
                }
            }

            Text {
                height: parent.height
                anchors.right: parent.right
                anchors.rightMargin: 15
                padding: 0
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                text: dummyCtrlShortcut.nativeText.substring(0, dummyCtrlShortcut.nativeText.length-1) + "Click"
                color: WisenetGui.contrast_00_white
                font.pixelSize: 8
            }
        }

        WisenetMediaMenuItem {
            id: clearSelection
            text: WisenetLinguist.clearSelection
            visible: false
            onTriggered: {
                selectionStartTime = 0
            }
        }

        WisenetMediaMenuItem {
            id: zoomToSelection
            text: WisenetLinguist.zoomToSelection
            visible: false
            onTriggered: {
                var rangeDelta = selectionEndTime - selectionStartTime - mediaControlModel.visibleTimeRange  // 표시 범위 변화량
                var startTimeDelta = selectionStartTime - mediaControlModel.visibleStartTime    // 표시 시작 시간 변화량

                if(mediaControlModel.visibleTimeRange + rangeDelta <= 10000) {
                    rangeDelta = 10000 - mediaControlModel.visibleTimeRange
                    startTimeDelta = selectionStartTime - (10000 - selectionEndTime + selectionStartTime) / 2 - mediaControlModel.visibleStartTime
                }

                timelineControlView.animationDuration = 500;
                mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)
            }
        }

        WisenetMediaMenuSeparator {
            visible: focusedItemIsCamera
        }

        WisenetMediaMenuItem {
            id: addBookmark
            text: WisenetLinguist.addBookmark
            visible: focusedItemIsCamera
            onTriggered: {
                setDefaultSelectionRange()
                bookmarkModel.newBookmark(mediaControlModel.selectedDeviceID, mediaControlModel.selectedChannelID, mediaControlModel.selectedOverlappedID,
                                          selectionStartTime, selectionEndTime)
                bookmarkPopup.setThumbanilImage()
                bookmarkPopup.editBookmark = false
                bookmarkPopup.show()
            }
        }

        WisenetMediaMenuItem {
            id: exportMenuItem
            text: WisenetLinguist.exportVideo
            visible: focusedItemIsCamera && userGroupModel.exportVideo
            onTriggered: {
                var from = selectionStartTime
                var to = selectionEndTime
                if(to == 0) {
                    from = timelineControlView.convertPositionToMsec(mouseX)
                    to = from + mediaControlModel.visibleTimeRange / 10
                    if(to > mediaControlModel.timelineEndTime)
                        to = mediaControlModel.timelineEndTime
                }
                showExportVideoPopup(from, to, [])
            }
        }

        WisenetMediaMenuItem {
            id: eventSrarchMenuItem
            text: WisenetLinguist.eventSearch
            visible: focusedItemIsCamera
            onTriggered: {
                showEventSearchView()
            }
        }

        WisenetMediaMenuItem {
            id: eventLogMenuItem
            text: WisenetLinguist.eventLog
            visible: focusedItemIsCamera && userGroupModel.isAdmin
            onTriggered: {
                showEventLogView()
            }
        }
    }
}
