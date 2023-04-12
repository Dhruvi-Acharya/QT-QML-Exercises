import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Item {
    id: focusControl
    anchors.fill: parent

    signal controlClosed();
    signal deviceFocusContinuous(int contValue);
    signal deviceFocusModeControl(int modeValue);

    property bool supportContinuousFocus: false
    property bool supportSimpleFocus: false
    property bool supportAutoFocus: false
    property bool supportResetFocus: false

    Rectangle{
        id: bg
        color: WisenetGui.contrast_09_dark
        opacity: 0.8
        anchors.centerIn: parent
        border.width: 1
        border.color: WisenetGui.contrast_07_grey

        property real maxHeight: 160
        property real minHeight: 20
        property real calcHeight: parent.width >= parent.height ? parent.height*0.22 : parent.width*0.22;

        height : Math.min(calcHeight, maxHeight)
        width: height*2.5
        property real btnSize : height*0.25
        property real rowCellMargin: height*0.3
    }

    component FocusButtonItem: WisenetMediaButtonItem {
        height: bg.btnSize
        width: bg.btnSize
        imgHeight: height
        imgWidth: width
        imgSourceSize : "32x32"
        normalOpacity: 0.0
        hoverOpacity: 0.5
        pressOpacity: 0.8
    }


    Row {
        id: focusRow
        anchors.centerIn: bg
        spacing: bg.rowCellMargin

        FocusButtonItem {
            id: focusNearAction
            visible: focusControl.supportContinuousFocus
            normalSource: "images/near_Default.svg"
            pressSource: "images/near_Press.svg"
            hoverSource: "images/near_Hover.svg"

            onMousePressed: {
                focusControl.deviceFocusContinuous(1);
            }
            onMouseReleased: {
                focusControl.deviceFocusContinuous(0);
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.near
            }
        }
        FocusButtonItem {
            id: focusFarAction
            visible: focusControl.supportContinuousFocus
            normalSource: "images/far_Default.svg"
            pressSource: "images/far_Press.svg"
            hoverSource: "images/far_Hover.svg"

            onMousePressed: {
                focusControl.deviceFocusContinuous(2);
            }
            onMouseReleased: {
                focusControl.deviceFocusContinuous(0);
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.far
            }
        }
        FocusButtonItem {
            id: simpleFocusAction
            visible: focusControl.supportSimpleFocus
            normalSource: "images/autofocus_Default.svg"
            pressSource: "images/autofocus_Press.svg"
            hoverSource: "images/autofocus_Hover.svg"
            onMouseClicked: {
                focusControl.deviceFocusModeControl(0);
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.simpleFocus
            }
        }
        FocusButtonItem {
            id: autoFocusAction
            visible: focusControl.supportAutoFocus
            normalSource: "images/autofocus_Default.svg"
            pressSource: "images/autofocus_Press.svg"
            hoverSource: "images/autofocus_Hover.svg"
            onMouseClicked: {
                focusControl.deviceFocusModeControl(2);
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.autoFocus
            }
        }
    }


    // 아래쪽 버튼 컨트롤들
    Item {
        id: focusBottomControl
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
                visible: focusControl.supportResetFocus
                text: WisenetLinguist.reset
                width: height*2.25
                height: parent.height
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                onClicked: {
                    focusControl.deviceFocusModeControl(1);
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

                onMouseClicked: focusControl.controlClosed();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.close
                }
            }

        }
    }

}
