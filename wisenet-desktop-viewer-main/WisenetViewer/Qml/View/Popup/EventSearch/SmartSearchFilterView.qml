import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Wisenet.DeviceSelectionTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import WisenetLanguage 1.0

Rectangle{
    id: searchFilterView
    visible: true
    color: WisenetGui.contrast_09_dark

    property int itemDefaultLeftMargin: 24

    signal search(date from, date to, bool allChannel, var channels, bool allEvent, var events)
    signal resetListPosition()

    function reset(){
        deviceSeletectionTree.reset()
    }

    function initializeTree(){
        deviceTreeSourceModel.initializeTree()
    }

    function expandAll() {
        deviceSeletectionTree.expandAll()
    }

    Component.onCompleted: {
        reset();
    }

    //장치
    Rectangle{
        id : resoureRect
        anchors.fill: parent
        anchors.bottomMargin: 30

        ResourceTreeProxyModel {
            id: deviceTreeProxyModel
            sourceModel: DeviceSelectionTreeSourceModel {
                id: deviceTreeSourceModel
                onInvalidate: deviceTreeProxyModel.invalidate()
                onTreeInitialized: expandAll()
            }

            Component.onCompleted: {
                doSort(Qt.AscendingOrder)
            }
        }

        WisenetDraggableTree {
            id: deviceSeletectionTree
            anchors.fill: parent
            treeProxyModel: deviceTreeProxyModel
            sourceModel: deviceTreeSourceModel
            isSmartSearchTree: true

            bgColor: WisenetGui.contrast_09_dark
            borderColor: WisenetGui.contrast_08_dark_grey

            onTreeDoubleClicked: {
                console.debug("SmartSearch::onTreeDoubleClicked", uuid)
                var id = uuid.split('_')
                if(id.length === 2)
                    searchPlaybackView.viewingGrid.checkNewVideoChannel(id[0], id[1], false, 0, -1, true, "", false)
            }
        }
    }

    Rectangle{
        id: eventSearchFilterBorder
        x: searchFilterView.width - 1
        y: 0
        width: 1
        height: searchFilterView.height
        color: WisenetGui.contrast_08_dark_grey
    }
}

