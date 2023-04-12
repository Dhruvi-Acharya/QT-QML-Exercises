import QtQuick 2.15
import WisenetStyle 1.0
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0
import QtQuick.Controls 2.15
import "qrc:/"

Rectangle {
    id: textListView
    color: WisenetGui.contrast_08_dark_grey

    property var selectedViewingGrid: null

    Flickable{
        id: scrollArea

        anchors.fill: parent

        contentHeight: textListViewArea.height + (textData.anchors.topMargin * 2)
        contentWidth: textData.width < textListView.width
                      ? textListView.width
                      : textData.width + (textData.anchors.topMargin * 2)
        interactive: false

        ScrollBar.vertical: WisenetScrollBar {
            id: verticalScrollBar

            parent: scrollArea

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            stepSize: verticalScrollBar.visualSize / 4
        }

        ScrollBar.horizontal: WisenetScrollBar {
            id: horizontalScrollBar

            parent: scrollArea

            anchors.bottom: parent.bottom
            anchors.right: parent.right
            stepSize: horizontalScrollBar.visualSize / 4
        }

        clip: true

        Rectangle{
            id: textItem
            anchors.fill: parent
            color: "white" //WisenetGui.contrast_08_dark_grey
            border.width: 1

            WisenetText{
                id: textData
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.bottomMargin: 10
                text: gPosMetaManager.posMeta
                color: "black"
            }
        }
    }
}
