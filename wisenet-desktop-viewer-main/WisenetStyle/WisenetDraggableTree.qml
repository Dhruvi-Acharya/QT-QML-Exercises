import QtQuick 2.15
import QtQuick.Controls 2.15 as Control2
import QtQml.Models 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

Item {
    id: wisenetDraggableTree
    anchors.fill: parent

    //public variable
    property var treeProxyModel : null
    property var sourceModel : null
    property var selectedDeviceValue: null
    property int treeSelectionMode : SelectionMode.MultiSelection
    property color bgColor : WisenetGui.contrast_08_dark_grey
    property color borderColor : WisenetGui.color_setup_line
    property real radius: 0
    property bool isSmartSearchTree : false

    property alias treeItemSelectionModelVal: treeItemSelectionModel

    function reset(){
        selectedDeviceValue = null;
        searchBox.reset()
    }

    //public signal
    signal selectedDeviceChanged()
    signal treeDoubleClicked(var uuid)

    function getResourceIcon(type, status, smartSearchCap){
        switch(type){
        case ItemType.EventEmail:
        case ItemType.MainRoot:
            break
        case ItemType.ResourceRoot:
            return WisenetImage.root_normal
        case ItemType.Group:
        case ItemType.EventEmailGroup:
            return WisenetImage.group_normal
        case ItemType.Device:
            if(status === ItemStatus.Normal)
                return WisenetImage.tree_nvr_normal
            else
                return WisenetImage.tree_nvr_disconnect
        case ItemType.Camera:
        case ItemType.Channel:
            if(status === ItemStatus.Normal) {
                if(!isSmartSearchTree || smartSearchCap)
                    return WisenetImage.camera_normal
                else
                    return WisenetImage.smart_search_lock_dim
            }
            else if(status === ItemStatus.Unused) {
                return WisenetImage.camera_unused
            }
            else {
                return WisenetImage.camera_disconnect
            }
        case ItemType.LayoutRoot:
            return WisenetImage.layout
        case ItemType.Layout:
            return WisenetImage.layout

        case ItemType.ShowreelRoot:
            return WisenetImage.layout
        case ItemType.Showreel:
            return WisenetImage.layout

        case ItemType.WebPageRoot:
            return WisenetImage.webpage
        case ItemType.WebPage:
            return WisenetImage.webpage

        case ItemType.LocalFileRoot:
            return WisenetImage.layout
        case ItemType.LocalFile:
            return WisenetImage.layout
        case ItemType.AlarmOutput:
            return WisenetImage.alarmOut
        case ItemType.AlarmIn:
            return WisenetImage.alarmOut

        default:
            return WisenetImage.server_normal
        }
    }

    function getDisplayName(type, name){
        switch(type){
        case ItemType.ResourceRoot:
            return WisenetLinguist.root
        case ItemType.LayoutRoot:
            return WisenetLinguist.layouts
        case ItemType.ShowreelRoot:
            return WisenetLinguist.showreels
        case ItemType.WebPageRoot:
            return WisenetLinguist.webPages
        case ItemType.UserRoot:
            return WisenetLinguist.users
        case ItemType.LocalFileRoot:
            return WisenetLinguist.mediaFiles
        default:
            return name;
        }
    }
    function getStatusColor(value) {
        switch(value) {
        case ItemStatus.Loading:
            return WisenetGui.treeTextColorLoading
        case ItemStatus.Normal:
            return WisenetGui.contrast_04_light_grey
        case ItemStatus.Abnormal:
            return WisenetGui.dimgray
        default:
            return WisenetGui.dimgray
        }
    }
    function expandAll() {
        var rootRowCount = deviceTreeView.model.rowCount(deviceTreeView.rootIndex)

        for(var i=0; i < rootRowCount; i++) {

            var idx = deviceTreeView.model.index(i, 0, deviceTreeView.rootIndex)
            deviceTreeView.expand(idx)

            var childRowCount = deviceTreeView.model.rowCount(idx)
            for(var childRow =0; childRow < childRowCount; childRow++ ){
                expandChild(childRow, idx);
            }
        }
    }

    function expandChild(row, parent){
        var idx = deviceTreeView.model.index(row, 0, parent);
        var item = deviceTreeView.model.data(idx, 0x100 +1);

        deviceTreeView.expand(idx)

        var rowCount = deviceTreeView.model.rowCount(idx);

        for(var i=0; i< rowCount; i++){
            expandChild(i, idx);
        }
    }

    DragItemListModel{
        id: dragItemList
    }

    Rectangle{
        id: deviceSelectionView
        anchors.fill: parent
        color: parent.borderColor // spliter color를 border와 동일하게 수정 (by younginn.yun, 20220627) //WisenetGui.contrast_07_grey
        border.color: parent.borderColor
        border.width: 1

        radius: parent.radius

        WisenetSearchBox{
            id: searchBox
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            height: 31
            anchors.margins: 1
            focus: true
            bgColor: wisenetDraggableTree.bgColor

            onSearchTextChanged: {
                console.debug("Search Text: " + text)
                deviceTreeView.model.filterText = text;
                expandAll();
            }
        }

        Rectangle{
            id: deviceTreeArea
            anchors.left: parent.left
            anchors.top: searchBox.visible ? searchBox.bottom : parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 1
            color: wisenetDraggableTree.bgColor
            radius: wisenetDraggableTree.radius

            TreeView{
                id: deviceTreeView
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.topMargin: 10
                anchors.rightMargin: 5
                anchors.bottomMargin: 10
                headerVisible: false
                alternatingRowColors: false
                frameVisible: false
                model: treeProxyModel

                Drag.dragType: Drag.Automatic // 꼭해야됨.. drop이 될려면
                Drag.supportedActions: Qt.MoveAction // icon 변경
                Drag.source: dragItemList

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

                MouseArea {
                    id: treeMouseArea
                    anchors.fill: parent
                    anchors.rightMargin: deviceTreeView.__verticalScrollBar.visible ? 13 : 0
                    anchors.bottomMargin: deviceTreeView.__horizontalScrollBar.visible ? 13 : 0
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    hoverEnabled: true
                    focus: true

                    property alias __listView: deviceTreeView.__listView
                    property alias modelAdaptor: deviceTreeView.__model
                    property var clickedIndex: undefined
                    property var pressedIndex: undefined
                    property int hoveredRow : -1
                    property bool selectOnRelease: false
                    readonly property alias currentRow: deviceTreeView.__currentRow
                    readonly property alias currentIndex: deviceTreeView.currentIndex
                    readonly property alias selection: deviceTreeView.selection
                    z: 0
                    preventStealing: false
                    property bool shiftPressed: false
                    property bool dragStarted: false

                    function dragStart()
                    {
                        if (!dragStarted) {
                            dragStarted = false;
                            dragItemList.removeAll()
                            for(let index of treeItemSelectionModel.selectedIndexes)
                            {
                                console.log("updateDragItemList ::", index);
                                var item = sourceModel.data(treeProxyModel.mapToSource(index), 0x0100+1);

                                if(item.itemType !== ItemType.Camera)
                                    continue

                                dragStarted = true
                                dragItemList.append(item.uuid, item.itemType)
                            }
                            console.log("updateDragItemList END--");

                            if (dragStarted) {
                                deviceTreeView.Drag.active = true
                            }
                        }
                    }

                    function dragStop()
                    {
                        if (dragStarted) {
                            dragStarted = false;
                            deviceTreeView.Drag.active = false;
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
                            console.log("---deviceTreeView::onPressed pressedRow === -1")
                            return
                        }

                        if (selection.isSelected(pressedIndex)) {
                            selectOnRelease = true
                            return
                        }

                        __listView.currentIndex = pressedRow

                        if (!clickedIndex)
                            clickedIndex = pressedIndex

                        mouseSelect(pressedIndex, mouse.modifiers)


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
                            if (!deviceTreeView.__activateItemOnSingleClick)
                                deviceTreeView.activated(modelIndex)
                            deviceTreeView.doubleClicked(modelIndex)
                        }
                    }
                }

                TableViewColumn {
                    id: tvColumn
                    role: "display"
                    title: "DeviceTree"
                }

                selection: ItemSelectionModel{
                    id: treeItemSelectionModel
                    model: treeProxyModel
                }

                selectionMode: SelectionMode.ExtendedSelection

                style: WisenetTreeViewStyle {
                    id: treeItemStyle
                    textColor: "white"
                    backgroundColor: wisenetDraggableTree.bgColor
                    indentation: 23

                    itemDelegate: Item{
                        id: itemArea

                        implicitWidth: 18 + resourceText.width

                        Rectangle {
                            id: resourceItem
                            // anchors.leftMargin: 5
                            anchors.fill: parent
                            height: 24
                            color : "transparent"

                            Row{
                                id : localRow
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 5

                                Image {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 18
                                    height: 18
                                    sourceSize: "18x18"
                                    source: !styleData.value || getResourceIcon(styleData.value.itemType, styleData.value.itemStatus, styleData.value.smartSearchCap)===undefined ? "" : getResourceIcon(styleData.value.itemType, styleData.value.itemStatus, styleData.value.smartSearchCap)
                                    visible: !styleData.value || styleData.value.itemType === ItemType.EventEmail ? false /* Email */ : true /* ETC */
                                }

                                Text{
                                    id: resourceText
                                    text: !styleData.value || !styleData.value.displayName  || getDisplayName(styleData.value.itemType, styleData.value.displayName)===undefined ? "" : getDisplayName(styleData.value.itemType, styleData.value.displayName)
                                    color: WisenetGui.contrast_01_light_grey
                                    visible: true
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignLeft
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    font.pixelSize: 12
                                    elide: Qt.ElideRight
                                }
                            }
                        }
                    }

                    rowDelegate: Rectangle {
                        id: rowD
                        height: 24
                        color: styleData.selected ? WisenetGui.color_primary_dark : (styleData.row === treeMouseArea.hoveredRow ?  WisenetGui.color_primary : "transparent")
                    }
                }

                onDoubleClicked: {
                    if(deviceTreeView.isExpanded(index)){
                        deviceTreeView.collapse(index)
                    }
                    else{
                        deviceTreeView.expand(index)
                        console.log(index);
                    }

                    var resourceItem = treeProxyModel.data(index, 0x0100+1);
                    if(resourceItem.itemType === ItemType.Camera || resourceItem.itemType === ItemType.Channel)
                        treeDoubleClicked(resourceItem.uuid)
                }
            }
        }
    }
}
