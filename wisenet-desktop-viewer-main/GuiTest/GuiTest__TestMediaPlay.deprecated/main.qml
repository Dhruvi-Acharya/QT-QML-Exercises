import QtQuick 2.15
import QtQuick.Window 2.15
import TestMediaPlay 1.0

Item {

    width: 1280
    height: 720

    onWidthChanged: {
        //console.log ("width changed::" + width);
        videoView.setVideoOutSize(width, height);
    }
    onHeightChanged: {
        //console.log ("height changed::" + height);
        videoView.setVideoOutSize(width, height);
    }

    QVideoQuickItem {
        id: videoView
    }

    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: "Drag and drop a media file first."
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
        Connections {
            target: videoView
            function onVideoInfoChanged(message) {
                label.text = message;
            }
        }
    }

    DropArea {
        id: dropArea;
        anchors.fill: parent
        onDropped: {
            console.log("filedrop" + drop.urls);
            videoView.openMediaFile(drop.urls);
        }
    }
}

