import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.3
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.15 as Controls2
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

Rectangle{
    id: resourceTreeView

    readonly property int contextPopupMargin : 7

    property bool treeDragStarted

    signal addNewLocalFile(var localFileUrl)
    signal addNewItems(var dragItemListModel)
    signal verifyFinished(var fileUuid, var verifyResult)

    function deleteItems(remove) {
        var localFileList = []

        for(let index of selectionModel.selectedIndexes)
        {
            var item = resourceViewModel.treeModel.data(resourceViewModel.treeProxyModel.mapToSource(index), 0x0100+1);

            if(item.itemType === ItemType.LocalFile)
                localFileList.push(item.uuid)
        }

        console.log("ResourceView.deleteItems() localfile ", localFileList)

        resourceViewModel.treeModel.deleteItems(localFileList, remove);
    }

    function dragAvailable(type) {
        switch(type){
        case ItemType.MainRoot:
            return false;
        case ItemType.ResourceRoot:
            return false;
        case ItemType.LocalFileRoot:
            return false;
        case ItemType.LocalFile:
            return true;
        default:
            return false;
        }
    }

    function openMediaFileDialog() {
        mediaFileLoadDialog.visible = true
    }

    function openResourceItems() {
        console.log("openResourceItems()");

        dragItemList.removeAll()
        for(let index of selectionModel.selectedIndexes) {
            var item = resourceViewModel.treeModel.data(resourceViewModel.treeProxyModel.mapToSource(index), 0x0100+1);

            if(!dragAvailable(item.itemType))
                continue

            dragItemList.append(item.uuid, item.itemType)
        }

        resourceTreeView.addNewItems(dragItemList)
    }

    function isWnmFile(itemUuid){
        var fileSplit = itemUuid.split('.');
        var ext = fileSplit[fileSplit.length-1];
        //console.log("isWnmFile ", itemUuid, ext)

        if(ext === "wnm")
            return true
        else
            return false
    }

    signal orderByAscending()
    signal orderByDescending()
    signal expandTree()
    signal collapseTree()

    Connections {
        target: resourceView

        function onOrderByAscending() {
            console.log("ResourceView onOrderByAscending")
            resourceViewModel.treeProxyModel.doSort(Qt.AscendingOrder)
        }

        function onOrderByDescending() {
            console.log("ResourceView onOrderByDescending")
            resourceViewModel.treeProxyModel.doSort(Qt.DescendingOrder)
        }

        function onExpandTree() {
            expandAll()
        }

        function onCollapseTree() {
            collapseAll()
        }
    }

    Connections {
        target: resourceViewModel.treeProxyModel

        function onFilterTextChanged() {
            console.log("~~onFilterTextChanged() resourceViewModel.treeProxyModel")

            if(searchTextBox.text !== "")
                expandAll()
            else
                collapseAll()
        }

        function onFilterParentExpand(index) {
            console.log("~~onFilterParentExpand " + index)
            resourceTree.expand(index)
        }

        function onFilterParentExpand2(index) {
            console.log("~~onFilterParentExpand2 " + index)
            var qindex = resourceTree.indexAt(index, 0)
            resourceTree.expand(qindex)
        }
    }

    Connections {
        target: resourceViewModel.treeModel

        function onExpandRoot() {
            resourceTreeView.expandRoot()
        }

        function onExpandSelectedIndex() {
            resourceTree.expand(selectionModel.currentIndex)
        }

        function onVerifyFinished(fileUuid, verifyResult) {
            resourceTreeView.verifyFinished(fileUuid, verifyResult)
        }
    }

    function expandRoot() {
        var rootRowCount = resourceViewModel.treeProxyModel.rowCount(resourceTree.rootIndex)

        for(var i=0; i < rootRowCount; i++) {
            var idx = resourceViewModel.treeProxyModel.index(i, 0, resourceTree.rootIndex)
            resourceTree.expand(idx)
        }
    }

    function expandAll() {
        var rootRowCount = resourceViewModel.treeProxyModel.rowCount(resourceTree.rootIndex)

        for(var i=0; i < rootRowCount; i++) {
            var idx = resourceViewModel.treeProxyModel.index(i, 0, resourceTree.rootIndex)
            resourceTree.expand(idx)

            var childRowCount = resourceViewModel.treeProxyModel.rowCount(idx)
            for(var childRow =0; childRow < childRowCount; childRow++ ){
                expandChild(childRow, idx);
            }
        }
    }

    function expandChild(row, parent) {
        var idx = resourceViewModel.treeProxyModel.index(row, 0, parent);

        resourceTree.expand(idx)

        var rowCount = resourceViewModel.treeProxyModel.rowCount(idx);

        for(var i=0; i< rowCount; i++) {
            expandChild(i, idx);
        }
    }

    function collapseAll() {
        var rootRowCount = resourceViewModel.treeProxyModel.rowCount(resourceTree.rootIndex)

        for(var i=0; i < rootRowCount; i++) {
            var idx = resourceViewModel.treeProxyModel.index(i, 0, resourceTree.rootIndex)
            resourceTree.collapse(idx)

            var childRowCount = resourceViewModel.treeProxyModel.rowCount(idx)
            for(var childRow =0; childRow < childRowCount; childRow++ ) {
                collapseChild(childRow, idx);
            }
        }
    }

    function collapseChild(row, parent) {
        var idx = resourceViewModel.treeProxyModel.index(row, 0, parent);

        resourceTree.collapse(idx)

        var rowCount = resourceViewModel.treeProxyModel.rowCount(idx);

        for(var i=0; i< rowCount; i++) {
            collapseChild(i, idx);
        }
    }

    FileDialog {
        id: mediaFileLoadDialog
        title: WisenetLinguist.openMediaFiles
        nameFilters: ["Video files(*.mkv *.mp4 *.avi *.wnm *.sec)", "Image files(*.png *.jpg)"]
        //folder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
        onAccepted: {
            console.log("mediaFileLoadDialog::onAccepted() " + mediaFileLoadDialog.fileUrl + " " + Qt.platform.os.toString())

            resourceViewModel.treeModel.addLocalFile(mediaFileLoadDialog.fileUrl)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    DragItemListModel {
        id: dragItemList
    }

    Rectangle {
        id: upperBorder
        height: 1
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: WisenetGui.contrast_08_dark_grey
    }

    WisenetSearchBox {
        id: searchTextBox
        visible: parent.width == 0 ? false : true
        height: 30
        anchors.top: upperBorder.bottom
        anchors.topMargin: 3
        anchors.left: parent.left
        anchors.right: parent.right
        bgColor: "transparent"

        onSearchTextChanged:{
            console.log("Resource tree - onSearchTextChanged :" + text)
            resourceViewModel.treeProxyModel.setFilterText(text)
        }
    }

    Rectangle{
        id: middleBorder
        height: 1
        anchors.top: searchTextBox.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 15
        anchors.rightMargin: 14
        color: WisenetGui.contrast_08_dark_grey
    }

    Rectangle{
        id: resourceTreeRectangle
        anchors.top: middleBorder.bottom
        anchors.left: parent.left
        anchors.leftMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: 3
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 3

        color: "transparent"

        TreeView {
            id: resourceTree
            anchors.fill: parent
            headerVisible: false
            backgroundVisible: false
            alternatingRowColors: false

            model: resourceViewModel.treeProxyModel

            selection: ItemSelectionModel {
                id: selectionModel
                model: resourceViewModel.treeProxyModel
            }

            selectionMode: SelectionMode.ExtendedSelection

            function updateDragItemList()
            {
                console.log("updateDragItemList START--");
                dragItemList.removeAll()
                for(let index of selectionModel.selectedIndexes)
                {
                    console.log("updateDragItemList ::", index);
                    var item = resourceViewModel.treeModel.data(resourceViewModel.treeProxyModel.mapToSource(index), 0x0100+1);

                    if(!dragAvailable(item.itemType))
                        continue

                    dragItemList.append(item.uuid, item.itemType)
                }
                console.log("updateDragItemList END--");
                //resourceTree.Drag.active = true
            }

            DropArea{
                id: upperArea

                visible: resourceTreeView.treeDragStarted
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 12

                onEntered: {
                    drag.accepted = false
                    if(resourceTree.__listView.contentY - 10 > 0)
                        resourceTree.__listView.contentY = resourceTree.__listView.contentY - 10
                    else
                        resourceTree.__listView.contentY = 0
                }
            }

            DropArea{
                id: lowerArea

                visible: resourceTreeView.treeDragStarted
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 12

                onEntered: {
                    drag.accepted = false
                    if(resourceTree.__listView.contentHeight >= resourceTree.__listView.contentY + resourceTree.height + 10)
                        resourceTree.__listView.contentY = resourceTree.__listView.contentY + 10
                    else
                        resourceTree.__listView.contentY = resourceTree.__listView.contentHeight - resourceTree.height
                }
            }

            MouseArea {
                id: treeMouseArea
                anchors.fill: parent
                anchors.rightMargin: resourceTree.__verticalScrollBar.visible ? 13 : 0
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                hoverEnabled: true
                focus: true

                property alias __listView: resourceTree.__listView
                property alias modelAdaptor: resourceTree.__model
                property var clickedIndex: undefined
                property var pressedIndex: undefined
                property int hoveredRow : -1
                property bool selectOnRelease: false
                readonly property alias currentRow: resourceTree.__currentRow
                readonly property alias currentIndex: resourceTree.currentIndex
                readonly property alias selection: resourceTree.selection
                z: 0
                preventStealing: false
                property bool shiftPressed: false
                property bool dragStarted: false

                function dragStart()
                {
                    if (!dragStarted) {
                        dragStarted = false;
                        dragItemList.removeAll()
                        for(let index of selectionModel.selectedIndexes)
                        {
                            console.log("updateDragItemList ::", index);
                            var item = resourceViewModel.treeModel.data(resourceViewModel.treeProxyModel.mapToSource(index), 0x0100+1);

                            if(!dragAvailable(item.itemType))
                                continue

                            dragStarted = true
                            dragItemList.append(item.uuid, item.itemType)
                        }
                        console.log("updateDragItemList END--");
                        if (dragStarted) {
                            resourceTree.Drag.active = true
                        }
                    }
                }

                function dragStop()
                {
                    if (dragStarted) {
                        dragStarted = false;
                        resourceTree.Drag.active = false;
                    }
                }

                function mouseSelect(modelIndex, modifiers) {
                    console.log("mouseSelect()", modelIndex, modifiers, drag);
                    selection.setCurrentIndex(modelIndex, ItemSelectionModel.NoUpdate)
                    var selectRowRange = modifiers & Qt.ShiftModifier
                    var itemSelection = !selectRowRange || clickedIndex === modelIndex ? modelIndex
                                                                                       : modelAdaptor.selectionForRowRange(clickedIndex, modelIndex)

                    if (modifiers & Qt.ControlModifier) {
                        selection.select(modelIndex, ItemSelectionModel.Toggle)
                    } else if (modifiers & Qt.ShiftModifier) {
                        selection.select(itemSelection, ItemSelectionModel.SelectCurrent)
                    } else {
                        clickedIndex = modelIndex
                        selection.select(modelIndex, ItemSelectionModel.ClearAndSelect)
                    }
                }

                function branchDecorationContains(x, y) {
                    var clickedItem = __listView.itemAt(0, y + __listView.contentY)
                    if (!(clickedItem && clickedItem.rowItem))
                        return false
                    var branchDecoration = clickedItem.rowItem.branchDecoration
                    if (!branchDecoration)
                        return false
                    var pos = mapToItem(branchDecoration, x, y)
                    return branchDecoration.contains(Qt.point(pos.x, pos.y))
                }

                function keySelect(keyModifiers) {
                    if (!keyModifiers)
                        clickedIndex = currentIndex
                    if (!(keyModifiers & Qt.ControlModifier))
                        mouseSelect(currentIndex, keyModifiers)
                }

                onPressed: {
                    console.log("---onPressed");
                    var pressedRow = __listView.indexAt(0, mouseY + __listView.contentY)
                    pressedIndex = modelAdaptor.mapRowToModelIndex(pressedRow)
                    selectOnRelease = false
                    __listView.forceActiveFocus()

                    if (branchDecorationContains(mouse.x, mouse.y)) {
                        return
                    }

                    if (pressedRow === -1) {
                        console.log("---ResourceTree::onPressed pressedRow === -1")
                        if (mouse.button == Qt.RightButton) {
                            resourceContextMenu.wnmList = []
                            resourceContextMenu.popupMenu(resourceTree, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin)
                        }
                        return
                    }

                    // Context menu
                    if (mouse.button == Qt.RightButton){
                        console.log("ResourceTree::onPressed items : ", selectionModel.selectedIndexes, selectionModel.selectedIndexes.length)

                        if(selectionModel.isSelected(pressedIndex)) {
                            var resourceItem = resourceViewModel.treeProxyModel.data(pressedIndex, 0x0100+1);

                            resourceContextMenu.wnmList = []

                            if(selectionModel.selectedIndexes.length > 1) {
                                var mediaFileSelected = false

                                for(let index of selectionModel.selectedIndexes) {
                                    var item = resourceViewModel.treeModel.data(resourceViewModel.treeProxyModel.mapToSource(index), 0x0100+1);

                                    if(item.itemType === ItemType.LocalFile) {
                                        mediaFileSelected = true
                                        if(isWnmFile(item.uuid)) {
                                            resourceContextMenu.wnmList.push(item.uuid)
                                        }
                                    }
                                }

                                if(mediaFileSelected) {
                                    resourceContextMenu.popupMenu(resourceItem, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin)
                                }
                            }
                            else {
                                if(isWnmFile(resourceItem.uuid)) {
                                    resourceContextMenu.wnmList.push(resourceItem.uuid)
                                }
                                resourceContextMenu.popupMenu(resourceItem, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin)
                            }

                            return;
                        }
                    }

                    if (selection.isSelected(pressedIndex)) {
                        selectOnRelease = true
                        return
                    }

                    __listView.currentIndex = pressedRow

                    if (!clickedIndex)
                        clickedIndex = pressedIndex

                    mouseSelect(pressedIndex, mouse.modifiers)

                    if (mouse.button == Qt.RightButton){
                        var resourceItem1 = resourceViewModel.treeProxyModel.data(pressedIndex, 0x0100+1);
                        resourceContextMenu.wnmList = []
                        if(resourceItem1.itemType === ItemType.LocalFile && isWnmFile(resourceItem1.uuid)) {
                            resourceContextMenu.wnmList.push(resourceItem1.uuid)
                        }
                        resourceContextMenu.popupMenu(resourceItem1, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin)
                    }

                    if (!mouse.modifiers)
                        clickedIndex = pressedIndex
                }

                onPressedChanged: {
                    console.log("---onPressedChanged", pressed);

                    if(pressed === false){
                        dragStop()
                    }
                }

                onReleased: {
                    console.log("---onReleased");

                    // Multi select 상태에서 특정 row를 press 하고 release 했을 때
                    if (selectOnRelease) {
                        var releasedRow = __listView.indexAt(0, mouseY + __listView.contentY)
                        var releasedIndex = modelAdaptor.mapRowToModelIndex(releasedRow)
                        if (releasedRow >= 0 && releasedIndex === pressedIndex)
                            mouseSelect(pressedIndex, mouse.modifiers)
                    }

                    pressedIndex = undefined
                    selectOnRelease = false
                    hoveredRow = -1

                    dragStop();
                }

                onPositionChanged: {
                    if (mouse.buttons == Qt.LeftButton && containsMouse) {
                        dragStart();
                    }

                    hoveredRow = __listView.indexAt(0, mouseY + __listView.contentY)
                }

                onExited: {
                    pressedIndex = undefined
                    selectOnRelease = false
                    hoveredRow = -1
                }

                onCanceled: {
                    pressedIndex = undefined
                    selectOnRelease = false
                }

                onClicked: {
                    var clickIndex = __listView.indexAt(0, mouseY + __listView.contentY)
                    if (clickIndex > -1) {
                        var modelIndex = modelAdaptor.mapRowToModelIndex(clickIndex)

                        // Tree exapnd/collapse 동작
                        if (branchDecorationContains(mouse.x, mouse.y)) {
                            if (modelAdaptor.isExpanded(modelIndex))
                                modelAdaptor.collapse(modelIndex)
                            else
                                modelAdaptor.expand(modelIndex)
                        }
                    }
                }

                onDoubleClicked: {
                    var clickIndex = __listView.indexAt(0, mouseY + __listView.contentY)
                    if (clickIndex > -1) {
                        var modelIndex = modelAdaptor.mapRowToModelIndex(clickIndex)
                        if (!resourceTree.__activateItemOnSingleClick)
                            resourceTree.activated(modelIndex)
                        resourceTree.doubleClicked(modelIndex)
                    }
                }

                onDragStartedChanged: {
                    resourceTreeView.treeDragStarted = dragStarted
                }
            }

            TableViewColumn {
                id: tvColumn
                role: "display"
                title: "DeviceTree"
            }

            style: WisenetTreeViewStyle {
                id: treeItemStyle
                textColor: "white"

                itemDelegate: DropArea {
                    id: itemDropArea
                    height : 24
                    implicitWidth: resourceItem.implicitWidth + 16

                    /*
                    onDropped: function(dragEvent)  {
                        console.log("tree onDropped !! dropped [" + dragEvent.text + "]")
                        console.log("Resource Tree onDropped !! current [" + resourceItem.uuid + "]")

                        resourceViewModel.treeModel.dropSource(styleData.index, resourceItem.uuid, dragEvent.source)
                        resourceTree.expand(styleData.index)
                    }
                    */

                    onEntered : function(dragEvent) {
                        dragEvent.accepted = false
                        return
                    }

                    states: [
                        State {
                            when: itemDropArea.containsDrag
                            PropertyChanges {
                                target: resourceItem
                                borderWidth : 1
                            }
                        }
                    ]

                    ResourceItem {
                        id: resourceItem
                        height : parent.height
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        backColor : "transparent"

                        textColor : (styleData.selected || styleData.row === treeMouseArea.hoveredRow) ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                        brightIcon : (styleData.selected || styleData.row === treeMouseArea.hoveredRow)
                    }
                }

                rowDelegate: Rectangle {
                    id: rowDelegate
                    height: 24
                    color: styleData.selected ? WisenetGui.color_primary_dark : (styleData.row === treeMouseArea.hoveredRow ?  WisenetGui.color_primary : "transparent")
                }
            }

            onDoubleClicked: {
                var resourceItem = resourceViewModel.treeProxyModel.data(index, 0x0100+1);
                //console.log("onDoubleClicked + [" + resourceItem.resourceName + "] row[" + resourceItem.rowNumber + "]" )

                if (resourceItem.itemType === ItemType.LocalFile) {
                    console.log("onDoubleClicked == LocalFile, id=", resourceItem.uuid);
                    resourceTreeView.addNewLocalFile(resourceItem.uuid);
                }
                else {
                    if(resourceTree.isExpanded(index)){
                        resourceTree.collapse(index)
                    }
                    else{
                        resourceTree.expand(index)
                    }
                }
            }

            Drag.dragType: Drag.Automatic // 꼭해야됨.. drop이 될려면
            Drag.supportedActions: Qt.MoveAction // icon 변경
            Drag.source: dragItemList
        }
    }

    ContextMenuView {
        id: resourceContextMenu

        onAddNewLocalFile: resourceTreeView.openResourceItems()
        onExpandAll: resourceTreeView.expandAll()
        onCollapseAll: resourceTreeView.collapseAll()
    }
}
