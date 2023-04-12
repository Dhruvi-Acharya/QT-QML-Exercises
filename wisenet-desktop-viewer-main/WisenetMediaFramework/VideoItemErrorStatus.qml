import QtQuick 2.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

Item {
    id: itemErrorStatus
    property int videoPlayStatus: 0
    property bool isPlayback: false
    visible: (videoPlayStatus !== WisenetMediaPlayer.Loaded && videoPlayStatus !== WisenetMediaPlayer.ReadyToLoaded)

    signal filePasswordEntered(string password)
    Rectangle {
        id: errorStatusBg
        anchors.fill: parent
        opacity:0.7
        visible: videoErrorImage.visible
        color: WisenetGui.contrast_12_black
    }

    WisenetMediaBusyIndicator {
        id: streamChaningIndicator
        visible:(videoPlayStatus === WisenetMediaPlayer.Loading)
        running:visible
        anchors.centerIn: parent
        property real maxWidth: 72
        property real calcWidth: visible?parent.width/8:0
        width: visible?Math.min(calcWidth, maxWidth):0
        height: width
    }

    onVideoPlayStatusChanged: {
        //console.log("onVideoPlayStatusChanged", videoPlayStatus);
        if (videoPlayStatus === WisenetMediaPlayer.NoMedia ||
            videoPlayStatus === WisenetMediaPlayer.Loading ||
            videoPlayStatus === WisenetMediaPlayer.EndOfMedia ||
            videoPlayStatus === WisenetMediaPlayer.Failed_PasswordRequired) {
            videoErrorImage.visible = false;
            return;
        }

        videoErrorImage.visible = true;
        if (videoPlayStatus === WisenetMediaPlayer.Failed_InvalidMedia ||
            videoPlayStatus === WisenetMediaPlayer.Failed_MediaDisconnected) {
            videoErrorImage.source = isPlayback ? "images/vloss_playback.svg" : "images/vloss_live.svg";
            videoErrorText.text = WisenetLinguist.mediaDisconnectedError
        }
        else if (videoPlayStatus === WisenetMediaPlayer.Failed_ChannelDisconnected) {
            videoErrorImage.source = "images/disconnected.svg";
            videoErrorText.text = WisenetLinguist.cameraDisconnectedError
        }
        else if (videoPlayStatus === WisenetMediaPlayer.Failed_InvalidFormat) {
            videoErrorImage.source = "images/vloss_playback.svg";
            videoErrorText.text = WisenetLinguist.invalidVideoFileError
        }
        else if (videoPlayStatus === WisenetMediaPlayer.Failed_NoPermission) {
            videoErrorImage.source = "images/nopermission.svg";
            videoErrorText.text = WisenetLinguist.noPermissionError
        }
        else if (videoPlayStatus === WisenetMediaPlayer.Failed_SessionFull) {
            videoErrorImage.source = "images/user_full.svg";
            videoErrorText.text = WisenetLinguist.playbackUserFullError
        }
        else if (videoPlayStatus === WisenetMediaPlayer.Failed_FirmwareUpdating) {
            videoErrorImage.source = isPlayback ? "images/vloss_playback.svg" : "images/vloss_live.svg";
            videoErrorText.text = WisenetLinguist.warningFirmwareUpdateInProgress
        }
        //else if (videoPlayStatus === WisenetMediaPlayer.Failed_Timedout) {
        else {
            videoErrorImage.source = isPlayback ? "images/vloss_playback.svg" : "images/vloss_live.svg";
            videoErrorText.text = isPlayback ? WisenetLinguist.playbackVideoLossError : WisenetLinguist.liveVideoLossError;
        }
    }

    Image {
        id: videoErrorImage
        visible: false;
        anchors.centerIn: parent
        property real maxWidth: 128
        property real calcWidth: visible?parent.width/5:0
        width: visible?Math.min(calcWidth, maxWidth):0
        height: width
        //sourceSize: "90x90"
        mipmap: true
        sourceSize.width: maxWidth
    }

    Text {
        id: videoErrorText
        width: parent.width
        visible: videoErrorImage.visible && width >= (paintedWidth+8)
        property real maxHeight: 36
        property real minHeight: 9
        property real calcHeight: parent.height/15

        anchors {
            horizontalCenter: videoErrorImage.horizontalCenter
            top: videoErrorImage.bottom
            topMargin: 2
        }

        horizontalAlignment: Text.AlignHCenter
        color: WisenetGui.contrast_05_grey
        font.bold: true

        height: Math.min(calcHeight, maxHeight)
        fontSizeMode: Text.VerticalFit
        minimumPixelSize: 4
        font.pixelSize: 18
    }

    VideoItemPasswordInput {
        anchors.centerIn: parent
        visible: videoPlayStatus === WisenetMediaPlayer.Failed_PasswordRequired && parent.height > 120
        onPasswordEntered: {
            itemErrorStatus.filePasswordEntered(password);
        }
    }
}
