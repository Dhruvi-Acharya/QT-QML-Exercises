import QtQuick 2.15
import Wisenet.ImageItem 1.0
import WisenetStyle 1.0

Rectangle {
    id: bookmarkItem

    property var bookmarkIconImage : "qrc:/Assets/Icon/Monitoring/RightPanel/bookmark_item.svg"
    property var bookmarkSelectedIconImage : "qrc:/Assets/Icon/Monitoring/RightPanel/bookmark_selected_item.svg"
    property string bookmarkId : ""
    property string bookmarkName : "Bookmark Name"
    property string cameraName : "Camera Name"
    property string displayTime : "2021-10-07"
    property string description : "Description"
    property string backgroundColor : WisenetGui.contrast_09_dark
    property string hoverBackgroundColor : WisenetGui.contrast_08_dark_grey
    property var image
    property int imageWidth
    property int imageHeight
    property bool isCurrentItem

    signal doubleClicked()
    signal pressed(var mouse)

    width: 240
    height: 212 + (imageRect.height - 117) + (bookmarkNameText.height - 16) + (cameraNameText.height - 14) + (descriptionText.height - 7)

    color: itemMouseArea.containsMouse ? hoverBackgroundColor : backgroundColor

    Rectangle{
        id: imageRect

        //height: 117
        height: width * imageHeight / imageWidth

        color: "transparent"
        border.color: isCurrentItem ? WisenetGui.color_primary : WisenetGui.contrast_07_grey
        border.width: 1

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 7
        anchors.leftMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 15

        ImageItem{
            id: imageArea
            anchors.fill: parent
            anchors.margins: 1
            image: bookmarkItem.image
        }
    }

    Text{
        id: bookmarkNameText

        anchors.top: imageRect.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 6
        anchors.leftMargin: 15
        anchors.rightMargin: 32

        width: parent.width
        text: bookmarkName
        color: isCurrentItem ? WisenetGui.color_primary : WisenetGui.contrast_00_white

        wrapMode: Text.Wrap
        font.pixelSize: 14
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }


    Image{
        id: bookmarkIcon

        anchors.top: imageRect.bottom
        anchors.right: parent.right
        anchors.topMargin: 6
        anchors.rightMargin: 16

        width: 16
        height: 16
        sourceSize.height: 16
        sourceSize.width: 16
        source: isCurrentItem ? bookmarkSelectedIconImage : bookmarkIconImage
    }

    Text{
        id: cameraNameText

        anchors.top: bookmarkNameText.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 6
        anchors.leftMargin: 15
        anchors.rightMargin: 15

        width: parent.width
        text: cameraName
        color: isCurrentItem ? WisenetGui.contrast_00_white : WisenetGui.contrast_04_light_grey

        wrapMode: Text.Wrap
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    Text{
        id: timeText

        anchors.top: cameraNameText.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 6
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        height: 14

        text: displayTime
        color: isCurrentItem ? WisenetGui.contrast_00_white : WisenetGui.contrast_04_light_grey

        font.pixelSize: 10
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    Text{
        id: descriptionText

        anchors.top: timeText.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 6
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        text: description
        color: isCurrentItem ? WisenetGui.contrast_00_white : WisenetGui.contrast_04_light_grey

        wrapMode: Text.Wrap
        font.pixelSize: 10
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }


    Rectangle{
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 15
        anchors.rightMargin: 14
        height: 1
        color: WisenetGui.contrast_08_dark_grey
    }

    MouseArea
    {
        id: itemMouseArea
        hoverEnabled: true
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onDoubleClicked: {
            if (mouse.button == Qt.LeftButton)
                bookmarkItem.doubleClicked()
        }

        onPressed: {
            bookmarkItem.pressed(mouse)

            console.log(imageWidth, imageHeight, imageArea.textureSize)
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:2}D{i:1}D{i:3}D{i:5}D{i:7}D{i:9}D{i:12}D{i:13}D{i:15}D{i:2}D{i:16}
D{i:17}
}
##^##*/
