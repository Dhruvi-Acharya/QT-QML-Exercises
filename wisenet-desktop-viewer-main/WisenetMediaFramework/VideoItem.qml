import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import QtGraphicalEffects 1.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"
import "qrc:/SmartSearch/"
import "qrc:/Masking/"

//import "VideoItemZoomArea.js" as TargetZoomManager

GridItemBase {
    id: videoItem
    // 미디어 플레이어 소스 정보
    readonly property var   player: mediaPlayer
    readonly property alias mediaParam: mediaPlayer.mediaParam
    property alias          mediaSource: mediaPlayer.mediaSource
    property alias          mediaStatus: mediaPlayer.status
    property alias          mediaPlaySpeed: mediaPlayer.speed
    readonly property alias mediaName: mediaPlayer.mediaName

    readonly property alias supportVideoIn: mediaPlayer.supportVideoIn
    readonly property alias supportAudioIn: mediaPlayer.supportAudioIn
    readonly property alias supportAudioOut: mediaPlayer.supportAudioOut
    readonly property alias supportDualStreamLive: mediaPlayer.supportDualStreamLive
    readonly property alias supportDualStreamPlayback: mediaPlayer.supportDualStreamPlayback
    readonly property alias supportSimpleFocus: mediaPlayer.supportSimpleFocus
    readonly property alias supportAutoFocus: mediaPlayer.supportAutoFocus
    readonly property alias supportResetFocus: mediaPlayer.supportResetFocus
    readonly property alias supportFocus: mediaPlayer.supportFocus
    readonly property alias supportFocusSetup: videoControl.supportFocusSetup
    readonly property alias supportFisheyeDewarp: mediaPlayer.supportFisheyeDewarp
    readonly property alias supportPreset: videoControl.supportPreset
    readonly property alias supportSwing: videoControl.supportSwing
    readonly property alias supportGroup: videoControl.supportGroup
    readonly property alias supportTour: videoControl.supportTour
    readonly property alias supportTrace: videoControl.supportTrace
    readonly property alias supportMic: mediaPlayer.supportMic

    readonly property alias supportSmartSearch: videoControl.supportSmartSearch
    readonly property alias supportSmartSearchFilter: videoControl.supportSmartSearchFilter


    property variant zoomTargetColors: [WisenetGui.color_primary, WisenetGui.color_secondary, WisenetGui.color_sky, WisenetGui.color_purple]
    property var zoomTargetRatio: [2.5, 3.0, 3.5, 4.0];
    property int zoomTargetColorsIndex: 0

    property int fisheyeViewIndex: 1

    objectName: "videoItem"
    isVideoItem: true
    hasPlayer:true
    mediaSeekable: mediaPlayer.seekable
    mediaPlayPosition: mediaPlayer.position

    signal createZoomTarget(Item item, rect normalRect, color rectColor);
    signal openResult(Item item, int error);
    signal mediaItemStatusChanged(Item item, int status);
    signal localRecordingStopped(Item item, bool showNotification);
    signal videoCaptured(bool success, string filePath);
    signal ptzPresetTriggered(Item item);
    signal ptzSwingTriggered(Item item);
    signal ptzGroupTriggered(Item item);
    signal ptzTourTriggered(Item item);
    signal ptzTraceTriggered(Item item);
    signal modelPropertyChanged(Item item);
    signal ptzPresetListUpdated(Item item);
    signal ptzSwingPresetUpdated(Item item);
    signal ptzGroupPresetUpdated(Item item);
    signal ptzTourPresetUpdated(Item item);
    signal ptzTracePresetUpdated(Item item);
    signal deviceTalkResult(string msg);
    signal deviceUsingSetupChanged();
    signal smartSearchRequest();    // 스마트 검색 요청. 데이터는 focusedItem에서 get
    signal openMessageDialog(var message);  // viewingGrid에 MessageDialog open 요청
    signal selectedMaskItemChanged(var maskId)
    signal showTrackingFailMessage();

    property var zoomAreas: []

    property bool isSmartSearchMode: false // SmartSearch 관련 컨트롤 활성화 및 미지원 기능 비활성화
    readonly property alias isSmartSearchDrawing: drawModeOsdControl.isDrawing
    property bool isMaskingMode: false  // Masking 설정 전용 VideoItem으로 사용 (나머지 기능들을 비활성화)
    readonly property alias isMaskDrawing: maskDrawOsdControl.isDrawing

    function updateFisheyeViewMode(mode) {
        if(!itemModel.fisheyeDewarpEnable) {
            videoControl.setFisheyeButton(true)
        }

        switch(mode) {
        case MediaLayoutItemViewModel.V_Original:
            if(videoControl.fisheyeViewMode !== MediaLayoutItemViewModel.V_Original) {
                videoControl.overViewTriggered()
            }
            break
        case MediaLayoutItemViewModel.V_Single:
            if(videoControl.fisheyeViewMode !== MediaLayoutItemViewModel.V_Single) {
                videoControl.singleViewTriggered()
            }
            break
        case MediaLayoutItemViewModel.V_Quad:
            if(videoControl.fisheyeViewMode !== MediaLayoutItemViewModel.V_Quad) {
                videoControl.quadViewTriggered()
            }
            break
        case MediaLayoutItemViewModel.V_Panorama:
            if(videoControl.fisheyeViewMode !== MediaLayoutItemViewModel.V_Panorama) {
                videoControl.panoramaViewTriggered()
            }
            break
        }
    }

    function updateFisheyeQuadViewIndex(index) {
        if(fisheyeControl.fisheyeViewMode === MediaLayoutItemViewModel.V_Quad) {
            fisheyeViewIndex = index
        }
    }

    function rotate(value) {
        if (value !== itemModel.rotation) {
            console.log("videoItem rotate value=", value)
            itemModel.rotation = value;
        }
    }

    function openBncControl()
    {
        osdControl.closeSubControls();
        bncControl.brightness = itemModel.brightness;
        bncControl.contrast = itemModel.contrast;
        bncControl.activated = true;
    }
    function openFocusControl()
    {
        osdControl.closeSubControls();
        focusControl.activated = true;
    }
    /*
    onVisibleChanged: {
        console.log("VideoItem visible Changed", visible);
    }
    */

    function updateLocalFisheyeSettings()
    {
        mediaPlayer.setLocalFileFisheyeSettings(itemModel.fileFisheyeEnable,
                                                itemModel.fileLensType,
                                                itemModel.fileLensLocation);
        videoItem.modelPropertyChanged(videoItem);
    }

    function startAutoMasking(isOn){
        maskDrawOsdControl.startAutoMasking(isOn)
    }

    function startFixedMasking(isOn){
        maskDrawOsdControl.startFixedMasking(isOn)
    }

    function startManualMasking(isOn){
        maskDrawOsdControl.startManualMasking(isOn)
    }

    function selectMaskItem(maskId){
        console.log("VideoItem selectMaskItem", maskId)
        if(mediaPlayer.isTracking) {
            mediaPlayer.stopTracking()
            mediaPlayer.pause()
            maskDrawOsdControl.trackingButtonOff()
        }
        maskDrawOsdControl.selectMaskItem(maskId)
    }

    function changeMaskItemVisible(maskId, isVisible){
        maskDrawOsdControl.changeMaskItemVisible(maskId, isVisible)
    }

    function changeMaskItemName(maskId, maskName){
        maskDrawOsdControl.changeMaskItemName(maskId, maskName)
    }

    function removeMaskItem(maskId){
        maskDrawOsdControl.removeMaskItem(maskId)
    }

    WisenetMediaPlayer {
        id: mediaPlayer
        itemProfile: itemModel.itemProfile
        enableAudioOut: videoItem.focusedTarget && videoItem.isFocusedWindow && visible
        enableFisheyeDewarp : itemModel.fisheyeDewarpEnable
        fisheyeViewMode: itemModel.fisheyeViewMode
        isSequence: videoItem.isSequenceItem
        enableMaskingMode: isMaskingMode

        onSupportFisheyeDewarpChanged: {
            if (supportFisheyeDewarp && itemModel.rotation !== 0) {
                console.log("VideoItem::Reset rotate to zero because of fisheye Dewarp");
                itemModel.rotation = 0; // reset rotation value
                videoItem.modelPropertyChanged(videoItem);
                for (var i = 0 ; i < videoItem.zoomAreas.length ; i++) {
                    var zoomTargetItem = videoItem.zoomAreas[i].zoomTargetItem;
                    if (zoomTargetItem) {
                        zoomTargetItem.rotate(rotation);
                        videoItem.modelPropertyChanged(zoomTargetItem);
                    }
                }
            }
        }

        onFisheyeDewarpStatusChanged: {
            mediaPlayer.updateKeepSize();
            if (fisheyeDewarpStatus) {
                mediaPlayer.udpateFisheyeDewarpOutputParam(videoOutput.width, videoOutput.height);
                mediaPlayer.setFisheyeViewPosition(itemModel.fisheyeViewPosition);
                fisheyeControl.updateZoomValue(itemModel.fisheyeViewPosition);
            }
            else {
                fisheyeControl.ptzEnable = false;
            }
            videoControl.setFisheyeButtonStatus(fisheyeDewarpStatus);
        }

        function updateKeepSize()
        {
            console.log("videoItem::updateKeepSize()", zoomAreas.length, mediaPlayer.fisheyeDewarpStatus);
            mediaPlayer.keepOriginalSize = (zoomAreas.length > 0) || (mediaPlayer.fisheyeDewarpStatus) || (mediaPlayer.enableObjectTracker)
        }

        onStatusChanged: {
            videoItem.mediaItemStatusChanged(videoItem, status);
        }

        onLocalRecordingStatusChanged: {
            console.log("videoItem::onLocalRecordingStatusChanged()");

            videoControl.setLocalRecordingButtonStatus(localRecordingStatus);
            if(localRecordingStatus == false)
                videoItem.localRecordingStopped(videoItem, false);
        }

        onImageSaved: {
            console.log("videoItem::onImageSaved()")
            videoItem.videoCaptured(result, path);
        }
        onTalkErrorResult :{
            console.log("videoItem::onTalkResult() errorCode={}", errorCode);
            if(errorCode == 1)
                videoItem.deviceTalkResult(WisenetLinguist.deviceTalkAlreadyUse);
            else
                 videoItem.deviceTalkResult(WisenetLinguist.micDeviceUnavailable);
        }

        onPtzPresetsChanged: {
            console.log("videoItem::onPtzPresetsChanged() maxPreset=", maxPtzPresets)
            videoItem.ptzPresetListUpdated(videoItem);
        }

        onSwingPanStartPresetChanged: {
            console.log("videoItem::onSwingPanStartPresetChanged()")
            videoItem.ptzSwingPresetUpdated(videoItem)
        }
        onGroupPresetsChanged: {
            console.log("videoItem::onGroupPresetsChanged()")
            videoItem.ptzGroupPresetUpdated(videoItem)
        }
        onTourPresetsChanged: {
            console.log("videoItem::onTourPresetsChanged()")
            videoItem.ptzTourPresetUpdated(videoItem)
        }
        onTracePresetsChanged: {
            console.log("videoItem::onTracePresetsChanged()")
            videoItem.ptzTracePresetUpdated(videoItem)
        }

        onPlaybackTimeChanged: {
            timeDisplay.show(positionSeconds*1000);
        }
        onPositionSecondsChanged: {
            if (timeDisplay.controlVisible) {
                timeDisplay.updateTime(positionSeconds*1000)
            }
        }
        onFirstVideoFrame: {
            //console.log("FirstVideoFrame arrived!");
            showVideoAnimation.start();
        }
        onMediaSourceChanged: {
            // mediaSource가 생성되었을 때 itemModel값 전달이 필요한 부분.
            if (mediaSource && videoItem.isLocalResource) {
                setLocalFileFisheyeSettings(itemModel.fileFisheyeEnable,
                                            itemModel.fileLensType,
                                            itemModel.fileLensLocation);
            }
        }
        onFisheyeViewPositionChanged: {
            var positions = mediaPlayer.fisheyeViewPosition();
            itemModel.fisheyeViewPosition = positions;
            videoItem.modelPropertyChanged(videoItem);
            fisheyeControl.updateZoomValue(positions);
        }
        onMicStatusChanged: {
            console.log("videoItem::onMicStatusChanged()", micStatus);

            videoControl.setMicButtonStatus(micStatus);
        }

        onDeviceUsingSetupChanged: {
            console.log("videoItem::onDeviceUsingSetupChanged()")
            videoItem.deviceUsingSetupChanged()
        }

        onMaskRectPositionChanged: {
            maskDrawOsdControl.updateMaskRectPosition(maskRectMap)
        }

        onShowTrackingFailMessage: {
            console.log("videoItem::onShowTrackingFailMessage()")
            videoItem.showTrackingFailMessage()
            maskDrawOsdControl.trackingButtonOff()
        }

        onIsTrackingChanged: {
            if(!isTracking)
                maskDrawOsdControl.trackingButtonOff()
        }
    }

    Rectangle {
        id: backgroundRect
        anchors.fill: parent
        anchors.margins: contentPadding
        color: backgroundColor
    }

    // 새로운 줌타겟 영역을 화면 가운데에 만든다.
    function createNewZoomTarget()
    {
        var cRect = videoOutput.contentRect;
        var ratio = 3.0;//zoomTargetRatio[zoomTargetColorsIndex];
        var rectColor = zoomTargetColors[zoomTargetColorsIndex];

        var targetRect = Qt.rect(0,0,0,0);
        targetRect.width = cRect.width / ratio;
        targetRect.height = cRect.height / ratio;
        targetRect.x = (cRect.width / 2) - (targetRect.width / 2);
        targetRect.y = (cRect.height / 2) - (targetRect.height / 2);

        if (videoOutput.orientation === 0) {
            var nx = targetRect.x/cRect.width;
            var ny = targetRect.y/cRect.height;
            var nw = targetRect.width/cRect.width;
            var nh = targetRect.height/cRect.height;
        }
        else if (videoOutput.orientation === 90) {
            nw = targetRect.height/cRect.height;
            nh = targetRect.width/cRect.width;
            nx = 1 - nw - (targetRect.y/cRect.height)
            ny = targetRect.x/cRect.width
        }
        else if (videoOutput.orientation === 180) {
            nw = targetRect.width/cRect.width;
            nh = targetRect.height/cRect.height;
            nx = 1 - nw - (targetRect.x/cRect.width)
            ny = 1 - nh - (targetRect.y/cRect.height)
        }
        else if (videoOutput.orientation === 270) {
            nw = targetRect.height/cRect.height;
            nh = targetRect.width/cRect.width;
            nx = targetRect.y/cRect.height
            ny = 1 - nh - (targetRect.x/cRect.width)
        }
        var normRect = Qt.rect(nx, ny, nw, nh);
        // zoomTarget영역시 profile을 auto가 아닌 현재프로파일로 고정하는 것으로 사양 변경 (2022-03-01)
        var newProfile = itemModel.itemProfile;
        if (mediaPlayer.videoProfile === "Live(H)" || mediaPlayer.videoProfile === "Playback(H)") {
            console.log("Change profile as high:", mediaPlayer.videoProfile);
            newProfile = MediaLayoutItemViewModel.High;
        }
        else if (mediaPlayer.videoProfile === "Live(L)" || mediaPlayer.videoProfile === "Playback(L)") {
            console.log("Change profile as low:", mediaPlayer.videoProfile);
            newProfile = MediaLayoutItemViewModel.Low;
        }
        if (itemModel.itemProfile !== newProfile) {
            itemModel.itemProfile = newProfile;
            videoItem.modelPropertyChanged(videoItem);
        }

        // 주의 : createZoomTarget은 시그널임 실제로 만드는 역할은 ViewingGrid에서 수행함
        videoItem.createZoomTarget(videoItem, normRect, rectColor);
        zoomTargetColorsIndex++;
        if (zoomTargetColorsIndex >= zoomTargetColors.length)
            zoomTargetColorsIndex = 0;
    }

    VideoOutput {
        id: videoOutput
        source: mediaPlayer
        fillMode : itemModel.itemFillMode===MediaLayoutItemViewModel.PreserveAspectRatio ? VideoOutput.PreserveAspectFit : VideoOutput.Stretch
        x: padding
        y: padding
        width: parent.width-padding2x
        height: parent.height-padding2x
        orientation: itemModel.rotation
        visible: !bncFilter.activated
        opacity: 0

        property real padding : videoItem.contentPadding
        property real padding2x : videoItem.contentPadding*2
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
    onPositionChangingChanged: {
        if (!positionChanging && mediaPlayer.fisheyeDewarpStatus) {
            mediaPlayer.udpateFisheyeDewarpOutputParam(videoOutput.width, videoOutput.height);
        }
    }


    // 밝기/대비 필터 적용시
    BrightnessContrast {
        id: bncFilter
        anchors.fill: videoOutput
        property bool activated: bncControl.activated || itemModel.brightness !== 0 || itemModel.contrast !== 0
        source: activated ? videoOutput : null
        visible: activated
        opacity: 0

        brightness: bncControl.activated ? bncControl.brightness : itemModel.brightness
        contrast: bncControl.activated ? bncControl.contrast : itemModel.contrast
        cached: false

        /*
        onVisibleChanged: {
            console.log("videoItem bncFilter visible status:", bncFilter.visible);
        }
        */
    }

    NumberAnimation{
        id: showVideoAnimation
        target: bncFilter.visible? bncFilter : videoOutput;
        properties:"opacity";
        to: 1.0
        duration:300
        onStopped: {
            //console.log("animation onStopped")
            bncFilter.opacity = 1.0;
            videoOutput.opacity = 1.0;
        }
    }

    VideoItemErrorStatus {
        id: errorStatus
        anchors.fill: videoOutput
        videoPlayStatus: mediaPlayer.status
        isPlayback: mediaPlayer.seekable
        // 백업파일 패스워드 입력시
        onFilePasswordEntered: {
            mediaPlayer.checkPassword(password)
        }
    }

    VideoItemStatus {
        id:videoStatusArea
        visible: (itemModel.videoStatus && !positionChanging)
        visibleProfileInfo : videoItem.isCamera
        visibleBufferingInfo: videoItem.isCamera && !mediaPlayer.seekable
        visibleVerificationInfo: videoItem.isLocalResource

        fps: mediaPlayer.videoFps
        kbps: mediaPlayer.videoKbps
        vcodec: mediaPlayer.videoCodec
        acodec: mediaPlayer.audioCodec
        resolution: mediaPlayer.videoSize
        outResolution: mediaPlayer.videoOutSize
        profile: mediaPlayer.videoProfile
        buffering: mediaPlayer.videoBufferingCount
        verification: itemModel.verifiedInfo
        anchors {
            bottom: videoItem.bottom
            right: videoItem.right
            bottomMargin: videoControl.height+5
            rightMargin: 5
        }

    }

    Connections {
        target: mediaPlayer
        function onOpenResult(error) {openResult(videoItem, error)}
        function onDewarpSetupResult(errorCode) {
            // 일반 카메라에 Fisheye 설정 시 E_ERR_NOTPANOMORPH가 계속 발생하여 로그 남기지 않음
            if(errorCode !== 10)
                console.log("videoItem::onDewarpSetupResult()", errorCode)
            fisheyeControl.fisheyeError = errorCode ? true : false
        }
        function onTalkErrorResult(errorCode){

        }
    }

    onMouseOverChanged: {
        if (mouseOver && !selected)
            osdControl.controlVisible = true;
        else if (videoControl.subControlActivated || videoControl.localRecordingOn)
            osdControl.controlVisible = true;
        else
            osdControl.controlVisible = false;
    }


    // 상단 OSD
    Item {
        visible: osdControl.controlVisible || videoItem.itemNameAlwaysVisible
        x: 0
        y: 0
        z: 3
        width: visible ? parent.width : 1
        height: visible ? parent.height : 1
        TopControl {
            id: osdTopControl
            isVideo: true
            isPlayback: mediaPlayer.seekable
            controlVisible : osdControl.controlVisible && !ptzControl.activated && !fisheyeControl.ptzEnable

            displayName: mediaPlayer.mediaName
            onItemClosed: {
                videoItem.itemClose(videoItem);
            }
        }
    }

    // 재생시 일정시간동안 시간 표시
    Item {
        id: timeDisplay
        x: 0
        y: 0

        property bool controlVisible: false
        visible: controlVisible && mediaPlayer.seekable && mediaPlayer.status ===  WisenetMediaPlayer.Loaded
        width: visible ? parent.width : 1
        height: visible ? parent.height : 1

        function show(positionMsec)
        {
            timeDisplayHideAnimation.stop();
            opacity = 1.0;
            controlVisible = true;
            timeDisplayHideTimer.restart();
            timeDisplayText.text = mediaPlayer.getDateTime24h(positionMsec);
        }
        function updateTime(positionMsec)
        {
            timeDisplayText.text = mediaPlayer.getDateTime24h(positionMsec);
        }

        Text {
            id: timeDisplayText
            anchors {
                bottom: parent.bottom
                bottomMargin: parent.height*0.1
                left: parent.left
                right: parent.right
            }
            property real maxHeight: 24
            property real minHeight: 10
            property real baseHeight: parent.height > parent.width ? parent.width : parent.height
            property real zoomFactor: 0.15
            property real calcHeight: baseHeight*zoomFactor
            visible: height < minHeight ? false : !osdControl.visible
            height: Math.min(calcHeight, maxHeight)
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 22
            style: Text.Outline
            styleColor: WisenetGui.contrast_08_dark_grey
            color: WisenetGui.contrast_02_light_grey
            minimumPixelSize: 4
            fontSizeMode: Text.VerticalFit
        }

        Timer {
            id: timeDisplayHideTimer
            running:false
            repeat: false
            interval: 3000
            onTriggered: {
                timeDisplayHideAnimation.restart();
            }
        }
        NumberAnimation{
            id: timeDisplayHideAnimation
            target: timeDisplay;
            properties:"opacity";
            to: 0.0
            duration:500
            onFinished: {
                timeDisplay.controlVisible = false;
                timeDisplayText.text = "";
            }
        }
    }


    // OSD 컨트롤
    Item {
        id: osdControl
        property bool controlVisible: false

        visible: opacity > 0.01 && !drawModeOsdControl.visible && !maskDrawOsdControl.visible
        opacity: 0.0
        x:0
        y:0
        z:1
        // CHECK POINT
        // 아이템간 바인딩이 많은 경우 성능 부하 발생하여 visible 상태가 아닌경우에는 binding을 풀어줌.
        onVisibleChanged: {
            if (visible) {
                width = Qt.binding(function() { return videoItem.width})
                height = Qt.binding(function() { return videoItem.height})
            }
            else {
                width = 1;
                height = 1;
            }
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

        function closeSubControls()
        {
            bncControl.activated = false;
            focusControl.activated = false;
            ptzControl.activated = false;
            fisheyeControl.ptzEnable = false;
        }

        // PTZ 제어 컨트롤
        PtzControl {
            id: ptzControl
            visible: activated
            supportPreset: mediaPlayer.supportPreset
            supportSwing: mediaPlayer.supportSwing
            supportGroup: mediaPlayer.supportGroup
            supportTour: mediaPlayer.supportTour
            supportTrace: mediaPlayer.supportTrace
            property bool activated: false

            property real lastDx: 0
            property real lastDy: 0
            property real lastDz: 0
            property real directionSpeed: 20 // 1~100
            property real maxPtz : 100
            property real minPtz : -100
            property real rangePtz : 200 // -100~100
            property real maxWrange : width*1.2 // 120%
            property real maxHrange : height*1.2
            onControlClosed: {
                activated = false;
            }
            onPresetTriggered: {
                // toggle 방식
                console.log("videoItem - onPresetTriggered")
                videoItem.ptzPresetTriggered(videoItem)
            }
            onSwingTriggered: {
                // toggle 방식
                console.log("videoItem - onSwingTriggered")
                videoItem.ptzSwingTriggered(videoItem)
            }
            onGroupTriggered: {
                // toggle 방식
                console.log("videoItem - onGroupTriggered")
                videoItem.ptzGroupTriggered(videoItem)
            }
            onTourTriggered: {
                // toggle 방식
                console.log("videoItem - onTourTriggered")
                videoItem.ptzTourTriggered(videoItem)
            }
            onTraceTriggered: {
                // toggle 방식
                console.log("videoItem - onTraceTriggered")
                videoItem.ptzTraceTriggered(videoItem)
            }

            navigator.onDirectionPtzTriggered: {
                var ptz_p = 0;
                var ptz_t = 0;

                switch(direction) {
                case PtzNavigator.PtzDirection.Left:
                    ptz_p = -1; break;
                case PtzNavigator.PtzDirection.Right:
                    ptz_p = 1; break;
                case PtzNavigator.PtzDirection.Down:
                    ptz_t = -1; break;
                case PtzNavigator.PtzDirection.Up:
                    ptz_t = 1; break;
                case PtzNavigator.PtzDirection.UpLeft:
                    ptz_p = -1; ptz_t = 1; break;
                case PtzNavigator.PtzDirection.DownLeft:
                    ptz_p = -1; ptz_t = -1; break;
                case PtzNavigator.PtzDirection.UpRight:
                    ptz_p = 1; ptz_t = 1; break;
                case PtzNavigator.PtzDirection.DownRight:
                    ptz_p = 1; ptz_t = -1; break;
                }
                ptz_p *= directionSpeed;
                ptz_t *= directionSpeed;
                //console.log("devicePtzContinuous(1)", ptz_p, ptz_t, 0)

                mediaPlayer.devicePtzContinuous(ptz_p, ptz_t, 0);
            }

            navigator.onStopPtzTriggered: {
                mediaPlayer.devicePtzStop();
                lastDx = 0;
                lastDy = 0;
                lastDz = 0;
            }

            navigator.onZoomTriggered: {
                var newDz = parseInt(dz);
                if (lastDz !== newDz) {
                    mediaPlayer.devicePtzContinuous(0, 0, newDz);
                    lastDz = newDz;
                }
            }

            navigator.onArrowPtzTriggered: {
                if (width <= 0 || height <= 0) {
                    return;
                }

                var sx_max = maxWrange/2
                var sy_max = maxHrange/2
                var sx_min = -sx_max
                var sy_min = -sy_max

                // transform range
                var gx = (((dx-sx_min) * rangePtz) / maxWrange) + minPtz;
                var gy = (((-dy-sy_min) * rangePtz) / maxHrange) + minPtz;

                var newDx = parseInt(gx)
                var newDy = parseInt(gy)
                if (lastDx !== newDx || lastDy !== newDy) {
                    //console.log("devicePtzContinuous(2)", newDx, newDy, 0)
                    mediaPlayer.devicePtzContinuous(newDx, newDy, 0);
                    lastDx = newDx;
                    lastDy = newDy;
                }

            }
        }

        // 밝기/대비 변경 컨트롤
        BncControl {
            id: bncControl
            visible: activated
            property bool activated: false
            onControlClosed: {
                var isChanged  = (itemModel.brightness !== bncControl.brightness ||
                                  itemModel.contrast !== bncControl.contrast);
                itemModel.brightness = bncControl.brightness
                itemModel.contrast = bncControl.contrast
                activated = false;
                if (isChanged) {
                    videoItem.modelPropertyChanged(videoItem);
                }
            }
        }

        // 포커스 컨트롤
        FocusControl {
            id: focusControl
            visible: activated
            property bool activated: false
            supportContinuousFocus: mediaPlayer.supportFocus
            supportSimpleFocus: mediaPlayer.supportSimpleFocus
            supportAutoFocus: mediaPlayer.supportAutoFocus
            supportResetFocus: mediaPlayer.supportResetFocus
            onControlClosed: {
                activated = false;
            }
            onDeviceFocusContinuous: {
                console.log("videoItem::deviceFocusContinuous()", contValue);
                mediaPlayer.deviceFocusContinuous(contValue);
            }
            onDeviceFocusModeControl: {
                console.log("videoItem::deviceFocusModeControl()", modeValue);
                mediaPlayer.deviceFocusModeControl(modeValue);
            }
        }

        FisheyeControl {
            id: fisheyeControl
            property bool activated: mediaPlayer.fisheyeDewarpStatus

            visible: activated
            fisheyeViewMode : itemModel.fisheyeViewMode
            fisheyeLocation : mediaPlayer.fisheyeLocation

            property real lastDx: 0
            property real lastDy: 0
            property real lastDz: 0
            property real directionSpeed: 20 // 1~100
            property real maxPtz : 100
            property real minPtz : -100
            property real rangePtz : 200 // -100~100
            property real maxWrange : width*1.2 // 120%
            property real maxHrange : height*1.2


            function updateZoomValue(positions)
            {
                for (var index = 0 ; index+2 < positions.length ; index=index+3) {
                    var zoomValue = positions[index+2];
                    if (index == 0) {
                        view1ZoomValue = zoomValue;
                    }
                    else if (index == 3) {
                        view2ZoomValue = zoomValue;
                    }
                    else if (index == 6) {
                        view3ZoomValue = zoomValue;
                    }
                    else {
                        view4ZoomValue = zoomValue;
                    }
                }
            }

            onDirectionPtzTriggered: {
                var ptz_p = 0;
                var ptz_t = 0;

                switch(direction) {
                case PtzNavigator.PtzDirection.Left:
                    ptz_p = -1; break;
                case PtzNavigator.PtzDirection.Right:
                    ptz_p = 1; break;
                case PtzNavigator.PtzDirection.Down:
                    ptz_t = -1; break;
                case PtzNavigator.PtzDirection.Up:
                    ptz_t = 1; break;
                case PtzNavigator.PtzDirection.UpLeft:
                    ptz_p = -1; ptz_t = 1; break;
                case PtzNavigator.PtzDirection.DownLeft:
                    ptz_p = -1; ptz_t = -1; break;
                case PtzNavigator.PtzDirection.UpRight:
                    ptz_p = 1; ptz_t = 1; break;
                case PtzNavigator.PtzDirection.DownRight:
                    ptz_p = 1; ptz_t = -1; break;
                }
                ptz_p *= directionSpeed;
                ptz_t *= directionSpeed;
                mediaPlayer.dewarpPtzContinuous(viewIndex, ptz_p, ptz_t, 0);

                fisheyeViewIndex = viewIndex
            }
            onArrowPtzTriggered: {
                if (width <= 0 || height <= 0) {
                    return;
                }

                var sx_max = maxWrange/2
                var sy_max = maxHrange/2
                var sx_min = -sx_max
                var sy_min = -sy_max

                // transform range
                var gx = (((dx-sx_min) * rangePtz) / maxWrange) + minPtz;
                var gy = (((-dy-sy_min) * rangePtz) / maxHrange) + minPtz;

                var newDx = parseInt(gx)
                var newDy = parseInt(gy)
                if (lastDx !== newDx || lastDy !== newDy) {
                    mediaPlayer.dewarpPtzContinuous(viewIndex, newDx, newDy, 0);
                    lastDx = newDx;
                    lastDy = newDy;
                }
                fisheyeViewIndex = viewIndex
            }
            onZoomTriggered: {
                var newDz = parseInt(dz);
                if (lastDz !== newDz) {
                    mediaPlayer.dewarpPtzContinuous(viewIndex, 0, 0, newDz);
                    lastDz = newDz;
                }
                fisheyeViewIndex = viewIndex
            }
            onAbsZoomTriggered: {
                //console.log("onAbsZoomTriggered::", zoomValue);
                mediaPlayer.dewarpPtzAbsZoom(viewIndex, zoomValue);
                fisheyeViewIndex = viewIndex
            }

            onStopPtzTriggered: {
                mediaPlayer.dewarpPtzStop(viewIndex);
                lastDx = 0;
                lastDy = 0;
                lastDz = 0;
                fisheyeViewIndex = viewIndex
            }
            onZoom1xTriggered: {
                mediaPlayer.dewarpPtzZoom1x(viewIndex);
                fisheyeViewIndex = viewIndex
            }
            onPointMovePtzTriggered: {
                var nx = px/width
                var ny = py/height
                mediaPlayer.dewarpPtzPointMove(viewIndex, nx, ny);
                fisheyeViewIndex = viewIndex
            }
            onAreaMovePtzTriggered: {
                var nx = px/width
                var ny = py/height
                var nw = pw/width
                var nh = ph/height
                mediaPlayer.dewarpPtzAreaMove(viewIndex, nx, ny, nw, nh);
                fisheyeViewIndex = viewIndex
            }
            onPtzControlClosed: {
                ptzEnable = false;
            }
        }

        VideoItemControl {
            id: videoControl
            property bool subControlActivated: ptzControl.activated || bncControl.activated ||
                                               focusControl.activated || fisheyeControl.ptzEnable
            visible: !subControlActivated

            videoPlayStatus: mediaPlayer.status
            isPlayback: mediaPlayer.seekable

            supportFocusSetup: (mediaPlayer.permissionPtzControl) &&(mediaPlayer.supportFocus || mediaPlayer.supportResetFocus || mediaPlayer.supportAutoFocus || mediaPlayer.supportSimpleFocus)
            supportPtzSetup: (mediaPlayer.permissionPtzControl) && (mediaPlayer.supportPanTilt || mediaPlayer.supportZoom || mediaPlayer.supportFisheyeDewarp)

            supportBnc: isEditable  // editable 이면 지원 (잠금, 공유가 아닌 경우)
            supportZoomTarget: isEditable   // editable 이면 지원 (잠금, 공유가 아닌 경우)
            supportPanTilt: mediaPlayer.supportPanTilt
            supportZoom: mediaPlayer.supportZoom
            supportZoom1x: mediaPlayer.supportZoom1x
            supportPreset: mediaPlayer.supportPreset && mediaPlayer.permissionPtzControl
            supportSwing: mediaPlayer.supportSwing && mediaPlayer.permissionPtzControl
            supportGroup: mediaPlayer.supportGroup && mediaPlayer.permissionPtzControl
            supportTour: mediaPlayer.supportTour && mediaPlayer.permissionPtzControl
            supportTrace: mediaPlayer.supportTrace && mediaPlayer.permissionPtzControl
            supportLocalRecording: videoItem.isCamera && mediaPlayer.permissionLocalRecording
            supportDualStream: isPlayback ? mediaPlayer.supportDualStreamPlayback : mediaPlayer.supportDualStreamLive
            profileMode : itemModel.itemProfile
            supportMic: mediaPlayer.supportMic  && mediaPlayer.permissionMic
            supportFisheye: mediaPlayer.supportFisheyeDewarp
            supportFisheyeZoomArea: mediaPlayer.fisheyeDewarpStatus && fisheyeControl.fisheyeViewMode === MediaLayoutItemViewModel.V_Original
            supportFisheyePtz: mediaPlayer.fisheyeDewarpStatus && (itemModel.fisheyeViewMode !== MediaLayoutItemViewModel.V_Original &&
                                                          !(itemModel.fisheyeViewMode === MediaLayoutItemViewModel.V_Panorama &&
                                                            mediaPlayer.fisheyeLocation === MediaLayoutItemViewModel.L_Wall))

            supportSmartSearch : mediaPlayer.supportSmartSearch
            supportSmartSearchFilter : mediaPlayer.supportSmartSearchFilter

            fisheyeViewMode : itemModel.fisheyeViewMode
            fisheyeLocation : mediaPlayer.fisheyeLocation

            onMouseMoved: videoItem.itemMouseMoved();
            onOverViewTriggered: {
                updateFisheyeViewMode(MediaLayoutItemViewModel.V_Original)
            }
            onSingleViewTriggered: {
                updateFisheyeViewMode(MediaLayoutItemViewModel.V_Single)
            }
            onQuadViewTriggered: {
                updateFisheyeViewMode(MediaLayoutItemViewModel.V_Quad)
            }
            onPanoramaViewTriggered: {
                updateFisheyeViewMode(MediaLayoutItemViewModel.V_Panorama)
            }

            function updateFisheyeViewMode(mode)
            {
                fisheyeViewIndex = 1
                if (itemModel.fisheyeViewMode !== mode) {
                    itemModel.fisheyeViewMode = mode;
                    videoItem.modelPropertyChanged(videoItem);
                }

            }

            function setFisheyeButton(isOn) {
                itemModel.fisheyeDewarpEnable = isOn
                videoItem.modelPropertyChanged(videoItem)
            }

            onProfileChangeClicked: {
                console.log("videoItem::profileChange(), changeProfile=", changeProfile)
                if (itemModel.itemProfile !== changeProfile) {
                    itemModel.itemProfile = changeProfile;
                    player.updateOutputParam(-1.0, videoOutput.contentRect.width, videoOutput.contentRect.height);
                    player.checkDualStream();
                    videoItem.modelPropertyChanged(videoItem);
                }
            }
            onFisheyeButtonClicked: {
                itemModel.fisheyeDewarpEnable = !itemModel.fisheyeDewarpEnable;
                videoItem.modelPropertyChanged(videoItem);
            }

            onPtzButtonClicked: {
                if (fisheyeControl.activated) {
                    fisheyeControl.ptzEnable = true;
                }
                else {
                    ptzControl.activated = true;
                }
            }

            onBncButtonClicked: {
                videoItem.openBncControl();
            }

            onFocusButtonClicked: {
                videoItem.openFocusControl();
            }

            onZoomButtonClicked: {
                videoItem.createNewZoomTarget();
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
                    console.log("videoItem::grapToImage() callback");
                    //result.saveToFile("hello.png");
                    mediaPlayer.saveSnapshot(result.image);
                }, Qt.size(targetWidth, targetHeight));
            }

            onMicButtonClicked: {
                var oldStatus = mediaPlayer.micStatus

                mediaPlayer.deviceTalk(!oldStatus)
                console.log("videoItem::micStatus=", oldStatus, mediaPlayer.micStatus)

            }

            onLocalRecordingButtonClicked: {
                var oldStatus = mediaPlayer.localRecordingStatus
                console.log("videoItem::localRecordingStatus=", mediaPlayer.localRecordingStatus)
                mediaPlayer.setLocalRecording(!mediaPlayer.localRecordingStatus, mediaPlayer.mediaName)
                if(oldStatus === true && mediaPlayer.localRecordingStatus === false) {
                    videoItem.localRecordingStopped(videoItem, true);
                }
            }
        }
    }

    DrawModeOsdControl {
        id: drawModeOsdControl
        visible: mediaPlayer.status === WisenetMediaPlayer.Loaded && opacity > 0.01 && isCamera && isSmartSearchMode && focusedTarget
        opacity: 0.0
        x:0
        y:0
        z:1
        searchFilterVisible : videoItem.supportSmartSearchFilter
        controlVisible: videoItem.width > 230 && videoItem.mouseOver && videoItem.supportSmartSearch

        onSmartSearchRequest: videoItem.smartSearchRequest()
        onOpenMessageDialog: videoItem.openMessageDialog(message)
    }

    MaskDrawOsdControl {
        id: maskDrawOsdControl
        visible: mediaPlayer.status === WisenetMediaPlayer.Loaded && opacity > 0.01 && isLocalResource && isMaskingMode && focusedTarget
        opacity: 0.0
        x:0
        y:0
        z:1
        controlVisible: videoItem.width > 230

        onOpenMessageDialog: videoItem.openMessageDialog(message)
        onDrawFinished: mediaPlayer.addMask(maskId, maskType, rectCoordinates)
        onMaskRemoved: mediaPlayer.removeMask(maskId, maskType)
        onUpdateFinished: mediaPlayer.updateMask(maskId, maskType, rectCoordinates)
        onManualBoxUpdated: mediaPlayer.updateManualTrackingRect(maskId, maskType, rectCoordinates)

        onStartTracking: {
            mediaPlayer.startTracking()
            mediaPlayer.play()
            mediaPlayer.speed = speed
        }
        onStopTracking: {
            mediaPlayer.stopTracking()
            mediaPlayer.pause()
        }

        onSelectedMaskItemChanged: {
            console.log("maskDrawOsdControl.onSelectedMaskItemChanged", maskId)
            videoItem.selectedMaskItemChanged(maskId)
        }
    }

    NumberAnimation{
        id:closeAnim
        target: videoItem;
        properties:"opacity";
        to:0
        duration:150
        onStopped: {
            console.log("videoItem::destroy()");
            videoItem.destroy();
        }
    }
    NumberAnimation{
        id:openAnim
        target: videoItem;
        properties:"opacity";
        to:1.0
        duration:150
    }

    function sleep(isOn)
    {
        console.log("VideoItemJS::sleep(), mediaName={}", mediaName);
        mediaPlayer.sleep(isOn)
    }

    function open()
    {
        openAnim.start();
    }

    function close()
    {
        // 영상 닫기시
        if (zoomAreas.length > 0) {
            disconnectZoomTargetAreas();
            clearZoomTargetAreas();
        }

        closeAnim.start();
        console.debug("close videoItem!!");
    }

    function closeWithoutAnimation()
    {
        // 영상 닫기시
        if (zoomAreas.length > 0) {
            disconnectZoomTargetAreas();
            clearZoomTargetAreas();
        }

        videoItem.destroy();
    }

    //----------------------------------------------------//
    // 줌타겟 연동
    //----------------------------------------------------//
    // false시에는 connection관계만 끊는다.
    function disconnectZoomTargetAreas()
    {
        if (zoomAreas.length > 0) {
            console.log("VideoItemJS::disconnectZoomTargetAreas(), zoomAreas=", zoomAreas.length);
            for (var i = 0 ; i < zoomAreas.length ; i++) {
                var zoomTargetItem = zoomAreas[i].zoomTargetItem;
                if (zoomTargetItem) {
                    zoomTargetItem.zoomTargetDestruction.disconnect(videoItem.onZoomTargetDestruction);
                }
            }
        }
    }

    function clearZoomTargetAreas()
    {
        if (zoomAreas.length > 0) {
            zoomAreas = [];
        }
    }

    property bool zoomAreaVisible: {
        if (videoItem.itemNameAlwaysVisible)
            return true;

        if (positionChanging)
            return false;

        if (mediaPlayer.fisheyeDewarpStatus && !videoControl.supportFisheyeZoomArea)
            return false;

        if (osdControl.visible && !ptzControl.activated && !fisheyeControl.ptzEnable)
            return true;
        return false;
    }

    function setZoomTargetArea(zoomAreaComponent, zoomTarget, zoomAreaRect, rectColor)
    {
        // layout model에서 로딩한 경우에는 색상값이 DB에 없으므로 할당시켜줘야 한다.
        if (rectColor) {
            var rectBorderColor = rectColor
        }
        else {
            rectBorderColor = zoomTargetColors[zoomTargetColorsIndex];
            zoomTargetColorsIndex++;
            if (zoomTargetColorsIndex >= zoomTargetColors.length)
                zoomTargetColorsIndex = 0;
        }


        var zoomArea = zoomAreaComponent.createObject(videoItem,
                                                      {
                                                          zoomAreaVisible: Qt.binding(function(){return videoItem.zoomAreaVisible}),
                                                          editable: Qt.binding(function(){return videoItem.isEditable}),
                                                          orientation:Qt.binding(function(){return itemModel.rotation}),
                                                          contentRectX:Qt.binding(function(){return videoOutput.contentRect.x}),
                                                          contentRectY:Qt.binding(function(){return videoOutput.contentRect.y}),
                                                          contentRectW:Qt.binding(function(){return videoOutput.contentRect.width}),
                                                          contentRectH:Qt.binding(function(){return videoOutput.contentRect.height}),
                                                          dewarpEnabled: Qt.binding(function(){return mediaPlayer.fisheyeDewarpStatus}),
                                                          fisheyeViewMode:Qt.binding(function(){return itemModel.fisheyeViewMode}),
                                                          normalRectX: zoomAreaRect.x,
                                                          normalRectY: zoomAreaRect.y,
                                                          normalRectW: zoomAreaRect.width,
                                                          normalRectH: zoomAreaRect.height,
                                                          zoomTargetItem: zoomTarget,
                                                          zoomTargetItemId: zoomTarget.itemId,
                                                          borderColor: rectBorderColor
                                                      }
                                                      );

        if (!zoomArea) {
            console.error("VideoItemJS::setZoomTargetArea()", zoomAreaComponent.errorString());
        }
        zoomTarget.zoomTargetFocused.connect(zoomArea.onZoomTargetFocused)
        zoomTarget.zoomTargetFisheyeViewPortChanged.connect(zoomArea.onZoomTargetFisheyeViewPortChanged)

        zoomTarget.zoomTargetDestruction.connect(videoItem.onZoomTargetDestruction)
        zoomAreas.push(zoomArea);
        mediaPlayer.updateKeepSize();
        return zoomArea;
    }

    function onZoomTargetDestruction(zoomTargetId)
    {
        console.log("VideoItemJS::onZoomTargetDestruction()", zoomTargetId, zoomAreas.length);
        for (var i = 0 ; i < zoomAreas.length ; i++) {
            if (zoomAreas[i].zoomTargetItemId === zoomTargetId) {
                var zoomArea = zoomAreas[i];
                zoomAreas.splice(i, 1);
                zoomArea.destroy();
                console.log("VideoItemJS::removeZoomTargetArea, itemId=", zoomTargetId, ",length=", zoomAreas.length);
                mediaPlayer.updateKeepSize();
                break;
            }
        }
    }

    function getThumbnailImage(callback, imageWidth, errorStatusOnly = false) {
        var srcWidth = videoOutput.width;
        var srcHeight = videoOutput.height;
        var screenRatio = srcHeight/srcWidth;
        var imageHeight = imageWidth*screenRatio;
        console.log('getThumbnailImage,', srcWidth, srcHeight, screenRatio, imageWidth, imageHeight)

        if(errorStatusOnly)
            errorStatus.grabToImage(callback, Qt.size(imageWidth, imageHeight));
        else
            videoOutput.grabToImage(callback, Qt.size(imageWidth, imageHeight));
    }

    //----------------------------------------------------//
    // Smart Search
    //----------------------------------------------------//

    // SmartSearchArea 모두 삭제
    function clearSmartSearchAreas()
    {
        //console.log("VideoItem::clearSmartSearchAreas()");
        drawModeOsdControl.clearSmartSearchAreas()
    }

    // SmartSearch 그리기 모드 종료
    function stopSmartSearchDrawMode()
    {
        drawModeOsdControl.stopSmartSearchDrawMode()
    }

    // 가상 선 Data List 획득
    function getVirtualLineList()
    {
        return drawModeOsdControl.getVirtualLineList()
    }

    // 가상 영역 Data List 획득
    function getVirtualAreaList()
    {
        return drawModeOsdControl.getVirtualAreaList()
    }
}
