import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import QtGraphicalEffects 1.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

GridItemBase {
    id: zoomTargetItem
    // 미디어 플레이어 소스 정보
    readonly property var   player: mediaPlayer
    readonly property alias mediaParam: mediaPlayer.mediaParam
    property alias          mediaSource: mediaPlayer.mediaSource
    property alias          mediaName: mediaPlayer.mediaName
    property alias          mediaStatus: mediaPlayer.status
    property alias          mediaPlaySpeed: mediaPlayer.speed
    property bool           zoomTargetOfCamera: false

    hasPlayer:true
    mediaSeekable: mediaPlayer.seekable
    mediaPlayPosition: mediaPlayer.position

    property real   videoPadding : contentPadding+1
    property real   videoPadding2: videoPadding*2
    property real   borderPadding: contentPadding
    property real   borderPadding2: contentPadding*2
    property real openAnimDuration : 300

    signal videoCaptured(bool success, string filePath);
    signal openAnimationFinished();
    signal zoomTargetDestruction(string itemID);
    signal modelPropertyChanged(Item item);
    signal zoomTargetFisheyeViewPortChanged(var positioins);
    signal zoomTargetFocused(bool focused);

    function rotate(value) {
        if (value !== itemModel.rotation) {
            console.log("zoomTarget rotate value=", value)
            itemModel.rotation = value;
        }
    }

    function openBncControl()
    {
        videoControl.visible = false;
        bncControl.openControl();
    }

    WisenetMediaPlayer {
        id: mediaPlayer
        viewPort: itemModel.viewPort

        // VideoOutput의 size를 미세하게 변경하여, 강제로 업데이트한다.
        // 다른 방법을 찾지 못함 (5.15.2 기준)
        // 줌타겟 영역 변경 업데이트시
        onRequestVideoOutputUpdate: {
            videoOutput.updateFlag = !videoOutput.updateFlag
        }

        onImageSaved: {
            zoomTargetItem.videoCaptured(result, path);
        }
        onFisheyeDewarpStatusChanged: {
            if (fisheyeDewarpStatus) {
                mediaPlayer.udpateFisheyeDewarpOutputParam(videoOutput.width, videoOutput.height);
            }
        }
    }

    Rectangle {
        id: backgroundRect
        anchors.fill: parent
        anchors.margins: contentPadding
        color: backgroundColor
    }

    VideoOutput {
        id: videoOutput
        source: mediaPlayer
        property bool updateFlag: false
        property real updatePadding: updateFlag ? 0.1 : 0
        x: parent.videoPadding
        y: parent.videoPadding+updatePadding
        width: parent.width-parent.videoPadding2
        height: parent.height-parent.videoPadding2
        orientation: itemModel.rotation
        visible: !bncFilter.activated
        fillMode : itemModel.itemFillMode===MediaLayoutItemViewModel.PreserveAspectRatio ? VideoOutput.PreserveAspectFit : VideoOutput.Stretch

        // fisheye dewarping의 경우 original을 제외하고는 화면에 꽉차게 output을 잡는다.
        onWidthChanged: {
            if (!positionChanging && mediaPlayer.fisheyeDewarpStatus) {
                fisheyeOutputUpdateTimer.restart()
            }
        }
        onHeightChanged: {
            if (!positionChanging && mediaPlayer.fisheyeDewarpStatus) {
                fisheyeOutputUpdateTimer.restart()
            }
        }
        Timer {
            id: fisheyeOutputUpdateTimer
            interval: 200
            onTriggered: {
                if (mediaPlayer.fisheyeDewarpStatus) {
                    mediaPlayer.udpateFisheyeDewarpOutputParam(videoOutput.width, videoOutput.height);
                }
            }
        }
    }

    // 밝기/대비 필터 적용시
    BrightnessContrast {
        id: bncFilter
        anchors.fill: videoOutput
        property bool activated: bncControl.opened || itemModel.brightness !== 0 || itemModel.contrast !== 0
        source: activated ? videoOutput : null
        visible: activated

        brightness: bncControl.opened ? bncControl.brightness : itemModel.brightness
        contrast: bncControl.opened ? bncControl.contrast : itemModel.contrast
        cached: false
        /*
        onVisibleChanged: {
            console.log("videoItem bncFilter visible status:", bncFilter.visible);
        }
        */
    }

    Rectangle {
        id: borderRect
        x: parent.borderPadding
        y: parent.borderPadding
        width: parent.width-parent.borderPadding2
        height: parent.height-parent.borderPadding2
        color:"transparent"
        border.color: WisenetGui.color_primary
    }

    Connections {
        target: mediaPlayer
        function onOpenResult(error) {openResult(zoomTargetItem, error)}
        function onDewarpSetupResult(errorCode) {
            // 일반 카메라에 Fisheye 설정 시 E_ERR_NOTPANOMORPH가 계속 발생하여 로그 남기지 않음
            if(errorCode !== 10)
                console.log("zoomTargetItem::onDewarpSetupResult()", errorCode)
            fisheyeErrorText.fisheyeError = errorCode ? true : false;
        }
        function onFisheyeViewPortChanged(positions) {
            zoomTargetItem.zoomTargetFisheyeViewPortChanged(positions);
        }
    }

    onMouseOverChanged: {
        osdControl.controlVisible = (mouseOver && !selected) ? true : false;
    }

    // 상단 OSD
    Item {
        visible: osdControl.controlVisible || zoomTargetItem.itemNameAlwaysVisible
        x: 0
        y: 0
        width: visible ? parent.width : 1
        height: visible ? parent.height : 1
        TopControl {
            id: osdTopControl
            isVideo: true
            isPlayback: (mediaPlayer.streamType === WisenetMediaParam.PlaybackHigh ||
                         mediaPlayer.streamType === WisenetMediaParam.PlaybackLow ||
                         mediaPlayer.streamType === WisenetMediaParam.PlaybackAuto)

            controlVisible : osdControl.controlVisible
            displayName: mediaPlayer.mediaName+":Zoom" //todo
            onItemClosed: {
                zoomTargetItem.itemClose(zoomTargetItem);
            }
        }
    }

    Item {
        id: osdControl
        property bool controlVisible: false
        visible: opacity > 0.01
        opacity: 0.0
        x:0
        y:0
        z:1
        // CHECK POINT
        // 아이템간 바인딩이 많은 경우 성능 부하 발생하여 visible 상태가 아닌경우에는 binding을 풀어줌.
        onVisibleChanged: {
            if (visible) {
                width = Qt.binding(function() { return zoomTargetItem.width})
                height = Qt.binding(function() { return zoomTargetItem.height})
            }
            else {
                width = 1;
                height = 1;
            }
            zoomTargetItem.zoomTargetFocused(visible);
        }

        states: [
            State {
                when: !osdControl.controlVisible
                PropertyChanges {
                    target: osdControl
                    opacity: 0.0
                }
            },
            State {
                when: osdControl.controlVisible
                PropertyChanges {
                    target: osdControl
                    opacity: 1.0
                }
            }
        ]
        transitions: [
            Transition {
                NumberAnimation{
                    target: osdControl;
                    properties:"opacity";
                    duration:300
                }
            }
        ]

        // 밝기/대비 변경 컨트롤
        BncControl {
            id: bncControl
            visible: false
            property bool opened: false
            onControlClosed: {
                var isChanged  = (itemModel.brightness !== bncControl.brightness ||
                                  itemModel.contrast !== bncControl.contrast);
                itemModel.brightness = bncControl.brightness
                itemModel.contrast = bncControl.contrast
                closeControl()

                videoControl.visible = true;
                if (isChanged) {
                    zoomTargetItem.modelPropertyChanged(zoomTargetItem);
                }
            }
            function openControl()
            {
                brightness = itemModel.brightness
                contrast = itemModel.contrast
                visible = true;
                opened = true;
            }
            function closeControl()
            {
                visible = false;
                opened = false;
            }
        }
        VideoItemControl {
            id: videoControl

            videoPlayStatus: mediaPlayer.status
            isPlayback: mediaPlayer.seekable

            supportBnc: isEditable  // editable 이면 지원 (잠금, 공유가 아닌 경우)
            supportZoomTarget: false

            onBncButtonClicked: {
                zoomTargetItem.openBncControl();
            }

            onSnapshotButtonClicked: {
                /* todo png 생성시 여백을 어떻게 할것인가?
                var isRotate = (videoOutput.orientation % 180 !== 0)
                var isStretch = (videoOutput.fillMode === VideoOutput.Stretch)
                */
                var srcWidth = videoOutput.sourceRect.width;
                var srcHeight = videoOutput.sourceRect.height;
                var scaleRateW = srcWidth / videoOutput.width;
                var scaleRateH = srcHeight / videoOutput.height;
                var targetWidth = videoOutput.width*scaleRateW;
                var targetHeight = videoOutput.height*scaleRateH;
                console.log('grapToImage,', srcWidth, srcHeight, scaleRateW, scaleRateH, targetWidth, targetHeight)

                videoOutput.grabToImage(function(result) {
                    console.log("zoomTargetItem::grapToImage() callback");
                    //result.saveToFile("hello.png");
                    mediaPlayer.saveSnapshot(result.image);
                }, Qt.size(targetWidth, targetHeight));
            }
        }
        Text {
            id: fisheyeErrorText
            width: parent.width-12
            visible: fisheyeError && width >= (paintedWidth+8)
            text: WisenetLinguist.fisheyeDewarpingError
            property bool fisheyeError: false
            property real maxHeight: 36
            property real minHeight: 9
            property real calcHeight: parent.height/15

            anchors.centerIn: parent

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            color: WisenetGui.contrast_00_white
            font.bold: true

            height: Math.min(calcHeight, maxHeight)
            fontSizeMode: Text.VerticalFit
            minimumPixelSize: 10
            font.pixelSize: 18
        }
    }


    NumberAnimation{
        id:closeAnim
        target: zoomTargetItem;
        properties:"opacity";
        to:0
        duration:150
        onStopped: {
            console.log("ZoomTargetItem::destroy()");
            zoomTargetItem.destroy();
        }
    }


    ParallelAnimation {
        id: openAnim
        NumberAnimation {
            target: zoomTargetItem
            properties: "tempX"
            to: zoomTargetItem.cellX
            duration: zoomTargetItem.openAnimDuration
        }
        NumberAnimation {
            target: zoomTargetItem
            properties: "tempY"
            to: zoomTargetItem.cellY
            duration: zoomTargetItem.openAnimDuration
        }
        NumberAnimation {
            target: zoomTargetItem
            properties: "tempW"
            to: zoomTargetItem.cellW
            duration: zoomTargetItem.openAnimDuration
        }
        NumberAnimation {
            target: zoomTargetItem
            properties: "tempH"
            to: zoomTargetItem.cellH
            duration: zoomTargetItem.openAnimDuration
        }
        NumberAnimation {
            target: zoomTargetItem
            properties: "tempH"
            to: zoomTargetItem.cellH
            duration: zoomTargetItem.openAnimDuration
        }
        NumberAnimation {
            target: zoomTargetItem
            properties: "opacity"
            to: 1.0
            duration: zoomTargetItem.openAnimDuration
        }
        onFinished: {
            zoomTargetItem.positionChanging = false;
            zoomTargetItem.openAnimationFinished();
        }
    }

    function setTargetItem(targetItem, normalRect)
    {
        console.log("ZoomTargetItem::setTargetItem(), rect=", normalRect);
        zoomTargetOfCamera = targetItem.isCamera;
        itemModel.viewPort = normalRect;
        itemModel.zoomTargetId = targetItem.itemId;
        itemModel.rotation = targetItem.itemModel.rotation;
        if (targetItem.isCamera) {
            setCamearaParam(targetItem.itemModel.deviceId, targetItem.itemModel.channelId);
        }
        else {
            setLocalResourceParam(targetItem.itemModel.localResource);
        }

        //mediaPlayer.setRefSourceType(targetItem.mediaSource);
    }

    NumberAnimation{
        id:openNoAnim
        target: zoomTargetItem;
        properties:"opacity";
        to:1.0
        duration:150
    }

//    Component.onDestruction: {

//    }

    function onUpdateNormalRect(zoomArea)
    {
        var nRect = Qt.rect(zoomArea.normalRectX, zoomArea.normalRectY, zoomArea.normalRectW, zoomArea.normalRectH)
        mediaPlayer.updateSurfaceViewPort(nRect);
        itemModel.viewPort = nRect;
        modelPropertyChanged(zoomTargetItem);
        //console.log("ZoomTargetItem:: itemMouseMoved()")
        zoomTargetItem.itemMouseMoved();
    }

    function open(screenPixelRatio, targetItem, zoomArea, useAnim)
    {
        borderRect.border.color = zoomArea.borderColor;
        zoomArea.updateNormalRect.connect(zoomTargetItem.onUpdateNormalRect);
        mediaPlayer.updateOutputParam(screenPixelRatio, cellW, cellH);

        if (useAnim) {
            tempX = targetItem.x + zoomArea.contentRectX + zoomArea.areaX;
            tempY = targetItem.y + zoomArea.contentRectY + zoomArea.areaY;
            tempW = zoomArea.areaWidth;
            tempH = zoomArea.areaHeight;
            opacity = 0.5;
            positionChanging = true;
            openAnim.start();
        }
        else {
            openNoAnim.start();
        }
    }

    function close()
    {
        zoomTargetItem.zoomTargetDestruction(itemId);
        closeAnim.start();
        console.debug("close zoomTargetItem!!");
    }

    function closeWithoutAnimation()
    {
        zoomTargetItem.destroy();
        console.debug("close zoomTargetItem!!");
    }

    function closeByTargetItemClose()
    {
        closeAnim.start();
    }

}
