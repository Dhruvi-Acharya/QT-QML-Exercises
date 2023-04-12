import QtQuick 2.15
import QtQuick.Controls 2.15


Item {
    id: videoItemStatus

    property alias visibleProfileInfo : profileInfo.visible
    property alias visibleBufferingInfo: bufferingInfo.visible
    property alias visibleVerificationInfo: verificationInfo.visible
    property real parentWidth: parent.width
    property real parentHeight: parent.height

    property int displayMode: VideoItemStatus.DisplayMode.ValueOnly

    property size resolution: "1920x1080"
    property size outResolution: "1920x1080"
    property real fps: 30
    property real kbps: 4096
    property string vcodec: "H.264"
    property string acodec: "G.711"
    property string profile: "None"
    property int buffering: 0
    property string verification: "Unknown"
    property int fontPixelSize: displayMode===VideoItemStatus.DisplayMode.Normal ? 12 : 10

    enum DisplayMode {
        Hide,
        Simple,
        ValueOnly,
        Normal
    }

    property size normalSizeLimit: "480x270"
    property size valuesSizeLimit: "160x160"
    property size fpsSizeLimit: "120x40"


    width: {
        if (displayMode===VideoItemStatus.DisplayMode.Normal)
            return (contentsColumn.width+24);
        if (displayMode===VideoItemStatus.DisplayMode.ValueOnly)
            return (contentsColumn.width+12);
        if (displayMode===VideoItemStatus.DisplayMode.Simple)
            return (contentsSimple.width+12);
        return 1;
    }

    height: {
        if (displayMode===VideoItemStatus.DisplayMode.Normal)
            return (contentsColumn.height+24);
        if (displayMode===VideoItemStatus.DisplayMode.ValueOnly)
            return (contentsColumn.height+12);
        if (displayMode===VideoItemStatus.DisplayMode.Simple)
            return (contentsSimple.height+12);
        return 1;
    }


    function calculateMode() {
        if (!visible) return;

        if (parentWidth >= normalSizeLimit.width && parentHeight >= normalSizeLimit.height) {
            displayMode = VideoItemStatus.DisplayMode.Normal
        }
        else if(parentWidth >= valuesSizeLimit.width && parentHeight >= valuesSizeLimit.height) {
            displayMode = VideoItemStatus.DisplayMode.ValueOnly
        }
        else if(parentWidth >= fpsSizeLimit.width && parentHeight >= fpsSizeLimit.height) {
            displayMode = VideoItemStatus.DisplayMode.Simple
        }
        else {
            displayMode = VideoItemStatus.DisplayMode.Hide
        }
    }

    Component.onCompleted: calculateMode();
    onVisibleChanged: calculateMode();
    onParentWidthChanged: calculateMode();
    onParentHeightChanged: calculateMode();

    component VideoItemStatusHeader: Text {
        color:"white"
        font.bold:true
        font.pixelSize: videoItemStatus.fontPixelSize
        width: 100
        visible: videoItemStatus.displayMode===VideoItemStatus.DisplayMode.Normal
    }
    component VideoItemStatusText: Text {
        color:"white"
        font.bold:true
        font.pixelSize: videoItemStatus.fontPixelSize
    }

    Rectangle {
        id: statusBg
        color: "black"
        opacity: 0.5
        anchors.fill: parent
    }

    Column {
        id: contentsColumn
        spacing: 3
        anchors.centerIn: parent
        visible: (displayMode >= VideoItemStatus.DisplayMode.ValueOnly)
        Row {
            spacing: 0
            VideoItemStatusHeader {text: "Input"}
            VideoItemStatusText {text: videoItemStatus.resolution.width+ " x " + videoItemStatus.resolution.height}
        }
        Row {
            spacing: 0
            VideoItemStatusHeader {text: "Output"}
            VideoItemStatusText {text: videoItemStatus.outResolution.width+ " x " + videoItemStatus.outResolution.height}
        }
        Row {
            VideoItemStatusHeader {text: "FrameRate"}
            VideoItemStatusText {text: videoItemStatus.fps.toFixed(2) + " fps"}
        }
        Row {
            VideoItemStatusHeader {text: "Bitrate"}
            VideoItemStatusText {text: videoItemStatus.kbps.toFixed() + " kbps"}
        }
        Row {
            VideoItemStatusHeader {width: 100; text: "Video Codec"}
            VideoItemStatusText {text: videoItemStatus.vcodec}
        }
        Row {
            VideoItemStatusHeader {width: 100; text: "Audio Codec"}
            VideoItemStatusText {text: videoItemStatus.acodec}
        }
        Row {
            id: profileInfo
            VideoItemStatusHeader {width: 100; text: "Video Profile"}
            VideoItemStatusText {text: videoItemStatus.profile}
        }
        Row {
            id: bufferingInfo
            VideoItemStatusHeader {width: 100; text: "Buffering"}
            VideoItemStatusText {text: videoItemStatus.buffering + " frames"}
        }
        Row {
            id: verificationInfo
            VideoItemStatusHeader {width: 100; text: "Verification"}
            VideoItemStatusText {text: videoItemStatus.verification}
        }
    }

    VideoItemStatusText {
        id: contentsSimple
        visible: (displayMode === VideoItemStatus.DisplayMode.Simple)
        anchors.centerIn: parent
        text: videoItemStatus.resolution.width+ "x" + videoItemStatus.resolution.height + ", " + videoItemStatus.fps.toFixed(2)
    }
}
