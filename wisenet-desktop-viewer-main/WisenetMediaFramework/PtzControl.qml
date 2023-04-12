import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Item {
    id: ptzControl

    anchors.fill: parent
    property bool supportPreset: true
    property bool supportSwing: true
    property bool supportGroup: true
    property bool supportTour: true
    property bool supportTrace: true
    property alias navigator : ptzNavigator

    signal controlClosed();
    signal presetTriggered();
    signal swingTriggered();
    signal groupTriggered();
    signal tourTriggered();
    signal traceTriggered();

    PtzNavigator {
        id: ptzNavigator
        sizeOk : ptzBottomControl.height >= ptzBottomControl.minHeight
    }

    // 아래쪽 버튼 컨트롤들
    Item {
        id: ptzBottomControl
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

        height: Math.min(calcHeight, maxHeight)
        visible: ptzNavigator.ptzMode !== PtzNavigator.PtzMode.ArrowLineMode ? ptzNavigator.sizeOk : false

        Row {
            id: buttonGroup
            topPadding: 0
            anchors.centerIn: parent
            height: parent.height
            spacing: 3

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

                onMouseClicked: ptzControl.controlClosed();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.close
                }
            }

            WisenetMediaControlTextButtonItem {
                id: presetButton
                visible:ptzControl.supportPreset
                text: WisenetLinguist.preset
                width: height*2.25
                height: parent.height
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                onClicked: ptzControl.presetTriggered()
            }

            WisenetMediaControlTextButtonItem {
                id: swingButton
                visible:ptzControl.supportSwing
                text: WisenetLinguist.swing
                width: height*2.25
                height: parent.height
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                onClicked: ptzControl.swingTriggered()
            }

            WisenetMediaControlTextButtonItem {
                id: groupButton
                visible:ptzControl.supportGroup
                text: WisenetLinguist.group
                width: height*2.25
                height: parent.height
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                onClicked: ptzControl.groupTriggered()
            }

            WisenetMediaControlTextButtonItem {
                id: tourButton
                visible:ptzControl.supportTour
                text: WisenetLinguist.tour
                width: height*2.25
                height: parent.height
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                onClicked: ptzControl.tourTriggered()
            }

            WisenetMediaControlTextButtonItem {
                id: traceButton
                visible:ptzControl.supportTrace
                text: WisenetLinguist.trace
                width: height*2.25
                height: parent.height
                normalOpacity: 0.7
                hoverOpacity: 0.8
                pressOpacity: 1.0
                onClicked: ptzControl.traceTriggered()
            }

        }
    }

}
