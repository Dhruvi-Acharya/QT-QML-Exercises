import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Wisenet.MediaController 1.0
import WisenetStyle 1.0
import "qrc:/"
import "qrc:/MediaController/"
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0

Item {
    id: eventSearchPlayback

    property bool isFocusedWindow : false
    property alias viewingGrid: viewingGrid
    property alias mediaControllerView: mediaControllerView

    readonly property alias viewItemCount : viewingGrid.itemCount

    signal smartSearchRequest()
    signal focusedViewingGridItemChanged()
    signal selectedTrackIdChanged(var trackId)
    signal openMessageDialog(var message)

    ViewingGrid {
        id: viewingGrid

        isFocusedWindow: eventSearchPlayback.isFocusedWindow
        focus: true
        anchors.fill:parent
        anchors.bottomMargin: mediaControllerView.height

        viewX : parent.x
        viewY : parent.y
        viewWidth: parent.width <= 220 ? 220 : parent.width
        viewHeight: parent.height - anchors.bottomMargin
        standalone: true
        sleepModeDuration: 0

        onSmartSearchRequest: eventSearchPlayback.smartSearchRequest()
        onFocusedItemChanged: focusedViewingGridItemChanged()
        onOpenMessageDialog: eventSearchPlayback.openMessageDialog(message)
    }

    MediaControllerView {
        id: mediaControllerView

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        simpleTimeline: true
        selectedViewingGrid: viewingGrid
        focusedViewingGridItem: viewingGrid.focusedItem

        onSelectedTrackIdChanged: eventSearchPlayback.selectedTrackIdChanged(trackId)

        onExportProgressView_Open: {
            searchService.exportProgressView_Open()
        }
    }
}
