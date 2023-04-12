import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import "qrc:/WisenetStyle/"

WisenetMediaDialog {
    id: dialog
    title: WisenetLinguist.group
    width: 298
    height: 540
    modal:false
    onlyCloseType: true
    property var groupItem

    property int groupComboboxIdx: 0
    property var groupKeyList

    function showDialog(item) {
        console.log("PtzGroupDialog::showDialog()")
        groupItem = item
        groupComboboxIdx = 0
        groupModel.clear()
        groupNumberModel.clear()
        presetList.clear()

        groupItem.player.devicePtzPresetGet()
        groupItem.player.devicePtzGroupGet()
        open()
    }

    function updateGroupPresets() {
        console.log("PtzGroupDialog::updateGroupPresets()")
        groupModel.clear()
        groupNumberModel.clear()
        groupKeyList = groupItem.player.getGroupKeyList()

        for(var i = 0; i< groupKeyList.length; i++) {
            var groupNumber = groupKeyList[i]
            groupModel.append({"text" : "Group " + groupNumber})
            groupNumberModel.append({"number" : groupNumber})
        }

        updateGroupName()
    }

    function getCurrentGroupNumber() {
        var item = groupNumberModel.get(groupComboboxIdx)
        if(item)
            return parseInt(item.number)
        else
            return 1
    }


    function updateGroupName() {
        console.log("PtzGroupDialog::updateGroupName()")
        presetList.clear()

        if(groupItem === null || groupItem === undefined) {
            return
        }

        if(groupItem.player === null || groupItem.player === undefined) {
            return
        }

        var ptzPresetList = groupItem.player.ptzPresets
        var newItems = groupItem.player.getGroupPresetList(getCurrentGroupNumber())

        if(ptzPresetList === null || newItems === null || ptzPresetList === undefined || newItems === undefined || ptzPresetList.length <=0 || newItems.length <= 0) {
            return
        }

//        console.log("PtzGroupDialog::refreshGroupPresets(), groupComboboxIdx=", groupComboboxIdx,"length=", newItems.length)
        for (var i = 0 ; i < newItems.length ; i++) {
//            console.log("PtzGroupDialog::updateGroupName(),presetNumber=", newItems[i], " name=", getPresetName(newItems[i], ptzPresetList))
            presetList.append({
                                  "presetNumber" : newItems[i],
                                  "presetName" : getPresetName(newItems[i], ptzPresetList) // TO-DO : from devicePtzPresetGet()
                              });
        }
    }

    function getPresetName(presetNoArg, presetListArg) {
        if (presetListArg) {
            for (var i  = 0 ; i < presetListArg.length ; i++) {
                if (presetListArg[i].presetNo === presetNoArg)
                    return presetListArg[i].presetName;
            }
        }
        return "";
    }
    onClosed: {
        console.log("PtzGroupDialog::onClosed");
        groupItem = null
        groupComboboxIdx = 0
        presetList.clear()
        groupModel.clear()
        groupNumberModel.clear()
    }
    ListModel {
        id: groupModel
    }
    ListModel {
        id: groupNumberModel
    }
    ListModel {
        id: presetList
    }
    contentItem: Item {
        WisenetComboBox{
            id: groupTypeComboBox
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: 30
            }
            currentIndex: groupComboboxIdx

            model: groupModel

            onCurrentIndexChanged: {
                groupComboboxIdx = currentIndex
                updateGroupName()
            }
        }

        Item {
            id: startStopRow
            anchors {
                top: groupTypeComboBox.bottom
                left: groupTypeComboBox.left
                right: groupTypeComboBox.right
                topMargin: 16
                leftMargin: 10
            }

            WisenetImageAndTextButton {
                id: startButton
                width: 85
                height: 16
                imageWidth: 14
                imageHeight: 13

                anchors {
                    left: parent.left
                    top: parent.top
                }

                txt: WisenetLinguist.start

                normalImage: WisenetImage.ptz_start_default
                hoverImage: WisenetImage.ptz_start_hover
                pressImage: WisenetImage.ptz_start_selected

                normalTextColor: WisenetGui.contrast_02_light_grey
                hoverTextColor: WisenetGui.contrast_00_white
                pressTextColor: WisenetGui.color_primary

                onButtonClicked: {
                    if (groupItem) {
                        groupItem.player.devicePtzGroupMove(getCurrentGroupNumber())
                    }
                }
            }

            WisenetImageAndTextButton {
                id: stopButton
                width: 85
                height: 16
                imageWidth: 12
                imageHeight: 12

                anchors {
                    top: parent.top
                    left: startButton.right
                    leftMargin: 30
                }

                txt: WisenetLinguist.stop

                normalImage: WisenetImage.ptz_stop_default
                hoverImage: WisenetImage.ptz_stop_hover
                pressImage: WisenetImage.ptz_stop_selected

                normalTextColor: WisenetGui.contrast_02_light_grey
                hoverTextColor: WisenetGui.contrast_00_white
                pressTextColor: WisenetGui.color_primary

                onButtonClicked: {
                    if (groupItem) {
                        groupItem.player.devicePtzGroupStop(getCurrentGroupNumber())
                    }
                }
            }

        }

        Text {
            id: presetListText
            text: WisenetLinguist.presetList
            color: WisenetGui.contrast_04_light_grey
            fontSizeMode: Text.VerticalFit
            minimumPixelSize: 4
            font.bold: true
            font.pixelSize: 12
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors {
                top: startStopRow.bottom
                left: startStopRow.left
                topMargin: 35
            }
        }

        Rectangle {
            id: bgRect
            color: "transparent"
            border.width:1
            border.color: WisenetGui.contrast_07_grey
            anchors {
                top: presetListText.bottom
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
