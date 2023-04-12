import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

Page {
    id: rightPanelView

    property var selectedViewingGrid: null
    signal openBookmarkPopup(var bookmarkId)
    signal bookmarkSelected(var bookmarkId)
    signal openBookmarkExportVideoPopup(var bookmarkId)

    function setVisibleTime(startTime, endTime){
        bookmarkListView.setVisibleTime(startTime, endTime)
    }

    function setSelectedBookmarkId(bookmarkId){
        bookmarkListView.setSelectedBookmarkId(bookmarkId)
    }

    function closeFilterPopups(){
        eventListView.closeCameraFilter()
        eventListView.closeEventFilter()
        bookmarkListView.closeCameraFilter()
        bookmarkListView.closeTimeFilter()
    }

    background: Rectangle{
        color: WisenetGui.contrast_09_dark
    }

    header: TabBar {
        id: bar
        width: parent.width
        height: 42

        background: Rectangle{
            color: WisenetGui.contrast_09_dark
        }

        WisenetTabIconButton {
            text: WisenetLinguist.event
            height: bar.height
            rightBarVisible: true
            imageNormal: "qrc:/Assets/Icon/Monitoring/RightPanel/event_tab_normal.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/RightPanel/event_tab_selected.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/RightPanel/event_tab_hover.svg"
        }

        WisenetTabIconButton {
            text: WisenetLinguist.bookmark
            height: bar.height
            imageNormal: "qrc:/Assets/Icon/Monitoring/RightPanel/bookmark_tab_normal.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/RightPanel/bookmark_tab_selected.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/RightPanel/bookmark_tab_hover.svg"
        }
    }

    StackLayout {
        id : listView
        visible: parent.width == 0 ? false : true
        anchors.fill: parent
        currentIndex: bar.currentIndex

        EventListView {
            id : eventListView

            selectedViewingGrid : rightPanelView.selectedViewingGrid
        }

        BookmarkListView {
            id : bookmarkListView

            selectedViewingGrid : rightPanelView.selectedViewingGrid
            onOpenBookmarkPopup: rightPanelView.openBookmarkPopup(bookmarkId)
            onBookmarkSelected: rightPanelView.bookmarkSelected(bookmarkId)
            onOpenBookmarkExportVideoPopup: rightPanelView.openBookmarkExportVideoPopup(bookmarkId)
        }

        onCurrentIndexChanged: {
            eventListView.closeCameraFilter()
            eventListView.closeEventFilter()
            bookmarkListView.closeCameraFilter()
            bookmarkListView.closeTimeFilter()
        }
    }
}
