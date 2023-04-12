import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"

WisenetMediaMenu {
    id: contextMenu
    implicitWidth: {
        var textWidth = Math.max(dummyEventLogText.contentWidth, dummyZoomToBookmarkText.contentWidth) + 32
        if(textWidth < 180)
            textWidth = 180
        return textWidth
    }

    WisenetMediaMenuItem {
        id: zoomToBookmarkMenu
        text: WisenetLinguist.zoomToBookmark
        onTriggered: {
            selectionStartTime = bookmarkModel.startTime
            selectionEndTime = bookmarkModel.endTime

            var startTime = bookmarkModel.startTime
            var endTime = bookmarkModel.endTime
            var rangeDelta = endTime - startTime - mediaControlModel.visibleTimeRange  // 표시 범위 변화량
            var startTimeDelta = startTime - mediaControlModel.visibleStartTime    // 표시 시작 시간 변화량

            if(mediaControlModel.visibleTimeRange + rangeDelta <= 10000) {
                rangeDelta = 10000 - mediaControlModel.visibleTimeRange
                startTimeDelta = startTime - (10000 - endTime + startTime) / 2 - mediaControlModel.visibleStartTime
            }

            timelineControlView.animationDuration = 500;
            mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)
        }

        Text {
            id: dummyZoomToBookmarkText
            text: zoomToBookmarkMenu.text
            color: WisenetGui.transparent
            font.pixelSize: 12
        }
    }

    WisenetMediaMenuItem {
        id: editBookmarkMenu
        text: WisenetLinguist.editBookmark
        onTriggered: {
            selectionStartTime = bookmarkModel.startTime
            selectionEndTime = bookmarkModel.endTime
            bookmarkPopup.editBookmark = true
            bookmarkPopup.show()
        }
    }

    WisenetMediaMenuItem {
        id: deleteBookmarkMenu
        text: WisenetLinguist.deleteBookmark
        onTriggered: {
            bookmarkDeleteConfirmPopup.show()
        }
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem {
        id: exportMenuItem
        text: WisenetLinguist.exportVideo
        visible: userGroupModel && userGroupModel.exportVideo
        onTriggered: {
            showExportVideoPopup(bookmarkModel.startTime, bookmarkModel.endTime, [])
        }
    }

    WisenetMediaMenuItem {
        id: eventSrarchMenuItem
        text: WisenetLinguist.eventSearch
        onTriggered: {
            selectionStartTime = bookmarkModel.startTime
            selectionEndTime = bookmarkModel.endTime
            showEventSearchView()
        }
    }

    WisenetMediaMenuItem {
        id: eventLogMenuItem
        text: WisenetLinguist.eventLog
        visible: userGroupModel && userGroupModel.isAdmin
        onTriggered: {
            showEventLogView()
        }

        Text {
            id: dummyEventLogText
            text: eventLogMenuItem.text
            color: WisenetGui.transparent
            font.pixelSize: 12
        }
    }
}


