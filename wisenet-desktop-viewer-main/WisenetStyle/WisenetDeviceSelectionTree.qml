import QtQuick 2.15
import QtQuick.Controls 2.15 as Control2
import QtQml.Models 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0
import WisenetLanguage 1.0

Item {
    id: deviceSelectionTreeItem
    anchors.fill: parent
    state: "UserDevice"

    //public variable
    property var treeModel : null
    property alias allDevice: allDeviceButton.checked
    property var selectedDeviceValue: null
    property int treeSelectionMode : SelectionMode.MultiSelection
    property color bgColor : WisenetGui.contrast_08_dark_grey
    property color borderColor : WisenetGui.color_setup_line
    property real radius: 0

    property alias treeItemSelectionModelVal: treeItemSelectionModel

    onTreeSelectionModeChanged: {
        deviceTreeView.selectionMode = treeSelectionMode
    }

    states: [
        State {
            name: "DeviceList"
            PropertyChanges {
                target: searchBox
                visible : true
            }
            PropertyChanges {
                target: allDeviceArea
                visible : false
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : false
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : false
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.ExtendedSelection
            }
        },
        State {
            name: "UserDevice"
            PropertyChanges {
                target: allDeviceArea
                visible : true
            }
            PropertyChanges {
                target: searchBox
                visible : true
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : true
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.MultiSelection
            }
        },
        State {
            name: "Email"
            PropertyChanges {
                target: allDeviceArea
                visible : false
            }
            PropertyChanges {
                target: searchBox
                visible : false
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : true
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.MultiSelection
            }
        },
        State {
            name: "AlarmOut"
            PropertyChanges {
                target: allDeviceArea
                visible : false
            }
            PropertyChanges {
                target: searchBox
                visible : true
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : true
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.MultiSelection
            }
        },
        State {
            name: "AlarmAlert"
            PropertyChanges {
                target: allDeviceArea
                visible : false
            }
            PropertyChanges {
                target: searchBox
                visible : true
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : true
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.ContiguousSelection
            }
        },

        State {
            name: "Layout"
            PropertyChanges {
                target: allDeviceArea
                visible : false
            }
            PropertyChanges {
                target: searchBox
                visible : false
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : true
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.MultiSelection
            }
        },

        State {
            name: "Layout_singleSelection"
            PropertyChanges {
                target: allDeviceArea
                visible : false
            }
            PropertyChanges {
                target: searchBox
                visible : false
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : true
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.ContiguousSelection
            }
        },

        State {
            name: "DashboardTree"
            PropertyChanges {
                target: allDeviceArea
                visible : false
            }
            PropertyChanges {
                target: searchBox
                visible : false
            }
            PropertyChanges {
                target: deviceTreeView
                checkImageVisible : false
            }
            PropertyChanges {
                target: deviceTreeView
                selectionMode : SelectionMode.SingleSelection
            }
        }
    ]

    function reset(){
        selectedDeviceValue = null;
        allDeviceButton.checked = true
        searchBox.reset()
    }

    function selectChannels(channels)
    {
        channels.forEach(function(item) {

            var sourceIndex = deviceTreeView.model.sourceModel.getModelIndex(item)
            var filterIndex = deviceTreeView.model.mapFromSource(sourceIndex)
            treeItemSelectionModel.select(filterIndex, ItemSelectionModel.Select)

        });
    }

    //public signal
    signal selectedDeviceChanged()
    signal treeDoubleClicked()

    function getResourceIcon(type, status){
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
            if(status === ItemStatus.Normal)
                return WisenetImage.camera_normal
            else if(status !== ItemStatus.Unused)
                return WisenetImage.camera_disconnect
            else
                return WisenetImage.camera_unused
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
        //console.log("deviceSelectionTreeItem~~expandAll")

        var rootRowCount = deviceTreeView.model.rowCount(deviceTreeView.rootIndex)
        //console.log("deviceSelectionTreeItem~~rootRowCount " + rootRowCount)

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

        //if(item)
        //    console.log("deviceSelectionTreeItem~~expandChild " + row + " " + parent + " parent " + item.displayName)
        //else
        //    console.log("deviceSelectionTreeItem~~expandChild " + row + " " + parent + " va " + item)

        deviceTreeView.expand(idx)

        var rowCount = deviceTreeView.model.rowCount(idx);

        //console.log("deviceSelectionTreeItem~~RowCount : " + rowCount)

        for(var i=0; i< rowCount; i++){
            expandChild(i, idx);
        }
    }

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            console.debug("onClicked in Popup")
        }
    }


    Rectangle{
        id: deviceSelectionView
        anchors.fill: parent
        color: parent.borderColor // spliter color를 border와 동일하게 수정 (by younginn.yun, 20220627) //WisenetGui.contrast_07_grey
        border.color: parent.borderColor
        border.width: 1

        radius: deviceSelectionTreeItem.radius

        WisenetSearchBox{
            id: searchBox
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            height: 31
            anchors.margins: 1
            focus: true
            bgColor: deviceSelectionTreeItem.bgColor

            onSearchTextChanged: {
                console.debug("Search Text: " + text)
                deviceTreeView.model.filterText = text;
                expandAll();
            }
        }

        Rectangle{
            id: allDeviceArea
            anchors.left: parent.left
            anchors.top: searchBox.bottom
            anchors.right: parent.right
            height: 33
            anchors.margins: 1
            color: deviceSelectionTreeItem.bgColor

            WisenetAllDeviceCheckBox{
                id: allDeviceButton
                anchors.fill: parent

                onCheckStateChanged: {
                    console.debug("onCheckStateChanged :" + state)
                    if(state){
                        treeItemSelectionModel.clear()
                    }
                }
            }
        }


        Rectangle{
            id: deviceTreeArea
            anchors.left: parent.left
            anchors.top:  allDeviceArea.visible?allDeviceArea.bottom : searchBox.visible? searchBox.bottom : parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 1
            color: deviceSelectionTreeItem.bgColor
            radius: deviceSelectionTreeItem.radius

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
                model:treeModel

                property bool checkImageVisible : false

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
                    id : treeMouseArea
                    property alias __listView: deviceTreeView.__listView
                    property int hoveredRow : -1
                    anchors.fill: parent
                    hoverEnabled: true
                    focus: true

                    onPressed: mouse.accepted = false

                    onPositionChanged: {

                        hoveredRow = __listView.indexAt(0, mouseY + __listView.contentY)

                        //console.log("[onPositionChanged]")
                    }
                    onReleased: {
                        hoveredRow = -1
                        //console.log("[onReleased]")
                    }

                    onExited: {
                        hoveredRow = -1
                        // console.log("[onExited]")
                    }
                }


                TableViewColumn {
                    id: tvColumn
                    role: "display"
                    title: "DeviceTree"
                }

                selection: ItemSelectionModel{
                    id: treeItemSelectionModel
                    model: treeModel

                    onSelectedIndexesChanged: {

                        //console.log("[WisenetDeviceSelection] onSelectedIndexesChanged " )

                        if(!hasSelection || (selectedIndexes.length === 0)){
                            allDeviceButton.checked = true
                            selectedDeviceValue = null
                        }else{
                            allDeviceButton.checked = false
                            selectedDeviceValue = [];
                            selectedIndexes.forEach(function(item) {
                                selectedDeviceValue.push(deviceTreeView.model.data(item, 0x100 +1));
                            });
                        }
                        selectedDeviceChanged()
                        //console.log("onSelectedIndexesChanged - selectedRows: " + selectedIndexes )
                        //console.log("onSelectedIndexesChanged after" + hasSelection + " selectedIndexes.length:" + selectedIndexes.length + "--" + selectedDeviceValue?selectedDeviceValue:"" )
                    }
                    onCurrentChanged: {
                        //console.log("[WisenetDeviceSelection] onCurrentChanged " + current + "-------" + previous )
                        //if(state ==="AlarmAlert" && previous){
                        //    select(previous, ItemSelectionModel.Clear)
                        //clearSelection();
                        //  }
                    }

                }

                selectionMode: treeSelectionMode

                style: WisenetTreeViewStyle {
                    id: treeItemStyle
                    textColor: "white"
                    backgroundColor: deviceSelectionTreeItem.bgColor
                    indentation: 23

                    itemDelegate: Item{
                        id: itemArea

                        implicitWidth: 16 + resourceText.width

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


                                Image{
                                    id: checkImage
                                    width: 16
                                    height: 16
                                    visible: deviceTreeView.checkImageVisible && ((deviceTreeView.model.flags(styleData.index) & Qt.ItemIsSelectable) && (treeSelectionMode === SelectionMode.MultiSelection))
                                    anchors.verticalCenter: parent.verticalCenter
                                    sourceSize: Qt.size(16, 16)
                                    source: styleData.selected ? WisenetImage.check_on : WisenetImage.check_off
                                }

                                Image {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 18
                                    height: 18
                                    sourceSize: "18x18"
                                    source: !styleData.value || getResourceIcon(styleData.value.itemType, styleData.value.itemStatus)===undefined ? "" : getResourceIcon(styleData.value.itemType, styleData.value.itemStatus)
                                    visible: !styleData.value || styleData.value.itemType === ItemType.EventEmail ? false /* Email */ : true /* ETC */
                                }

                                Text{
                                    id: resourceText
                                    text: !styleData.value || !styleData.value.displayName || getDisplayName(styleData.value.itemType, styleData.value.displayName)===undefined ? "" : getDisplayName(styleData.value.itemType, styleData.value.displayName)
                                    color: WisenetGui.contrast_01_light_grey  //getStatusColor(styleData.value.itemStatus)
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

                    treeDoubleClicked()
                }
            }
        }
    }
}
