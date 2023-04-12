import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import "qrc:/WisenetStyle/"

WisenetMediaDialog {
    id: dialog
    title: WisenetLinguist.ptzPreset
    width: 342
    height: 540
    modal:false
    onlyCloseType: true
    property var presetItem
    property int maxPresets: 0

    function showDialog(item) {
        presetItem = item
        presetItem.player.devicePtzPresetGet();
        open();
    }

    function updatePresetList() {
        presetListView.currentIndex = -1
        presetList.clear();
        maxPresets = presetItem.player.maxPtzPresets;

        var newItems = presetItem.player.ptzPresets;
        console.log("PtzPresetDialog::updatePresetList(),lenth=", newItems.length);
        for (var i = 0 ; i < newItems.length ; i++) {
            presetList.append({
                                  "presetNumber" : newItems[i].presetNo,
                                  "presetName" : newItems[i].presetName
                              });
        }
    }
    onClosed: {
        console.log("PtzPresetDialog::onClosed");
        presetItem = null
        presetListView.currentIndex = -1
        presetList.clear();
    }
    AddPresetDialog {
        id: addPresetDialog
        maxPresets:dialog.maxPresets
        opacity: dialog.opacity
        onPresetAdded: {
            console.log("PtzPresetDialog::AddPreset()", no, name);
            if (presetItem) {
                presetItem.player.devicePtzPresetAdd(no, name);
            }
        }
    }
    ListModel {
        id: presetList
    }
    contentItem: Item {
        Item {
            id: addDeleteRow
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: 16
            }
            height: 18
            WisenetMediaTextLabel {
                text: WisenetLinguist.preset
                height: parent.height
                anchors {
                    left: parent.left
                    top: parent.top
                }
            }
            WisenetMediaIconButtonItem {
                id: deleteButton
                text: WisenetLinguist.delete_
                normalSource: "images/delete_Default.svg"
                pressSource: "images/delete_Press.svg"
                hoverSource: "images/delete_Hover.svg"
                anchors {
                    right: parent.right
                    top: parent.top
                }
                onClicked: {
                    if (presetItem) {
                        if (presetListView.currentIndex !== -1) {
                            var item = presetListView.currentItem;
                            if (item) {
                                var number = parseInt(item.no)
                                presetItem.player.devicePtzPresetDelete(number);
                            }
                        }
                    }
                }
            }
            WisenetMediaIconButtonItem {
                id: addButton
                text: WisenetLinguist.add
                normalSource: "images/add_Default.svg"
                pressSource: "images/add_Press.svg"
                hoverSource: "images/add_Hover.svg"
                anchors {
                    right: deleteButton.left
                    rightMargin: 10
                    top: parent.top
                }
                onClicked: {
                    addPresetDialog.showDialog(presetItem);
                }
            }

        }

        Rectangle {
            id: bgRect
            color: "transparent"
            border.width:1
            border.color: WisenetGui.contrast_07_grey
            anchors {
                top: addDeleteRow.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
                bottom : parent.bottom
                bottomMargin: 6
            }
        }

        component ListRowRectangle : Rectangle {
            id: croot
            color:"transparent"
            property string no
            property string name
            property bool hovered: false
            property bool selected: false

            Row {
                id: contentRow
                height: parent.height
                width: parent.width

                Rectangle {
                    width: 57
                    height: parent.height
                    color:{
                        if (croot.selected)
                            return WisenetGui.color_primary_press
                        else if (croot.hovered)
                            return WisenetGui.color_primary
                        return "transparent"
                    }
                    Rectangle{
                        width: parent.width
                        height:1
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        color: WisenetGui.contrast_07_grey
                    }
                    Text {
                        anchors.fill: parent
                        color:  {
                            if (croot.selected)
                                return WisenetGui.white
                            else if (croot.hovered)
                                return WisenetGui.contrast_01_light_grey
                            return WisenetGui.contrast_02_light_grey
                        }
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                        text:croot.no
                    }
                }

                Rectangle {
                    width: 1
                    height: parent.height
                    color: WisenetGui.contrast_07_grey
                }
                Rectangle {
                    width: parent.width-58
                    height: parent.height
                    color:{
                        if (croot.selected)
                            return WisenetGui.color_primary_press
                        else if (croot.hovered)
                            return WisenetGui.color_primary
                        return "transparent"
                    }
                    Rectangle{
                        width: parent.width
                        height:1
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        color: WisenetGui.contrast_07_grey
                    }
                    Text {
                        anchors.fill: parent
                        color:  {
                            if (croot.selected)
                                return WisenetGui.white
                            else if (croot.hovered)
                                return WisenetGui.contrast_01_light_grey
                            return WisenetGui.contrast_02_light_grey
                        }
                        font.pixelSize: 12
                        text:croot.name
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
            }
        }

        ListRowRectangle {
            id: header
            height:32
            anchors {
                top: bgRect.top
                left: bgRect.left
                right: bgRect.right
                margins: 1
            }

            no: WisenetLinguist.no_
            name: WisenetLinguist.name
        }

        Component {
            id: presetListDelegate
            ListRowRectangle {
                id: presetViewItem
                width: presetListView.width
                height:32
                no: presetNumber
                name : presetName
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (presetListView.currentIndex !== -1) {
                            var item = presetListView.itemAtIndex(presetListView.currentIndex);
                            if (item !== presetViewItem)
                                item.selected = false;
                        }
                        presetViewItem.selected = true
                        presetListView.currentIndex = index
                    }
                    onDoubleClicked: {
                        console.log("preset go", no, name, " presetViewItem.no=",presetViewItem.no);
                        var ino = parseInt(presetViewItem.no);
                        if (presetItem) {
                            presetItem.player.devicePtzPresetMove(ino);
                        }
                    }
                    onContainsMouseChanged: {
                        if (containsMouse) {
                            presetViewItem.hovered = true;
                        }
                        else {
                            presetViewItem.hovered = false;
                        }
                    }
                }
            }
        }

        ListView {
            id: presetListView
            currentIndex: -1
            anchors {
                top: header.bottom
                bottom: bgRect.bottom
                left: bgRect.left
                right: bgRect.right
                margins: 1
            }
            model: presetList
            clip:true
            ScrollBar.vertical: WisenetMediaScrollBar {}
            delegate:presetListDelegate
        }
    }
}
