import QtQuick 2.15
import QtQuick.Controls 2.5
import WisenetPlayer 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "qrc:/"
import "qrc:/WisenetStyle"

Rectangle {
    id: shortcutBoxView
    height: 44

    signal fullScreen()
    signal clearScreen()
    signal toggleVideoStatus()
    signal toggleChannelNameVisible()

    property alias videoStatus : videoStatusButton.checked
    property alias channelNameVisible : channelNameVisibleButton.checked
    property alias volumeControlButtonEnabled : volumeControlButton.enabled

    component ShortcutButton: Button {
        id: control
        width: 30
        height: 30
        anchors.verticalCenter: parent.verticalCenter
        background: Rectangle {
            anchors.fill: parent
            color: WisenetGui.transparent
        }

        property var imageNormal
        property var imageSelected
        property var imageHover
        property var imageDisabled
        property string tooltipText

        Image {
            width: parent.width
            height: parent.width
            anchors.centerIn: parent
            sourceSize: Qt.size(width, height)
            source: !control.enabled ? control.imageDisabled :
                                       control.hovered? control.imageHover : ((control.checked && control.checkable) ? control.imageSelected : control.imageNormal)
        }

        WisenetMediaToolTip {
            id: shortcutTooltip
            delay: 500
            visible: parent.hovered
            text: parent.tooltipText
        }
    }

    component ShortcutRect: Rectangle {
        width: 44
        height: 44
        anchors.margins: 0
        anchors.verticalCenter: shortcutBoxView.verticalCenter
        color: WisenetGui.color_primary
        opacity: color == WisenetGui.color_primary ? 1 : 0.1
    }

    ShortcutRect {
        id: volumeControlButtonRect
        visible: volumeControlButton.hovered || volumeControlButton.checked
        color: volumeControlButton.hovered ? WisenetGui.color_primary : WisenetGui.contrast_06_grey
    }

    ShortcutRect {
        id: fullScreenButtonRect
        anchors.left: volumeControlButtonRect.right
        visible: fullScreenButton.hovered
    }

    ShortcutRect {
        id: clearScreenButtonRect
        anchors.left: fullScreenButtonRect.right
        visible: clearScreenButton.hovered
    }

    ShortcutRect {
        id: channelNameVisibleButtonRect
        anchors.left: clearScreenButtonRect.right
        visible: channelNameVisibleButton.hovered || channelNameVisibleButton.checked
        color: channelNameVisibleButton.hovered ? WisenetGui.color_primary : WisenetGui.contrast_06_grey
    }

    ShortcutRect {
        id: videoStatusButtonRect
        anchors.left: channelNameVisibleButtonRect.right
        visible: videoStatusButton.hovered || videoStatusButton.checked
        color: videoStatusButton.hovered ? WisenetGui.color_primary : WisenetGui.contrast_06_grey
    }

    ShortcutRect {
        id: setupButtonRect
        anchors.left: videoStatusButtonRect.right
        visible: setupButton.hovered
    }

    ShortcutButton {
        id: volumeControlButton
        anchors.centerIn: volumeControlButtonRect

        property bool volumeEnabled: gAudioManager && gAudioManager.enable

        imageNormal:{
            if (!volumeEnabled)
                return "qrc:/images/ShortcutBox/mute_default.svg"
            if (gAudioManager.volume <= 33)
                return "qrc:/images/ShortcutBox/volume1_default.svg"
            if (gAudioManager.volume <= 66)
                return "qrc:/images/ShortcutBox/volume2_default.svg"
            return "qrc:/images/ShortcutBox/volume_default.svg"
        }
        imageHover:{
            if (!volumeEnabled)
                return "qrc:/images/ShortcutBox/mute_hover.svg"
            if (gAudioManager.volume <= 33)
                return "qrc:/images/ShortcutBox/volume1_hover.svg"
            if (gAudioManager.volume <= 66)
                return "qrc:/images/ShortcutBox/volume2_hover.svg"
            return "qrc:/images/ShortcutBox/volume_hover.svg"
        }
        imageSelected:{
            if (!volumeEnabled)
                return "qrc:/images/ShortcutBox/mute_press.svg"
            if (gAudioManager.volume <= 33)
                return "qrc:/images/ShortcutBox/volume1_select.svg"
            if (gAudioManager.volume <= 66)
                return "qrc:/images/ShortcutBox/volume2_select.svg"
            return "qrc:/images/ShortcutBox/volume_press.svg"
        }
        imageDisabled:{
            if (!volumeEnabled)
                return "qrc:/images/ShortcutBox/mute_dim.svg"
            if (gAudioManager.volume <= 33)
                return "qrc:/images/ShortcutBox/volume1_dim.svg"
            if (gAudioManager.volume <= 66)
                return "qrc:/images/ShortcutBox/volume2_dim.svg"
            return "qrc:/images/ShortcutBox/volume_dim.svg"
        }

        checked: volumeControlView.visible

        onClicked: {
            volumeControlView.x = volumeControlButtonRect.x + 5
            volumeControlView.y = volumeControlButtonRect.y + volumeControlButtonRect.height
            volumeControlView.open()
        }
        tooltipText: WisenetLinguist.volume
    }

    VolumeControlView {
        id: volumeControlView
        normalSource: volumeControlButton.imageNormal
        pressSource: volumeControlButton.imageSelected
        hoverSource: volumeControlButton.imageHover
    }

    ShortcutButton {
        id: fullScreenButton
        anchors.centerIn: fullScreenButtonRect

        checked: mainWindow.viewerMode === MainViewModel.FullScreen

        imageNormal: "qrc:/images/ShortcutBox/full_screen_default.svg"
        imageHover: "qrc:/images/ShortcutBox/full_screen_hover.svg"
        imageSelected: "qrc:/images/ShortcutBox/full_screen_press.svg"

        onClicked: shortcutBoxView.fullScreen()
        tooltipText: WisenetLinguist.fullScreen
    }

    ShortcutButton {
        id: clearScreenButton
        anchors.centerIn: clearScreenButtonRect

        imageNormal: "qrc:/images/ShortcutBox/clear_screen_default.svg"
        imageHover: "qrc:/images/ShortcutBox/clear_screen_hover.svg"

        onClicked: shortcutBoxView.clearScreen()
        tooltipText: WisenetLinguist.clearScreen
    }

    ShortcutButton{
        id: channelNameVisibleButton
        anchors.centerIn: channelNameVisibleButtonRect

        imageNormal: "qrc:/images/ShortcutBox/channel_name_default.svg"
        imageHover: "qrc:/images/ShortcutBox/channel_name_hover.svg"
        imageSelected: "qrc:/images/ShortcutBox/channel_name_press.svg"

        onClicked: shortcutBoxView.toggleChannelNameVisible()
        tooltipText: WisenetLinguist.channelName
    }

    ShortcutButton {
        id: videoStatusButton
        anchors.centerIn: videoStatusButtonRect

        imageNormal: "qrc:/images/ShortcutBox/video_info_default.svg"
        imageHover: "qrc:/images/ShortcutBox/video_info_hover.svg"
        imageSelected: "qrc:/images/ShortcutBox/video_info_press.svg"

        onClicked: shortcutBoxView.toggleVideoStatus()
        tooltipText: WisenetLinguist.videoStatus
    }

    ShortcutButton {
        id: setupButton
        anchors.centerIn: setupButtonRect

        imageNormal: "qrc:/images/ShortcutBox/setup_default.svg"
        imageHover: "qrc:/images/ShortcutBox/setup_hover.svg"

        onClicked: setpuPopupView.show()
        tooltipText: WisenetLinguist.setup
    }

    SetupPopupView {
        id: setpuPopupView
    }
}

