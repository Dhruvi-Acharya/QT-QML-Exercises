import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15
import "qrc:/WisenetStyle/"

Item {
    id: topControl

    property real viewZoomFactor: 1.0
    property real maxHeight: 24
    property real minHeight: 1
    property real baseHeight: parent.height > parent.width ? parent.width : parent.height
    property real zoomFactor: 0.15
    property real calcHeight: baseHeight*zoomFactor

    property real iconSize: height
    property real titleHeight: iconSize*0.7
    property real smallIconSize: iconSize*0.7
    property real iconMargin: iconSize*0.3
    property real smallIconMargin: iconSize*0.2

    height: Math.min(calcHeight, maxHeight)
    visible: height < minHeight ? false : true

    // interface
    property alias displayName: videoNameText.text
    signal itemClosed()
    signal reloaded();
    signal zoomIn();
    signal zoomOut();

    anchors {
        left : parent.left
        right: parent.right
        top : parent.top
        topMargin: 2
        rightMargin: 2
        leftMargin: height*0.4
    }

    Rectangle {
        id: ostTopControlBg
        anchors.fill: topControl
        color: WisenetGui.contrast_10_dark
    }

    Text {
        id: videoNameText
        anchors {
            left: parent.left
            leftMargin: 0
            right: zoomInButton.left
            rightMargin: iconMargin
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

    WisenetMediaButtonItem {
        id:zoomOutButton

        anchors {
            verticalCenter: parent.verticalCenter
            right:zoomValueText.left
            rightMargin: smallIconMargin
        }

        height: parent.smallIconSize
        width: height
        imgWidth:  parent.smallIconSize
        imgHeight:  parent.smallIconSize

        imgSourceSize: "16x16"
        radius: width*0.5
        bgOpacity: 0.5
        hoverColor: WisenetGui.contrast_11_bg
        pressColor: WisenetGui.contrast_11_bg
        normalSource: "images/minus_Default.svg"
        pressSource: "images/minus_Press.svg"
        hoverSource: "images/minus_Hover.svg"
        onMouseClicked: parent.zoomOut()
    }

    Text {
        id: zoomValueText
        height: parent.smallIconSize
        width:height*1.5
        anchors {
            verticalCenter: parent.verticalCenter
            right: zoomInButton.left
            rightMargin: smallIconMargin
        }
        text: Math.round(viewZoomFactor*100)+"%"
        color: WisenetGui.contrast_02_light_grey
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 11
        minimumPixelSize: 4
        fontSizeMode: Text.VerticalFit
    }

    WisenetMediaButtonItem {
        id:zoomInButton

        anchors {
            verticalCenter: parent.verticalCenter
            right:splitter.left
            rightMargin: iconMargin
        }

        height: parent.smallIconSize
        width: height
        imgWidth:  parent.smallIconSize
        imgHeight:  parent.smallIconSize

        imgSourceSize: "16x16"
        radius: width*0.5
        bgOpacity: 0.5
        hoverColor: WisenetGui.contrast_11_bg
        pressColor: WisenetGui.contrast_11_bg
        normalSource: "images/add_Default.svg"
        pressSource: "images/add_Press.svg"
        hoverSource: "images/add_Hover.svg"
        onMouseClicked: parent.zoomIn()
    }

    Rectangle {
        id: splitter
        anchors {
            verticalCenter: parent.verticalCenter
            right:refreshButton.left
            rightMargin: iconMargin
        }
        height: parent.smallIconSize
        width:1
        color: WisenetGui.contrast_07_grey
    }

    WisenetMediaButtonItem {
        id:refreshButton

        anchors {
            verticalCenter: parent.verticalCenter
            right:closeButton.left
            rightMargin: iconMargin
        }

        height: parent.smallIconSize
        width: height
        imgWidth:  parent.smallIconSize
        imgHeight:  parent.smallIconSize

        imgSourceSize: "16x16"
        radius: width*0.5
        bgOpacity: 0.5
        hoverColor: WisenetGui.contrast_11_bg
        pressColor: WisenetGui.contrast_11_bg
        normalSource: "images/reload_Default.svg"
        pressSource: "images/reload_Press.svg"
        hoverSource: "images/reload_Hover.svg"
        onMouseClicked: parent.reloaded()
    }

    CloseButtonItem {
        id:closeButton
        anchors {
            top:parent.top
            right:parent.right
            topMargin: 0
            rightMargin: 2
        }
        height: parent.iconSize
        width: height
        imgWidth:  parent.iconSize
        imgHeight:  parent.iconSize
        onMouseClicked: parent.itemClosed()
    }
}
