import QtQuick 2.15
import QtGraphicalEffects 1.15
import "qrc:/WisenetStyle/"

Item {
    id: topControl

    property bool isVideo: false
    property bool isPlayback: false
    property bool recordingStatus: false
    property bool noDisplayName: false
    property real maxHeight: 24
    property real minHeight: 4
    property real baseHeight: parent.height > parent.width ? parent.width : parent.height
    property real zoomFactor: 0.15
    property real calcHeight: baseHeight*zoomFactor

    property real iconSize: height
    property real titleHeight: iconSize*0.7
    property real videoStatusHeight: iconSize*0.5
    property bool controlVisible: true

    height: Math.min(calcHeight, maxHeight)
    visible: height < minHeight ? false : true

    // interface
    property alias displayName: videoNameText.text
    signal itemClosed()

    anchors {
        left : parent.left
        right: parent.right
        top : parent.top
        topMargin: 2
        rightMargin: 2
        leftMargin: height*0.4
    }

    Item {
        id: videoStatus
        visible: isVideo
        property real minSize: 12
        property bool showSimple: height <= minSize
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        height : topControl.titleHeight + 2
        width : showSimple ? height : height*1.94

        Rectangle {
            visible : !videoStatus.showSimple
            anchors.fill: parent
            border.width: 1
            border.color:topControl.isPlayback ? "black" : "white"
            color:topControl.isPlayback ? "white" : "black"
            radius: height*0.5
            Image {
                height: parent.height*0.44
                anchors.centerIn: parent
                source: topControl.isPlayback ? "images/play_status.svg" : "images/live_status.svg"
                fillMode: Image.PreserveAspectFit
                //mipmap: true
                sourceSize.height: 24
            }
        }

        Rectangle {
            id: videoStatusSimple
            visible: videoStatus.showSimple
            anchors.fill: parent
            color: topControl.isPlayback? WisenetGui.white  : WisenetGui.black
            border.width: 1
            border.color: topControl.isPlayback? WisenetGui.black  : WisenetGui.white
            radius: width*0.5
        }
    }


    Text {
        id: videoNameText
        visible: !noDisplayName
        anchors {
            left: videoStatus.visible ? videoStatus.right : parent.left
            leftMargin: videoStatus.visible ? parent.iconSize*0.3 : 0
            right: closeButton.left
            verticalCenter: parent.verticalCenter
        }

        height: parent.titleHeight

        fontSizeMode: Text.VerticalFit
        minimumPixelSize: 4
        font.pixelSize: 24

        elide: Text.ElideRight
        color: WisenetGui.contrast_02_light_grey
        //font.bold: true
        style: Text.Outline
        styleColor: WisenetGui.contrast_08_dark_grey
    }

    CloseButtonItem {
        id:closeButton
        visible: topControl.controlVisible
        anchors {
            top:parent.top
            right:parent.right
            topMargin: 0
            rightMargin: 2
        }
        height: visible ? parent.iconSize : 1
        width: height
        imgWidth:  parent.iconSize
        imgHeight:  parent.iconSize
        onMouseClicked: itemClosed()
    }
}
