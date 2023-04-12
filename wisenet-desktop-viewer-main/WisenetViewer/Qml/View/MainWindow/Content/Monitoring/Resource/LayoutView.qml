import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.3
import WisenetStyle 1.0
import Wisenet.Define 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

Rectangle{
    id: layoutView
    readonly property int contextPopupMargin : 7

    property bool treeDragStarted
    property alias horizontalScrollBarPolicy: layoutTree.horizontalScrollBarPolicy

    signal addNewVideoChannel(var deviceId, var channelId)
    signal addNewWebpage(var webpageId)
    signal addNewLocalFile(var localFileUrl)
    signal addNewItems(var dragItemListModel)
    signal selectLayout(var type, var name, var uuid, var layoutViewModel)
    signal selectSequence(var type, var name, var uuid)
    signal openRenameDialog(var itemType, var uuid, var name);

    function addLayoutItems(layoutId, items){
        resourceViewModel.layoutTreeModel.addLayoutItems(layoutId, items)
    }

    function removeLayoutItems(layoutId, itemIds){
        resourceViewModel.layoutTreeModel.removeLayoutItems(layoutId, itemIds)
    }

    function updateLayoutItems(layoutId, items){
        resourceViewModel.layoutTreeModel.updateLayoutItems(layoutId, items)
    }

    function updateLayoutProperty(layoutId, layoutModel){
        resourceViewModel.layoutTreeModel.updateLayoutProperty(layoutId, layoutModel)
    }

    function applyChangeName(resourceType, uuid, name){
        console.log("LayoutView.applyChangeName : resourceType{}, uuid{}, name{}",resourceType,uuid,name)
        resourceViewModel.layoutTreeModel.changeDisplayName(uuid, resourceType, name)
    }

    function deleteItems()
    {
        var keys = "";

        for(let index of selectionModel.selectedIndexes)
        {
            var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

            if(item.itemType === ItemType.Layout)
            {
                keys += item.uuid
            }
            else if(item.itemType === ItemType.Camera)
            {
                keys += item.itemId
            }
            else if(item.itemType === ItemType.WebPage)
            {
                keys += item.itemId
            }
            else if(item.itemType === ItemType.LocalFile)
            {
                keys += item.itemId
            }
            else if(item.itemType === ItemType.Sequence)
            {
                keys += item.uuid
            }

            keys += ":"
        }

        keys = keys.substring(0, keys.length - 1)

        resourceViewModel.layoutTreeModel.deleteItems(keys);
    }

    function getSelectedCameras(){
        var cameraList = []

        for(let index of selectionModel.selectedIndexes)
        {
            var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

            if(item.itemType === ItemType.Camera)
                cameraList.push(item.uuid)
        }

        return cameraList
    }

    function tabClosedWithoutSave(closedLayouts){
        resourceViewModel.layoutTreeModel.tabClosedWithoutSave(closedLayouts)
    }

    function dragAvailable(type){
        switch(type){
        case ItemType.Camera:
            return true;
        case ItemType.Channel:
            return true;
        case ItemType.LayoutRoot:
            return false;
        case ItemType.Layout:
            return true;
        case ItemType.WebPage:
            return true;
        case ItemType.LocalFile:
            return true;
        case ItemType.Sequence:
            return true;
        default:
            return false;
        }
    }

    function openLayout(layoutId){
        layoutTree.openLayout(layoutId)
    }

    function openSequence(sequenceId){
        layoutTree.openSequence(sequenceId)
    }

    function openLayoutItems(){
        console.log("openLayoutItems()")

        var layoutOpened = false

        dragItemList.removeAll()
        for(let index of selectionModel.selectedIndexes)
        {
            var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

            if(item.itemType === ItemType.Camera)
                dragItemList.append(item.uuid, item.itemType)
            else if(item.itemType === ItemType.WebPage)
                dragItemList.append(item.uuid, item.itemType)
            else if(item.itemType === ItemType.LocalFile)
                dragItemList.append(item.uuid, item.itemType)
            else if(item.itemType === ItemType.Layout){
                layoutOpened = true
                layoutView.selectLayout(ItemType.Layout, item.displayName, item.uuid, item.layoutViewModel)
            }
            else if(item.itemType === ItemType.Sequence)
            {
                console.log("LayoutView.openLayoutItems : ItemType.Sequence")
                dragItemList.append(item.uuid, item.itemType)
            }
        }

        if(!layoutOpened)
            layoutView.addNewItems(dragItemList)
    }

    function openLayoutItemsNewWindow(){
        console.log("openLayoutItemsNewWindow()")

        var layoutList = []

        dragItemList.removeAll()
        for(let index of selectionModel.selectedIndexes)
        {
            var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

            if(item.itemType === ItemType.Camera)
                dragItemList.append(item.uuid, item.itemType)
            else if(item.itemType === ItemType.WebPage)
                dragItemList.append(item.uuid, item.itemType)
            else if(item.itemType === ItemType.LocalFile)
                dragItemList.append(item.uuid, item.itemType)
            else if(item.itemType === ItemType.Layout)
                layoutList.push(item.uuid)
        }

        if(layoutList.length > 0)
            windowHandler.initializeNewWindowWithLayouts(layoutList)
        else
            windowHandler.initializeNewWindowWithItems(dragItemList)
    }

    function expandRoot(){
        var rootRowCount = resourceViewModel.layoutTreeProxyModel.rowCount(layoutTree.rootIndex)

        for(var i=0; i < rootRowCount; i++) {
            var idx = resourceViewModel.layoutTreeProxyModel.index(i, 0, layoutTree.rootIndex)
            layoutTree.expand(idx)
        }
    }

    function expandAll() {
        var rootRowCount = resourceViewModel.layoutTreeProxyModel.rowCount(layoutTree.rootIndex)

        for(var i=0; i < rootRowCount; i++) {
            var idx = resourceViewModel.layoutTreeProxyModel.index(i, 0, layoutTree.rootIndex)
            layoutTree.expand(idx)

            var childRowCount = resourceViewModel.layoutTreeProxyModel.rowCount(idx)
            for(var childRow =0; childRow < childRowCount; childRow++ ){
                expandChild(childRow, idx);
            }
        }
    }

    function expandChild(row, parent){
        var idx = resourceViewModel.layoutTreeProxyModel.index(row, 0, parent);

        layoutTree.expand(idx)

        var rowCount = resourceViewModel.layoutTreeProxyModel.rowCount(idx);

        for(var i=0; i< rowCount; i++){
            expandChild(i, idx);
        }
    }

    function collapseAll() {
        var rootRowCount = resourceViewModel.layoutTreeProxyModel.rowCount(layoutTree.rootIndex)

        for(var i=0; i < rootRowCount; i++) {
            var idx = resourceViewModel.layoutTreeProxyModel.index(i, 0, layoutTree.rootIndex)
            layoutTree.collapse(idx)

            var childRowCount = resourceViewModel.layoutTreeProxyModel.rowCount(idx)
            for(var childRow =0; childRow < childRowCount; childRow++ ){
                collapseChild(childRow, idx);
            }
        }
    }

    function collapseChild(row, parent){
        var idx = resourceViewModel.layoutTreeProxyModel.index(row, 0, parent);

        layoutTree.collapse(idx)

        var rowCount = resourceViewModel.layoutTreeProxyModel.rowCount(idx);

        for(var i=0; i< rowCount; i++){
            collapseChild(i, idx);
        }
    }

    Connections{
        target: (resourceViewModel && resourceViewModel.layoutTreeModel) ? resourceViewModel.layoutTreeModel : null

        onExpandRoot:{
            layoutView.expandRoot()
        }

        onLayoutAdded:{
            layoutView.expandRoot()
        }
    }


    DragItemListModel{
        id: dragItemList
    }

    Rectangle{
        id: upperBorder
        height: 1
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: WisenetGui.contrast_08_dark_grey
    }

    TreeView{
        id: layoutTree
        anchors.fill: parent
        anchors.topMargin: 1
        headerVisible: false
        backgroundVisible: false
        alternatingRowColors: false

        model: (resourceViewModel && resourceViewModel.layoutTreeProxyModel) ? resourceViewModel.layoutTreeProxyModel : null

        selection: ItemSelectionModel {
            id: selectionModel
            model: (resourceViewModel && resourceViewModel.layoutTreeProxyModel) ? resourceViewModel.layoutTreeProxyModel : null
        }

        selectionMode: SelectionMode.ExtendedSelection

        __listView.onCountChanged: {
            var minWidth = 0
            var listdata = __listView.children[0]

            for (var row = 0 ; row < listdata.children.length ; ++row) {
                var item = listdata.children[row] ? listdata.children[row].rowItem : undefined
                if (item) { // FocusScope { id: rowitem }
                    item = item.children[1]
                    if (item) { // Row { id: itemrow }
                        item = item.children[0]
                        if (item) { // Repeater.delegate a.k.a. __view.__itemDelegateLoader
                            var indent = item.__itemIndentation
                            item = item.item
                            if (item && item.hasOwnProperty("implicitWidth")) {
                                minWidth = Math.max(minWidth, item.implicitWidth + indent)
                            }
                        }
                    }
                }
            }

            if(minWidth > 0)
                tvColumn.width = minWidth + 20
        }

        function updateDragItemList()
        {
            console.log("updateDragItemList START--");
            dragItemList.removeAll()
            for(let index of selectionModel.selectedIndexes)
            {
                console.log("updateDragItemList ::", index);
                var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

                if(!dragAvailable(item.itemType))
                    continue

                dragItemList.append(item.uuid, item.itemType)
            }
            console.log("updateDragItemList END--");
        }

        function openLayout(layoutId){
            for(let index of selectionModel.selectedIndexes)
            {
                var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

                if(item.uuid === layoutId)
                {
                    console.log("layoutTree.openLayout()", item.displayName, item.uuid);
                    layoutView.selectLayout(ItemType.Layout, item.displayName, item.uuid, item.layoutViewModel)

                    break;
                }
            }
        }

        function openSequence(sequenceId){
            for(let index of selectionModel.selectedIndexes)
            {
                var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

                if(item.uuid === sequenceId)
                {
                    console.log("layoutTree.openSequence()", item.displayName, item.uuid);
                    layoutView.selectSequence(ItemType.Sequence, item.displayName, item.uuid)

                    break;
                }
            }
        }

        DropArea{
            id: upperArea

            visible: layoutView.treeDragStarted
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 12

            onEntered: {
                drag.accepted = false
                if(layoutTree.__listView.contentY - 10 > 0)
                    layoutTree.__listView.contentY = layoutTree.__listView.contentY - 10
                else
                    layoutTree.__listView.contentY = 0
            }
        }

        DropArea{
            id: lowerArea

            visible: layoutView.treeDragStarted
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottomMargin: layoutTree.__horizontalScrollBar.visible ? 13 : 0
            height: 12

            onEntered: {
                drag.accepted = false
                if(layoutTree.__listView.contentHeight >= layoutTree.__listView.contentY + layoutTree.height + 10)
                    layoutTree.__listView.contentY = layoutTree.__listView.contentY + 10
                else
                    layoutTree.__listView.contentY = layoutTree.__listView.contentHeight - layoutTree.height
            }
        }

        MouseArea {
            id: treeMouseArea
            anchors.fill: parent
            anchors.rightMargin: layoutTree.__verticalScrollBar.visible ? 13 : 0
            anchors.bottomMargin: layoutTree.__horizontalScrollBar.visible ? 13 : 0
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            hoverEnabled: true

            property alias __listView: layoutTree.__listView
            property alias modelAdaptor: layoutTree.__model
            property var clickedIndex: undefined
            property var pressedIndex: undefined
            property int hoveredRow : -1
            property bool selectOnRelease: false
            readonly property alias currentRow: layoutTree.__currentRow
            readonly property alias currentIndex: layoutTree.currentIndex
            readonly property alias selection: layoutTree.selection
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
                        var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);

                        if(!dragAvailable(item.itemType))
                            continue

                        dragStarted = true
                        dragItemList.append(item.uuid, item.itemType)
                    }
                    console.log("updateDragItemList END--");
                    if (dragStarted) {
                        layoutTree.Drag.active = true
                    }
                }
            }

            function dragStop()
            {
                if (dragStarted) {
                    dragStarted = false;
                    layoutTree.Drag.active = false;
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
                    console.log("---LayoutTree::onPressed pressedRow === -1")
                    if (mouse.button == Qt.RightButton)
                        resourceContextMenu.popupExpandMenu(layoutTree, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin, false)
                    return
                }

                // Context menu
                if (mouse.button == Qt.RightButton){
                    console.log("LayoutTree::onPressed items : ", selectionModel.selectedIndexes, selectionModel.selectedIndexes.length)
                    console.log("pressedIndex=",pressedIndex)

                    if(selectionModel.isSelected(pressedIndex))
                    {
                        console.log("selectionModel.isSelected(pressedIndex)=",pressedIndex)
                        var resourceItem = resourceViewModel.layoutTreeProxyModel.data(pressedIndex, 0x0100+1);
                        console.log("resourceItem=",resourceItem)

                        if(selectionModel.selectedIndexes.length > 1){
                            var layoutSelected = false
                            var sequenceSelected = false
                            var cameraSelected = false
                            var webpageSelected = false
                            var mediaFileSelected = false

                            for(let index of selectionModel.selectedIndexes)
                            {
                                var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);
                                console.log("item=",item)

                                if(item.itemType === ItemType.Layout)
                                    layoutSelected = true
                                else if(item.itemType === ItemType.Sequence)
                                    sequenceSelected = true
                                else if(item.itemType === ItemType.Camera)
                                    cameraSelected = true
                                else if(item.itemType === ItemType.WebPage)
                                    webpageSelected = true
                                else if(item.itemType === ItemType.LocalFile)
                                    mediaFileSelected = true
                            }

                            console.log("layoutSelected=",layoutSelected," sequenceSelected=",sequenceSelected)

                            if(layoutSelected || sequenceSelected)
                                resourceContextMenu.popupMultiItemMenu(resourceItem, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin, false)
                            else
                            {
                                if(webpageSelected || mediaFileSelected)
                                    resourceContextMenu.popupMultiItemMenu(resourceItem, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin, false)
                                else
                                {
                                    resourceContextMenu.popupMultiChannelMenu(resourceItem, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin, true, false)
                                }

                            }
                        }
                        else{
                            resourceContextMenu.popupMenu(resourceItem, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin, false, resourceItem.layoutViewModel)
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
                    var resourceItem1 = resourceViewModel.layoutTreeProxyModel.data(pressedIndex, 0x0100+1);
                    console.log("pressedIndex=",pressedIndex)
                    console.log("resourceItem1=",resourceItem1)
                    resourceContextMenu.popupMenu(resourceItem1, mouse.x + contextPopupMargin, mouse.y + contextPopupMargin, false, resourceItem1.layoutViewModel)
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
                    if (!layoutTree.__activateItemOnSingleClick)
                        layoutTree.activated(modelIndex)
                    layoutTree.doubleClicked(modelIndex)
                }
            }

            onDragStartedChanged: {
                layoutView.treeDragStarted = dragStarted
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

            itemDelegate: DropArea{
                id: itemDropArea
                height : 24
                implicitWidth: resourceItem.itemWidth

                onDropped: function(dragEvent)  {
                    console.log("tree onDropped !! dropped [" + dragEvent.text + "]")
                    console.log("tree onDropped !! current [" + resourceItem.uuid + "]")

                    resourceViewModel.layoutTreeModel.dropSource(resourceItem.uuid, dragEvent.source)
                    layoutTree.expand(styleData.index)
                }

                onEntered : function(dragEvent) {
                    if(styleData.value.itemType === ItemType.Layout)
                    {
                        //console.log("LayoutView dropArea userGroupModel:", userGroupModel.userId, ", layoutViewModel:", styleData.value.layoutViewModel.ownerId)

                        if(dragEvent.source === null)
                        {
                            dragEvent.accepted = false
                            return
                        }

                        if(userGroupModel.userId === styleData.value.layoutViewModel.ownerId)
                            dragEvent.accepted = true
                        else
                        {
                            dragEvent.accepted = false
                            return
                        }


                        if(styleData.value.layoutViewModel.locked)
                        {
                            dragEvent.accepted = false
                            return
                        }
                        else
                            dragEvent.accepted = true
                    }
                    else
                        dragEvent.accepted = false

                    //console.log("tree onEntered !!")
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
                    anchors.fill: parent
                    height: 24

                    backColor : "transparent"
                    userId: userGroupModel ? userGroupModel.userId : ""

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

        Keys.onPressed: {
            if (event.key === Qt.Key_Delete)
                deleteItems();
            else if (event.key === Qt.Key_F2 && selectionModel.selectedIndexes.length === 1) {
                var index = selectionModel.selectedIndexes[0];
                var item = resourceViewModel.layoutTreeModel.data(resourceViewModel.layoutTreeProxyModel.mapToSource(index), 0x0100+1);
                if (item.itemType === ItemType.Layout || item.itemType === ItemType.Sequence) {
                    layoutView.openRenameDialog(item.itemType, item.uuid, item.displayName);
                }
            }
        }

        onDoubleClicked: {
            var resourceItem = resourceViewModel.layoutTreeProxyModel.data(index, 0x0100+1);

            if(resourceItem.itemType === ItemType.Camera){
                var ids = resourceItem.uuid.split('_')
                console.log("onDoubleClicked == Camera [deviceId[" + ids[0] + "] number[" + ids[1] + "]" )
                layoutView.addNewVideoChannel(ids[0], ids[1])
            }
            else if(resourceItem.itemType === ItemType.Layout){
                console.log("onDoubleClicked == Layout [name[" + resourceItem.displayName + "] uuid[" + resourceItem.uuid + "]" )
                layoutView.selectLayout(resourceItem.itemType, resourceItem.displayName, resourceItem.uuid, resourceItem.layoutViewModel)
            }
            else if(resourceItem.itemType === ItemType.Sequence) {
                console.log("onDoubleClicked == Sequence [name[" + resourceItem.displayName + "] uuid[" + resourceItem.uuid + "]" )
                layoutView.selectSequence(resourceItem.itemType, resourceItem.displayName, resourceItem.uuid)
            }

            else if (resourceItem.itemType === ItemType.WebPage) {
                console.log("onDoubleClicked == Web, id=", resourceItem.uuid);
                layoutView.addNewWebpage(resourceItem.uuid);
            }
            else if (resourceItem.itemType === ItemType.LocalFile) {
                console.log("onDoubleClicked == LocalFile, id=", resourceItem.uuid);
                layoutView.addNewLocalFile(resourceItem.uuid);
            }
            else{
                if(layoutTree.isExpanded(index)){
                    layoutTree.collapse(index)
                }
                else{
                    layoutTree.expand(index)
                    console.log(index);
                }
            }
        }

        Drag.dragType: Drag.Automatic // 꼭해야됨.. drop이 될려면
        Drag.supportedActions: Qt.MoveAction // icon 변경
        Drag.source: dragItemList
    }
}

