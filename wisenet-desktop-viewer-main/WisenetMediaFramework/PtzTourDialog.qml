import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import "qrc:/WisenetStyle/"

WisenetMediaDialog {
    id: dialog
    title: WisenetLinguist.tour
    width: 298
    height: 507
    modal:false
    onlyCloseType: true
    property var tourItem

    function showDialog(item) {
        tourItem = item
        tourItem.player.devicePtzTourGet()
        open()
    }

    function updateTourPresets() {
//        console.log("PtzTourDialog::updateTourPresets()");
        presetList.clear()

        var newItems = tourItem.player.tourPresets

        if(newItems === null || newItems === undefined || newItems.length <= 0) {
            return
        }

        for (var i = 0 ; i < newItems.length ; i++) {
//            console.log("PtzGroupDialog::updateTourPresets(),presetNumber=", newItems[i])
            presetList.append({
                                  "presetNumber" : newItems[i],
                                  "presetName" : "Group " + newItems[i] // TO-DO : from devicePtzPresetGet()
                              });
        }
    }
    onClosed: {
        console.log("PtzTourDialog::onClosed");
        tourItem = null
        presetList.clear();
    }
    ListModel {
        id: presetList
    }
    contentItem: Item {
        Item {
            id: startStopRow
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: 30
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
                    if (tourItem) {
                        tourItem.player.devicePtzTourMove()
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
                    if (tourItem) {
                        tourItem.player.devicePtzTourStop()
                    }
                }
            }
        }


        Text {
            id: groupListText
            text: WisenetLinguist.groupList
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
                top: groupListText.bottom
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
