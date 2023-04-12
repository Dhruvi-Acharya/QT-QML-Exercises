import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Item {
    id: bncControl
    anchors.fill: parent

    signal controlClosed();
    signal brightnessValueChanged(real value);
    signal contrastValueChanged(real value);

    property alias brightness: brightnessSlider.value
    property alias contrast: contrastSlider.value;


    Rectangle{
        id: bg
        color: WisenetGui.contrast_09_dark
        opacity: 0.8
        anchors.centerIn: parent
        border.width: 1
        border.color: WisenetGui.contrast_07_grey

        property real maxHeight: 240
        property real minHeight: 20
        property real baseHeight: parent.height > parent.width ? parent.width : parent.height
        property real zoomFactor: 0.45
        property real calcHeight: baseHeight*zoomFactor

        height : Math.min(calcHeight, maxHeight)
        width: height*1.81
        property real topMargin : height*0.15
        property real labelMargin : height*0.09
        property real btnSize : height*0.08
        property real barWidth : width*0.5
        property real barHandleSize: height*0.068
        property real rowCellMargin: height*0.05
    }

    component BncLabel : WisenetMediaTextLabel {
        autoResize: true
        height: bg.btnSize
        width: brightnessRow.width
    }

    component PlusButtonItem : WisenetMediaButtonItem {
        width: bg.btnSize
        height: bg.btnSize
        imgWidth:width
        imgHeight:height
        imgSourceSize: "18x18"
        pressColor: "transparent"
        hoverColor: "transparent"
        normalSource: "images/plus_Default.svg"
        hoverSource: "images/plus_Hover.svg"
        pressSource: "images/plus_Press.svg"
    }
    component MinusButtonItem : WisenetMediaButtonItem {
        width: bg.btnSize
        height: bg.btnSize
        imgWidth:width
        imgHeight:height
        imgSourceSize: "18x18"
        pressColor: "transparent"
        hoverColor: "transparent"
        normalSource: "images/minus_Default.svg"
        hoverSource: "images/minus_Hover.svg"
        pressSource: "images/minus_Press.svg"
    }

    component BncSlider : Slider {
        id: bncSlider
        from: -0.8
        to:0.8
        value: 0.0
        width: bg.barWidth
        height: bg.btnSize
        background: Rectangle {
            x: bncSlider.leftPadding
            y: bncSlider.topPadding + bncSlider.availableHeight / 2 - height / 2
            //implicitWidth: 200
            implicitHeight: 1
            width: bncSlider.availableWidth
            height: implicitHeight
            color: WisenetGui.contrast_05_grey

            Rectangle {
                width: bncSlider.visualPosition * parent.width
                height: parent.height
                color: WisenetGui.color_primary
            }
        }
        handle: Rectangle {
            x: bncSlider.leftPadding + bncSlider.visualPosition * (bncSlider.availableWidth - width)
            y: bncSlider.topPadding + bncSlider.availableHeight / 2 - height / 2
            implicitWidth: bg.barHandleSize
            implicitHeight: bg.barHandleSize
            radius: bg.barHandleSize*0.5
            color: {
                if(bncSlider.pressed)
                    return WisenetGui.contrast_06_grey
                else if(bncSlider.hovered)
                    return WisenetGui.contrast_00_white
                else
                    return WisenetGui.contrast_02_light_grey
            }
        }
    }
    BncLabel {
        id: brightnessLabel
        text: WisenetLinguist.brightness
        anchors.top: bg.top
        anchors.topMargin: bg.topMargin
        anchors.left: brightnessRow.left
    }

    Row {
        id: brightnessRow
        anchors.horizontalCenter: bg.horizontalCenter
        anchors.top: brightnessLabel.bottom
        anchors.topMargin: bg.labelMargin
        spacing: bg.rowCellMargin

        MinusButtonItem {
            onMouseClicked: brightnessSlider.decrease()
        }

        BncSlider {
            id: brightnessSlider
            onValueChanged: {
                bncControl.brightnessValueChanged(value);
            }
        }
        PlusButtonItem {
            onMouseClicked: brightnessSlider.increase()
        }

    }

    BncLabel {
        id: contrastLabel
        text: WisenetLinguist.contrast
        anchors.top: brightnessRow.bottom
        anchors.topMargin: bg.topMargin
        anchors.left: brightnessRow.left
    }

    Row {
        id: contrastRow
        anchors.horizontalCenter: bg.horizontalCenter
        anchors.top: contrastLabel.bottom
        anchors.topMargin: bg.labelMargin
        spacing: bg.rowCellMargin

        MinusButtonItem {
            onMouseClicked: contrastSlider.decrease()
        }
        BncSlider {
            id: contrastSlider
            onValueChanged: {
                bncControl.contrastValueChanged(value);
            }
        }
        PlusButtonItem {
            onMouseClicked: contrastSlider.increase()

        }
    }

    // 아래쪽 버튼 컨트롤들
    Item {
        id: bncBottomControl
        anchors {
            left : parent.left
            right: parent.right
            bottom : parent.bottom
            bottomMargin: height * 0.3
        }

        property real maxHeight: 32
        property real minHeight: 12
        property real baseHeight: parent.height > parent.width ? parent.width : parent.height
        property real zoomFactor: 0.12
        property real calcHeight: baseHeight*zoomFactor

        property real iconSize: height
        height: Math.min(calcHeight, maxHeight)

        Row {
            id: buttonGroup
            topPadding: 0
            anchors.centerIn: parent
            height: parent.height
            spacing: 3

            WisenetMediaControlTextButtonItem {
                id: resetButton
                text: WisenetLinguist.reset
                width: height*2.25
                height: parent.height
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                onClicked: {
                    contrastSlider.value = 0;
                    brightnessSlider.value = 0;
                }
            }

            WisenetMediaButtonItem {
                height: parent.height
                width: height*2.25
                imgHeight: height*0.625
                imgWidth: imgHeight
                imgSourceSize : "32x32"
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                normalColor: WisenetGui.color_live_control_bg
                normalSource: "images/return_Default.svg"
                pressSource: "images/return_Press.svg"
                hoverSource: "images/return_Hover.svg"

                onMouseClicked: bncControl.controlClosed();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.close
                }
            }
        }
    }
}
