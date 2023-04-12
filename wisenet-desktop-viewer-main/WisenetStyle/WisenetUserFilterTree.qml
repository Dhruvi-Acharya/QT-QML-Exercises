import QtQuick 2.15
import QtQuick.Controls 2.15 as Control2
import QtQml.Models 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0
import WisenetLanguage 1.0

Item {
    id: userFilterTreeItem
    anchors.fill: parent

    //public variable
    property var treeModel : null
    property var allUser: allUserButton.checked
    property var selectedUserValue: null
    property var treeSlectionMode : SelectionMode.MultiSelection
    property var visibleImage : true

    //public function
    function reset(){
         selectedUserValue = null
         allUserButton.checked = true         
         searchBox.reset()
    }
    function unCheckItem(name){
        var sourceIndex = userFilterTreeView.model.sourceModel.getIndexByValue(name)
        var filterIndex = userFilterTreeView.model.mapFromSource(sourceIndex)
        treeItemSelectionModel.select(filterIndex, ItemSelectionModel.Deselect)
    }

    //public signal
    signal selectedUserChanged()


    function getDisplayName(name){
        if(name === "Service"){
            return WisenetLinguist.service;
        }

        return name;
    }

    function expandAll() {
        //console.log("deviceSelectionTreeItem~~expandAll")

        var rootRowCount = userFilterTreeView.model.rowCount(userFilterTreeView.rootIndex)
        //console.log("deviceSelectionTreeItem~~rootRowCount " + rootRowCount)

        for(var i=0; i < rootRowCount; i++) {

            var idx = userFilterTreeView.model.index(i, 0, userFilterTreeView.rootIndex)
            userFilterTreeView.expand(idx)

            var childRowCount = userFilterTreeView.model.rowCount(idx)
            for(var childRow =0; childRow < childRowCount; childRow++ ){
                expandChild(childRow, idx);
            }
        }

        userFilterTreeView.resizeColumnsToContents()
    }

    function expandChild(row, parent){
        var idx = userFilterTreeView.model.index(row, 0, parent);
        var item = userFilterTreeView.model.data(idx, 0x100 +1);

        //if(item)
        //    console.log("deviceSelectionTreeItem~~expandChild " + row + " " + parent + " parent " + item.displayName)
        //else
        //    console.log("deviceSelectionTreeItem~~expandChild " + row + " " + parent + " va " + item)

        userFilterTreeView.expand(idx)

        var rowCount = userFilterTreeView.model.rowCount(idx);

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
        id: userFilterView
        anchors.fill: parent
        color: WisenetGui.contrast_07_grey



        WisenetSearchBox{
            id: searchBox
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            height: 31
            anchors.margins: 1
            focus: true

            onSearchTextChanged: {
                console.debug("Search Text: " + text)
                userFilterTreeView.model.filterText = text;
                expandAll();
            }

        }

        Rectangle{
            id: allUserArea
            anchors.left: parent.left
            anchors.top: searchBox.bottom
            anchors.right: parent.right
            height: 33
            anchors.margins: 1
            color: WisenetGui.contrast_08_dark_grey

            WisenetAllDeviceCheckBox{
                id: allUserButton
                anchors.fill: parent

                Component.onCompleted: {
                    changeCheckBoxText(WisenetLinguist.allUsers)
                }

                onCheckStateChanged: {
                    console.debug("onCheckStateChanged :" + state)
                    if(state){
                        treeItemSelectionModel.clear()
                    }
                }
            }
        }


        Rectangle{
            id: userTreeArea
            anchors.left: parent.left
            anchors.top: allUserArea.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 1
            color: WisenetGui.contrast_08_dark_grey

            TreeView{
                id: userFilterTreeView
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.topMargin: 10
                anchors.rightMargin: 20
                anchors.bottomMargin: 10
                headerVisible: false
                alternatingRowColors: false
                frameVisible: false
                model:treeModel

                TableViewColumn {
                    id: tvColumn
                    role: "display"
                    title: "UserTree"
                    width: 300
                    resizable: true
                }

                selection: ItemSelectionModel{
                    id: treeItemSelectionModel
                    model: treeModel
                    onSelectedIndexesChanged: {

                        if(!hasSelection || (selectedIndexes.length === 0)){
                            allUserButton.checked = true
                            selectedUserValue = null
                        }else{
                            allUserButton.checked = false
                            selectedUserValue = [];
                            selectedIndexes.forEach(function(item) {
                                selectedUserValue.push(userFilterTreeView.model.data(item, 0x100 +1));
                            });
                        }
                        selectedUserChanged()
                        console.log("onSelectedIndexesChanged - selectedRows: " + selectedIndexes )
                    }
                }

                selectionMode: treeSlectionMode

                style: TreeViewStyle {
                    id: treeItemStyle
                    textColor: "white"
                    backgroundColor: WisenetGui.contrast_08_dark_grey

                    itemDelegate: Item{
                        id: itemArea

                        Rectangle {
                            id: resourceItem
                            anchors.leftMargin: 5
                            anchors.fill: parent
                            height: 25
                            color : "transparent"

                            Row{
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 10

                                Image{
                                    id: checkImage
                                    width: 16
                                    height: 16
                                    visible: ((userFilterTreeView.model.flags(styleData.index) & Qt.ItemIsSelectable) && (treeSlectionMode === SelectionMode.MultiSelection))
                                    anchors.verticalCenter: parent.verticalCenter
                                    sourceSize: Qt.size(16, 16)
                                    source: styleData.selected ? WisenetImage.check_on : WisenetImage.check_off
                                }

                                Text{
                                    id: resourceText
                                    text: (getDisplayName(styleData.value.displayName)===undefined ? "" : getDisplayName(styleData.value.displayName)) + "                        "
                                    color: WisenetGui.contrast_02_light_grey
                                    visible: true
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignLeft
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    font.pixelSize: 12
                                    elide: Qt.ElideRight
                                }

                                /*
                                BusyIndicator {
                                    running: styleData.value.itemStatus === 0
                                    implicitHeight: 30
                                }
                                */
                            }
                        }
                    }

                    rowDelegate: Rectangle {
                        id: rowD
                        height: 20
                        color: styleData.selected === true ? WisenetGui.contrast_07_grey : "transparent"
                    }

                    branchDelegate: Item {
                        width: indentation
                        height: 20
                        Image {
                            id: branchImage
                            visible: styleData.column === 0 && styleData.hasChildren
                            anchors.centerIn: parent
                            sourceSize: Qt.size(14, 14)
                            source: styleData.isExpanded ? WisenetImage.tree_branch_expanded : WisenetImage.tree_branch_collapsed
                        }
                    }

                    //scrollToClickedPosition: true
                    transientScrollBars: true
                }

                onDoubleClicked: {

                    if(userFilterTreeView.isExpanded(index)){
                        userFilterTreeView.collapse(index)
                    }
                    else{
                        userFilterTreeView.expand(index)
                        console.log(index);
                    }
                }
            }
        }
    }
}

