import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.15
import "ViewingGrid.js" as GridManager
//import QtWebEngine 1.10
import WisenetMediaFramework 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"

Item {
    id: viewingGridRoot

    /////////////////////////////////////////////////////////////
    // 프로퍼티
    /////////////////////////////////////////////////////////////

    // standalone모드에서는 단축키, fullscreen 등 일부기능 사용불가 (이벤트검색페이지)
    property bool standalone : false

    // OSD ON/OFF : 비디오 아이템 상단 이름 정보 항시 표시 ON/OFF
    property bool itemNameAlwaysVisible: false

    // Audio Property
    property bool   audioOutEnable : false
    property int    audioOutVolume : 0

    // FullScreen setting
    property bool   isFullScreen: false

    // 잠금 설정 여부
    readonly property bool isLocked: layoutModel.locked

    // 비디오 Status 표시 여부
    property alias videoStatus: layoutModel.videoStatus

    // 공유레이아웃 여부
    readonly property bool isShared: (layoutModel.ownerId && (layoutModel.loginId !== layoutModel.ownerId))

    // Admin 사용자 여부
    property alias isUserAdmin: layoutModel.userAdmin

    // 편집 가능 여무 (locked + shared)
    readonly property bool isEditable: !isLocked && !isShared

    // 백그라운드 색상
    property alias color: viewingGridBackground.color

    // 상하좌우 패널을 제외하고 실제 아이템들이 보여질 영역 (fit in view)
    property alias viewX: gridViewArea.x
    property alias viewY: gridViewArea.y
    property alias viewWidth: gridViewArea.width
    property alias viewHeight: gridViewArea.height

    // 현재 아이템의 갯수
    readonly property int itemCount: gridItemsArea.itemCount

    // MediaLayoutViewModel
    readonly property var  layout:layoutModel

    // 현재 로고&그리드라인 표시 여부 확인
    readonly property bool gridLineVisible: viewGridLines.bShowGrid

    // 현재 포커스 아이템
    property Item focusedItem: nullItem

    // 선택된 아이템 리스트
    property var selectedItems: []

    // 전체 아이템 리스트
    property var gridItems: []

    // GRID PTZ 애니메이션 시간
    property alias gridPtzDuration: gridAnimation.duration

    // Sleep mode timeout duration (milli seconds)
    // Sleep mode를 사용하지 않을 경우 해당 값을 0으로 셋할 것.
    property int sleepModeDuration : 5000 // 기본값 5초

    property bool isFocusedWindow : false
    property real screenPixelRatio: Screen.devicePixelRatio

    property bool isMediaFileOnly: false // MediaFile 전용 ViewingGrid(WisenetPlayer)인 경우 불필요한 UI 숨김 처리
    property bool isSmartSearchMode: false // SmartSearch 미지원 기능 비활성화
    property bool isMaskingMode: false  // Masking 설정 전용 viewingGrid로 사용 (나머지 기능들을 비활성화)

    property bool isSequence : false

    property real spc2000ScreenInputCount : 0 // 기본값 0
    property Item screenCurrentItem: nullItem // 현재 위치를 저장하기 위한 아이템. 스크린모드에서 resize된 focusedItem이 차지하는 영역을 지날 때 사용
    property bool isLinkedMode : true // 동기화 재생 모드 flag

    onIsFocusedWindowChanged: {
        console.log("ViewingGrid::onIsFocusedWindowChanged:", isFocusedWindow, viewingGridRoot);
    }

    /////////////////////////////////////////////////////////////
    // 시그널
    /////////////////////////////////////////////////////////////
    // 미디어 오픈 결과
    signal mediaOpenResult(Item item, int error);
    // 미디어 오픈 요청 리스트
    signal mediaOpenRequested(var items);
    // 미디어 상태 변경
    signal mediaStatusChanged(Item item, int status);
    // 아이템 추가
    signal itemsAdded(var items);
    // 아이템 삭제
    signal itemsRemoved(var itemIds);
    // 아이템 업데이트
    signal itemsUpdated(var items);
    // 아이템을 제외한 레이아웃 프로퍼티 변경
    signal layoutPropertyUpdated(var layoutModel);
    // viewingGrid에 저장 된 레이아웃을 신규 로드 하거나, viewingGrid가 저장 된 레이아웃으로 업데이트 된 경우 발생
    signal savedLayoutLoaded();

    // 새탭에서 아이템 열기
    signal openInNewTabTriggered(var items);

    // 새탭에서 윈도우 열기
    signal openInNewWindowTriggered(var items);

    // full screen ON/OFF
    signal showFullscreen(var onOff);

    // 카메라 설정 열기
    signal cameraSetupOpenTriggered(var items);
    // NVR 설정 열기
    signal nvrSetupOpenTriggered(var items);

    // Event rule 설정 열기
    signal eventRulesSetupOpenTriggered(var items);

    // 웹페이지 설정 열기
    signal webpageSettingTriggered(var webpageId);

    // 레이아웃 드랍 이벤트
    signal layoutDropped(var layoutItems);

    // 시퀀스 드랍 이벤트
    signal sequneceDropped(var sequenceItems);

    signal editNameTriggered(Item item);

    signal smartSearchRequest() // 스마트 검색 요청. 데이터는 focusedItem에서 get
    signal openMessageDialog(var message)   // MessageDialog open 요청
    signal selectedMaskItemChanged(var maskId) // mask selected
    signal showTrackingFailMessage()

    /////////////////////////////////////////////////////////////
    // 함수
    /////////////////////////////////////////////////////////////
    // 초기 레이아웃 설정
    function setLayout(layout)
    {
        GridManager.loadLayout(layout);
    }

    // 레이아웃 저장
    function saveLayout()
    {
        console.log("ViewingGrid::saveLayout()");
        if (layoutModel.isNewLayout()) {
            layoutSaveAsDialog.isSaveAs = false
            layoutSaveAsDialog.text = "Layout";
            layoutSaveAsDialog.showDialog();
        }
        else {
            layoutModel.saveToCoreService();
        }
    }

    // 레이아웃 다른 이름으로 저장
    function saveAsDialog()
    {
        console.log("ViewingGrid::onSaveAsTriggered()");
        layoutSaveAsDialog.isSaveAs = true
        layoutSaveAsDialog.text = layoutModel.name + "#";
        layoutSaveAsDialog.showDialog();
    }

    function openMediaDialog()
    {
        mediaFileOpenDialog.open();
    }

    // 코어서비스 장비 영상 추가 (Live, Playback)
    // Playback인 경우에는 startTime값을 채워줘야함. (overlappedId는 필요시 set)
    function addNewVideoChannel(deviceId, channelId,
                                isPlayback = false, startTime = 0, overlappedId = -1, linkToNewItem = false,
                                posId = 0, isPosPlayback = false)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewVideoChannel(deviceId, channelId,
                                           isPlayback, startTime, overlappedId, linkToNewItem,
                                           posId, isPosPlayback)
        }
    }

    // 이미 영상이 있는 경우에는 해당 영상 제어 (play 변경)
    // 새로운 채널일 경우 addNewVideoChannel()과 동일
    function checkNewVideoChannel(deviceId, channelId, isPlayback = false, startTime = 0, overlappedId = -1, linkToNewItem = true, posId = 0, isPosPlayback = false)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.checkNewVideoChannel(deviceId, channelId,
                                             isPlayback, startTime, overlappedId, linkToNewItem, posId, isPosPlayback);
        }
    }

    function addNewDragItems(dragItemListModel)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewDragItems(dragItemListModel)
        }
    }

    function addNewVideoChannels(devices)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewVideoChannels(devices)
        }
    }

    function addNewGroup(groupId)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewGroup(groupId)
        }
    }

    // 로컬 파일 추가
    function addNewLocalFile(fileUrl)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewLocalFile(fileUrl);
        }
    }

    function addNewLocalFiles(urls)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewLocalFiles(urls);
        }
    }

    // 웹페이지 ID로 추가
    function addNewWebpage(webPageId)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewWebpage(webPageId);
        }
    }

    // 아이템리스트로 추가 (AddNewTab, AddNewWindow에서 사용)
    function addNewGridItems(viewItems)
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.addNewGridItems(viewItems);
        }
    }

    // AutoFit On/Off
    function setAutoFit(isAutofit)
    {
        console.log("ViewingGrid::setAutoFit()", isAutofit)
        gridViewArea.autoFit = isAutofit;
        if (isAutofit) {
            GridManager.fitAndMediaCheck();
        }
    }

    function cleanUp()
    {
        if (!layoutWarning.checkLocked()) {
            GridManager.cleanUp();
        }
    }

    function cleanUpForSequence()
    {
        GridManager.cleanUpForSequence();
    }

    function onLocalRecordingStopped(item, showNotification)
    {
        var filePath = item.player.localRecordingPath
        console.log("ViewingGrid::onLocalRecordingStopped() filePath=", filePath, "showNotification=", showNotification);
        if(showNotification) {
            // 로컬 레코딩 수동 중지인 경우 (저장 폴더 팝업)
            fileSavedPopup.show(WisenetLinguist.videoFileSaved, filePath);
        }
        else {
            // LocalRecordingStatus 변경시 호출되는 경우 (WisenetPlayer 복사)
            GridManager.checkLocalRecordingFinished(item);
        }
    }

    function onVideoCaptured(success, filePath)
    {
        console.log("ViewingGrid::onVideoCaptured(), success=", success, ",filePath=", filePath);
        fileSavedPopup.show(WisenetLinguist.imageFileSaved, filePath);
    }

    function showVerifyFinished()
    {
        console.log("ViewingGrid::onVerifyFinished()");
        verifyFinishedPopup.showMessage(WisenetLinguist.verificationFinished);
    }

    // Digital Signature Verify 결과를 LayoutItem에 업데이트
    function updateVerifyStatus(fileUuid, verifyResult)
    {
        layout.updateVerifyResult(fileUuid, verifyResult)
    }

    function onDeviceTalkResult(msg)
    {
        console.log("ViewingGrid::onDeviceTalkResult()" + msg);
        talkPopup.showMessage(msg);
    }

    function trackingFailed()
    {
        console.log("ViewingGrid::trackingFailed()");
        verifyFinishedPopup.showMessage(WisenetLinguist.trackingFailed);
    }

    /////////////////////////////////////////////////////////////
    // 비디오 제어 Helper API
    // NVR 동기 재생을 위해서 ViewingGrid에 media 제어 인터페이스를 추가함.
    /////////////////////////////////////////////////////////////
    // 비디오 플레이중 seek 또는 live에서 playback으로 전환
    function mediaSeek(targetItem, pos, trackId)
    {
        console.log("ViewingGrid::mediaSeek(), pos=", pos, ", trackID=", trackId);
        if (!targetItem.isVideoItem) {
            return;
        }

        if (targetItem.isLocalResource) {
            if(isMediaFileOnly && isLinkedMode) {
                // 미디어파일 동기화 재생 모드인 경우, 전체 파일 동기화 재수행
                GridManager.linkAllMediaFiles(targetItem, pos)
            }
            else {
                targetItem.player.seek(pos);
            }

            return;
        }

        if (!targetItem.mediaSeekable || targetItem.mediaParam.trackId !== trackId) {
            // 라이브 혹은 TRACK ID가 변경된 경우에는 새로 Open한다.
            console.log("ViewingGrid::mediaSeek(), trackID changed or live, trackId=", trackId);
            var speed = targetItem.mediaPlaySpeed
            var paused = targetItem.player.playbackState === WisenetMediaPlayer.PausedState
            targetItem.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", pos, 0, trackId, speed, paused);
        }
        else {
            // 이외에는 seek 수행
            targetItem.player.seek(pos)
        }

        if(isLinkedMode) {
            // 동기화 재생 모드인 경우, 전체 채널 동기화 재수행
            GridManager.linkAllVideoChannels(targetItem, pos)
        }
        else {
            // 동기화 재생 모드가 아닌 경우, 동일장비내 모든 player의 상태값을 갱신한다.
            _mediaUpdateAllChannelState(targetItem, true, pos)
        }
    }

    // speed 변경
    function mediaSetSpeed(targetItem, speed)
    {
        console.log("ViewingGrid::mediaSetSpeed(), speed=", speed);
        if (!targetItem.isVideoItem) {
            return;
        }

        if (targetItem.isLocalResource) {
            if (targetItem.player.playbackState === WisenetMediaPlayer.PausedState) {
                targetItem.player.play();
            }
            targetItem.player.speed = speed;

            if(isMediaFileOnly && isLinkedMode) {
                // 미디어파일 동기화 재생 모드인 경우, 전체 파일 동기화 재수행
                GridManager.linkAllMediaFiles(targetItem)
            }

            return;
        }

        // 하나의 채널에 대해서 명령을 날리고, 동일장비내 모든 player의 상태값을 갱신한다.
        if (targetItem.player.playbackState === WisenetMediaPlayer.PausedState) {
            targetItem.player.play();
        }
        targetItem.player.speed = speed;

        if(isLinkedMode) {
            // 동기화 재생 모드인 경우, 전체 채널 동기화 재수행
            GridManager.linkAllVideoChannels(targetItem)
        }
        else {
            // 동기화 재생 모드가 아닌 경우, 동일장비내 모든 player의 상태값을 갱신한다.
            _mediaUpdateAllChannelState(targetItem, false)
        }
    }

    // PAUSE
    function mediaPause(targetItem)
    {
        console.log("ViewingGrid::mediaPause()");
        if (!targetItem.isVideoItem) {
            return;
        }

        if (targetItem.isLocalResource) {
            targetItem.player.pause();

            if(isMediaFileOnly && isLinkedMode) {
                // 미디어파일 동기화 재생 모드인 경우, 전체 파일 동기화 재수행
                GridManager.linkAllMediaFiles(targetItem)
            }

            return;
        }
        // 하나의 채널에 대해서 명령을 날리고, 동일장비내 모든 player의 상태값을 갱신한다.
        targetItem.player.pause();

        if(isLinkedMode) {
            // 동기화 재생 모드인 경우, 전체 채널 동기화 재수행
            GridManager.linkAllVideoChannels(targetItem, 0, true)
        }
        else {
            // 동기화 재생 모드가 아닌 경우, 동일장비내 모든 player의 상태값을 갱신한다.
            _mediaUpdateAllChannelState(targetItem, false)
        }
    }

    // PLAY
    function mediaPlay(targetItem)
    {
        console.log("ViewingGrid::mediaPlay()");
        if (!targetItem.isVideoItem) {
            return;
        }

        if (targetItem.isLocalResource) {
            targetItem.player.play();

            if(isMediaFileOnly && isLinkedMode) {
                // 미디어파일 동기화 재생 모드인 경우, 전체 파일 동기화 재수행
                GridManager.linkAllMediaFiles(targetItem)
            }

            return;
        }

        // 하나의 채널에 대해서 명령을 날리고, 동일장비내 모든 player의 상태값을 갱신한다.
        targetItem.player.play();

        if(isLinkedMode) {
            // 동기화 재생 모드인 경우, 전체 채널 동기화 재수행
            GridManager.linkAllVideoChannels(targetItem)
        }
        else {
            // 동기화 재생 모드가 아닌 경우, 동일장비내 모든 player의 상태값을 갱신한다.
            _mediaUpdateAllChannelState(targetItem, true)
        }
    }

    // STEP F/B
    function mediaStep(targetItem, isForward)
    {
        console.log("ViewingGrid::mediaStep(), isForward=", isForward);
        if (!targetItem.isVideoItem) {
            return;
        }
        if (targetItem.isLocalResource) {
            if(isMediaFileOnly && isLinkedMode) {
                // 미디어파일 동기화 재생 모드인 경우, 전체 파일 동기화 재수행
                GridManager.linkAllMediaFiles(targetItem, 0, true, isForward)
            }
            else {
                targetItem.player.step(isForward);
            }

            return;
        }

        if(isLinkedMode) {
            // 동기화 재생 모드인 경우, 장비별 대표 채널에 Step 명령
            var deviceMap = new Map()   // Device 별 대표 Item Map
            deviceMap.set(targetItem.mediaParam.deviceId, targetItem)   // targetItem은 해당 Device의 대표 Item으로 고정

            for (var i = gridItems.length-1 ; i >=0 ; i--) {
                var item = gridItems[i]
                if(!item || !item.isCamera)
                    continue

                if(item.mediaParam.deviceId !== targetItem.mediaParam.deviceId) {
                    deviceMap.set(item.mediaParam.deviceId, item)   // Device 별 대표 Channel 저장
                }
            }

            for(var channelItem of deviceMap.values()) {
                channelItem.player.step(isForward);
                _mediaUpdateAllChannelState(channelItem, false)
            }

        }
        else {
            // 동기화 재생 모드가 아닌 경우, 하나의 채널에 대해서 명령을 날리고, 동일장비내 모든 player의 상태값을 갱신한다.
            targetItem.player.step(isForward);
            _mediaUpdateAllChannelState(targetItem, false)
        }
    }

    // NVR 채널 재생상태를 업데이트하는 내부 공통루틴
    // Pause 및 step f/b는 playwait 타이머 동작 x
    function _mediaUpdateAllChannelState(targetItem, updatePlayWait, syncTime = 0)
    {
        var channelItems = GridManager.getPlaybackChannelsOfDevice(targetItem.mediaParam.deviceId);

        var targetState = targetItem.player.playbackState;
        var targetSpeed = targetItem.mediaPlaySpeed;
        var targetTrackId = targetItem.mediaParam.trackId;
        console.log("ViewingGrid::_mediaUpdateAllChannelState(), channels=",
                    channelItems.length, targetState, targetSpeed, targetTrackId);

        for (var i = 0 ; i < channelItems.length ; i++) {
            var item = channelItems[i];
            if (item !== targetItem) {
                if(item.mediaParam.trackId !== targetTrackId) {
                    // Track이 변경되면 MediaOpen 재수행
                    /*
                    console.log("ViewingGrid::_mediaUpdateAllChannelState(), trackId Changed. index :", i,
                                "before :", item.mediaParam.trackId, "after :", targetTrackId);
                                */
                    var pos = syncTime !== 0 ? syncTime : targetItem.mediaPlayPosition
                    var paused = targetState === WisenetMediaPlayer.PausedState
                    item.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", pos, 0, targetTrackId, targetSpeed, paused);
                }
                else {
                    // 이외에는 상태만 업데이트
                    //console.log("ViewingGrid::_mediaUpdateAllChannelState(), sync playback status:", i);
                    item.player.updatePlaybackStatus(targetState, targetSpeed, targetTrackId, updatePlayWait);
                }
            }
        }
    }

    // 전체 채널에 대한 재생시간을 동기화 하는 함수
    // targetItem 및 syncTime 기준으로 동기화
    function linkAllVideoChannels(targetItem, syncTime)
    {
        GridManager.linkAllVideoChannels(targetItem, syncTime)
    }

    // 전체 MediaFile에 대한 재생시간을 동기화 하는 함수
    function linkAllMediaFiles()
    {
        GridManager.linkAllMediaFiles()
    }

    // 동기화 재생 대상 Item을 찾아서 리턴하는 함수
    function getLinkedModeTargetItem(deviceId)
    {
        if(focusedItem && focusedItem.isCamera && focusedItem.mediaPlayPosition !== 0)
            return focusedItem  // focusedItem이 Camera인 경우 focusedItem 리턴

        var firstCameraItem = null

        for (var i = 0 ; i < gridItems.length ; i++) {
            var item = gridItems[i];
            if(!item || !item.isCamera)
                continue

            if(!firstCameraItem && item.mediaPlayPosition !== 0)
                firstCameraItem = item

            if (item.mediaParam.deviceId === deviceId && item.mediaPlayPosition !== 0) {
                return item // 같은 장비의 첫 번째 Camera 리턴
            }
        }

        return firstCameraItem  // 전체 중 첫 번째 Camera 리턴
    }

    function findNextFocusedItem(panValue, tiltValue, zoomValue) {
        var nextFocusedItem = focusedItem
        if(screenCurrentItem == null) {
            screenCurrentItem = focusedItem
        }
//        console.log("screenCurrentItem.column=",screenCurrentItem.column, "screenCurrentItem.row=",screenCurrentItem.row)
//        console.log("focusedItem.column=",focusedItem.column, "focusedItem.row=",focusedItem.row)
//        console.log("focusedItem.columnCount=",focusedItem.columnCount, "focusedItem.rowCount=",focusedItem.rowCount)

        var i = 0

        var vpos = GridManager._getItemsViewPosition() // 현재 뷰잉그리드의 타일 배치(top,bottom,left,right)를 알기 위함
        var rowStart = vpos.top
        var columnStart = vpos.left
        var columnCount = vpos.right-vpos.left;
        var rowCount = vpos.bottom-vpos.top;

//        console.log("getItemMatrix() total count", rowCount, columnCount)
//        console.log("getItemMatrix() rowStart=",rowStart," columnStart=",columnStart)

        var itemMatrix = new Array(rowCount);
        for (i = 0; i < itemMatrix.length; i++) {
            itemMatrix[i] = new Array(columnCount);
        }

        // resized된 비디오 타일이 차지하는 빈 타일도 비디오 타일로 채우는 작업 (얕은 복사)
        for (var j = 0 ; j < gridItems.length ; j++) {
            var item = gridItems[j];
//            console.log("getItemMatrix() itemrow", item.row, "itemcolumn", item.column, "rowCount", item.rowCount, "columnCount", item.columnCount)
            for(var row = 0 ; row < item.rowCount ; row++) {
                for(var col = 0 ; col < item.columnCount ; col++) {
//                    console.log("getItemMatrix() row=", item.row - rowStart + row, "column=", item.column - columnStart + col)
                    itemMatrix[item.row - rowStart + row][item.column - columnStart + col] = item
                }
            }
        }

        var itemMatrixColumn = screenCurrentItem.column - columnStart
        var itemMatrixRow = screenCurrentItem.row - rowStart
        var minDistance = -1
        var nextDistance, c, r

        // 타일 왼쪽 이동
        if(panValue < 0) {
//            console.log("왼쪽start=",screenCurrentItem.column-columnStart-1," end=", columnStart)
            // 같은 라인의 가장 가까운 타일 찾기
            for(c = screenCurrentItem.column-columnStart-1; c >= 0; c--) {
//                console.log("searching.. c=",c," itemMatrixRow=",itemMatrixRow, "itemMatrix[itemMatrixRow][c]=",itemMatrix[itemMatrixRow][c])
                if(itemMatrix[itemMatrixRow][c] !== undefined && itemMatrix[itemMatrixRow][c] !== null && itemMatrix[itemMatrixRow][c] !== screenCurrentItem) {
                    nextFocusedItem = itemMatrix[itemMatrixRow][c]
                    break
                }
            }
            // 왼쪽 영역의 가장 가까운 타일 찾기
//            console.log("1 nextFocusedItem=",nextFocusedItem,"screenCurrentItem=",screenCurrentItem)
            if(nextFocusedItem == screenCurrentItem) {
                minDistance = -1
                for(c = itemMatrixColumn - 1; c >= 0; c--) {
                    for(r = 0; r < rowCount; r++) {
//                        console.log("1 왼쪽 r=",r," c=", c)
                        if(itemMatrix[r][c] !== undefined && itemMatrix[r][c] !== null && itemMatrix[r][c] !== screenCurrentItem) {
                            nextDistance = Math.abs(itemMatrixColumn - c) + Math.abs(itemMatrixRow - r)
//                            console.log("2 왼쪽 r=",r," c=", c, "nextDistance=",nextDistance, "minDistance=",minDistance,"itemMatrix[r][c]=",itemMatrix[r][c])
                            if(minDistance === -1) {
                                minDistance = nextDistance
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                            else if(nextDistance < minDistance) {
                                minDistance = Math.min(minDistance, nextDistance)
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                        }
                    }
                    if(nextFocusedItem != screenCurrentItem) {
                        break
                    }
                }
            }
        }

        // 타일 오른쪽 이동
        if(panValue > 0) {
//            console.log("오른쪽start=",screenCurrentItem.column-columnStart+1, " end=", columnCount)
            // 같은 라인의 가장 가까운 타일 찾기
            for(c = screenCurrentItem.column-columnStart+1; c < columnCount; c++) {
//                console.log("searching.. c=",c," itemMatrixRow=",itemMatrixRow, "itemMatrix[screenCurrentItem.row][c]=",itemMatrix[itemMatrixRow][c])
                if(itemMatrix[itemMatrixRow][c] !== undefined && itemMatrix[itemMatrixRow][c] !== null && itemMatrix[itemMatrixRow][c] !== screenCurrentItem) {
                    nextFocusedItem = itemMatrix[itemMatrixRow][c]
                    break
                }
            }
            // 오른쪽 영역의 가장 가까운 타일 찾기
//            console.log("1 nextFocusedItem=",nextFocusedItem,"screenCurrentItem=",screenCurrentItem)
            if(nextFocusedItem == screenCurrentItem) {
                minDistance = -1
                for(c = itemMatrixColumn + 1; c < columnCount; c++) {
                    for(r = 0; r < rowCount; r++) {
//                        console.log("1 오른쪽 r=",r," c=", c)
                        if(itemMatrix[r][c] !== undefined && itemMatrix[r][c] !== null && itemMatrix[r][c] !== screenCurrentItem) {
                            nextDistance = Math.abs(itemMatrixColumn - c) + Math.abs(itemMatrixRow - r)
//                            console.log("2 오른쪽 r=",r," c=", c, "nextDistance=",nextDistance, "minDistance=",minDistance,"itemMatrix[r][c]=",itemMatrix[r][c])
                            if(minDistance === -1) {
                                minDistance = nextDistance
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                            else if(nextDistance < minDistance) {
                                minDistance = Math.min(minDistance, nextDistance)
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                        }
                    }
                    if(nextFocusedItem != screenCurrentItem) {
                        break
                    }
                }
            }
        }

        // 타일 아래쪽 이동
        if(tiltValue < 0) {
//            console.log("아래start=",screenCurrentItem.row-rowStart+1," end=", rowCount)
            // 같은 라인의 가장 가까운 타일 찾기
            for(r = screenCurrentItem.row-rowStart+1; r < rowCount; r++) {
//                console.log("searching.. itemMatrixColumn=",itemMatrixColumn," r=",r, "itemMatrix[r][itemMatrixColumn]=",itemMatrix[r][itemMatrixColumn])
                if(itemMatrix[r][itemMatrixColumn] !== undefined && itemMatrix[r][itemMatrixColumn] !== null && itemMatrix[r][itemMatrixColumn] !== screenCurrentItem) {
                    nextFocusedItem = itemMatrix[r][itemMatrixColumn]
                    break
                }
            }
            // 아래쪽 영역의 가장 가까운 타일 찾기
//            console.log("1 nextFocusedItem=",nextFocusedItem,"screenCurrentItem=",screenCurrentItem)
            if(nextFocusedItem == screenCurrentItem) {
                minDistance = -1
                for(r = itemMatrixRow + 1; r < rowCount; r++) {
                    for(c = 0; c < columnCount; c++) {
//                        console.log("1 아래쪽 r=",r," c=", c)
                        if(itemMatrix[r][c] !== undefined && itemMatrix[r][c] !== null && itemMatrix[r][c] !== screenCurrentItem) {
                            nextDistance = Math.abs(itemMatrixColumn - c) + Math.abs(itemMatrixRow - r)
//                            console.log("2 아래쪽 r=",r," c=", c, "nextDistance=",nextDistance, "minDistance=",minDistance,"itemMatrix[r][c]=",itemMatrix[r][c])
                            if(minDistance === -1) {
                                minDistance = nextDistance
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                            else if(nextDistance < minDistance) {
                                minDistance = Math.min(minDistance, nextDistance)
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                        }
                    }
                    if(nextFocusedItem != screenCurrentItem) {
                        break
                    }
                }
            }
        }

        // 타일 위쪽 이동
        if(tiltValue > 0) {
//            console.log("위start=",screenCurrentItem.row-rowStart-1," end=",rowStart)
            // 같은 라인의 가장 가까운 타일 찾기
            for(r = screenCurrentItem.row-rowStart-1; r >= 0; r--) {
//                console.log("searching.. itemMatrixColumn=",itemMatrixColumn," r=",r, "itemMatrix[r][itemMatrixColumn]=",itemMatrix[r][itemMatrixColumn])
                if(itemMatrix[r][itemMatrixColumn] !== undefined && itemMatrix[r][itemMatrixColumn] !== null && itemMatrix[r][itemMatrixColumn] !== screenCurrentItem) {
                    nextFocusedItem = itemMatrix[r][itemMatrixColumn]
                    break
                }
            }
            // 위쪽 영역의 가장 가까운 타일 찾기
//            console.log("1 nextFocusedItem=",nextFocusedItem,"screenCurrentItem=",screenCurrentItem)
            if(nextFocusedItem == screenCurrentItem) {
                minDistance = -1
                for(r = itemMatrixRow - 1; r >= 0; r--) {
                    for(c = 0; c < columnCount; c++) {
//                        console.log("1 위쪽 r=",r," c=", c)
                        if(itemMatrix[r][c] !== undefined && itemMatrix[r][c] !== null && itemMatrix[r][c] !== screenCurrentItem) {
                            nextDistance = Math.abs(itemMatrixColumn - c) + Math.abs(itemMatrixRow - r)
//                            console.log("2 위쪽 r=",r," c=", c, "nextDistance=",nextDistance, "minDistance=",minDistance,"itemMatrix[r][c]=",itemMatrix[r][c])
                            if(minDistance === -1) {
                                minDistance = nextDistance
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                            else if(nextDistance < minDistance) {
                                minDistance = Math.min(minDistance, nextDistance)
                                nextFocusedItem = itemMatrix[r][c]
//                                console.log("nextFocusedItem=",nextFocusedItem)
                            }
                        }
                    }
                    if(nextFocusedItem != screenCurrentItem) {
                        break
                    }
                }
            }
        }

        if(nextFocusedItem != null) {
//            console.log("nextFocusedItem=",nextFocusedItem,"nextFocusedItem.itemType=",nextFocusedItem.itemType,"nextFocusedItem.column=",nextFocusedItem.column, "nextFocusedItem.row=",nextFocusedItem.row)
            screenCurrentItem = nextFocusedItem
            GridManager.selectStatus.setSingleFocusedItem(nextFocusedItem) // PTZ 방향(4가지)에 따라 focusedItem 갱신
        }
    }

    // focusedItem을 외부에서 변경하기 위한 함수
    function changeFocusedItem(deviceId, channelId) {
        if(focusedItem && focusedItem.isCamera
                && focusedItem.mediaParam.deviceId === deviceId && focusedItem.mediaParam.channelId === channelId) {
            return  // 이미 같은 채널이 선택 된 경우 리턴
        }

        for (var i = 0 ; i < gridItems.length ; i++) {
            var item = gridItems[i];
            if(!item || !item.isCamera)
                continue

            if (item.mediaParam.deviceId === deviceId && item.mediaParam.channelId === channelId) {
                GridManager.selectStatus.setSingleFocusedItem(item) // item을 찾아 Focus 이동
                return
            }
        }
    }

    function closeAllPtzControlPopup()
    {
        ptzPresetWindow.visible = false
        ptzSwingWindow.visible = false
        ptzGroupWindow.visible = false
        ptzTourWindow.visible = false
        ptzTraceWindow.visible = false
    }

    /////////////////////////////////////////////////////////////
    // visible false 상태일 때 sleep mode 동작
    /////////////////////////////////////////////////////////////
    onVisibleChanged: {
        console.log("ViewingGrid::onVisibleChanged()", visible);
        if (sleepModeDuration == 0) {
            return;
        }

        if (visible) {
            sleepModeTimer.stopTimer();
        }
        else {
            if (ptzPresetWindow.opened) {
                ptzPresetWindow.close()
            }

            sleepModeTimer.startTimer();
        }
    }

    Timer {
        id: sleepModeTimer
        interval: viewingGridRoot.sleepModeDuration
        running: false
        repeat: false
        onTriggered: {
            GridManager.sleepMode(true);
        }
        function startTimer() {
            restart();
        }
        function stopTimer() {
            stop();
            GridManager.sleepMode(false);
        }
    }

    /////////////////////////////////////////////////////////////
    // 모델 데이터
    /////////////////////////////////////////////////////////////
    // 레이아웃 모델기본 값
    MediaLayoutViewModel {
        id: layoutModel
        cellRatioW:160
        cellRatioH:90
        locked:false
        viewingGridViewModel: true
        isMaskingMode: viewingGridRoot.isMaskingMode

        onLayoutModelUpdated: {
            console.log("ViewingGrid::onLayoutModelUpdated()", newModel);
            GridManager.updateLayout(newModel);
        }
        onDeviceRemoved: {
            console.log("ViewingGrid::onDeviceRemoved()", deviceIDs);
            GridManager.checkRemovedDevice(deviceIDs)
        }
        onCameraRemoved: {
            console.log("ViewingGrid::onCameraRemoved()", cameraIDs);
            GridManager.checkRemovedCamera(cameraIDs)
        }
        onWebpageSaved: {
            console.log("ViewingGrid::onWebpageSaved()", webpageId);
            GridManager.checkWebpageSaved(webpageId);
        }
        onIsMaskingModeChanged: {
            if(isMaskingMode) {
                // fix fill mode to stretch mode in masking mode
                layoutFillMode = MediaLayoutItemViewModel.Stretch
                viewingGridRoot.layoutPropertyUpdated(layoutModel);
            }
        }
    }

    // 레이아웃 모델기본 값
    GridItemBase {
        id: nullItem
        visible: false
        objectName: "nullItem"
    }


    /////////////////////////////////////////////////////////////
    // 백그라운드
    /////////////////////////////////////////////////////////////
    Rectangle {
        id: viewingGridBackground
        anchors.fill: parent
        color: WisenetGui.contrast_11_bg
    }

    Image {
        id: backgroundLogo_16_9
        source: logoManager.useCustomLogo ? "qrc:/images/blank_16_9.svg" : "qrc:/images/logo_16_9.svg"
        visible: false
        width: viewGridLines.cellWidth
        height: viewGridLines.cellHeight
        mipmap: true
        //sourceSize: "7680x4320"
        sourceSize: "1920x1080"
    }
    Image {
        id: backgroundLogo_4_3
        source: logoManager.useCustomLogo ? "qrc:/images/blank_4_3.svg" : "qrc:/images/logo_4_3.svg"
        visible: false
        width: viewGridLines.cellWidth
        height: viewGridLines.cellHeight
        mipmap: true
        //sourceSize: "7200x5400"
        sourceSize: "1800x1350"
    }

    /////////////////////////////////////////////////////////////
    // 그리드 라인
    /////////////////////////////////////////////////////////////
    ShaderEffect {
        id: viewGridLines
        anchors.fill: parent
        visible: gridAlpha > 0.01
        property variant bgLogo: layoutModel.cellRatioH===90 ? backgroundLogo_16_9 : backgroundLogo_4_3


        property real   gridX : 0           // 그리드 left/top X좌표
        property real   gridY : 0           // 그리드 left/top Y좌표
        property real   gridScale : 1.0     // 줌 스케일

        // 16:9==>160:90, 4:3==>160:120
        property real   cellWidth : layoutModel.cellRatioW * gridScale // 셀의 실제 픽셀 Width
        property real   cellHeight : layoutModel.cellRatioH * gridScale //셀의 실제 픽셀 Height
        property real   gridAlpha: 0.0

        property bool   bShowGrid: true
        property real   showGridAnimationDuration: 400 // 그리드 라인 show/hide 애니메이션 시간

        property real   minCellSize : 16
        property real   maxCellSize : 40960

        //onGridVisibleChanged: console.log('grid visible:', gridVisible, gridViewArea.autoFit, bShowGrid);
        //onBShowGridChanged: console.log('show grid', bShowGrid);
        //onGridScaleChanged: console.log("grid scale changed.", gridScale);
        function showGrid(show)
        {
            if (bShowGrid !== show)
                bShowGrid = show;
        }

        states: [
            State {
                when: !viewGridLines.bShowGrid
                PropertyChanges {
                    target: viewGridLines
                    gridAlpha: 0.0
                }
            },
            State {
                when: viewGridLines.bShowGrid
                PropertyChanges {
                    target: viewGridLines
                    gridAlpha: 1.0
                }
            }
        ]
        transitions: [
            Transition {
                NumberAnimation{
                    target: viewGridLines;
                    properties:"gridAlpha";
                    duration:viewGridLines.showGridAnimationDuration
                }
            }
        ]

        fragmentShader: "qrc:shaders/gridline.frag"
    }
    /////////////////////////////////////////////////////////////
    // 마우스 이벤트 처리
    /////////////////////////////////////////////////////////////
//    PinchArea {
//        anchors.fill: parent
//    }

    MouseArea {
        id: gridMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        cursorShape: Qt.ArrowCursor
        focus:true          // 키보드 입력 이벤트를 위해서 true
        hoverEnabled: true  // 마우스 오버 이벤트 처리를 위해서 true

        readonly property real contextMenuOpacity : 0.90

        // 마우스 좌클릭 드래그(아이템이동)에 대한 감도조정
        Timer {
            id: itemPosChangeTimer
            interval: 200
            running: false
            repeat: false
        }

        // 마우스 우클릭 드래그(패닝)에 대한 감도조정
        Timer {
            id: gridPanningChangeTimer
            interval: 200
            running: false
            repeat: false
        }

        // 마우스 움직임이 있는지 체크 타이머
        Timer {
            id: mouseMovementCheckTimer
            interval: 5000
            repeat: false
            running:false
            onTriggered: {
                if(focusedItem && focusedItem.isVideoItem && (focusedItem.isSmartSearchDrawing || focusedItem.isMaskDrawing))
                    return  // SmartSearchDrawing 또는 MaskDrawing 중이면 UI 숨기지 않음

                if(gridMouseArea.pressed)
                    return  // Mouse pressed 상태면 리턴

                var isShow = gridItems.length > 0 ? false : true;
                GridManager.showGuide(isShow);
                GridManager.resetMouseOverItem();
            }
        }

        function refreshShowGuide()
        {
            // 그리드 가이드라인 show 타이머 다시 시작
            if (gridItems.length > 0) {
                mouseMovementCheckTimer.restart();
            }

            if (!viewGridLines.bShowGrid) {
                GridManager.showGuide(true);
            }
        }

        // 마우스 이동
        onPositionChanged: {
            // 뷰영역 밖의 마우스 움직임에 대해서는 무시
            if (mouse.buttons == Qt.NoButton) {
                if (!GridManager._isMouseIn(mouse.x, mouse.y, gridViewArea)) {
                    return;
                }
            }

            refreshShowGuide();

            // 마우스 우다운 + 드래그 ==> 그리드 팬/틸트
            if (GridManager.panStatus.ready) {
                GridManager.panStatus.update(mouse.x, mouse.y);
                return;
            }

            // 마우스 버튼 down없이 오버시 리사이징 활성화 체크
            if (mouse.buttons == Qt.NoButton) {
                GridManager.checkMouseOverItem(mouse.x, mouse.y);
                return;
            }

            // 마우스 좌다운 + 드래그
            if (mouse.buttons == Qt.LeftButton) {
                //console.log("OnPositionChanged check left down");
                // 리사이즈 상태 확인 후 시작
                if(GridManager.resizingStatus.ready) {
                    GridManager.resizingStatus.update(mouse.x, mouse.y);
                    return;
                }

                // 마우스 아이템 이동
                if (GridManager.itemMoveStatus.ready) {
                    GridManager.itemMoveStatus.update(mouse.x, mouse.y);
                    return;
                }

                // 아이템 선택 상태 확인 후 시작
                if (GridManager.selectStatus.ready) {
                    GridManager.selectStatus.update(mouse.x, mouse.y);
                    return;
                }
            }
        }

        onPressed: {
            if (!GridManager._isMouseIn(mouse.x, mouse.y, gridViewArea)) {
                return;
            }

            // 마스킹 모드에서 Drag 동작 안하도록 함.
            if(isMaskingMode)
                return;

            refreshShowGuide()

            if (!focus) {
                focus = true;
            }
            // 마우스 좌다운
            var mousePos = Qt.point(mouse.x, mouse.y);
            if (mouse.button == Qt.LeftButton) {
                // resizing 상태 확인 후 시작 (최우선 순위여야 한다.)
                if (GridManager.resizingStatus.ready) {
                    GridManager.resizingStatus.start(mousePos);
                    return;
                }

                // 마우스 좌표내 아이템이 있는 지 확인한다.
                var item = GridManager.getMouseOverItem(mousePos.x, mousePos.y);
                if (item) {
                    // 선택영역안에서 클릭한 경우 이동 준비를 먼저 한다.
                    if (GridManager.itemMoveStatus.check(item, mousePos)) {
                        return;
                    }
                    // 포커스 아이템 확인
                    if (GridManager.checkSingleFocusItem(item, mousePos)) {
                        return;
                    }
                }

                // 아이템 외 영역 좌표인 경우 선택영역 그리기 준비
                GridManager.selectStatus.start(mousePos);
                return;
            }

            // 마우스 우다운 ==> 그리드 팬/틸트 준비
            if (mouse.button == Qt.RightButton) {
                GridManager.panStatus.start(mousePos);
            }
        }

        onPressAndHold: {
            console.log("ViewingGrid::onPressAndHold()", mouse.button);
            if (!GridManager._isMouseIn(mouse.x, mouse.y, gridViewArea)) {
                return;
            }

            if (mouse.button == Qt.RightButton) {
                GridManager.panStatus.updatePanning();
            }
        }

        onReleased: {
            refreshShowGuide();

            if (!focus) {
                focus = true;
            }
            if (mouse.button == Qt.RightButton) {
                if (!GridManager.panStatus.panning) {
                    //console.log("ViewingGrid::mouse right click!! show context menu,", selectedItems.length);
                    // 컨텍스트 메뉴 표시
                    if (!GridManager._isMouseIn(mouse.x, mouse.y, gridViewArea)) {
                        return;
                    }
                    GridManager.showContextMenu();
                }
            }
            //console.log("ViewingGrid::mouse released");
            GridManager.mouseReleased(mouse.x, mouse.y);

        }
        onClicked: {
            // 마우스 좌클릭 : 시나리오 없음.
            // 마우스 우클릭 : 컨텍스트 메뉴, 마우스 패닝이랑 구별을 위해서 onReleased에서 처리
//            console.log("screenCurrentItem=",screenCurrentItem," focusedItem=",focusedItem)
            screenCurrentItem = focusedItem // spc2000 조작시, screen mode에서 타일 이동에 사용될 아이템 갱신
//            console.log("screenCurrentItem.itemType=", screenCurrentItem.itemType, "screenCurrentItem.columnCount=",screenCurrentItem.columnCount,"screenCurrentItem.rowCount",screenCurrentItem.rowCount)
//            console.log("gridItems.length=",gridItems.length)
        }
        onDoubleClicked: {
            if (!GridManager._isMouseIn(mouse.x, mouse.y, gridViewArea)) {
                return;
            }

            // 마우스 좌더블클릭
            if (mouse.button == Qt.LeftButton) {
                console.log("ViewingGrid::mouse double click!!");
                var item = GridManager.getMouseOverItem(mouse.x, mouse.y);
                if (item) {
                    GridManager.toggleItemFullScreen(item);
                }
            }
        }

        onWheel: {
            if (!GridManager._isMouseIn(wheel.x, wheel.y, gridViewArea)) {
                //console.log("ViewingGrid::skip this mouse wheel event");
                return;
            }

            // 마스킹 모드에서 Zoom in/out 동작 안하도록 함.
            if(isMaskingMode)
                return;

            refreshShowGuide();

            /* support for high resolution track pad */
            if (wheel.modifiers & Qt.ShiftModifier) { // shift key + trackpad movement
                if (wheel.pixelDelta.x !== 0 || wheel.pixelDelta.y != 0) {
                    //var dx = wheel.angleDelta.x * 0.33;
                    //var dy = wheel.angleDelta.y * 0.33;
                    viewGridLines.gridX += wheel.pixelDelta.x;
                    viewGridLines.gridY += wheel.pixelDelta.y;
                    gridViewArea.autoFit = false;
                    return;
                }
            }


            if (wheel.angleDelta.y === 0)
                return;


            // 마우스 휠 ==> 줌 인/아웃
            var isRegularWheel = (wheel.pixelDelta.y === 0 || wheel.angleDelta.y === 120 || wheel.angleDelta.y === -120);
            var deltaY = (wheel.angleDelta.y / 120) * 0.15;
            var scaleDelta = 1.0 + deltaY;
            //console.log("here..", scaleDelta, isRegularWheel, deltaY, wheel.angleDelta.y)

            GridManager.zoom(wheel.x, wheel.y, scaleDelta, isRegularWheel);
        }

        Keys.onShortcutOverride: event.accepted = (event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace)
        Keys.onPressed: {
            switch (event.key) {
            case Qt.Key_Delete:
                console.log("ViewingGrid::delete key pressed");
                if (!layoutWarning.checkLocked()) {
                    GridManager.removeSelectedItems();
                }

                break;
            case Qt.Key_Backspace:
                console.log("ViewingGrid::back space key pressed");
                if (!layoutWarning.checkLocked()) {
                    GridManager.removeSelectedItems();
                }
                break;
            case Qt.Key_Escape:
                console.log("ViewingGrid::esc key pressed");
                if (viewingGridRoot.isFullScreen) {
                    viewingGridRoot.showFullscreen(false);
                }
            }
        }


        /////////////////////////////////////////////////////////////
        // 컨텍스트 메뉴
        /////////////////////////////////////////////////////////////
        ContextMenu_Layout {
            id: layoutContextMenu
            opacity: gridMouseArea.contextMenuOpacity
            layoutLocked: layoutModel.locked
            layoutShared: viewingGridRoot.isShared
            isFullScreen: viewingGridRoot.isFullScreen
            isStandAlone: viewingGridRoot.standalone
            isMediaFileOnly: viewingGridRoot.isMediaFileOnly
            isMaskingMode: viewingGridRoot.isMaskingMode

            onSaveTriggered: {
                viewingGridRoot.saveLayout();
            }
            onSaveAsTriggered: {
                viewingGridRoot.saveAsDialog();
            }
            onOpenMediaFileTriggered: {
                viewingGridRoot.openMediaDialog();
            }
            onFullScreenTriggered: {
                viewingGridRoot.showFullscreen(isFullScreen);
            }
            onAutoFitTriggered: {
                gridViewArea.autoFit = true;
                GridManager.fitAndMediaCheck();
            }

            onCellRatioUpdateTriggered: {
                console.log("ViewingGrid::onCellRatioTriggered()", w, h);
                layoutModel.cellRatioW = w;
                layoutModel.cellRatioH = h;
                if (gridViewArea.autoFit)
                    GridManager.fitAndMediaCheck();

                viewingGridRoot.layoutPropertyUpdated(layoutModel);
            }
            onFillModeUpdateTriggered: {
                layoutModel.layoutFillMode = fillMode;
                viewingGridRoot.layoutPropertyUpdated(layoutModel);
            }
            onProfileUpdateTriggered: {
                layoutModel.layoutProfile = profile;
                viewingGridRoot.layoutPropertyUpdated(layoutModel);
                GridManager.checkMediaStatus();
            }
            onVideoStatusTriggered: {
                layoutModel.videoStatus = isOn;
            }
            onLockTriggered: {
                layoutModel.locked = locked;
                layoutModel.saveToCoreService();
            }

        }

        ContextMenu_Video {
            id: videoItemContextMenu
            opacity: gridMouseArea.contextMenuOpacity
            layoutLocked: !viewingGridRoot.isEditable
            isFullScreen: viewingGridRoot.isFullScreen
            isStandAlone: viewingGridRoot.standalone
            isUserAdmin: viewingGridRoot.isUserAdmin
            isMediaFileOnly : viewingGridRoot.isMediaFileOnly

            onFullScreenTriggered: {
                viewingGridRoot.showFullscreen(isFullScreen);
            }

            onEditNameTriggered: {
                viewingGridRoot.editNameTriggered(videoItem);
            }

            onOpenInNewTabTriggered: {
                var sourceItems = [videoItem];
                var filtered = GridManager.getNewLayoutItemList(sourceItems);
                var items = [];
                for (var i = 0 ; i < filtered.length ; i++) {
                    items.push(filtered[i].itemModel);
                }
                viewingGridRoot.openInNewTabTriggered(items);
            }
            onOpenInNewWindowTriggered: {
                var sourceItems = [videoItem];
                var filtered = GridManager.getNewLayoutItemList(sourceItems);
                var items = [];
                for (var i = 0 ; i < filtered.length ; i++) {
                    items.push(filtered[i].itemModel);
                }
                viewingGridRoot.openInNewWindowTriggered(items);
            }
            onDeleteTriggered: {
                GridManager.removeOneItem(videoItem);
            }
            onSetRotationTriggered: {
                videoItem.rotate(rotation);

                // zoom target이 있는 경우 줌타겟도 모두 회전 업데이트한다.
                for (var i = 0 ; i < videoItem.zoomAreas.length ; i++) {
                    var zoomTargetItem = videoItem.zoomAreas[i].zoomTargetItem;
                    if (zoomTargetItem) {
                        zoomTargetItem.rotate(rotation);
                    }
                }
                var updateItems = [videoItem.itemModel]
                itemsUpdated(updateItems)
            }
            onFillModeUpdateTriggered: {
                videoItem.itemModel.itemFillMode = fillMode;
                var updateItems = [videoItem.itemModel]
                itemsUpdated(updateItems)
            }
            onProfileUpdateTriggered: {
                videoItem.itemModel.itemProfile = profile;
                GridManager.checkProfileOfChannelItem(videoItem);
                var updateItems = [videoItem.itemModel]
                itemsUpdated(updateItems)
            }

            onPtzPresetOpen: {
                viewingGridRoot.onPtzPresetTriggered(videoItem);
            }
            onBncSettingsOpen: {
                videoItem.openBncControl();
            }
            onCameraFocusSettingsOpen: {
                videoItem.openFocusControl();
            }

            onFisheyeSetupOpen: {
                viewingGridRoot.onFileFisheyeSetupTriggered(videoItem);
            }
            onVideoStatusTriggered: {
                videoItem.itemModel.videoStatus = isOn;
            }
            onCameraSetupOpen: {
                var items = [videoItem.itemModel];
                viewingGridRoot.cameraSetupOpenTriggered(items);
            }
            onNvrSetupOpen: {
                console.log("ViewingGrid onNvrSetupOpen")
                var items = [videoItem.itemModel];
                viewingGridRoot.nvrSetupOpenTriggered(items);
            }
            onEventRuleSetupOpen: {
                var items = [videoItem.itemModel];
                viewingGridRoot.eventRulesSetupOpenTriggered(items);
            }
        }

        ContextMenu_Image {
            id: imageItemContextMenu
            opacity: gridMouseArea.contextMenuOpacity
            layoutLocked: !viewingGridRoot.isEditable
            isFullScreen: viewingGridRoot.isFullScreen
            isStandAlone: viewingGridRoot.standalone
            isMediaFileOnly: viewingGridRoot.isMediaFileOnly
            onFullScreenTriggered: {
                viewingGridRoot.showFullscreen(isFullScreen);
            }
            onSetBackgroundTriggered: {
                GridManager.setBackgroundImageItem(imageItem, setOn);
            }
            onOpenInNewTabTriggered: {
                var items = [imageItem.itemModel];
                viewingGridRoot.openInNewTabTriggered(items);
            }
            onOpenInNewWindowTriggered: {
                var items = [imageItem.itemModel];
                viewingGridRoot.openInNewWindowTriggered(items);
            }
            onDeleteTriggered: {
                GridManager.removeOneItem(imageItem);
            }
        }

        ContextMenu_Webpage {
            id: webItemContextMenu
            opacity: gridMouseArea.contextMenuOpacity
            layoutLocked: !viewingGridRoot.isEditable
            isFullScreen: viewingGridRoot.isFullScreen
            isStandAlone: viewingGridRoot.standalone
            onFullScreenTriggered: {
                viewingGridRoot.showFullscreen(isFullScreen);
            }
            onEditNameTriggered: {
                viewingGridRoot.editNameTriggered(webpageItem);
            }
            onOpenInNewTabTriggered: {
                var items = [webpageItem.itemModel];
                viewingGridRoot.openInNewTabTriggered(items);
            }
            onOpenInNewWindowTriggered: {
                var items = [webpageItem.itemModel];
                viewingGridRoot.openInNewWindowTriggered(items);
            }
            onDeleteTriggered: {
                GridManager.removeOneItem(webpageItem);
            }
            onBackTriggered: {
                webpageItem.webView.goBack();
            }
            onReloadTriggered: {
                webpageItem.webView.reload();
            }
            onForwardTriggered: {
                webpageItem.webView.goForward();
            }
            onFixedSizeTriggered: {
                console.log("zoomFactorChange", webpageItem.webView.zoomFactor, zoomFactor);
                webpageItem.webView.zoomFactor = zoomFactor;
                webpageItem.webView.zoomFactor = zoomFactor;
            }
            onShowInBrowserTriggered: {
                console.log("Show in Browser", webpageItem.webUrl);
                Qt.openUrlExternally(webpageItem.webUrl);
            }

            onSettingTriggered: viewingGridRoot.webpageSettingTriggered(webpageId)
        }

        ContextMenu_MultipleItems {
            id: selectedItemsContextMenu
            opacity: gridMouseArea.contextMenuOpacity
            layoutLocked: !viewingGridRoot.isEditable
            isStandAlone: viewingGridRoot.standalone
            isUserAdmin: viewingGridRoot.isUserAdmin
            isMediaFileOnly: viewingGridRoot.isMediaFileOnly

            onOpenInNewTabTriggered: {
                var filtered = GridManager.getNewLayoutItemList(mitems);
                var items = [];
                for (var i = 0 ; i < filtered.length ; i++) {
                    items.push(filtered[i].itemModel);
                }
                viewingGridRoot.openInNewTabTriggered(items);
            }
            onOpenInNewWindowTriggered: {
                var filtered = GridManager.getNewLayoutItemList(mitems);
                var items = [];
                for (var i = 0 ; i < filtered.length ; i++) {
                    items.push(filtered[i].itemModel);
                }
                viewingGridRoot.openInNewWindowTriggered(items);
            }
            onDeleteTriggered: {
                GridManager.removeSelectedItems();
            }
            onFillModeUpdateTriggered: {
                var updatedItems = [];
                for (var i = 0 ; i < mitems.length ; i++) {
                    var item = mitems[i];
                    if (item.isCamera || item.isLocalResource || item.isZoomTarget) {
                        if (item.itemModel.itemFillMode !== fillMode) {
                            item.itemModel.itemFillMode = fillMode;
                            updatedItems.push(item.itemModel);
                        }
                    }
                }
                if (updatedItems.length > 0) {
                    itemsUpdated(updatedItems);
                }
            }
            onProfileUpdateTriggered: {
                var updatedItems = [];
                for (var i = 0 ; i < mitems.length ; i++) {
                    var item = mitems[i];
                    if (item.isCamera) {
                        if (item.itemModel.itemProfile !== profile) {
                            item.itemModel.itemProfile = profile;
                            GridManager.checkProfileOfChannelItem(item);
                            updatedItems.push(item.itemModel);
                        }
                    }
                }
                if (updatedItems.length > 0) {
                    itemsUpdated(updatedItems);
                }
            }
            onCameraSetupOpen: {
                var items = [];
                for (var i = 0 ; i < mitems.length ; i++) {
                    var item = mitems[i];
                    if (item.isCamera) {
                        items.push(item.itemModel);
                    }
                    else if (item.isZoomTarget && item.zoomTargetOfCamera) {
                        items.push(item.itemModel);
                    }
                }
                if (items.length > 0)
                    viewingGridRoot.cameraSetupOpenTriggered(items);
            }
            onNvrSetupOpen: {
                console.log("ViewingGrid.qml onNvrSetupOpen()2")
                var items = [];
                for (var i = 0 ; i < mitems.length ; i++) {
                    var item = mitems[i];
                    if (item.isCamera) {
                        items.push(item.itemModel);
                    }
                    else if (item.isZoomTarget && item.zoomTargetOfCamera) {
                        items.push(item.itemModel);
                    }
                }
                if (items.length > 0)
                    viewingGridRoot.nvrSetupOpenTriggered(items);
            }
            onEventRuleSetupOpen: {
                var items = [];
                for (var i = 0 ; i < mitems.length ; i++) {
                    var item = mitems[i];
                    if (item.isCamera) {
                        items.push(item.itemModel);
                    }
                    else if (item.isZoomTarget && item.zoomTargetOfCamera) {
                        items.push(item.itemModel);
                    }
                }
                if (items.length > 0)
                    viewingGridRoot.eventRulesSetupOpenTriggered(items);
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // 오브젝트가 추가되는 영역
    /////////////////////////////////////////////////////////////
    Item {
        id: gridItemsArea
        anchors.fill: parent
        property int  itemCount: 0

        function onItemClose(item)
        {
            if (!layoutWarning.checkLocked()) {
                GridManager.removeOneItem(item);
            }
        }
        function onItemMouseMoved()
        {
            //console.log("ViewingGrid::onItemMouseMoved() update hide time!");
            gridMouseArea.refreshShowGuide();
        }

        function onCreateZoomTarget(item, normalRect, rectColor)
        {
            GridManager.createZoomTarget(item, normalRect, rectColor);
        }
        function onOpenZoomTargetAnimationFinished()
        {
            GridManager.fitAndMediaCheck();
        }
    }


    /////////////////////////////////////////////////////////////
    // 아이템 뷰 영역
    /////////////////////////////////////////////////////////////
    Rectangle {
        id: gridViewArea
        visible: false
        property bool autoFit: true
        x: viewGridLines.x
        y: viewGridLines.y
        width: viewGridLines.width
        height: viewGridLines.height

        property real autoFitPadding: viewingGridRoot.isFullScreen ? 1 : 8
        property real autoFitPadding2x: autoFitPadding*2

        onWidthChanged: {
            fitInViewTimer.restart();
        }
        onHeightChanged: {
            fitInViewTimer.restart();
        }
        Timer {
            id: fitInViewTimer
            interval: 150
            onTriggered: {
                //console.log("ViewingGrid::gridViewArea size changed", x, y, width, height, viewingGridRoot.width, viewingGridRoot.height);
                GridManager.fitAndMediaCheck();
            }
        }
    }

    // 마우스 select 가이드 사각형
    Rectangle {
        id: selectTargetRect
        visible: false
        width: 0;height: 0; x: 0; y: 0;
        color: WisenetGui.color_primary_opacity50
        border {color: WisenetGui.color_primary; width: 1}
    }
    // 마우스 드랍영역 하이라이트 사각형
    Rectangle {
        id: dropTargetRect
        visible: false
        width: 0;height: 0; x: 0; y: 0;
        color: WisenetGui.color_secondary_bright
        opacity: 0.3

        function draw(column, row)
        {
            visible = true;
            x = (column*viewGridLines.cellWidth) + viewGridLines.gridX
            y = (row*viewGridLines.cellHeight) + viewGridLines.gridY
            width = viewGridLines.cellWidth
            height = viewGridLines.cellHeight
        }

        function reset()
        {
            visible = false;
            width = 0; height = 0; x = 0; y = 0;
        }
    }

    // Sleep mode 표시 사각형
    /* 사용하지 않음
    SleepModePanel  {
        id: sleepModeStatusRect
        anchors.fill: gridViewArea
    }
    */

    /////////////////////////////////////////////////////////////
    // 패턴뷰시 패턴사용영역에 대해서 하이라이트 표시
    /////////////////////////////////////////////////////////////
    // TODO :: 현재는 deprecated
    Rectangle {
        id:patternIndicateRect
        visible: false
        opacity: 0.0
        width: 0;height: 0; x: 0; y: 0; z: 3;
        color: "#80FF6C00"
        border {color: "#FF6C00"; width: 1}

        property int column:0
        property int row:0
        property int columnCount:0
        property int rowCount:0
        function reset()
        {
            width = 0;
            height = 0;
            x = 0;
            y = 0;
            visible = false;
            opacity = 0.0
        }
    }

    SequentialAnimation  {
        id:patternAnimation
        property int column:0
        property int columnCount:0
        property int row:0
        property int rowCount:0

        function onGridAnimationFinished()
        {
            patternIndicateRect.x = (patternIndicateRect.column * viewGridLines.cellWidth)+viewGridLines.gridX;
            patternIndicateRect.y = (patternIndicateRect.row * viewGridLines.cellHeight)+viewGridLines.gridY;
            patternIndicateRect.width =  (patternIndicateRect.columnCount * viewGridLines.cellWidth);
            patternIndicateRect.height = (patternIndicateRect.rowCount * viewGridLines.cellHeight);
            patternIndicateRect.visible = true;
            patternIndicateRect.opacity = 0.0;

            console.log("ViewingGrid::onGridAnimationFinished()");
            restart();
        }

        function readyIndicate(vpos)
        {
            if (running) {
                stop();
                patternIndicateRect.reset();
            }

            gridAnimation.finished.connect(patternAnimation.onGridAnimationFinished);
            patternIndicateRect.column = vpos.left;
            patternIndicateRect.columnCount = vpos.right-vpos.left;
            patternIndicateRect.row = vpos.top;
            patternIndicateRect.rowCount = vpos.bottom-vpos.top;
        }

        SequentialAnimation {
            loops:2
            NumberAnimation {
                target: patternIndicateRect; property: "opacity"; to: 1.0; duration:500
            }
            NumberAnimation {
                target: patternIndicateRect; property: "opacity"; to: 0; duration:500
            }
        }

        onStopped: {
            console.log('ViewingGrid::patternAnimation stopped\n');
            patternIndicateRect.reset();
            gridAnimation.finished.disconnect(patternAnimation.onGridAnimationFinished);
        }
    }

    /////////////////////////////////////////////////////////////
    // Item Model property chagned
    /////////////////////////////////////////////////////////////
    function onItemModelPropertyChanged(item)
    {
        //console.log("ViewingGrid::onItemModelPropertyChanged()", layoutModel.locked, layoutModel.saveStatus )
        if (!layoutModel.locked) {
            var updateItems = [item.itemModel]
            itemsUpdated(updateItems)
        }
    }

    /////////////////////////////////////////////////////////////
    // PTZ Preset Popup Dialog
    /////////////////////////////////////////////////////////////
    function onPtzPresetTriggered(item)
    {
        console.log("viewingGrid - onPtzPresetTriggered")
        closeAllPtzControlPopup()
        ptzPresetWindow.showDialog(item);
    }
    function onPtzPresetListUpdated(item)
    {
        if (ptzPresetWindow.visible && ptzPresetWindow.presetItem === item) {
            ptzPresetWindow.updatePresetList();
        }

        if (ptzSwingWindow.visible) {

            if(ptzSwingWindow.swingItem === item) {
                ptzSwingWindow.updateSwingName()
            }

            if(ptzGroupWindow.groupItem === item) {
                ptzGroupWindow.updateGroupName()
            }
        }
    }
    function onPtzSwingTriggered(item)
    {
        console.log("viewingGrid - onPtzSwingTriggered")
        closeAllPtzControlPopup()
        ptzSwingWindow.showDialog(item);
    }
    function onPtzSwingPresetUpdated(item)
    {
        console.log("viewingGrid - onPtzSwingPresetUpdated")
        if (ptzSwingWindow.visible && ptzSwingWindow.swingItem === item) {
            ptzSwingWindow.updateSwingPresets();
        }
    }

    function onPtzGroupTriggered(item)
    {
        console.log("viewingGrid - onPtzGroupTriggered")
        closeAllPtzControlPopup()
        ptzGroupWindow.showDialog(item);
    }
    function onPtzGroupPresetUpdated(item)
    {
        console.log("viewingGrid - onPtzGroupPresetUpdated")
        if (ptzGroupWindow.visible && ptzGroupWindow.groupItem === item) {
            ptzGroupWindow.updateGroupPresets();
        }
    }
    function onPtzTourTriggered(item)
    {
        console.log("viewingGrid - onPtzTourTriggered")
        closeAllPtzControlPopup()
        ptzTourWindow.showDialog(item);
    }
    function onPtzTourPresetUpdated(item)
    {
        console.log("viewingGrid - onPtzTourPresetUpdated")
        if (ptzTourWindow.visible && ptzTourWindow.tourItem === item) {
            ptzTourWindow.updateTourPresets();
        }
    }
    function onPtzTraceTriggered(item)
    {
        console.log("viewingGrid - onPtzTraceTriggered")
        closeAllPtzControlPopup()
        ptzTraceWindow.showDialog(item);
    }
    function onPtzTracePresetUpdated(item)
    {
        console.log("viewingGrid - onPtzTracePresetUpdated")
        if (ptzTraceWindow.visible && ptzTraceWindow.traceItem === item) {
            ptzTraceWindow.updateTracePresets();
        }
    }
    function onDeviceUsingSetupChanged()
    {
        console.log("viewingGrid - onDeviceUsingSetupChanged")
        layoutWarning.showMessage(WisenetLinguist.systemMenuUsed)
    }

    WisenetMessageDialog{
        id: errorDialog

        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    PtzPresetDialog {
        id: ptzPresetWindow
        opacity: 0.85
    }
    PtzSwingDialog {
        id: ptzSwingWindow
        opacity: 0.85
    }
    PtzGroupDialog {
        id: ptzGroupWindow
        opacity: 0.85
    }
    PtzTourDialog {
        id: ptzTourWindow
        opacity: 0.85
    }
    PtzTraceDialog {
        id: ptzTraceWindow
        opacity: 0.85
    }

    /////////////////////////////////////////////////////////////
    // 로컬파일 Fisheye Setup Dialog
    /////////////////////////////////////////////////////////////
    function onFileFisheyeSetupTriggered(item)
    {
        fileFisheyeSetupWindow.showDialog(item);
    }
    FileFisheyeSetupDialog {
        id: fileFisheyeSetupWindow
        opacity: 0.85
    }

    /////////////////////////////////////////////////////////////
    // AutoFit 버튼
    /////////////////////////////////////////////////////////////
    WisenetMediaButtonItem {
        id: autoFitButton
        opacity: gridViewArea.autoFit ? 0.0 : 1.0
        visible : opacity > 0.0
        anchors {
            top: gridViewArea.top
            right: gridViewArea.right
            margins: 15
        }

        property bool stateVisible: viewGridLines.bShowGrid && !gridViewArea.autoFit

        width: 100
        height: 28
        imgWidth: 100
        imgHeight: 28
        normalSource: "images/autofit_Default.svg"
        hoverSource: "images/autofit_Hover.svg"
        pressSource: "images/autofit_Press.svg"
        normalColor: "transparent"
        pressColor: "transparent"
        hoverColor: "transparent"
        imgSourceSize: "100x28"

        states: [
            State {
                when: autoFitButton.stateVisible
                PropertyChanges {
                    target: autoFitButton
                    opacity: 1.0
                }
            },
            State {
                when: !autoFitButton.stateVisible
                PropertyChanges {
                    target: autoFitButton
                    opacity: 0.0
                }
            }
        ]
        transitions: Transition {
            NumberAnimation {
                target: autoFitButton
                property: "opacity"
                duration: 200
            }
        }
        onMouseClicked: {
            gridViewArea.autoFit = true;
            GridManager.fitAndMediaCheck();
        }
    }

    /////////////////////////////////////////////////////////////
    // 파일 저장 알림 팝업
    /////////////////////////////////////////////////////////////
    FileSaveNotification {
        id: fileSavedPopup
        anchors {
            top: autoFitButton.visible ? autoFitButton.bottom : gridViewArea.top
            right: gridViewArea.right
            margins: 8
        }
        onOpenFolder: {
            layoutModel.openLocalDirPath(fileSavedPopup.fileDir);
        }
    }

    /////////////////////////////////////////////////////////////
    // 레이아웃 잠금 팝업
    /////////////////////////////////////////////////////////////
    NotificationMessage {
        id: layoutWarning
        anchors {
            top: fileSavedPopup.visible ? fileSavedPopup.bottom : (autoFitButton.visible ? autoFitButton.bottom : gridViewArea.top)
            right: gridViewArea.right
            margins: 8
        }
        function checkLocked()
        {
            console.log("ViewingGrid::checkLocked()", layoutModel.loginId, layoutModel.ownerId);
            if (viewingGridRoot.isShared) {
                layoutWarning.showMessage(WisenetLinguist.warning_sharedLayout);
                return true;
            }
            else if (viewingGridRoot.isLocked) {
                //console.log("checkLocked!! alert message");
                layoutWarning.showMessage(WisenetLinguist.warning_layoutLocked);
                return true;
            }
            return false;
        }
        function showMaxVideoLimitWarning()
        {
            layoutWarning.showMessage(WisenetLinguist.warning_maxVideoitemOnLayout);
        }
        function showMaxWebLimitWarning()
        {
            layoutWarning.showMessage(WisenetLinguist.warning_maxWebitemOnLayout);
        }
    }

    /////////////////////////////////////////////////////////////
    // 서명 검증 팝업
    /////////////////////////////////////////////////////////////
    NotificationMessage {
        id: verifyFinishedPopup
        anchors {
            top: fileSavedPopup.visible ? fileSavedPopup.bottom : (autoFitButton.visible ? autoFitButton.bottom : gridViewArea.top)
            right: gridViewArea.right
            margins: 8
        }
    }
    /////////////////////////////////////////////////////////////
    // 토크 이미 사용중 팝업
    /////////////////////////////////////////////////////////////
    NotificationMessage {
        id: talkPopup
        anchors {
            top: fileSavedPopup.visible ? fileSavedPopup.bottom : (autoFitButton.visible ? autoFitButton.bottom : gridViewArea.top)
            right: gridViewArea.right
            margins: 8
        }
    }
    /////////////////////////////////////////////////////////////
    // 트래킹 실패 팝업
    /////////////////////////////////////////////////////////////
    NotificationMessage {
        id: trackingFailedPopup
        anchors {
            top: fileSavedPopup.visible ? fileSavedPopup.bottom : (autoFitButton.visible ? autoFitButton.bottom : gridViewArea.top)
            right: gridViewArea.right
            margins: 8
        }
    }

    // 컨트롤러(SPC-2000, ?) 연동 동작
    Connections {
        target: gControllerManager
        enabled: viewingGridRoot.visible && isFocusedWindow
        function onPtzChanged(controllerMode, panValue, tiltValue, zoomValue){
//            console.log("onPtzChanged - panValue:", panValue, " tiltValue:", tiltValue, "zoomValue:", zoomValue)
            if(focusedItem == null || focusedItem == undefined) {
                return
            }

            switch(controllerMode) {
            case ControllerModel.PtzMode:
                if(focusedItem && focusedItem.hasPlayer && focusedItem.isVideoItem && isFocusedWindow) {
//                    console.log("focusedItem=",focusedItem, this)
                    // fisheye 카메라의 DPTZ
                    if(focusedItem.player.supportFisheyeDewarp) {
//                        console.log("focusedItem.fisheyeViewIndex:",focusedItem.fisheyeViewIndex)
                        focusedItem.player.dewarpPtzContinuous(focusedItem.fisheyeViewIndex, panValue, tiltValue, zoomValue);
                    }
                    // PTZ 카메라의 PTZ
                    else {
//                        console.log("focusedItem is normal PTZ")
                        focusedItem.player.devicePtzContinuous(panValue, tiltValue, zoomValue)
                    }
                }
                break
            case ControllerModel.ScreenMode:
                findNextFocusedItem(panValue, tiltValue, zoomValue)
                break
            default:
                break
            }
        }

        function onButtonChanged(controllerMode, controllerButtonAction){
//            console.log("onButtonChanged() - controllerMode=",controllerMode,"controllerButtonAction=",controllerButtonAction)
            if(focusedItem == null || focusedItem == undefined) {
                return
            }

            switch(controllerMode) {
            case ControllerModel.PtzMode:
                switch(controllerButtonAction) {
                case ControllerSpc2000.DoPreset1:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeViewMode(MediaLayoutItemViewModel.V_Original)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(1)
                        }
                    }
                    break;
                case ControllerSpc2000.DoPreset2:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeViewMode(MediaLayoutItemViewModel.V_Single)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(2)
                        }
                    }
                    break;
                case ControllerSpc2000.DoPreset3:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeViewMode(MediaLayoutItemViewModel.V_Quad)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(3)
                        }
                    }
                    break;
                case ControllerSpc2000.DoPreset4:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeViewMode(MediaLayoutItemViewModel.V_Panorama)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(4)
                        }
                    }
                    break;
                case ControllerSpc2000.DoPreset5:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeQuadViewIndex(1)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(5)
                        }
                    }
                    break;
                case ControllerSpc2000.DoPreset6:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeQuadViewIndex(2)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(6)
                        }
                    }
                    break;
                case ControllerSpc2000.DoPreset7:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeQuadViewIndex(3)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(7)
                        }
                    }
                    break;
                case ControllerSpc2000.DoPreset8:
                    if(focusedItem.player) {
                        if(focusedItem.player.supportFisheyeDewarp) {
                            focusedItem.updateFisheyeQuadViewIndex(4)
                        }
                        else {
                            focusedItem.player.devicePtzPresetMove(8)
                        }
                    }
                    break;
                case ControllerSpc2000.SetScreenMode:
                    gControllerManager.spc2000ControllerMode = ControllerSpc2000.ScreenMode
                    break;
                case ControllerSpc2000.SwitchSingleChannelMode:
                    GridManager.setSingleChannelScreen(focusedItem)
                    break;
                case ControllerSpc2000.SwitchMultiChannelMode:
                    GridManager.setMultiChannelScreen(focusedItem)
                    break;
                default:
                    break;
                }
            case ControllerModel.ScreenMode:
                switch(controllerButtonAction) {
                case ControllerSpc2000.SetPtzMode:
                    gControllerManager.spc2000ControllerMode = ControllerSpc2000.PtzMode
                    break
                case ControllerSpc2000.MoveLeftWindow:
                    if(spc2000ScreenInputCount < 3) {
                        spc2000ScreenInputCount += 1
                    }
                    else {
                        spc2000ScreenInputCount = 0
                        windowHandler.switchWindow(windowGuid, true)
                    }
                    break
                case ControllerSpc2000.MoveRightWindow:
                    if(spc2000ScreenInputCount < 3) {
                        spc2000ScreenInputCount += 1
                    }
                    else {
                        spc2000ScreenInputCount = 0
                        windowHandler.switchWindow(windowGuid, false)
                    }
                    break
                default:
                    break
                }
            default:
                break
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // Fullscreen 알림 메시지
    /////////////////////////////////////////////////////////////
    Rectangle {
        id: fullscreenMessage
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1
        width: fullscreenMessageText.width
        height: fullscreenMessageText.height
        visible: false
        property bool isFullScreen: viewingGridRoot.isFullScreen
        onIsFullScreenChanged: {
            if (isFullScreen) {
                visible = true
                opacity = 1.0
                fullMessageHideTimer.start();
            }
            else {
                fullMessageHideTimer.stop();
                closeAnim.stop();
                visible = false;
                opacity = 1.0;
            }
        }

        anchors {
            top: gridViewArea.top
            topMargin: 10
            horizontalCenter: gridViewArea.horizontalCenter
        }

        Text {
            id: fullscreenMessageText
            anchors.centerIn: parent
            width: implicitWidth+30
            height: implicitHeight+30
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: WisenetGui.contrast_02_light_grey
            text: WisenetLinguist.fullscreenOnMessage + layoutContextMenu.fullscreenShortText
        }
        Timer {
            id: fullMessageHideTimer
            interval: 3000
            repeat: false
            onTriggered: {
                closeAnim.start();
            }
        }
        NumberAnimation{
            id:closeAnim
            target: fullscreenMessage;
            properties:"opacity";
            to:0
            duration:1000
            onStopped: {
                fullscreenMessage.visible = false;
                fullscreenMessage.opacity = 0;
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // 패스워드 입력 팝업 (TODO design)
    /////////////////////////////////////////////////////////////
    PasswordInputWindow {
        id: passwordInoutWindow
        onAccepted: {
            for (var i = 0 ; i < gridItems.length ; i++) {
                var item = gridItems[i];
                if(item.hasPlayer && item.player.status === WisenetMediaPlayer.Failed_PasswordRequired)
                    item.player.checkPassword(password)
            }
        }
    }

    onMediaOpenResult: {
        if (error === WisenetMediaPlayer.PasswordRequired) {
            passwordInoutWindow.password = ""
            passwordInoutWindow.showDialog()
        }
        else if (error === WisenetMediaPlayer.NoError){
            if(isLinkedMode && isMediaFileOnly && item.isLocalResource) {
                // MediaFile Link 모드인 경우, 신규 Open 된 Item Link
                var targetItem = null
                for (var i = 0 ; i < gridItems.length ; i++) {
                    if(gridItems[i] && item !== gridItems[i] && gridItems[i].isLocalResource) {
                        targetItem = gridItems[i]  // 먼저 나오는 LocalResource
                        if(targetItem.player.supportTimeline === item.player.supportTimeline)
                            break   // targetItem과 재생시간까지 Link 가능 한 경우 break
                    }
                }

                GridManager.linkMediaFile(item, targetItem)
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // 다른 이름으로 저장 팝업
    /////////////////////////////////////////////////////////////
    LayoutSaveAsMediaDialog {
        id: layoutSaveAsDialog
        property bool isSaveAs: true
        title: isSaveAs ? WisenetLinguist.saveLayoutAs : WisenetLinguist.saveLayout
        onAccepted: {
            console.log("ViewingGrid::SaveAsDialog onAccepted()", layoutSaveAsDialog.text);

            if(layoutSaveAsDialog.text.trim() === ""){
                errorDialog.message = WisenetLinguist.enterNameMessage
                errorDialog.visible = true
                return;
            }

            if (layoutSaveAsDialog.text.length > 0) {
                if (!isSaveAs) {
                    layoutModel.name = layoutSaveAsDialog.text;
                    layoutModel.saveToCoreService();
                }
                else {
                    layoutModel.saveAsToCoreService(layoutSaveAsDialog.text);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // 미디어파일 열기 팝업
    /////////////////////////////////////////////////////////////
    FileDialog {
        id: mediaFileOpenDialog
        title: WisenetLinguist.openMediaFiles
        nameFilters: isMaskingMode ? ["Wisenet media file(*.wnm)"] : ["Video files(*.mkv *.mp4 *.avi *.wnm *.sec)", "Image files(*.png *.jpg)"]
        // todo : 경로 확인 필요
        folder:shortcuts.home
        selectMultiple : true
        onAccepted: {
            console.log("ViewingGrid::mediaFileOpenDialog urls:", mediaFileOpenDialog.fileUrls);
            mediaFileOpenDialog.close();
            var validList = [];
            for (var i = 0 ; i < mediaFileOpenDialog.fileUrls.length ; i++) {
                var srcUrl = mediaFileOpenDialog.fileUrls[i];
                if (layoutModel.isAcceptableUrl(srcUrl)) {
                    console.log("ViewingGrid::open url:", srcUrl);
                    validList.push(srcUrl);
                }
            }
            if (validList.length > 0)
                addNewLocalFiles(validList);
        }
        onRejected: {
            mediaFileOpenDialog.close();
        }
    }

    /////////////////////////////////////////////////////////////
    // 드롭 영역
    /////////////////////////////////////////////////////////////
    DropArea {
        id: dropArea;
        anchors.fill: gridViewArea

        onExited: {
            GridManager.dropStatus.reset();
        }

        onPositionChanged: {
            if (drag.accepted) {
                var dragX = drag.x + gridViewArea.x
                var dragY = drag.y + gridViewArea.y
                GridManager.dropStatus.update(dragX, dragY);
                gridMouseArea.refreshShowGuide();
            }
        }

        onEntered: {
//            console.log("ViewingGrid::DropArea() onEntered::key=", drag.keys, "urls=", drag.urls, ",format=", drag.formats, ",hasUrls=", drag.hasUrls,
//                        ",hasText=", drag.hasText,  drag.text, ",source=", drag.source);
            gridMouseArea.refreshShowGuide();
            drag.accepted = GridManager.dropStatus.check(drag);
        }
        onDropped: {
//            console.log("ViewingGrid::DropArea() onDropped::key=", drop.keys, "urls=", drop.urls, ",format=", drop.formats, ",hasUrls=", drop.hasUrls,
//                        ",hasText=", drop.hasText,  drop.text, ",source=", drop.source);

            var isAccepted = GridManager.dropStatus.stop(drop)
            if (isAccepted) {
                drop.accept();
            }
            else {
                layoutWarning.checkLocked();
            }
        }
    }


    // 그리드 PTZ 애니메이션 /////////////////////////////////////////////////
    SequentialAnimation  {
        id: gridAnimation

        property int    easingType: Easing.OutQuad
        property int    duration: 400
        property real   targetGridX: 0      // 애니메이션시 타겟 X
        property real   targetGridY: 0      // 애니메이션시 타겟 Y
        property real   targetGridScale: 0   // 애니메이션시 타겟 스케일
        property real   overshoot: 1.0
        property int    delayedStartDuration: 0

        function startPtz(x, y, scale, delay = 0, easing = 2, animDuration=400)
        {
            //console.log("before:", viewGridLines.gridScale, ",after:", scale);
            stop();

            delayedStartDuration = delay
            targetGridX = x;
            targetGridY = y;
            targetGridScale = scale;
            easingType = easing;
            duration = animDuration
            if (easingType == Easing.InBack || easingType == Easing.OutBack) {
                if (targetGridScale > viewGridLines.gridScale &&
                        viewGridLines.gridScale < 1.5) {
                    overshoot = viewGridLines.gridScale;
                }
                else {
                    overshoot = 1.5;
                }
            }

            start();
        }

        PauseAnimation { duration: gridAnimation.delayedStartDuration }

        ParallelAnimation {
            NumberAnimation {
                target: viewGridLines
                properties: "gridX"
                to: gridAnimation.targetGridX
                duration: gridAnimation.duration
                easing.type: gridAnimation.easingType
                easing.overshoot: gridAnimation.overshoot
            }
            NumberAnimation {
                target: viewGridLines
                properties: "gridY"
                to: gridAnimation.targetGridY
                duration: gridAnimation.duration
                easing.type: gridAnimation.easingType
                easing.overshoot: gridAnimation.overshoot
            }
            NumberAnimation {
                target: viewGridLines
                properties: "gridScale"
                to: gridAnimation.targetGridScale
                duration: gridAnimation.duration
                easing.type: gridAnimation.easingType
                easing.overshoot: gridAnimation.overshoot
            }
            onStarted: {
                //var startDate = new Date()
                //console.log("started animation at", startDate.toISOString(), gridAnimation.duration)
            }

            onFinished: {
                //var startDate = new Date()
                //console.log("ended animation at", startDate.toISOString())
            }
        }

    }

    // 초기화 동작
    Component.onCompleted : {
        initialize();
    }

    // LayoutModel에서 LayoutItemModel로부터 삭제 이벤트를 무시하도록 처리
    Component.onDestruction: {
        console.log('ViewingGrid::onDestruction!!');
        //layoutModel.released();
    }

    // 자바스크립트 함수들 ///////////////////////////////////////////////////////////

    // 초기화
    function initialize()
    {
        console.log("ViewingGrid::initialize()----")
        if (!gridViewArea.autoFit) {
            GridManager.patternInView(2);
        }
        else {
            GridManager.fitAndMediaCheck(false, false);
        }
    }

}
