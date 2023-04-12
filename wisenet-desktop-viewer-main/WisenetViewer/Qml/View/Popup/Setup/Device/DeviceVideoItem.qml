import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import "qrc:/"
import WisenetMediaFramework 1.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Item {
    id: deviceVideoItem
    anchors.fill: parent

    function open(deviceId, channelId)
    {
        console.log("DeviceVideoItem::open()", deviceId, channelId);
        // 이전에 open했었으면 무시한다.
        if (
            mediaPlayer.hasControl() &&
            mediaPlayer.mediaParam.deviceId === deviceId &&
            mediaPlayer.mediaParam.channelId === channelId) {
            console.log("DeviceVideoItem::already opened media.", deviceId, channelId);
            return;
        }

        mediaPlayer.close();
        mediaPlayer.setCameraType(WisenetMediaParam.LiveAuto, "0",
                                         deviceId, channelId, "layoutModel.uuid");
        mediaPlayer.updateOutputParam(Screen.devicePixelRatio, videoOutput.width, videoOutput.height)
        mediaPlayer.open();
    }
    function close()
    {
        console.log("DeviceVideoItem::close()");
        mediaPlayer.close();
    }

    WisenetMediaPlayer {
        id: mediaPlayer
        enableAudioOut: false
    }

    VideoOutput {
        id: videoOutput
        source: mediaPlayer
        anchors.fill: parent
        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
    }
    VideoItemErrorStatus {
        id: errorStatus
        anchors.fill: parent
        videoPlayStatus: mediaPlayer.status
    }
}
