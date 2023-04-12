import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

Item {
    id: bottomControl

    property bool isPlayback: false
    property int videoPlayStatus: 0

    property bool supportPtzSetup: false
    property bool supportFocusSetup : false
    property bool supportMic : false

    property bool supportBnc: true
    property bool supportZoomTarget: true
    property bool supportPanTilt: false
    property bool supportZoom: false
    property bool supportZoom1x: false
    property bool supportAreaZoom: false // nvr 지원안함. (todo)
    property bool supportPreset: false
    property bool supportSwing: false
    property bool supportGroup: false
    property bool supportTour: false
    property bool supportTrace: false
    property bool supportPtzHome: false
    property bool supportFisheye: false
    property bool supportFisheyePtz: false
    property bool supportFisheyeZoomArea: false
    property bool supportLocalRecording: false
    property bool supportDualStream: false
    property bool supportSmartSearch: false
    property bool supportSmartSearchFilter: false

    property int  profileMode:0
    property alias localRecordingOn: localRecordingButton.toggleOn

    // fisheye properties
    property int fisheyeViewMode
    property int fisheyeLocation

    signal ptzButtonClicked();
    signal focusButtonClicked();
    signal fisheyeButtonClicked();
    signal bncButtonClicked();
    signal zoomButtonClicked();
    signal snapshotButtonClicked();
    signal localRecordingButtonClicked();
    signal profileChangeClicked(int changeProfile);
    signal micButtonClicked();

    signal overViewTriggered();
    signal singleViewTriggered();
    signal quadViewTriggered();
    signal panoramaViewTriggered();
    signal mouseMoved();

    function setLocalRecordingButtonStatus(isOn)
    {
        localRecordingButton.toggleOn = isOn
    }
    function setFisheyeButtonStatus(isOn)
    {
        fisheyeButton.toggleOn = isOn
    }
    function setMicButtonStatus(isOn)
    {
        micButton.toggleOn = isOn
    }

    anchors {
        left : parent.left
        right: parent.right
        bottom : parent.bottom
        bottomMargin: height * 0.3
    }

    property real maxHeight: 32
    property real minHeight: 10
    property real baseHeight: parent.height > parent.width ? parent.width : parent.height
    property real zoomFactor: 0.12
    property real calcHeight: baseHeight*zoomFactor

    property real iconSize: height

    height: Math.min(calcHeight, maxHeight)
    visible: (videoPlayStatus !== WisenetMediaPlayer.Loaded) ? false : (height >= minHeight ? true : false);

    component BottomButtonItem: WisenetMediaButtonItem {
        height: bottomControl.iconSize
        width: bottomControl.iconSize
        imgHeight: bottomControl.iconSize*0.625//hover ? bottomControl.iconSize*0.8 : bottomControl.iconSize*0.625
        imgWidth: imgHeight
        imgSourceSize : "32x32"
        normalOpacity: 0.0
        hoverOpacity: 0.5
        pressOpacity: 0.8
        onHoverChanged: {
            if (hover) {
                bottomControl.mouseMoved();
            }
        }

        Timer {
            id: bottomButtontimer
            interval: 1000
        }
        onMouseClicked: bottomButtontimer.start()
        enabled: !bottomButtontimer.running
    }
    component BottomToggleButtonItem: WisenetMediaToggleButtonItem {
        height: bottomControl.iconSize
        width: bottomControl.iconSize
        imgHeight: bottomControl.iconSize*0.625//hover ? bottomControl.iconSize*0.8 : bottomControl.iconSize*0.625
        imgWidth: imgHeight
        imgSourceSize : "32x32"
        normalOpacity: 0.0
        normalOnOpacity: 0.8
        hoverOpacity: 0.5
        pressOpacity: 0.8
        onHoverChanged: {
            if (hover) {
                bottomControl.mouseMoved();
            }
        }

        Timer {
            id: bottomToggleButtonTimer
            interval: 1000
        }
        onMouseClicked: bottomToggleButtonTimer.start()
        enabled: !bottomToggleButtonTimer.running
    }
    component BottomSeperator: Item {
        height: bottomControl.iconSize
        width: height*0.5
        Rectangle {
            anchors.centerIn: parent
            height: parent.width
            width:1
            color: WisenetGui.contrast_04_light_grey
        }
    }

    Rectangle{
        id: bottomBg
        color: WisenetGui.color_live_control_bg
        opacity: 0.7
        anchors.centerIn: parent
        width: bottomButtons.calcWidth
        height: parent.height
        radius: parent.height * 0.2
    }

    Row {
        id: bottomButtons
        anchors.centerIn: parent
        topPadding: 0
        height: parent.height
        spacing: 1
        property real calcWidth: width + (parent.iconSize*0.5)

        BottomToggleButtonItem {
            id: fisheyeButton
            visible: bottomControl.supportFisheye
            normalSource: "images/fish_Default.svg"
            normalOnSource: "images/fish_Press.svg"
            pressSource: "images/fish_Press.svg"
            hoverSource: "images/fish_Hover.svg"
            onMouseClicked: bottomControl.fisheyeButtonClicked();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.fisheyeDewarping
            }
        }

        BottomSeperator {
            visible : fisheyeButton.toggleOn
        }

        BottomToggleButtonItem {
            id: fisheyeOverViewButton
            visible: fisheyeButton.toggleOn
            normalSource: "images/fisheOverView_Default.svg"
            normalOnSource: "images/fisheOverView_Press.svg"
            pressSource: "images/fisheOverView_Press.svg"
            hoverSource: "images/fisheOverView_Hover.svg"
            onMouseClicked: bottomControl.overViewTriggered();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.overView
            }
            toggleOn: fisheyeViewMode === MediaLayoutItemViewModel.V_Original
        }
        BottomToggleButtonItem {
            id: fisheyeSingleViewButton
            visible: fisheyeButton.toggleOn
            normalSource: "images/fisheSingleView_Default.svg"
            normalOnSource: "images/fisheSingleView_Press.svg"
            pressSource: "images/fisheSingleView_Press.svg"
            hoverSource: "images/fisheSingleView_Hover.svg"
            onMouseClicked: bottomControl.singleViewTriggered();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.singleView
            }
            toggleOn: fisheyeViewMode === MediaLayoutItemViewModel.V_Single
        }
        BottomToggleButtonItem {
            id: fisheyeQuadViewButton
            visible: fisheyeButton.toggleOn
            normalSource: "images/fisheQuadView_Default.svg"
            normalOnSource: "images/fisheQuadView_Press.svg"
            pressSource: "images/fisheQuadView_Press.svg"
            hoverSource: "images/fisheQuadView_Hover.svg"
            onMouseClicked: bottomControl.quadViewTriggered();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.quadrView
            }
            toggleOn: fisheyeViewMode === MediaLayoutItemViewModel.V_Quad

        }
        BottomToggleButtonItem {
            id: fisheyePanoramaViewButton
            visible: fisheyeButton.toggleOn
            normalSource: "images/fishePanorama_Default.svg"
            normalOnSource: "images/fishePanorama_Press.svg"
            pressSource: "images/fishePanorama_Press.svg"
            hoverSource: "images/fishePanorama_Hover.svg"
            onMouseClicked: bottomControl.panoramaViewTriggered();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.panoramaView
            }
            toggleOn: fisheyeViewMode === MediaLayoutItemViewModel.V_Panorama

        }
        BottomSeperator {
            visible : fisheyeButton.toggleOn
        }

        BottomButtonItem {
            id: ptzButton
            visible: {
                if (bottomControl.supportFisheye) {
                    return (fisheyeButton.toggleOn)
                            ? (bottomControl.supportPtzSetup && bottomControl.supportFisheyePtz)
                            : false;
                }

                return (!isPlayback && bottomControl.supportPtzSetup)
            }
            normalSource: "images/ptz_Default.svg"
            pressSource: "images/ptz_Press.svg"
            hoverSource: "images/ptz_Hover.svg"
            onMouseClicked: bottomControl.ptzButtonClicked();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.ptz
            }
        }

        BottomButtonItem {
            id: zoomButton
            visible: {
                if (fisheyeButton.toggleOn) {
                    return (bottomControl.supportZoomTarget && bottomControl.supportFisheyeZoomArea)
                }

                return bottomControl.supportZoomTarget
            }
            normalSource: "images/zoom_Default.svg"
            pressSource: "images/zoom_Press.svg"
            hoverSource: "images/zoom_Hover.svg"
            onMouseClicked: bottomControl.zoomButtonClicked()
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.zoomArea
            }
        }

        BottomButtonItem {
            id: focusButton
            visible: (!isPlayback && bottomControl.supportFocusSetup)
            normalSource: "images/focus_Default.svg"
            pressSource: "images/focus_Press.svg"
            hoverSource: "images/focus_Hover.svg"
            onMouseClicked: bottomControl.focusButtonClicked();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.focus
            }
        }

        BottomButtonItem {
            id: bncButton
            visible: bottomControl.supportBnc
            normalSource: "images/bnc_Default.svg"
            pressSource: "images/bnc_Press.svg"
            hoverSource: "images/bnc_Hover.svg"
            onMouseClicked: bottomControl.bncButtonClicked();
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.brightnessContrast
            }
        }

        BottomButtonItem {
            id: profileButton
            visible: bottomControl.supportDualStream
            property url autoNormal: "images/autop_normal.svg"
            property url autoPress: "images/autop_normal.svg"
            property url autoHover: "images/autop_hover.svg"
            property url highNormal:"images/highp_normal.svg"
            property url highPress: "images/highp_normal.svg"
            property url highHover: "images/highp_hover.svg"
            property url lowNormal:"images/lowp_normal.svg"
            property url lowPress: "images/lowp_normal.svg"
            property url lowHover: "images/lowp_hover.svg"

            normalSource: {
                if (profileMode === MediaLayoutItemViewModel.Auto)
                    return autoNormal;
                if (profileMode === MediaLayoutItemViewModel.High)
                    return highNormal;
                return lowNormal;

            }
            pressSource: {
                if (profileMode === MediaLayoutItemViewModel.Auto)
                    return autoPress;
                if (profileMode === MediaLayoutItemViewModel.High)
                    return highPress;
                return lowPress;
            }
            hoverSource: {
                if (profileMode === MediaLayoutItemViewModel.Auto)
                    return autoHover;
                if (profileMode === MediaLayoutItemViewModel.High)
                    return highHover;
                return lowHover;
            }

            onMouseClicked: {
                var changeProfileMode = profileMode >= 2 ? 0 : (profileMode+1)
                bottomControl.profileChangeClicked(changeProfileMode)
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.videoProfile
            }
        }

        BottomToggleButtonItem {
            id: localRecordingButton
            visible: (!isPlayback && supportLocalRecording)
            normalSource: "images/rec_Default.svg"
            normalOnSource: "images/rec_Press.svg"
            pressSource: "images/rec_Press.svg"
            hoverSource: "images/rec_Hover.svg"
            onMouseClicked: bottomControl.localRecordingButtonClicked()
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.localRecording
            }
        }

        BottomButtonItem {
            id: snapshotButton
            visible: true
            normalSource: "images/camera_Default.svg"
            pressSource: "images/camera_Press.svg"
            hoverSource: "images/camera_Hover.svg"
            onMouseClicked: bottomControl.snapshotButtonClicked()
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.screenShot
            }
        }
        BottomToggleButtonItem {
            id: micButton
            visible: (!isPlayback && bottomControl.supportMic)
            normalSource: "images/mic_Default.svg"
            normalOnSource: "images/mic_Select.svg"
            pressSource: "images/mic_Press.svg"
            hoverSource: "images/mic_Hover.svg"
            onMouseClicked: bottomControl.micButtonClicked()
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.mic
            }
        }
    }
}
