import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "qrc:/"

Rectangle {
    id: objectStatusList

    // 높이 가변
    height: titleText.height + separator.height + itemList.height + titleTextTopMargin + separatorTopMargin + itemListTopMargin + itemListBottomMargin

    property alias errorColor : errorRect.color
    property alias title : titleText.text
    property var listModel

    property int titleTextTopMargin: 12
    property int separatorTopMargin: 44
    property int itemListTopMargin: 15
    property int itemListBottomMargin: 15

    color: WisenetGui.contrast_08_dark_grey
    radius: 4

    visible: errorModel.count > 0

    Rectangle{
        id: errorRect

        width: 14
        height: 14
        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 15
    }

    Text{
        id: titleText

        height: 20
        anchors.top: parent.top
        anchors.topMargin: titleTextTopMargin
        anchors.left: errorRect.right
        anchors.leftMargin: 10

        font.pixelSize: 16

        color: WisenetGui.contrast_02_light_grey

        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        id: separator
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: separatorTopMargin
        height: 1
        color: WisenetGui.contrast_10_dark
    }

    Flow{
        id: itemList
        anchors.top: separator.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: itemListTopMargin
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 5
        Repeater{
            id: errorModel
            model: listModel

            Rectangle{
                width: 220
                height: 20

                color: "transparent"

                //radius: 8

                Image{
                    id: itemIcon
                    anchors.top: parent.top
                    anchors.topMargin: 1
                    anchors.left: parent.left

                    width: 18
                    height: 18
                    source: model.objectType === 0 ? WisenetImage.tree_nvr_normal : WisenetImage.camera_normal
                }
                Text{
                    id: nameText
                    anchors.left: itemIcon.right
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 4

                    text: model.objectName

                    color: WisenetGui.contrast_04_light_grey

                    elide: Text.ElideRight

                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12

                    MouseArea{
                        id: nameTextMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                    }

                    WisenetMediaToolTip {
                        text: parent.text
                        visible: nameText.truncated && nameTextMouseArea.containsMouse
                        delay: 200
                    }
                }
            }
        }
    }
}
