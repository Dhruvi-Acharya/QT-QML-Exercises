var videoItemComponent = Qt.createComponent("VideoItem.qml");
var zoomTargetItemComponent = Qt.createComponent("ZoomTargetItem.qml");
var webItemComponent = Qt.createComponent("WebItem.qml");
var imageItemComponent = Qt.createComponent("ImageItem.qml");
var videoItemZoomAreaComponent = Qt.createComponent("VideoItemZoomArea.qml");
var pFocusedItem = null;
var pMouseOverItem = null;


/////////////////////////////////////////////////////////////////////////////
// 오브젝트 생성, 초기화
/////////////////////////////////////////////////////////////////////////////

function createItemObject(itemComponentType)
{
    var item = itemComponentType.createObject(gridItemsArea,
                                              {
                                                  isFocusedWindow: Qt.binding(function(){return viewingGridRoot.isFocusedWindow}),
                                                  itemNameAlwaysVisible:  Qt.binding(function(){return viewingGridRoot.itemNameAlwaysVisible}),
                                                  isEditable: Qt.binding(function(){return viewingGridRoot.isEditable}),
                                                  gridX:  Qt.binding(function(){return viewGridLines.gridX}),
                                                  gridY:  Qt.binding(function(){return viewGridLines.gridY}),
                                                  gridCellWidth:  Qt.binding(function(){return viewGridLines.cellWidth}),
                                                  gridCellHeight:  Qt.binding(function(){return viewGridLines.cellHeight}),
                                                  gridViewX: Qt.binding(function(){return gridViewArea.x}),
                                                  gridViewX: Qt.binding(function(){return gridViewArea.y}),
                                                  gridViewX: Qt.binding(function(){return gridViewArea.width}),
                                                  gridViewX: Qt.binding(function(){return gridViewArea.height})
                                              }
                                              );
    if (!item) {
        console.error("ViewingGridJS::createItemObject()", itemComponentType, item);
    }
    return item;
}

// 비디오아이템 오브젝트 생성
function createVideoItemObject()
{
    if (videoItemComponent.status !== Component.Ready) {
        console.error("ViewingGridJS::createVideoItemObject error!!", videoItemComponent.errorString());
    }
    var videoItem = createItemObject(videoItemComponent);
    if (!videoItem) {
        console.error("ViewingGridJS::createVideoItemObject()", videoItem);
    }
    videoItem.isSmartSearchMode = isSmartSearchMode;
    videoItem.isMaskingMode = isMaskingMode;
    return videoItem;
}

// 줌타겟아이템 오브젝트 생성
function createZoomTargetItemObject()
{
    if (zoomTargetItemComponent.status !== Component.Ready) {
        console.error("ViewingGridJS::createZoomTargetItemObject error!!", zoomTargetItemComponent.errorString());
    }
    var zoomTargetIem = createItemObject(zoomTargetItemComponent);
    if (!zoomTargetIem) {
        console.error("ViewingGridJS::createZoomTargetItemObject()", zoomTargetIem);
    }
    return zoomTargetIem;
}

// 이미지아이템 오브젝트 생성
function createImageItemObject()
{
    if (imageItemComponent.status !== Component.Ready) {
        console.error("ViewingGridJS::createImageItemObject error!!", imageItemComponent.errorString());
    }
    var imageItem = createItemObject(imageItemComponent);
    if (!imageItem) {
        console.error("ViewingGridJS::createImageItemObject()", imageItem);
    }
    return imageItem;
}

// 웹아이템 오브젝트 생성
function createWebItemObject()
{
    if (webItemComponent.status !== Component.Ready) {
        console.error("ViewingGridJS::createWebItemObject error!!", webItemComponent.errorString());
    }
    var webItem = createItemObject(webItemComponent);
    if (!webItem) {
        console.error("ViewingGridJS::createWebItemObject()", webItem);
    }
    return webItem;
}

// 아이템 오픈시 사용하는 공통 초기화 함수(ZoomTargetItem 제외한 아이템)
function initBaseItem(item)
{
    if (!pFocusedItem && selectedItems.length === 0) {
        selectStatus.setSingleFocusedItem(item);
    }

    // internal signal
    item.itemClose.connect(gridItemsArea.onItemClose);
    item.itemMouseMoved.connect(gridItemsArea.onItemMouseMoved);
    item.open();
}


/////////////////////////////////////////////////////////////////////////////
// 아이템 삭제
/////////////////////////////////////////////////////////////////////////////
function releaseItem(item, changeSaveStatus = true)
{
    if (item === pMouseOverItem) {
        pMouseOverItem = null;
    }

    layoutModel.removeItem(item.itemId, changeSaveStatus);
    if(item.isVideoItem) {
        item.clearSmartSearchAreas(); // Videoitem 삭제 시 SmartSearchArea 모두 삭제
    }
    //item.destroy();

    if(changeSaveStatus)
        item.close();
    else
        item.closeWithoutAnimation();
}

// 모든 아이템 삭제
function cleanUp()
{
    console.log("ViewingGridJS::cleanUp() all items, count=", gridItems.length)
    var removedItemIds = [];
    gridItems.forEach(function(item) {
        if (item) {
            removedItemIds.push(item.itemId);
            releaseItem(item);
        }
    });
    gridItems = [];
    _setFocusedItem(null);
    selectedItems = [];

    mouseCanceled();

    fitAndMediaCheck(false);
    _resetItemCount();
    if (removedItemIds.length > 0) {
        itemsRemoved(removedItemIds);
    }
}

function cleanUpForSequence()
{
    var removedItemIds = [];
    gridItems.forEach(function(item) {
        if (item) {
            removedItemIds.push(item.itemId);
            releaseItem(item, false);
        }
    });
    gridItems = [];
    _setFocusedItem(null);
    selectedItems = [];

    mouseCanceled();

    fitAndMediaCheck(false);
    _resetItemCount();
    gc();
}


// 줌타겟 관계에 있는 아이템을 포함하여 모두 삭제한다.
// newItems에는 removedItems항목이 삭제된 상태여야 한다.
function removeItemsWithZoomTargets(removedItems, newItems, removedItemIds)
{
    for (var i = 0 ; i < removedItems.length ; i++) {
        if (!removedItems[i].isVideoItem) {
            console.log("ViewingGridJS::removeItemsWithZoomTargets() remove non videoItem", removedItems[i].itemId);
            releaseItem(removedItems[i]);
            continue;
        }
        if (removedItems[i].zoomAreas.length === 0) {
            console.log("ViewingGridJS::removeItemsWithZoomTargets() remove videoItem no zoomTarget", removedItems[i].itemId);
            releaseItem(removedItems[i]);
            continue;
        }

        var videoItem = removedItems[i];
        videoItem.disconnectZoomTargetAreas();

        for (var j = 0 ; j < videoItem.zoomAreas.length ; j++) {
            var zoomTargetItem = videoItem.zoomAreas[j].zoomTargetItem;
            if (!zoomTargetItem) {
                console.error("ViewingGridJS::removeItemsWithZoomTargets() invalid zoomTargetId", videoItem.zoomAreas[j].zoomTargetItemId);
                continue;
            }
            var findIdx = newItems.indexOf(zoomTargetItem);
            if (findIdx > -1) {
                console.log("ViewingGridJS::removeItemsWithZoomTargets() remove zoomTargetId", newItems[findIdx].itemId);
                removedItemIds.push(newItems[findIdx].itemId);
                releaseItem(newItems[findIdx]);
                newItems.splice(findIdx, 1);
            }
        }
        videoItem.clearZoomTargetAreas();
        console.log("ViewingGridJS::removeItemsWithZoomTargets() remove videoItem with zoomTarget", videoItem.itemId);

        releaseItem(videoItem);
    }
}



// 선택된 아이템 List 삭제
function removeSelectedItems()
{
    console.log('ViewingGridJS::removeSelectedItems()', gridItems.length, selectedItems.length);
    if (selectedItems.length === 0) {
        console.log('ViewingGridJS::no selected items!');
        return;
    }

    var removedItems = [];
    var removedItemIds = [];
    var newItems = gridItems.slice();

    for (var i = 0 ; i < newItems.length ; i ++) {
        if (newItems[i]) {
            var item = newItems[i];
            if (item.selected || item.focusedTarget) {
                removedItems.push(item);
                removedItemIds.push(item.itemId);
                newItems.splice(i, 1);
                i--;
            }
        }
    }

    removeItemsWithZoomTargets(removedItems, newItems, removedItemIds);
    _resetNewItems(newItems)

    if (removedItemIds.length > 0) {
        itemsRemoved(removedItemIds);
    }
}

// 특정 아이템 삭제
function removeOneItem(item)
{
    var removedItems = [item];
    var removedItemIds = [item.itemId];
    var newItems = gridItems.slice();
    var findIndex = newItems.indexOf(item);
    if (findIndex > -1) {
        newItems.splice(findIndex, 1);
    }

    removeItemsWithZoomTargets(removedItems, newItems, removedItemIds);
    _resetNewItems(newItems);

    // 이벤트 전달
    itemsRemoved(removedItemIds);
}

// 여러 아이템 삭제 (시그널을 올리지 않음)
function removeItemsFromLayout(removedItems)
{
    var removedItemIds = [];
    var newItems = gridItems.slice();

    for (var i = 0 ; i < removedItems.length ; i++) {
        removedItemIds.push(removedItems[i].itemId);
        var findIndex = newItems.indexOf(removedItems[i]);
        if (findIndex > -1) {
            newItems.splice(findIndex, 1);
        }
    }

    removeItemsWithZoomTargets(removedItems, newItems, removedItemIds);
    _resetNewItems(newItems)

}

/////////////////////////////////////////////////////////////////////////////
// 레이아웃에서 아이템 로딩 인터페이스
/////////////////////////////////////////////////////////////////////////////

// 레이아웃 아이템 모델에서 아이템 생성 및 초기화 (줌타겟은 여기서 제외하고, 비디오 아이템 생성이 끝나고 처리해야 한다.
function loadItemFromModel(itemModel)
{
    if (itemModel.itemType === MediaLayoutItemViewModel.Camera) {
        var channelItem = createNewVideoItemFromModel(itemModel);
        initChannelItem(channelItem, itemModel.deviceId, itemModel.channelId);
    }
    else if (itemModel.itemType === MediaLayoutItemViewModel.LocalResource) {
        var videoItem = createNewVideoItemFromModel(itemModel);
        initVideoFileItem(videoItem, itemModel.localResource);
    }
    else if (itemModel.itemType === MediaLayoutItemViewModel.Image) {
        var imageItem = createNewImageItemFromModel(itemModel);
        initImageItem(imageItem, itemModel.localResource);
    }
    else if (itemModel.itemType === MediaLayoutItemViewModel.Webpage) {
        var webItem = createNewWebpageItemFromModel(itemModel);
        initWebpageItem(webItem, itemModel.webPageId);
    }
}

// 레이아웃에서 로딩
function loadLayout(newLayout)
{
    layoutModel.updateProperty(newLayout);

    var items = newLayout.layoutItems;

    var i = 0;
    var videoItem = null;
    var itemModel = null;
    for(i = 0 ; i < items.length ; i ++) {
        itemModel = items[i];
        loadItemFromModel(itemModel);
    }

    var addedZoomTargetItems = [];

    // ZoomTarget타입은 원 소스를 기준으로 체크 후 연다.
    for(i = 0 ; i < items.length ; i ++) {
        itemModel = items[i];
        if (itemModel.itemType === MediaLayoutItemViewModel.ZoomTarget) {
            //console.log("ViewingGridJS::loadLayout, zoomTargetType=", itemModel.itemId, itemModel.zoomTargetId);
            var targetItem = _findItem(itemModel.zoomTargetId);
            if (targetItem) {
                console.log("ViewingGridJS::loadLayout, new zoomTarget item = ", itemModel.itemId, itemModel.zoomTargetId);
                var zoomTargetItem = createNewZoomTargetItemFromModel(itemModel);
                initZoomTargetItem(zoomTargetItem, targetItem, itemModel.viewPort, false);
                addedZoomTargetItems.push(zoomTargetItem);
            }
        }
    }
    if (items.length > 0) {
         fitAndMediaCheck();
        // 줌타겟 아이템은 소스 아이템 초기화 완료 후 영상소스를 연결한다.
        for(i = 0 ; i < addedZoomTargetItems.length ; i ++) {
            zoomTargetItem = addedZoomTargetItems[i];
            targetItem = _findItem(zoomTargetItem.itemModel.zoomTargetId);
            if (targetItem) {
                zoomTargetItem.player.setRefSourceType(targetItem.mediaSource);
            }
        }
    }
    layoutModel.saveStatus = true;

    savedLayoutLoaded() // 저장 된 레이아웃 로드 signal 발생
}

// 코어서비스의 레이아웃 저장이벤트를 받았을 때 현재 레이아웃 변경사항 반영
function updateLayout(newLayout)
{
    layoutModel.updateProperty(newLayout);

    var newItems = newLayout.layoutItems;
    console.log("ViewingGridJS::updateLayout, item count = ", newItems.length);

    var removedItems = gridItems.slice();
    var addedItems = [];
    var updatedItems = [];
    var addedZoomTargetItems = [];

    for (var i = 0 ; i < newItems.length ; i++) {
        var newItem = newItems[i];

        var findIndex = -1;
        for (var j = 0 ; j < removedItems.length ; j++) {
            var oldItem = removedItems[j];
            if (newItem.itemId === oldItem.itemId) {
                findIndex = j;
                break;
            }
        }

        // update item
        if (findIndex > -1) {
            removedItems.splice(findIndex, 1);
            updatedItems.push(newItem);
        }
        // new item
        else {
            addedItems.push(newItem)
        }
    }

    console.log("ViewingGridJS::updateLayout, updated=", updatedItems.length, ", added=", addedItems.length, ", removed=", removedItems.length);

    var needReopen = false;
    // 아이템 삭제
    if (removedItems.length > 0) {
        needReopen = true;
        removeItemsFromLayout(removedItems);
    }

    // 아이템 추가
    if (addedItems.length > 0) {
        needReopen = true;
        // 일반 아이템
        for (i = 0 ; i < addedItems.length ; i++) {
            var itemModel = addedItems[i];
            loadItemFromModel(itemModel);
        }
        // 줌타겟 아이템
        for(i = 0 ; i < addedItems.length ; i ++) {
            itemModel = addedItems[i];
            if (itemModel.itemType === MediaLayoutItemViewModel.ZoomTarget) {
                var targetItem = _findItem(itemModel.zoomTargetId);
                if (targetItem) {
                    var zoomTargetItem = createNewZoomTargetItemFromModel(itemModel);
                    initZoomTargetItem(zoomTargetItem, targetItem, itemModel.viewPort, false);
                    addedZoomTargetItems.push(zoomTargetItem);
                }
            }
        }
    }

    // 아이템 변경
    if (updatedItems.length > 0) {
        needReopen = true;
        for (i = 0 ; i < updatedItems.length ; i++) {
            itemModel = updatedItems[i];
            var item = _findItem(itemModel.itemId);
            item.itemModel.updateProperty(itemModel);
        }
    }

    if (needReopen) {
        fitAndMediaCheck();
        // 줌타겟 아이템은 소스 아이템 초기화 완료 후 영상소스를 연결한다.
        for(i = 0 ; i < addedZoomTargetItems.length ; i ++) {
            zoomTargetItem = addedZoomTargetItems[i];
            targetItem = _findItem(zoomTargetItem.itemModel.zoomTargetId);
            if (targetItem) {
                zoomTargetItem.player.setRefSourceType(targetItem.mediaSource);
            }
        }
    }

    layoutModel.saveStatus = true;

    savedLayoutLoaded() // 저장 된 레이아웃 로드 signal 발생
}

// 장비 삭제 이벤트를 받았을 때
function checkRemovedDevice(deviceIDs)
{
    var removedItems = [];
    var removedItemIds = [];
    var newItems = gridItems.slice();

    for (var dix = 0 ; dix < deviceIDs.length ; dix++) {
        var deviceID = deviceIDs[dix];

        for (var i = 0 ; i < newItems.length ; i++) {
            var item = newItems[i];
            if(item.isCamera) {
                if (item.itemModel.deviceId === deviceID) {
                    console.log("ViewingGridJS::checkRemovedDevice(), remove item", deviceID, item.itemId);
                    removedItems.push(item);
                    removedItemIds.push(item.itemId);
                    newItems.splice(i, 1);
                    i--;
                }
            }
        }
    }

    if (removedItemIds.length > 0) {
        removeItemsWithZoomTargets(removedItems, newItems, removedItemIds);
        _resetNewItems(newItems)
        itemsRemoved(removedItemIds);
    }
}

function checkRemovedCamera(cameraIds)
{
    var removedItems = [];
    var removedItemIds = [];
    var newItems = gridItems.slice();

    for (var dix = 0 ; dix < cameraIds.length ; dix++) {
        var cameraID = cameraIds[dix];

        for (var i = 0 ; i < newItems.length ; i++) {
            var item = newItems[i];

            if(item.isCamera) {
                if ((item.itemModel.deviceId + "_" + item.itemModel.channelId) === cameraID) {
                    console.log("ViewingGridJS::checkRemovedCamera(), remove item", item.itemId);
                    removedItems.push(item);
                    removedItemIds.push(item.itemId);
                    newItems.splice(i, 1);
                    i--;
                }
            }
        }
    }

    if (removedItemIds.length > 0) {
        removeItemsWithZoomTargets(removedItems, newItems, removedItemIds);
        _resetNewItems(newItems)
        itemsRemoved(removedItemIds);
    }
}


// 웹페이지 저장 이벤트를 받았을 때
// 이름이나, URL 등 설정이 변경되었을 때, 웹페이지 RELOAD가 필요하다.
function checkWebpageSaved(webPageId)
{
    for (var i = 0; i < gridItems.length ; i++) {
        if (gridItems[i].isWebpage) {
            var item = gridItems[i];
            if (item.itemModel.webPageId === webPageId) {
                item.itemModel.updateWebpageInfo()
            }
        }
    }
}


// 다른 ViewingGrid의 아이템리스트에서 추가
function addNewGridItems(viewItems)
{
    var addedItems = [];

    console.log("ViewingGridJS::addNewGridItems, item count = ", viewItems.length, viewItems);

    var itemModel = null;
    for(var i = 0 ; i < viewItems.length ; i ++) {
        console.log("ViewingGridJS::addNewGridItems, item = ", viewItems[i]);
        itemModel = viewItems[i];
        loadItemFromModel(itemModel);
        addedItems.push(itemModel)
    }

    var addedZoomTargetItems = [];
    // ZoomTarget타입은 원 소스를 기준으로 체크 후 연다.
    for(i = 0 ; i < viewItems.length ; i ++) {
        itemModel = viewItems[i];
        if (itemModel.itemType === MediaLayoutItemViewModel.ZoomTarget) {
            var targetItem = _findItem(itemModel.zoomTargetId);
            if (targetItem) {
                console.log("ViewingGridJS::addNewGridItems, new zoomTarget item = ", itemModel.itemId, itemModel.zoomTargetId);
                var zoomTargetItem = createNewZoomTargetItemFromModel(itemModel);
                initZoomTargetItem(zoomTargetItem, targetItem, itemModel.viewPort, false);
                addedZoomTargetItems.push(zoomTargetItem);
            }
        }
    }
    if (viewItems.length > 0) {
         fitAndMediaCheck();
        // 줌타겟 아이템은 소스 아이템 초기화 완료 후 영상소스를 연결한다.
        for(i = 0 ; i < addedZoomTargetItems.length ; i ++) {
            zoomTargetItem = addedZoomTargetItems[i];
            targetItem = _findItem(zoomTargetItem.itemModel.zoomTargetId);
            if (targetItem) {
                zoomTargetItem.player.setRefSourceType(targetItem.mediaSource);
            }
        }

        itemsAdded(addedItems);
    }
}


/////////////////////////////////////////////////////////////////////////////
// 비디오 카메라, 비디오 파일 공통
/////////////////////////////////////////////////////////////////////////////

// 비디오 아이템을 아이템 모델에서 생성
function createNewVideoItemFromModel(itemModel)
{
    var videoItem = createVideoItemObject();
    if (videoItem) {
        videoItem.isSequenceItem = isSequence
        videoItem.itemModel.updateProperty(itemModel);
        gridItems.push(videoItem);
        layoutModel.addItem(videoItem.itemModel);
        _resetItemCount();
    }
    return videoItem;
}

// 비디오 아이템 생성 공통 함수
function createNewVideoItem(type, pos)
{
    var videoItem = createVideoItemObject();
    if (videoItem) {
        videoItem.setItem(type, pos.x, pos.y, pos.width, pos.height);
        videoItem.setFillMode(layoutModel.layoutFillMode);
        videoItem.setProfile(layoutModel.layoutProfile);
        videoItem.setVideoStatus(layoutModel.videoStatus);
        gridItems.push(videoItem);
        layoutModel.addItem(videoItem.itemModel);
        _resetItemCount();
    }
    return videoItem;
}

/////////////////////////////////////////////////////////////////////////////
// 비디오 카메라 추가
// startTime, overlappedId, speed, paused는 playback일때만 유효한 값
/////////////////////////////////////////////////////////////////////////////
function initChannelItem(channelItem, deviceId, channelId,
                         isPlayback = false, startTime = 0, overlappedId = -1, speed = 1, paused = false,
                         posId = 0, isPosPlayback = false)
{
    channelItem.setCamearaParam(deviceId, channelId);

    if (!isPlayback) {
        console.log("ViewingGridJS::initChannelItem() for live", startTime, overlappedId);
        channelItem.player.setCameraType(WisenetMediaParam.LiveAuto, "0",
                                         deviceId, channelId, layoutModel.uuid);
    }
    else {
        // Playback (included POS Playback)
        console.log("ViewingGridJS::initChannelItem() for playback || POS Playback", startTime, overlappedId, posId, isPosPlayback);
        channelItem.player.setCameraType(WisenetMediaParam.PlaybackAuto, "0",
                                         deviceId, channelId, layoutModel.uuid, "",
                                         startTime, 0, overlappedId, speed, paused, posId, isPosPlayback);
    }

    channelItem.createZoomTarget.connect(gridItemsArea.onCreateZoomTarget);

    // interface signal
    channelItem.openResult.connect(viewingGridRoot.mediaOpenResult);
    channelItem.mediaItemStatusChanged.connect(viewingGridRoot.mediaStatusChanged);
    channelItem.localRecordingStopped.connect(viewingGridRoot.onLocalRecordingStopped);
    channelItem.videoCaptured.connect(viewingGridRoot.onVideoCaptured);
    channelItem.ptzPresetTriggered.connect(viewingGridRoot.onPtzPresetTriggered);
    channelItem.ptzPresetListUpdated.connect(viewingGridRoot.onPtzPresetListUpdated);
    channelItem.ptzSwingTriggered.connect(viewingGridRoot.onPtzSwingTriggered);
    channelItem.ptzSwingPresetUpdated.connect(viewingGridRoot.onPtzSwingPresetUpdated);
    channelItem.ptzGroupTriggered.connect(viewingGridRoot.onPtzGroupTriggered);
    channelItem.ptzGroupPresetUpdated.connect(viewingGridRoot.onPtzGroupPresetUpdated);
    channelItem.ptzTourTriggered.connect(viewingGridRoot.onPtzTourTriggered);
    channelItem.ptzTourPresetUpdated.connect(viewingGridRoot.onPtzTourPresetUpdated);
    channelItem.ptzTraceTriggered.connect(viewingGridRoot.onPtzTraceTriggered);
    channelItem.ptzTracePresetUpdated.connect(viewingGridRoot.onPtzTracePresetUpdated);
    channelItem.modelPropertyChanged.connect(viewingGridRoot.onItemModelPropertyChanged);
    channelItem.deviceTalkResult.connect(viewingGridRoot.onDeviceTalkResult);
    channelItem.deviceUsingSetupChanged.connect(viewingGridRoot.onDeviceUsingSetupChanged);
    channelItem.smartSearchRequest.connect(viewingGridRoot.smartSearchRequest);
    channelItem.openMessageDialog.connect(viewingGridRoot.openMessageDialog);
    initBaseItem(channelItem);
}

function _addSingleVideoChannel(deviceId, channelId,
                                isPlayback = false, startTime = 0, overlappedId = -1, speed = 1, paused = false,
                                hasBasePos = false, startX = -1, startY = -1,
                                posId = 0, isPosPlayback = false)
{
    if (!hasBasePos) {
        var pos = layoutModel.getNewItemPosition(1, 1);
    }
    else {
        pos = layoutModel.getNewItemPositionWithStartPos(1, 1, startX, startY);
    }

    var channelItem = createNewVideoItem(MediaLayoutItemViewModel.Camera, pos);

    initChannelItem(channelItem, deviceId, channelId, isPlayback, startTime, overlappedId, speed, paused, posId, isPosPlayback);
    return channelItem;
}

// 신규 비디오 카메라 아이템 추가
function addNewVideoChannel(deviceId, channelId,
                            isPlayback, startTime, overlappedId, linkToNewItem = false,
                            posId, isPosPlayback = false)
{
    console.log("ViewingGridJS::addNewVideoChannel()", deviceId, channelId);
    var videoItemCount = _getVideoItemsCount();
    if (videoItemCount >= layoutModel.maxVideoItems) {
        layoutWarning.showMaxVideoLimitWarning();
        return;
    }

    // 동기화 재생인 경우, 기존 채널 기준으로 신규 채널 초기화
    var linkTargetItem = null
    var speed = 1
    var paused = false
    if(isLinkedMode && !linkToNewItem) {
        linkTargetItem = getLinkedModeTargetItem(deviceId)
        if(linkTargetItem !== null) {
            isPlayback = linkTargetItem.mediaSeekable
            startTime = linkTargetItem.mediaPlayPosition
            speed = linkTargetItem.player.speed
            paused = linkTargetItem.player.playbackState === WisenetMediaPlayer.PausedState
        }
    }

    var channelItem = _addSingleVideoChannel(deviceId, channelId,
                                             isPlayback, startTime, overlappedId, speed, paused,
                                             false, -1, -1,
                                             posId, isPosPlayback);
    fitAndMediaCheck();

    var addedItems = [channelItem.itemModel];
    itemsAdded(addedItems);

    if(isLinkedMode) {
        // 전체 채널 재생 동기화
        var targetItem = linkToNewItem ? channelItem : linkTargetItem
        var syncTime = linkToNewItem ? startTime : 0
        linkAllVideoChannels(targetItem, syncTime)
    }
}

function addNewVideoChannels(devices)
{
    var videoItemCount = _getVideoItemsCount();
    if (videoItemCount >= layoutModel.maxVideoItems) {
        layoutWarning.showMaxVideoLimitWarning();
        return;
    }

    var addableCount = layoutModel.maxVideoItems - videoItemCount;
    var deviceCount = devices.count;
    var showWarning = false;

    if (addableCount < deviceCount) {
        deviceCount = addableCount;
        showWarning = true;
    }

    // 동기화 재생 중이면 대표 카메라의 StreamType과 PlayPosition 적용
    var linkTargetItem = null
    var isPlayback = false
    var isPosPlayback = false
    var startTime = 0
    var speed = 1
    var paused = false
    if(isLinkedMode) {
        linkTargetItem = getLinkedModeTargetItem("")
        if(linkTargetItem !== null) {
            isPlayback = linkTargetItem.mediaSeekable
            startTime = linkTargetItem.mediaPlayPosition
            speed = linkTargetItem.player.speed
            paused = linkTargetItem.player.playbackState === WisenetMediaPlayer.PausedState
        }
    }

    var addedItems = [];
    for (var i = 0 ; i < deviceCount ; i ++) {
        var deviceId = devices.get(i).deviceID;
        var channelId = devices.get(i).channelID;
        console.log("ViewingGridJS::addNewVideoChannel(), deviceID", deviceId, ",channelID", channelId);
        var channelItem = _addSingleVideoChannel(deviceId, channelId,
                                                 isPlayback, startTime, -1, speed, paused);
        addedItems.push(channelItem.itemModel)
    }

    if (addedItems.length > 0) {
        fitAndMediaCheck();
        itemsAdded(addedItems);
    }

    if(isLinkedMode) {
        // 전체 채널 재생 동기화
        linkAllVideoChannels(linkTargetItem)
    }

    if (showWarning) {
        layoutWarning.showMaxVideoLimitWarning();
        return;
    }
}

function addNewDragItems(dragItemListModel)
{
    console.time(   "ViewingGridJS::addNewDragItems()===");

    var openItems = layoutModel.parseDragItemListModel(dragItemListModel);
    addOpenItems(openItems);
    console.timeEnd("ViewingGridJS::addNewDragItems()===")
}


function addNewDropItems(model, startX, startY)
{
    //console.time(   "ViewingGridJS::addNewDropItems()===");

    var openItems = layoutModel.parseDragItemListModel(model);
    if (openItems.length > 0) {
        if (openItems[0].itemTypeId === 3) {
            console.log("drop layout, emit signal for window");
            viewingGridRoot.layoutDropped(openItems);
            return true;
        }
        if (openItems[0].itemTypeId === 4) {
            console.log("drop seqeunce, emit signal for window");
            viewingGridRoot.sequneceDropped(openItems);
            return true;
        }
        if (!viewingGridRoot.isEditable) {
            return false;
        }

        addOpenItems(openItems, true, startX, startY);
        return true;
        //console.timeEnd("ViewingGridJS::addNewDropItems()===")
    }
    return false;
}

// 그룹으로 열기
function addNewGroup(groupId)
{
    console.time(   "ViewingGridJS::addNewGroup()===")

    var openItems = layoutModel.getChannelsInGroup(groupId);
    addOpenItems(openItems);
    console.timeEnd("ViewingGridJS::addNewGroup()===")
}

function addOpenItems(openItems, hasBasePos = false, startX = -1, startY = -1)
{
    // Play only one channel
    if(isMaskingMode)
        cleanUp()

    var addedItems = [];
    var viewItem = null;

    var isEmptyLayout = gridItems.length > 0 ? false : true;
    var basePos = hasBasePos;
    var rStartX = startX;
    var rStartY = startY;

    var videoItemsCount = _getVideoItemsCount();
    var webItemsCount = _getWebItemsCount();
    var remainVideoCount = (layoutModel.maxVideoItems < videoItemsCount) ? 0 : (layoutModel.maxVideoItems-videoItemsCount)
    var remainWebCount = (layoutModel.maxWebItems < webItemsCount) ? 0 : (layoutModel.maxWebItems-webItemsCount)
    var videoLimit = false;
    var webLimit = false;

    if (isEmptyLayout) {
        var itemsSqrt = Math.ceil(Math.sqrt(openItems.length));
        var currentRow = 0;
        console.log("ViewingGridJS::addOpenItems():: empty layout calculate", itemsSqrt, gridItems.length);
        if (!hasBasePos) {
            basePos = true;
            rStartX = 0;
            rStartY = 0;
        }
        var originStartX = rStartX;
        var originStartY = rStartY;
    }

    // 동기화 재생 중이면 대표 카메라의 StreamType과 PlayPosition 적용
    var linkTargetItem = null
    var isPlayback = false
    var isPosPlayback = false
    var startTime = 0
    var speed = 1
    var paused = false
    if(isLinkedMode) {
        linkTargetItem = getLinkedModeTargetItem("")
        if(linkTargetItem !== null) {
            isPlayback = linkTargetItem.mediaSeekable
            startTime = linkTargetItem.mediaPlayPosition
            speed = linkTargetItem.player.speed
            paused = linkTargetItem.player.playbackState === WisenetMediaPlayer.PausedState
        }
    }

    var videoChannelAdded = false

    var currentVideoItemCount = 0;
    var currentWebItemCount = 0;
    for (var i = 0 ; i < openItems.length ; i++) {
        var oitem = openItems[i];
        console.log("ViewingGridJS::addOpenItems(), item index=", i, ",type=", oitem.itemType, ",typeId=", oitem.itemTypeId, ",itemId=", oitem.itemId, ",channelId=", oitem.subId, ",name=", oitem.name);

        viewItem = null;
        if (oitem.itemTypeId === 0) {
            if (currentVideoItemCount < remainVideoCount) {
                viewItem = _addSingleVideoChannel(oitem.itemId, oitem.subId,                                                  
                                                  isPlayback, startTime, -1, speed, paused,
                                                  basePos, rStartX, rStartY);
                currentVideoItemCount++;

                videoChannelAdded = true
            }
            else {
                videoLimit = true;
            }
        }
        else if (oitem.itemTypeId === 1) {
            if (currentWebItemCount < remainWebCount) {
                viewItem = _createNewWebPageItem(oitem.itemId, basePos, rStartX, rStartY);
                currentWebItemCount++;
            }
            else {
                webLimit = true;
            }
        }
        else if (oitem.itemTypeId === 2) {
            if (layoutModel.isVideoFile(oitem.itemId)) {
                if (currentVideoItemCount < remainVideoCount) {
                    viewItem = _addSingleLocalFile(oitem.itemId, basePos, rStartX, rStartY);
                    currentVideoItemCount++;
                }
                else {
                    videoLimit = true;
                }
            }
            else {
                viewItem = _addSingleLocalFile(oitem.itemId, basePos, rStartX, rStartY);
            }
        }

        if (viewItem) {
            addedItems.push(viewItem.itemModel)
            // 빈레이아웃인 경우에는 채널순으로 추가한다.
            if (isEmptyLayout) {
                currentRow++;
                if (currentRow < itemsSqrt) {
                    rStartX++;
                }
                else {
                    currentRow = 0;
                    rStartX = originStartX
                    rStartY++;
                }
            }
        }
    }

    if (addedItems.length > 0) {
        fitAndMediaCheck();
        itemsAdded(addedItems);
    }

    if(isLinkedMode && videoChannelAdded) {
        // 전체 채널 재생 동기화
        linkAllVideoChannels(linkTargetItem)
    }

    if (videoLimit) {
        layoutWarning.showMaxVideoLimitWarning();
    }
    else if(webLimit) {
        layoutWarning.showMaxWebLimitWarning();
    }
}

// 해당 카메라가 없는 경우, 신규 추가하고 없는 경우에는 기존 아이템의 재생정보 등 변경
// TODO:: FOCUS 이동 처리
function checkNewVideoChannel(deviceId, channelId,
                              isPlayback = false, startTime = 0, overlappedId = -1, linkToNewItem = true,
                              posId = 0, isPosPlayback = false)
{
    console.log("ViewingGridJS::checkNewVideoChannel()", deviceId, channelId)

    // 동기화 재생인 경우, 기존 채널 기준으로 신규 채널 초기화
    var linkTargetItem = null
    var speed = 1
    var paused = false
    if(isLinkedMode) {
        linkTargetItem = getLinkedModeTargetItem(deviceId)
        if(linkTargetItem !== null) {
            // speed와 paused 상태는 항상 기존 linkTargetItem을 따름
            speed = linkTargetItem.player.speed
            paused = linkTargetItem.player.playbackState === WisenetMediaPlayer.PausedState

            if(!linkToNewItem) {
                // 신규 Item 기준 동기화가 아니면 playback 상태와 startTime까지 기존 linkTargetItem을 따름
                isPlayback = linkTargetItem.mediaSeekable
                startTime = linkTargetItem.mediaPlayPosition
            }
        }
    }

    var channelItem = _findChannelItem(deviceId, channelId);
    if (!channelItem) {
        var videoItemCount = _getVideoItemsCount();
        if (videoItemCount >= layoutModel.maxVideoItems) {
            layoutWarning.showMaxVideoLimitWarning();
            return;
        }

        // autofit이 아닌 경우 autofit모드로 자동 변경
        if (!gridViewArea.autoFit) {
            gridViewArea.autoFit = true;
        }

        channelItem = _addSingleVideoChannel(deviceId, channelId,
                                             isPlayback, startTime, overlappedId, speed, paused,
                                             false, -1, -1,
                                             posId, isPosPlayback);
        fitAndMediaCheck();

        var addedItems = [channelItem.itemModel];
        itemsAdded(addedItems);
    }
    else {
        var trackId = overlappedId ? overlappedId : -1;
        // auto fit이 아니었으면 좌표계만 미리 이동시켜 놓고 영상 명령을 다시 보낸다.
        if (!gridViewArea.autoFit) {
            gridViewArea.autoFit = true;
            var vpos = _getItemsViewPosition();
            var gridInfo = _setViewPos(vpos, anim, delayedTime);
            if (zoomFullScreen)
                zoomFullScreen = false;
            checkMediaStatus(gridInfo.gridScale, true);
        }

        if(linkToNewItem) {
            // check한 카메라 기준으로 동기화 하는 경우 speed와 pause 상태 유지
            speed = channelItem.player.speed
            paused = channelItem.player.playbackState === WisenetMediaPlayer.PausedState
        }

        //console.log("ViewingGridJS::checkNewVideoChannel(), use exist item", channelItem, deviceId, channelId);
        if (channelItem.mediaParam.isLive && isPlayback && !isPosPlayback) {
            console.log("ViewingGridJS::checkNewVideoChannel(), live to playback");
            channelItem.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", startTime, 0, trackId, speed, paused);
        }
        else if (!channelItem.mediaParam.isLive && !isPlayback && !isPosPlayback) {
            console.log("ViewingGridJS::checkNewVideoChannel(), playback to live");
            channelItem.player.changeCameraStream(WisenetMediaParam.LiveAuto, "", 0, 0, -1);
        }
        else if (!channelItem.mediaParam.isLive && isPlayback && !isPosPlayback) {
            console.log("ViewingGridJS::checkNewVideoChannel(), playback change time", trackId, channelItem.mediaParam.trackId);
            if (trackId !== channelItem.mediaParam.trackId) {
                channelItem.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", startTime, 0, trackId, speed, paused);
            }
            else {
                channelItem.player.seek(startTime);
            }
        }
        else if (!channelItem.mediaParam.isLive && !isPlayback && isPosPlayback) {
            console.log("ViewingGridJS::checkNewVideoChannel(), pos playback");
            channelItem.player.changeCameraStream(WisenetMediaParam.PosPlayback, "", startTime, 0, trackId, speed, paused);
        }
    }

    if(isLinkedMode) {
        // 전체 채널 재생 동기화
        var targetItem = linkToNewItem ? channelItem : linkTargetItem
        var syncTime = linkToNewItem ? startTime : 0
        linkAllVideoChannels(targetItem, syncTime)
    }

    // 해당 채널로 포커스 이동
    if (pFocusedItem === channelItem) {
        channelItem.updateFocusedRect(true)
    }

    selectStatus.setSingleFocusedItem(channelItem);
}

// 전체 VideoChannel의 재생 동기화 함수
function linkAllVideoChannels(targetItem = null, syncTime = 0, pauseOnly = false)
{
    if(!targetItem)
        targetItem = getLinkedModeTargetItem("") // targetItem이 없으면 자동 선택

    if(!targetItem) {
        console.log("linkAllVideoChannels() targetItem is null")
        return  // 자동 선택 된 targetItem도 없으면 리턴
    }

    var isPlayback = targetItem.mediaSeekable
    var startTime = syncTime !== 0 ? syncTime : targetItem.mediaPlayPosition
    var speed = targetItem.mediaPlaySpeed
    var paused = targetItem.player.playbackState === WisenetMediaPlayer.PausedState

    if(isPlayback && startTime === 0) {
        // Playback 동기화 시간이 없으면 Default TargetItem을 찾아 동기화 시간으로 사용
        console.log("linkAllVideoChannels() startTime is 0")
        var defaultTarget = getLinkedModeTargetItem(targetItem.mediaParam.deviceId)
        if(!defaultTarget)
            return  // 동기화 시간을 획득할 수 없으면 리턴
        else
            startTime = defaultTarget.mediaPlayPosition
    }

    var i = 0
    var item = null
    var deviceTrackIdMap = new Map()    // Device 별 대표 TrackId Map
    if(isPlayback) {
        //console.log("deviceTrackIdMap set. deviceId :", targetItem.mediaParam.deviceId, "trackId :", targetItem.mediaParam.trackId)
        deviceTrackIdMap.set(targetItem.mediaParam.deviceId, targetItem.mediaParam.trackId) // targetItem은 해당 Device의 대표 Item으로 고정
        for (i = gridItems.length-1 ; i >=0 ; i--) {
            item = gridItems[i]
            if(item && item.isCamera && item.mediaSeekable
                    && item.mediaParam.deviceId !== targetItem.mediaParam.deviceId && item.mediaParam.trackId !== -1) {
                //console.log("deviceTrackIdMap set. deviceId :", item.mediaParam.deviceId, "trackId :", item.mediaParam.trackId)
                deviceTrackIdMap.set(item.mediaParam.deviceId, item.mediaParam.trackId)   // Device 별 대표 TrackId 저장
            }
        }
    }

    var deviceMap = new Map()   // Device 별 대표 Item Map
    deviceMap.set(targetItem.mediaParam.deviceId, targetItem)   // targetItem은 해당 Device의 대표 Item으로 고정

    for (i = gridItems.length-1 ; i >=0 ; i--) {
        item = gridItems[i]
        if(!item || !item.isCamera)
            continue

        if(!isPlayback) {
            // Playback 중인 채널을 Live로 전환
            if(item.mediaSeekable) {
                item.player.changeCameraStream(WisenetMediaParam.LiveAuto, "", 0, 0, -1)
            }
        }
        else if(!item.mediaSeekable && startTime !== 0) {
            // Live인 채널을 Playback으로 전환
            var trackId = deviceTrackIdMap.has(item.mediaParam.deviceId) ? deviceTrackIdMap.get(item.mediaParam.deviceId) : -1
            //console.log("deviceTrackIdMap get. deviceId :", item.mediaParam.deviceId, "trackId :", trackId)
            item.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", startTime, 0, trackId, speed, paused)
        }

        if(item.mediaParam.deviceId !== targetItem.mediaParam.deviceId) {
            deviceMap.set(item.mediaParam.deviceId, item)   // Device 별 대표 Channel 저장
        }
    }

    if(isPlayback) {
        // 각 Device의 대표 Channel에 mediaControl 명령 전송
        for(var channelItem of deviceMap.values()) {
            if(channelItem.player.speed !== speed)
                channelItem.player.speed = speed

            if(paused && channelItem.player.playbackState !== WisenetMediaPlayer.PausedState) {
                channelItem.player.pause()
            }
            else if(!paused && channelItem.player.playbackState === WisenetMediaPlayer.PausedState) {
                channelItem.player.play()
            }

            if(pauseOnly) {
                // pause일 때는 seek 및 영상수신 대기하지 않음
                _mediaUpdateAllChannelState(channelItem, false, startTime)
            }
            else {
                channelItem.player.seek(startTime)
                _mediaUpdateAllChannelState(channelItem, true, startTime)
            }
        }
    }
}

function linkAllMediaFiles(targetItem = null, syncTime = 0, isStep = false, isForward = true)
{
    // 입력 targetItem이 null인 경우에만 유효한 시간 동기화 대표 Item들.
    var supportTimelineItem = null      // Timeline(날짜) 지원 파일의 시간 동기화 대표 Item
    var unsupportTimelineItem = null    // Timeline(날짜) 미지원 파일의 시간 동기화 대표 Item

    // 입력 targetItem이 null인 경우 동기화 Target을 검색
    if(!targetItem) {
        if(focusedItem && focusedItem.isLocalResource) {
            // focusedItem이 LocalResource인 경우 우선 Target
            if(focusedItem.player.supportTimeline) {
                targetItem = focusedItem
                supportTimelineItem = focusedItem
            }
            else {
                targetItem = focusedItem
                unsupportTimelineItem = focusedItem
            }
        }
        for (var i = 0 ; i < gridItems.length ; i++) {
            if(gridItems[i] && gridItems[i].isLocalResource) {
                if(!targetItem)
                    targetItem = gridItems[i]

                if(!supportTimelineItem && gridItems[i].player.supportTimeline)
                    supportTimelineItem = gridItems[i]
                else if(!unsupportTimelineItem && !gridItems[i].player.supportTimeline)
                    unsupportTimelineItem = gridItems[i]
            }

            if(supportTimelineItem && unsupportTimelineItem)
                break
        }
    }

    if(!targetItem)
        return

    var additionalTargetItem = targetItem.player.supportTimeline ? unsupportTimelineItem : supportTimelineItem

    // targetItem 기준으로 전체 MediaFile 동기화
    for (var j = 0 ; j < gridItems.length ; j++) {
        var item = gridItems[j]
        //console.log("linkAllMediaFiles()", item, item.isLocalResource, item.player.supportTimeline)
        if(!item || !item.isLocalResource)
            continue

        // item별 동기화 수행
        linkMediaFile(item, targetItem, syncTime, isStep, isForward)

        // targetItem과 supportTimeline이 다른 Item은 별도로 시간 동기화 (seek)
        if(!isStep && additionalTargetItem && item.player.supportTimeline !== targetItem.player.supportTimeline) {
            item.player.seek(additionalTargetItem.mediaPlayPosition)
        }
    }
}

function linkMediaFile(item, targetItem, syncTime = 0, isStep = false, isForward = true) {
    if(!targetItem)
        return  // targetItem이 없으면 리턴

    if(syncTime === 0)
        syncTime = targetItem.mediaPlayPosition
    var speed = targetItem.mediaPlaySpeed
    var paused = targetItem.player.playbackState === WisenetMediaPlayer.PausedState
    var supportTimeline = targetItem.player.supportTimeline

    // pause / play 상태 동기화
    if(paused && item.player.playbackState !== WisenetMediaPlayer.PausedState) {
        item.player.pause()
    }
    else if(!paused && item.player.playbackState === WisenetMediaPlayer.PausedState) {
        item.player.play()
    }

    // 속도 동기화
    if(item.player.speed !== speed)
        item.player.speed = speed

    // Step or Seek 수행
    if(isStep) {
        item.player.step(isForward)
    }
    else if(supportTimeline === item.player.supportTimeline) {
        // Timeline 지원(=날짜 구분 가능) 여부가 같은 item만 시간 동기화 seek
        item.player.seek(syncTime)
    }
}

// item과 동일한 경로의 LocalRecording 완료 여부를 체크하고, WisenetPlayer를 복사하는 함수
function checkLocalRecordingFinished(item) {
    var filePath = item.player.localRecordingPath
    var finished = true

    for (var i = 0 ; i < gridItems.length ; i++) {
        var cameraItem = gridItems[i]
        if(!cameraItem || !cameraItem.isCamera || cameraItem === item)
            continue

        if(cameraItem.player.localRecordingStatus === true && cameraItem.player.localRecordingPath === filePath) {
            // item과 동일한 경로에 아직 LocalRecording 중인 경우
            finished = false
            break;
        }
    }

    if(finished == true) {
        // Wisent Player 복사 수행
        item.player.copyWisenetPlayer()
    }
}


/////////////////////////////////////////////////////////////////////////////
// 이미지 아이템 생성함수
/////////////////////////////////////////////////////////////////////////////

// 이미지 아이템 초기화
function initImageItem(imageItem, path)
{
    imageItem.setLocalResourceParam(path);
    imageItem.imageSource = path;

    initBaseItem(imageItem);
}

// 이미지 아이템을 아이템 모델에서 생성
function createNewImageItemFromModel(itemModel)
{
    var imageItem = createImageItemObject();
    if (imageItem) {
        imageItem.itemModel.updateProperty(itemModel);
        gridItems.push(imageItem);
        layoutModel.addItem(imageItem.itemModel);
        _resetItemCount();
    }
    return imageItem;
}

// 이미지 아이템 생성
function createNewImageItem(pos)
{
    var imageItem = createImageItemObject();
    if (imageItem) {
        imageItem.setItem(MediaLayoutItemViewModel.Image, pos.x, pos.y, pos.width, pos.height);
        gridItems.push(imageItem);
        layoutModel.addItem(imageItem.itemModel);
        _resetItemCount();
    }
    return imageItem;
}

// 이미지 아이템 백그라운드 설정
function setBackgroundImageItem(sourceItem, setOn) {
    sourceItem.itemModel.imageBackground = setOn;
    var needToUpdatePos = false;
    if (setOn) {
        /* todo 사양 검토 */
    }
    else {
        /* 다른 아이템과 충돌이 나는 경우 새로운 위치&크기로 이동 */
        for (var i = 0 ; i < gridItems.length ; i++) {
            var targetItem = gridItems[i];

            // 자기 자신은 제외
            if (sourceItem === targetItem) {
                continue;
            }

            if (!_isValidPositionItem(targetItem)) {
                continue;
            }

            if (_cellCollistionTest(sourceItem.column, sourceItem.columnCount,
                                   sourceItem.row, sourceItem.rowCount,
                                   targetItem.column, targetItem.columnCount,
                                   targetItem.row, targetItem.rowCount)) {
                needToUpdatePos = true;
                break;
            }
        }
    }

    if (needToUpdatePos) {
        sourceItem.itemModel.invalid = true;
        var pos = layoutModel.getNewItemPosition(1, 1);
        sourceItem.itemModel.invalid = false;
        /* pos 값이 QRect이지만 실제 리턴값은 col, columnCount와 같은 셀 인덱스 정보 */
        sourceItem.movePosition(pos.x, pos.width, pos.y, pos.height);
    }
    var updatedItems = [sourceItem.itemModel];
    itemsUpdated(updatedItems);
}


/////////////////////////////////////////////////////////////////////////////
// 비디오/이미지 파일 추가
/////////////////////////////////////////////////////////////////////////////

// 비디오 파일 아이템 초기화
function initVideoFileItem(videoItem, path)
{
    videoItem.setLocalResourceParam(path)
    videoItem.player.setLocalResourceType(path);

    videoItem.createZoomTarget.connect(gridItemsArea.onCreateZoomTarget);

    // interface signal
    videoItem.openResult.connect(viewingGridRoot.mediaOpenResult);
    videoItem.mediaItemStatusChanged.connect(viewingGridRoot.mediaStatusChanged);
    videoItem.videoCaptured.connect(viewingGridRoot.onVideoCaptured);
    videoItem.modelPropertyChanged.connect(viewingGridRoot.onItemModelPropertyChanged);
    videoItem.selectedMaskItemChanged.connect(viewingGridRoot.selectedMaskItemChanged);
    videoItem.showTrackingFailMessage.connect(viewingGridRoot.showTrackingFailMessage);
    initBaseItem(videoItem);
}

function _addSingleLocalFile(path, hasBasePos = false, startX = -1, startY = -1)
{
    if (!hasBasePos) {
        var pos = layoutModel.getNewItemPosition(1, 1);
    }
    else {
        pos = layoutModel.getNewItemPositionWithStartPos(1, 1, startX, startY);
    }

    if (layoutModel.isVideoFile(path)) {
        var videoItem = createNewVideoItem(MediaLayoutItemViewModel.LocalResource, pos);
        initVideoFileItem(videoItem, path);
        return videoItem;
    }
    else {
        var imageItem = createNewImageItem(pos);
        initImageItem(imageItem, path);
        return imageItem;
    }
}

// 신규 비디오 파일 아이템 추가 (로컬리소스)
function addNewLocalFile(path)
{
    if (layoutModel.isVideoFile(path)) {
        var videoItemCount = _getVideoItemsCount();
        if (videoItemCount >= layoutModel.maxVideoItems) {
            layoutWarning.showMaxVideoLimitWarning();
            return;
        }
    }

    var addedItems = [];
    console.log("ViewingGridJS::addNewLocalFile()", path);
    var localItem = _addSingleLocalFile(path);
    fitAndMediaCheck();
    addedItems.push(localItem.itemModel);
    itemsAdded(addedItems);
}

function addNewLocalFiles(urls, hasBasePos = false, startX = -1, startY = -1)
{
    var videoItemsCount = _getVideoItemsCount();
    var remainVideoCount = (layoutModel.maxVideoItems < videoItemsCount) ? 0 : (layoutModel.maxVideoItems-videoItemsCount)
    var videoLimit = false;

    var addedItems = [];
    var currentVideoItemCount = 0;
    var localItem = null;
    for(var i = 0 ; i < urls.length ; i++) {
        var srcUrl = urls[i];
        if (layoutModel.isAcceptableUrl(srcUrl)) {

            // Play only one channel
            if(isMaskingMode)
                cleanUp()

            localItem = null;
            if (layoutModel.isVideoFile(srcUrl)) {
                if (currentVideoItemCount < remainVideoCount) {
                    localItem = _addSingleLocalFile(srcUrl, hasBasePos, startX, startY);
                    currentVideoItemCount++
                }
                else {
                    videoLimit = true;
                }
            }
            else {
                localItem = _addSingleLocalFile(srcUrl, hasBasePos, startX, startY);
            }

            if (localItem) {
                console.log("ViewingGridJS::addNewLocalFiles(), open url:", srcUrl);
                addedItems.push(localItem.itemModel);

                // Play only one channel
                if(isMaskingMode)
                    break
            }
        }
    }
    if (addedItems.length > 0) {

        fitAndMediaCheck();
        itemsAdded(addedItems);
    }
    if (videoLimit) {
        layoutWarning.showMaxVideoLimitWarning();
    }
}


/////////////////////////////////////////////////////////////////////////////
// 타겟줌 비디오 아이템 추가
/////////////////////////////////////////////////////////////////////////////

// ZoomTarget 아이템 초기화
function initZoomTargetItem(zoomTargetItem, targetItem, normalRect, useAnim, rectColor)
{
    zoomTargetItem.setTargetItem(targetItem, normalRect);
    zoomTargetItem.itemClose.connect(gridItemsArea.onItemClose);
    zoomTargetItem.itemMouseMoved.connect(gridItemsArea.onItemMouseMoved);

    var zoomArea = targetItem.setZoomTargetArea(videoItemZoomAreaComponent, zoomTargetItem, normalRect, rectColor);
    if (useAnim) {
        zoomTargetItem.openAnimationFinished.connect(gridItemsArea.onOpenZoomTargetAnimationFinished);
    }
    zoomTargetItem.videoCaptured.connect(viewingGridRoot.onVideoCaptured);
    zoomTargetItem.modelPropertyChanged.connect(viewingGridRoot.onItemModelPropertyChanged);
    zoomTargetItem.open(viewingGridRoot.screenPixelRatio, targetItem, zoomArea, useAnim);
}

// zoom target 아이템을 아이템 모델에서 생성
function createNewZoomTargetItemFromModel(itemModel)
{
    var zoomTargetItem = createZoomTargetItemObject();
    if (zoomTargetItem) {
        zoomTargetItem.itemModel.updateProperty(itemModel);
        gridItems.push(zoomTargetItem);
        layoutModel.addItem(zoomTargetItem.itemModel);
        _resetItemCount();
    }
    return zoomTargetItem;
}

// zoomtarget 아이템 생성 공통 함수
function createNewZoomTargetItem(pos)
{
    var zoomTargetItem = createZoomTargetItemObject();
    if (zoomTargetItem) {
        zoomTargetItem.setItem(MediaLayoutItemViewModel.ZoomTarget, pos.x, pos.y, pos.width, pos.height);
        gridItems.push(zoomTargetItem);
        layoutModel.addItem(zoomTargetItem.itemModel);
        _resetItemCount();
    }
    return zoomTargetItem;
}

function createZoomTarget(targetItem, normalRect, rectColor)
{
    console.log("ViewingGridJS::createZoomTarget()", targetItem, normalRect);
    if (!targetItem.isVideoItem) {
        console.log('ViewingGridJS::invoid item to create zoomTarget');
        return;
    }

    var addedItems = [];

    var pos = layoutModel.getNewItemPosition(1, 1);
    var zoomTargetItem = createNewZoomTargetItem(pos);

    initZoomTargetItem(zoomTargetItem, targetItem, normalRect, true, rectColor);
    zoomTargetItem.player.setRefSourceType(targetItem.mediaSource);

    addedItems.push(zoomTargetItem.itemModel);
    itemsAdded(addedItems);
}

/////////////////////////////////////////////////////////////////////////////
// 웹 아이템 생성함수
/////////////////////////////////////////////////////////////////////////////

// 웹아이템 초기화
function initWebpageItem(webItem, webPageId)
{
    webItem.setWebpageParam(webPageId);
    webItem.webContextMenuRequested.connect(webItemContextMenu.show);
    initBaseItem(webItem);
}

// 웹아이템 생성
function createNewWebpageItem(pos)
{
    var webItem = createWebItemObject();
    if (webItem) {
        webItem.setItem(MediaLayoutItemViewModel.Webpage, pos.x, pos.y, pos.width, pos.height);
        gridItems.push(webItem);
        layoutModel.addItem(webItem.itemModel);
        _resetItemCount();
    }
    return webItem;
}

function _createNewWebPageItem(webPageId, hasBasePos = false, startX = -1, startY = -1)
{
    if (!hasBasePos) {
        var pos = layoutModel.getNewItemPosition(1, 1);
    }
    else {
        pos = layoutModel.getNewItemPositionWithStartPos(1, 1, startX, startY);
    }

    var webItem = createWebItemObject();
    if (webItem) {
        initWebpageItem(webItem, webPageId);
        webItem.setItem(MediaLayoutItemViewModel.Webpage, pos.x, pos.y, pos.width, pos.height);
        gridItems.push(webItem);
        layoutModel.addItem(webItem.itemModel);
        _resetItemCount();
    }
    return webItem;
}

// 웹아이템을 아이템 모델에서 생성
function createNewWebpageItemFromModel(itemModel)
{
    var webItem = createWebItemObject();
    if (webItem) {
        webItem.itemModel.updateProperty(itemModel);
        gridItems.push(webItem);
        layoutModel.addItem(webItem.itemModel);
        _resetItemCount();
    }
    return webItem;
}

// 웹페이지 추가
function addNewWebpage(webPageId)
{
    var webItemCount = _getWebItemsCount();
    if (webItemCount >= layoutModel.maxWebItems) {
        layoutWarning.showMaxWebLimitWarning();
        return;
    }

    var addedItems = [];
    console.log("ViewingGridJS::addNewWebpage()", webPageId);
    var webItem = _createNewWebPageItem(webPageId);
    fitAndMediaCheck();
    addedItems.push(webItem.itemModel);
    itemsAdded(addedItems);
}

/////////////////////////////////////////////////////////////////////////////
// Item의 크기 변경 혹은 카메라 이동 (HotSpot, FullScreen, Normal)
/////////////////////////////////////////////////////////////////////////////
var zoomFullScreen = false;
function toggleItemFullScreen(item)
{
    console.log("ViewingGridJS::toggleItemFullScreen()");

    // 백그라운드 아이템은 무시
    if (item.isBackground) {
        return;
    }

    // 한화면 보기 toggle 동작시에는 autoFitMode를 자동으로 켠다.
    gridViewArea.autoFit = true;

    var vpos;
    zoomFullScreen = !zoomFullScreen;
    if (zoomFullScreen) {
        var lx, rx, ty, by;
        lx = item.column;
        rx = item.column + item.columnCount;
        ty = item.row;
        by = item.row + pFocusedItem.rowCount;
        vpos = {left: lx, right: rx, top: ty, bottom: by}
    }
    else {
        vpos = _getItemsViewPosition();
    }

    var gridInfo = _setViewPos(vpos, true, 0, Easing.InBack);
    if (item.isVideoItem) {
        if (item.mediaStatus === WisenetMediaPlayer.Loaded) {
            var cellSize = _calculateCellSize(gridInfo.gridScale);
            var itemSize = _calculateItemSize(item, cellSize);

            item.player.updateOutputParam(viewingGridRoot.screenPixelRatio,
                                          itemSize.Width, itemSize.Height);
            if (item.isCamera) {
                item.player.checkDualStream();
            }
        }
    }

    mouseCanceled();
}

/////////////////////////////////////////////////////////////////////////////
// Item의 크기 변경 혹은 카메라 이동 (SingleChannelScreen)
/////////////////////////////////////////////////////////////////////////////
function setSingleChannelScreen(item)
{
    console.log("ViewingGridJS::setSingleChannelScreen()");

    // 백그라운드 아이템은 무시
    if (item.isBackground) {
        return;
    }

    if(zoomFullScreen) {
        return;
    }

    if(pFocusedItem === null) {
        return;
    }

    // 한화면 보기 toggle 동작시에는 autoFitMode를 자동으로 켠다.
    gridViewArea.autoFit = true;
    zoomFullScreen = !zoomFullScreen;

    var vpos;
    var lx, rx, ty, by;
    lx = item.column;
    rx = item.column + item.columnCount;
    ty = item.row;
    by = item.row + pFocusedItem.rowCount;
    vpos = {left: lx, right: rx, top: ty, bottom: by}

    var gridInfo = _setViewPos(vpos, true, 0, Easing.InBack);
    if (item.isVideoItem) {
        if (item.mediaStatus === WisenetMediaPlayer.Loaded) {
            var cellSize = _calculateCellSize(gridInfo.gridScale);
            var itemSize = _calculateItemSize(item, cellSize);

            item.player.updateOutputParam(viewingGridRoot.screenPixelRatio,
                                          itemSize.Width, itemSize.Height);
            if (item.isCamera) {
                item.player.checkDualStream();
            }
        }
    }

    mouseCanceled();
}

/////////////////////////////////////////////////////////////////////////////
// Item의 크기 변경 혹은 카메라 이동 (MultiChannelScreen)
/////////////////////////////////////////////////////////////////////////////
function setMultiChannelScreen(item)
{
    console.log("ViewingGridJS::setMultiChannelScreen()");

    // 백그라운드 아이템은 무시
    if (item.isBackground) {
        return;
    }

    if(!zoomFullScreen) {
        return;
    }

    if(pFocusedItem === null) {
        return;
    }

    // 한화면 보기 toggle 동작시에는 autoFitMode를 자동으로 켠다.
    gridViewArea.autoFit = true;
    zoomFullScreen = !zoomFullScreen;

    var vpos = _getItemsViewPosition();

    var gridInfo = _setViewPos(vpos, true, 0, Easing.InBack);
    if (item.isVideoItem) {
        if (item.mediaStatus === WisenetMediaPlayer.Loaded) {
            var cellSize = _calculateCellSize(gridInfo.gridScale);
            var itemSize = _calculateItemSize(item, cellSize);

            item.player.updateOutputParam(viewingGridRoot.screenPixelRatio,
                                          itemSize.Width, itemSize.Height);
            if (item.isCamera) {
                item.player.checkDualStream();
            }
        }
    }

    mouseCanceled();
}


/////////////////////////////////////////////////////////////////////////////
// 레이아웃 팬/틸트
/////////////////////////////////////////////////////////////////////////////
var panStatus = {
    ready: false,
    panning: false,
    startx: 0,
    starty: 0,
    mx: 0,
    my: 0,
    start: function (mousePos) {
        this.ready = true;
        this.startx = viewGridLines.gridX;
        this.starty = viewGridLines.gridY;
        this.mx = mousePos.x;
        this.my = mousePos.y;
        gridPanningChangeTimer.restart();
    },
    update: function (x, y) {
        if (this.ready === true) {
            //감도 조정
            if (this.panning === false && !gridPanningChangeTimer.running) {
                this.panning = true;
            }

            if (gridMouseArea.cursorShape !== Qt.ClosedHandCursor)
                gridMouseArea.cursorShape = Qt.ClosedHandCursor;

            var dx = (this.mx - x);
            var dy = (this.my - y);

            if (gridAnimation.running)
                gridAnimation.stop();

            viewGridLines.gridX -= dx;
            viewGridLines.gridY -= dy;
            this.mx = x;
            this.my = y;

            if (this.panning) {
                gridViewArea.autoFit = false;
            }
        }
    },
    updatePanning: function() {
        if (this.ready === true) {
            this.panning = true;
            if (gridMouseArea.cursorShape !== Qt.ClosedHandCursor)
                gridMouseArea.cursorShape = Qt.ClosedHandCursor;
        }
    },
    stop: function() {
        if (this.ready && !this.panning) {
            if (gridViewArea.autoFit) {
                fitAndMediaCheck(false);
            }
            else if (this.startx !== viewGridLines.gridX || this.starty !== viewGridLines.gridY) {
                gridAnimation.startPtz(this.startx, this.starty, viewGridLines.gridScale);
            }
        }

        this.reset();
    },
    reset: function() {
        this.ready = false;
        this.panning = false;
        this.mx = 0;
        this.my = 0;
        this.startx = 0;
        this.starty = 0;
    }
};

/////////////////////////////////////////////////////////////////////////////
// 특정 아이템 리사이징
/////////////////////////////////////////////////////////////////////////////
var resizingStatus = {
    ready: false,
    targetItem : null,
    enable_x0 : false,
    enable_x1 : false,
    enable_y0 : false,
    enable_y1 : false,
    mx: 0, my: 0,

    start: function(mousePos) {
        if (this.ready && this.targetItem) {
            //console.log("resizingStatus start()");
            this.mx = mousePos.x;
            this.my = mousePos.y;
        }
    },

    update: function(x, y) {
        if (this.ready && this.targetItem) {
            //console.log("resizingStatus update()");
            if (!this.targetItem.positionChanging)
                this.targetItem.setPositionChanging(true);

            var dx = x - this.mx;
            var dy = y - this.my;

            // 임시로 값을 계산 한 후 예외처리 후에 반영한다.
            var tempX = this.targetItem.tempX;
            var tempY = this.targetItem.tempY;
            var tempW = this.targetItem.tempW;
            var tempH = this.targetItem.tempH;

            if (this.enable_x0) {
                tempX += dx;
                tempW -= dx;
            }
            if (this.enable_y0) {
                tempY += dy;
                tempH -= dy;
            }
            if (this.enable_x1) {
                tempW += dx;
            }
            if (this.enable_y1) {
                tempH += dy;
            }


            this.mx = x;
            this.my = y;

            // 너무 작아지지 않도록 예외처리
            if (tempW < viewGridLines.minCellSize || tempH < viewGridLines.minCellSize) {
                return;
            }

            if (this.targetItem.targetColumnCount === 1) {
                if (tempW < (viewGridLines.cellWidth/2)) {
                    return;
                }
            }
            if (this.targetItem.targetRowCount === 1) {
                if ( tempH < (viewGridLines.cellHeight/2)) {
                    return;
                }
            }

            // 변경된 값으로 업데이트
            this.targetItem.tempX = tempX;
            this.targetItem.tempY = tempY;
            this.targetItem.tempW = tempW;
            this.targetItem.tempH = tempH;

            var limit = 2;
            var ltX = _getGridColumn(this.targetItem.tempX+limit);
            var ltY = _getGridRow(this.targetItem.tempY+limit);
            var rbX = _getGridColumn(this.targetItem.tempX+this.targetItem.tempW-limit);
            var rbY = _getGridRow(this.targetItem.tempY+this.targetItem.tempH-limit);

            this.targetItem.targetColumn = ltX;
            this.targetItem.targetRow = ltY;
            this.targetItem.targetColumnCount = rbX-ltX+1;
            this.targetItem.targetRowCount = rbY-ltY+1;
            this.targetItem.mapToTargetItemPos();
            //console.log("count=", this.targetItem.targetColumnCount, rbX, ltX);

        }
    },
    stop: function() {
        if (this.ready) {
            if (this.targetItem) {
                var items = [this.targetItem];
                if (_moveItemGeometry(items)) {
                    fitAndMediaCheck(true, true, 160);
                }
            }

            this.reset();
        }
    },
    reset: function() {
        this.ready = false;
        this.targetItem = null;
        this.enable_x0 = false;
        this.enable_x1 = false;
        this.enable_y0 = false;
        this.enable_y1 = false;
    }
};

/////////////////////////////////////////////////////////////////////////////
// 아이템 이동
/////////////////////////////////////////////////////////////////////////////
var itemMoveStatus = {
    ready: false,
    updating: false,
    mx: 0, my: 0,
    focusedIndex: 0,

    start: function(mousePos) {
        if (!viewingGridRoot.isEditable) {
            return;
        }
        this.mx = mousePos.x;
        this.my = mousePos.y;
        this.ready = true;
        console.log("ViewingGridJS::itemMoveStatus.check() item move ready!!", selectedItems.length);
        itemPosChangeTimer.restart();
    },
    check: function(item, mousePos) {
        this.ready = false;
        if (!viewingGridRoot.isEditable) {
            return false;
        }
        for (var i = 0 ; i < selectedItems.length ; i++) {
            if (item === selectedItems[i]) {
                this.focusedIndex = i;
                this.start(mousePos);
                return true;
            }
        }
        //console.log('not found selected items=', selectedItems.length)
        return false;
    },
    update: function(x,y) {
        if (this.ready) {
            var fakeUpdate = itemPosChangeTimer.running;
            this.updating = !fakeUpdate;

            var dx = (x - this.mx);
            var dy = (y - this.my);

            //console.log("move update::", selectedItems.length, dx, dy);
            for (var i = 0 ; i < selectedItems.length ; i++) {
                var item = selectedItems[i];
                if (!item) {
                    console.log("ViewingGridJS::itemMoveStatus.update() invalid selected item, index=", i);
                    continue;
                }

                if (item.fakeUpdaing !== fakeUpdate) {
                    item.setFakeUpdating(fakeUpdate);
                }

                if (!item.positionChanging) {
                    item.setPositionChanging(true);
                }

                item.tempX += dx;
                item.tempY += dy;

                if (!fakeUpdate) {
                    var tempRight = item.tempX + item.tempW;
                    var tempBottom = item.tempY + item.tempH;

                    var start_col = _getGridColumn(item.tempX);
                    var start_row = _getGridRow(item.tempY);
                    var end_col = _getGridColumn(tempRight);
                    var end_row = _getGridRow(tempBottom);

                    var start_cx = _getCenterOfColumn(start_col);
                    var start_cy = _getCenterOfRow(start_row);

                    var end_cx = (start_col === end_col) ? start_cx : _getCenterOfColumn(end_col);
                    var end_cy = (start_row === end_row) ? start_cy : _getCenterOfRow(end_row);


                    if (item.tempX < start_cx)
                        item.targetColumn = start_col;
                    else if (tempRight > end_cx)
                        item.targetColumn = end_col - item.targetColumnCount + 1;

                    if (item.tempY < start_cy)
                        item.targetRow = start_row;
                    else if (tempBottom > end_cy)
                        item.targetRow = end_row - item.targetRowCount + 1;

                    item.mapToTargetItemPos();
                }
            }
            this.mx = x;
            this.my = y;
        }
    },
    stop: function() {
        if (this.ready) {
            itemPosChangeTimer.stop();
            // 타이머내에서 마우스 릴리즈시 아이템 위치를 원상복구한다.
            if (!this.updating) {
                //console.log("ViewingGridJS::itemMoveStatus.stop()", this.updating);
                var slength = selectedItems.length;
                for (var i = 0 ; i < slength ; i++) {
                    var item = selectedItems[i];
                    if (item) {
                        item.setPositionChanging(false);
                    }
                }

                //복수선택시 선택영역을 해제한다.
                if (slength > 1 && this.focusedIndex < slength) {
                    //console.log("ViewingGridJS::itemMoveStatus.stop(), clear selectedItems", this.updating);
                    selectStatus.setSingleFocusedItem(selectedItems[this.focusedIndex]);
                    selectStatus.reset();
                }
                this.reset();
                return false;
            }

            if (_moveItemGeometry(selectedItems))
                fitAndMediaCheck(true, true, 160);
            this.reset();
            return true;
        }
    },
    reset: function() {
        this.ready = false;
        this.updating = false;
        this.mx = 0;
        this.my = 0;
        this.focusedIndex = 0;
    }

}

/////////////////////////////////////////////////////////////////////////////
// 아이템 선택 상태
/////////////////////////////////////////////////////////////////////////////
var selectStatus = {
    ready: false,
    start_x: 0, start_y: 0,
    tmpItems: [],

    start: function(mousePos) {
        //console.log("selectStatus start()");
        this.clearSelectedItems(); /* todo : shift key */
        this.ready = true;
        this.start_x = mousePos.x;
        this.start_y = mousePos.y;
        this.tmpItems = [];
    },
    update: function(x, y) {
        if (this.ready) {
            //console.log("selectStatus update()");
            selectTargetRect.visible = true;
            selectTargetRect.width = (Math.abs (x - this.start_x));
            selectTargetRect.height =(Math.abs (y - this.start_y));
            selectTargetRect.x = (x < this.start_x) ? x : this.start_x;
            selectTargetRect.y = (y < this.start_y) ? y : this.start_y;

            this.tmpItems = [];
            for (var i = 0 ; i < gridItems.length ; i ++) {
                var item = gridItems[i];
                if (item) {
                    if (_rectCollisionTest(selectTargetRect, item)) {
                        item.selected = true;
                        this.tmpItems.push(item);
                    }
                    else {
                        item.selected = false;
                    }
                }
            }
        }
    },
    stop: function() {
        if (this.ready) {
            if (this.tmpItems.length === 0) {
                this.clearSelectedItems();
                return;
            }

            var findFocused = false;
            for (var i = 0 ; i < this.tmpItems.length ; i ++) {
                var item = this.tmpItems[i];
                if (item === pFocusedItem) {
                    findFocused = true;
                    break;
                }
            }

            if (!findFocused && this.tmpItems.length > 0) {
                _setFocusedItem(this.tmpItems[0])
            }

            // 선택한 아이템이 한개인 경우에는 선택영역은 표시하지 않는다.
            if (this.tmpItems.length === 1)
                pFocusedItem.selected = false;

            selectedItems = this.tmpItems;
            //console.log("ViewingGridJS::selectStatus stop(): selectedItems", selectedItems, selectedItems.length)
            this.reset();
        }
    },
    reset : function() {
        this.ready = false;
        selectTargetRect.visible = false;
        this.start_x = 0;
        this.start_y = 0;
        this.tmpItems = [];
    },

    // 선택목록을 포커스아이템을 제외하고 초기화한다.
    clearSelectedItems: function() {
        for (var i = 0 ; i < selectedItems.length ; i ++) {
            var item = selectedItems[i];
            if (item) {
                item.selected = false;
            }
        }

        if (pFocusedItem) {
            if (selectedItems.length == 1) {
                if (selectedItems[0] !== pFocusedItem)
                    selectedItems[0] = pFocusedItem;
            }
            else {
                selectedItems = [pFocusedItem];
            }
        }
        else if (selectedItems.length > 0) {
            selectedItems = [];
        }

        this.reset();
    },

    // 싱글 아이템 선택시 포커스 아이템을 변경하고, 선택아이템 목록을 초기화한다.
    setSingleFocusedItem: function(item) {
        if (pFocusedItem !== item) {
            _setFocusedItem(item);
            item.selected = false;
            this.clearSelectedItems();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// 드래그&드랍 아이템 상태
/////////////////////////////////////////////////////////////////////////////
var dropStatus = {
    ready: false,
    mColumn: 0, mRow: 0,

    // 유효한 드래그 아이템인지
    util_isValidDragItem: function(drag) {
        if (drag.source) {
            if (this.util_isDragItemListModel(drag.source)) {
                return true;
            }
            else if (drag.hasUrls){
                var srcUrl = drag.urls[0].toString();
                if (srcUrl === "wisenetviewer://dragsource/cameras") {
                    return true;
                }
            }
        }
        else if (drag.hasUrls) {
            return true;
        }
        return false;
    },
    // DragItemListModel 소스인지 확인
    util_isDragItemListModel: function(item) {
      return item instanceof DragItemListModel
    },
    check: function(drag) {
        var accepted = this.util_isValidDragItem(drag);
        if (accepted) {
            var dragX = drag.x + gridViewArea.x
            var dragY = drag.y + gridViewArea.y
            this.start(dragX, dragY);
        }
        else {
            this.reset();
        }
        return accepted;
    },
    start: function(x, y) {
        this.ready = true;
        var start_col = _getGridColumn(x);
        var start_row = _getGridRow(y);
        this.mColumn = start_col
        this.mRow = start_row
        dropTargetRect.draw(this.mColumn, this.mRow);
    },
    update: function(x, y) {
        if (this.ready) {
            var start_col = _getGridColumn(x);
            var start_row = _getGridRow(y);
            if (this.mColumn !== start_col || this.mRow !== start_row) {
//                console.log("dropStatus ", start_col, start_row, x, y);
                this.mColumn = start_col
                this.mRow = start_row
                dropTargetRect.draw(this.mColumn, this.mRow);
            }
        }
    },
    stop: function(drop) {
        var accepted = false;
        if (this.ready) {
            var dropX = drop.x + gridViewArea.x
            var dropY = drop.y + gridViewArea.y
            var start_col = _getGridColumn(dropX);
            var start_row = _getGridRow(dropY);

            // Set base location as 0,0
            if(isMaskingMode){
                start_col = 0
                start_row = 0
            }

            // internal drag & drop
            if (drop.source) {
                if (this.util_isDragItemListModel(drop.source)) {
                    var dragItemListModel = drop.source;
                    accepted = addNewDropItems(dragItemListModel, start_col, start_row)
                }
                // only for test
                else if (drop.hasUrls && viewingGridRoot.isEditable){
                    var srcUrl = drop.urls[0].toString();
                    if (srcUrl === "wisenetviewer://dragsource/cameras") {
                        var devices = drop.source;
                        accepted = true;
                        addNewVideoChannels(devices);
                    }
                }
            }
            // file drag & drop
            else if (drop.hasUrls && viewingGridRoot.isEditable) {
                var validList = [];
                for(var i = 0 ; i < drop.urls.length ; i++) {
                    srcUrl = drop.urls[i];
                    if (layoutModel.isAcceptableUrl(srcUrl)) {
                        console.log("ViewingGrid::open url:", srcUrl);
                        validList.push(srcUrl);
                    }
                }
                if (validList.length > 0) {
                    accepted = true;
                    addNewLocalFiles(validList, true, start_col, start_row);
                }
            }
        }
        this.reset();
        return accepted;
    },
    reset : function() {
        this.ready = false;
        this.mColumn = 0;
        this.mRow = 0;
        dropTargetRect.reset();
    }
}

/////////////////////////////////////////////////////////////////////////////
// 마우스 위치에 레이아웃 아이템이 있는지 확인한다.
/////////////////////////////////////////////////////////////////////////////
function checkMouseOverItem(mx, my)
{
    if (resizingStatus.ready) {
        resizingStatus.reset();
    }

    var item = getMouseOverItem(mx, my);
    if (item) {
        //console.log("mouseoveritem=", item.x, item.y, item.width, item.height, mx, my);
        // 마우스 오버 상태의 아이템은 컨트롤버튼과 추가정보를 표시한다.
        if (pMouseOverItem !== item) {
            if (pMouseOverItem) {
                pMouseOverItem.mouseOver = false;
            }
            item.mouseOver = true;
            pMouseOverItem = item;
        }

        // 마우스 리사이즈 영역에 있는지 확인한다.
        if (viewingGridRoot.isEditable && !viewingGridRoot.isMaskingMode) {
            if (_checkMouseResizable(mx, my, item)) {
                resizingStatus.ready = true;
                resizingStatus.targetItem = item;
            }
        }
    }
    else if (pMouseOverItem) {
        pMouseOverItem.mouseOver = false;
        pMouseOverItem = null;
    }

    if (!resizingStatus.ready)
        gridMouseArea.cursorShape = Qt.ArrowCursor;
}

function resetMouseOverItem()
{
    if (pMouseOverItem) {
        pMouseOverItem.mouseOver = false;
        pMouseOverItem = null;
    }
    resizingStatus.reset();
    gridMouseArea.cursorShape = Qt.ArrowCursor;
}


/////////////////////////////////////////////////////////////////////////////
// 마우스 버튼 down 후 release시 정리 하는 동작들
// 순서를 바꾸면 오동작할 수 있다.(주의)
/////////////////////////////////////////////////////////////////////////////
function mouseReleased(x, y)
{
    if (panStatus.ready) {
        panStatus.stop();
    }

    if (resizingStatus.ready) {
        resizingStatus.stop();
    }

    if (itemMoveStatus.ready) {
        itemMoveStatus.stop();
    }

    if (selectStatus.ready) {
        selectStatus.stop();
    }

    if (gridMouseArea.cursorShape !== Qt.ArrowCursor)
        gridMouseArea.cursorShape = Qt.ArrowCursor;
}

function mouseCanceled()
{
    panStatus.reset();
    selectStatus.reset();
    resizingStatus.reset();
    itemMoveStatus.reset();
}


// 포커스할 수 있는 아이템 찾기
function checkSingleFocusItem(item, mousePos)
{
    if (item) {
        selectStatus.setSingleFocusedItem(item);
        itemMoveStatus.start(mousePos);
        return true;
    }
    return false;
}

// 그리드라인, 백그라운드로고, 포커스 사각형 등 안내요소 표시/숨김처리
function showGuide(show)
{
    viewGridLines.showGrid(show);
    if (pFocusedItem) {
        pFocusedItem.updateFocusedRect(show);
    }
}


/////////////////////////////////////////////////////////////////////////////
// 마우스상에 엤는 아이템 확인
/////////////////////////////////////////////////////////////////////////////
function getMouseOverItem(mx, my)
{
    var bgItems = [];

    // 백그라운드 아이템이 아닌 것부터 체크
    for (var i = 0 ; i < gridItems.length ; i ++) {
        var item = gridItems[i];
        if (item) {
            if (item.isBackground) {
                bgItems.push(item);
                continue;
            }
            if (_isMouseIn(mx, my, item)) {
                return item;
            }
        }
    }

    // 못찾은 경우 백그라운드 아이템에서 다시 체크
    if (bgItems.length > 0) {
        for (i = 0 ; i < bgItems.length ; i++) {
            if (_isMouseIn(mx, my, bgItems[i])) {
                return bgItems[i];
            }
        }
    }
    return null;
}


/////////////////////////////////////////////////////////////////////////////
// 레이아웃 줌 인/아웃
/////////////////////////////////////////////////////////////////////////////
function zoom(mx, my, scaleDelta, anim)
{
    //console.log("ViewingGridJS::zoom()::", mx, my, scaleDelta, autoFitOff);

    var originX = mx - viewGridLines.gridX;
    var originY = my - viewGridLines.gridY;
    var zoomScale = viewGridLines.gridScale * scaleDelta;
    var cellPixelWidth = zoomScale*layoutModel.cellRatioW;

    // 셀의 폭이 min/max를 초과한 경우에는 예외처리
    var overflowed = false;
    if ((cellPixelWidth  < viewGridLines.minCellSize && scaleDelta < 1.0) ||
        (cellPixelWidth > viewGridLines.maxCellSize && scaleDelta > 1.0)) {
        // TODO:: 스케일값 조정
        console.log("ViewingGridJS::zoom(), overflowed wheel::", cellPixelWidth, scaleDelta);
        overflowed = true;
        return;
    }

    gridViewArea.autoFit = false;


    var px = viewGridLines.gridX  + (originX * (1 - scaleDelta));
    var py = viewGridLines.gridY  + (originY * (1 - scaleDelta));

    if (anim) {
        gridAnimation.startPtz(px, py, zoomScale, 0, 2, 500);
    }
    else {
        viewGridLines.gridScale = zoomScale;
        viewGridLines.gridX = px;
        viewGridLines.gridY = py;
    }

    /* 애니메이션 적용 안하고 바로 값 변경 시 공식
        viewGridLines.gridScale *= scaleDelta;
        viewGridLines.gridX += (originX * (1 - scaleDelta));
        viewGridLines.gridY += (originY * (1 - scaleDelta));
    */
}



/////////////////////////////////////////////////////////////////////////////
// 화면에 현재 view 맞추기
/////////////////////////////////////////////////////////////////////////////


// AUTO FIT + MEDIA CHECK 메인 함수
function fitAndMediaCheck(checkMedia = true, anim = true, delayedTime = 0)
{
    if (gridViewArea.autoFit) {
        var vpos = _getItemsViewPosition();
        var gridInfo = _setViewPos(vpos, anim, delayedTime);
        if (zoomFullScreen)
            zoomFullScreen = false;
        if (checkMedia) {
            checkMediaStatus(gridInfo.gridScale);
        }
    }
    else if (checkMedia) {
        checkMediaStatus();
    }
}


// 패턴에 맞추기 autofit 이 false일때
function patternInView(columns)
{
    console.log('ViewingGridJS::patternInView()!!', columns);

    var vpos = _getItemsViewPosition();
    vpos.right = vpos.left + columns;
    vpos.bottom = vpos.top + columns;

    patternAnimation.readyIndicate(vpos);
    _setViewPos(vpos, true);
}

function needToVideoUpdate(item)
{
    if (item && item.isVideoItem) {
        if (item.mediaStatus === WisenetMediaPlayer.NoMedia ||
            item.mediaStatus === WisenetMediaPlayer.Loaded ||
            item.mediaStatus === WisenetMediaPlayer.Loading) {
            return true;
        }
    }
    //console.log("needToVideoUpdate false!", item);
    return false;
}

function checkMediaStatus(gridScale=null, onlyUpdateOutputParam=false)
{
    if (gridScale) {
        var cellSize = _calculateCellSize(gridScale)
    }
    else {
        cellSize = _calculateCellSize(viewGridLines.gridScale);
    }
    for (var i = 0 ; i < gridItems.length ; i ++) {
        var item = gridItems[i];
        if (needToVideoUpdate(item)) {
            var itemSize = _calculateItemSize(item, cellSize);
            item.player.updateOutputParam(viewingGridRoot.screenPixelRatio,
                                          itemSize.Width, itemSize.Height);

            if (!onlyUpdateOutputParam) {
                if (item.mediaStatus === WisenetMediaPlayer.NoMedia) {
                    item.player.open();
                }
                else if (item.isCamera){
                    // 줌영역이 있는 아이템은 임의로 high/low 전환하지 않는다.
                    if (item.zoomAreas.length === 0) {
                        item.player.checkDualStream();
                    }
                }
            }
        }
    }
}

function checkProfileOfChannelItem(item)
{
    if (needToVideoUpdate(item)) {
        var cellSize = _calculateCellSize(viewGridLines.gridScale);
        var itemSize = _calculateItemSize(item, cellSize);

        item.player.updateOutputParam(viewingGridRoot.screenPixelRatio,
                                      itemSize.Width, itemSize.Height);
        if (item.mediaStatus === WisenetMediaPlayer.NoMedia) {
            item.player.open();
        }
        else if (item.isCamera){
            item.player.checkDualStream();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// 컨텍스트 메뉴 팝업
/////////////////////////////////////////////////////////////////////////////
function showContextMenu()
{
    console.log("ViewingGridJS::showContextMenu() enter, overItem=", pMouseOverItem);
    // 선택영역 위인지, 빈 레이아웃 영역인지 확인 후, 컨텍스트 메뉴 분기
    var viewLayoutMenu = pMouseOverItem ? false : true;

    if (!viewLayoutMenu) {
        // Don't use channel context menu
        if(viewingGridRoot.isMaskingMode)
            return;

        // 복수 선택 아이템들의 컨텍스트 메뉴인지 확인
        var isMultiple = false;
        if (selectedItems.length > 1) {
            for (var i = 0 ; i < selectedItems.length ; i++) {
                if (selectedItems[i] === pMouseOverItem) {
                    isMultiple = true;
                    break;
                }
            }
        }
        console.log("ViewingGridJS::showContextMenu(), isMultiple=", isMultiple);
        if (isMultiple) {
            selectedItemsContextMenu.show(selectedItems);
        }
        else {
            // 포커스 아이템 변경
            if (pMouseOverItem != pFocusedItem) {
                selectStatus.setSingleFocusedItem(pMouseOverItem)
            }

            if (pMouseOverItem.isImage) {
                imageItemContextMenu.show(pMouseOverItem);
            }
            else if (pMouseOverItem.isCamera) {
                videoItemContextMenu.show(pMouseOverItem);
            }
            else if (pMouseOverItem.isLocalResource) {
                videoItemContextMenu.show(pMouseOverItem);
            }
            else if (pMouseOverItem.isZoomTarget) {
                videoItemContextMenu.show(pMouseOverItem);
            }
            else if (pMouseOverItem.isWebpage) {
                webItemContextMenu.show(pMouseOverItem);
            }
        }

    }
    else {
        console.log("ViewingGridJS::showContextMenu(), layout context menu");
        layoutContextMenu.show(layoutModel);
    }
}

/////////////////////////////////////////////////////////////////////////////
// OpenInNewTab, OpenInNewWindow 등 새로운 레이아웃을 생성할 때
// 줌타겟 아이템의 경우, 소스 아이템을 포함해서 리스트를 올려주는 함수
/////////////////////////////////////////////////////////////////////////////
function getNewLayoutItemList(sourceItems)
{
    var zoomTargetItems = sourceItems.filter(sourceItem => sourceItem.isZoomTarget);
    console.log("ViewingGridJS::getNewLayoutItemList(), sources=", sourceItems.length,
                "zoomTargets=", zoomTargetItems.length);
    // 줌타겟이 없으면 그대로 리턴
    if (zoomTargetItems.length === 0) {
        console.log("ViewingGridJS::getNewLayoutItemList(), noZoomTarget");
        return sourceItems;
    }

    // 줌타겟이 있으면 소스에서 먼저 찾고, 소스에서 없으면 전체리스트에서 다시 찾는다.
    var resultItems = sourceItems.slice();
    for (var i = 0 ; i < zoomTargetItems.length ; i++) {
        var itemId = zoomTargetItems[i].itemModel.zoomTargetId;
        var fItem = resultItems.find(sourceItem => sourceItem.itemId === itemId);
        if (!fItem) {
            fItem = _findItem(itemId)
            if (fItem) {
                resultItems.push(fItem);
            }
        }
    }
    console.log("ViewingGridJS::getNewLayoutItemList(), results=", resultItems.length);
    return resultItems;
}

/////////////////////////////////////////////////////////////////////////////
// 동일 DEVICE에서 재생중인 채널아이템 리스트를 획득 (FOR NVR)
// TODO :: 추후 카메라 재생, 서버 재생 고려시에는 수정필요함 (DEVICE TYPE 확인 필요)
/////////////////////////////////////////////////////////////////////////////
function getPlaybackChannelsOfDevice(deviceId)
{
    var channelItems = [];
    for (var i = 0 ; i < gridItems.length ; i++) {
        var item = gridItems[i];
        //console.log("item info", item.isCamera, item.mediaSeekable, item.mediaParam.deviceId, deviceId);
        if (item && item.isCamera && item.mediaSeekable) {
            if (item.mediaParam.deviceId === deviceId) {
                channelItems.push(item);
            }
        }
    }
    return channelItems;
}



/////////////////////////////////////////////////////////////////////////////
// Sleep Mode ON/OFF
/////////////////////////////////////////////////////////////////////////////
function sleepMode(isOn)
{
    console.log('ViewingGridJS::sleepMode(), isOn', isOn, viewingGridRoot);
    for (var i = 0 ; i < gridItems.length ; i++) {
        var item = gridItems[i];
        if (item) {
            if (item.isVideoItem) {
                item.sleep(isOn);
            }
        }
    }
    //sleepModeStatusRect.updateStatus(isOn)
}


/* -----------------------------------------------------------------------
  #######################################################################
  Internal functions
  #######################################################################
  ----------------------------------------------------------------------- */

/////////////////////////////////////////////////////////////////////////////
// 아이템 아이디로 리스트에서 해당 아이템 찾기
/////////////////////////////////////////////////////////////////////////////
function _findItem(itemId)
{
    console.log('ViewingGridJS::_findItem() itemId=', itemId);
    for (var i = 0 ; i < gridItems.length ; i ++) {
        var item = gridItems[i];
        if (item) {
            if (item.itemId === itemId) {
                return item;
            }
        }
    }
    return null;
}

/////////////////////////////////////////////////////////////////////////////
// 디바이스 아이디, 채널 아이디로 채널아이템 찾기
/////////////////////////////////////////////////////////////////////////////
function _findChannelItem(deviceId, channelId)
{
    for (var i = 0 ; i < gridItems.length ; i ++) {
        var item = gridItems[i];
        if(item.isCamera) {
            if (item.itemModel.deviceId === deviceId && item.itemModel.channelId === channelId) {
                return item;
            }
        }
    }
    return null;
}

/////////////////////////////////////////////////////////////////////////////
// 아이템 카운트 재계산
// gridItems는 변경이 빈번하게 일어날 수 있어 별도 값을로 외부 노출한다.
/////////////////////////////////////////////////////////////////////////////
function _resetItemCount()
{
    gridItemsArea.itemCount = gridItems.length;
}


/////////////////////////////////////////////////////////////////////////////
// 포커스아이템 설정
/////////////////////////////////////////////////////////////////////////////
function _setFocusedItem(item)
{
    //console.log("_setFocusedItem!!");
    if (pFocusedItem === item)
        return;

    if (pFocusedItem) {
        pFocusedItem.selected = false;
        pFocusedItem.focusedTarget = false;

        if(pFocusedItem.isVideoItem && pFocusedItem.isSmartSearchDrawing) {
            pFocusedItem.stopSmartSearchDrawMode()  // focus 해제되는 Item의 그리기 모드 종료
        }
    }

    pFocusedItem = item;
    if (!pFocusedItem ) {
        focusedItem = nullItem;
    }
    else {
        pFocusedItem.focusedTarget = true;
        focusedItem = pFocusedItem;
    }
}


/////////////////////////////////////////////////////////////////////////////
// 셀사이즈, 아이템 사이즈 계산
/////////////////////////////////////////////////////////////////////////////
function _calculateCellSize(scale)
{
    var cellWidth = layoutModel.cellRatioW * scale;
    var cellHeight = layoutModel.cellRatioH * scale;
    return {Width: cellWidth, Height: cellHeight};
}

function _calculateItemSize(item, cellSize)
{
    var itemWidth = cellSize.Width*item.columnCount;
    var itemHeight = cellSize.Height*item.rowCount;
    return {Width: itemWidth, Height: itemHeight};
}


/////////////////////////////////////////////////////////////////////////////
// vpos 위치에 zoom & translate
/////////////////////////////////////////////////////////////////////////////
function _setViewPos(vpos, animation, delayedTime = 0, easing = 2)
{
    var xCount = vpos.right-vpos.left;
    var yCount = vpos.bottom-vpos.top;

    var xCellRatio = xCount*layoutModel.cellRatioW;
    var yCellRatio = yCount*layoutModel.cellRatioH;

    var viewRatio = gridViewArea.width / gridViewArea.height;
    var cellRatio = xCellRatio / yCellRatio;

    var calculatedCellWidth = 0;
    var calculatedCellHeight = 0;
    var dx = 0, dy = 0;

    var areaX = gridViewArea.x + gridViewArea.autoFitPadding
    var areaY = gridViewArea.y + gridViewArea.autoFitPadding
    var areaW = gridViewArea.width - gridViewArea.autoFitPadding2x;
    var areaH = gridViewArea.height - gridViewArea.autoFitPadding2x;

    if (viewRatio <= cellRatio) {
        calculatedCellWidth = areaW;
        calculatedCellHeight = areaW / xCellRatio  * yCellRatio;
        dy = (areaH - calculatedCellHeight) / 2;
    }
    else {
        calculatedCellWidth = areaH / yCellRatio * xCellRatio;
        calculatedCellHeight = areaH;
        dx = (areaW - calculatedCellWidth) / 2;
    }

    var scale = calculatedCellWidth / xCellRatio;
    var mx = areaX+dx-(vpos.left*scale*layoutModel.cellRatioW);
    var my = areaY+dy-(vpos.top*scale*layoutModel.cellRatioH);

    if (animation) {
        if (gridAnimation.running)
            gridAnimation.stop();
        gridAnimation.startPtz(mx, my, scale, delayedTime, easing);
    }
    else {
        viewGridLines.gridScale = scale;
        viewGridLines.gridX = mx;
        viewGridLines.gridY = my;
    }
    gridMouseArea.refreshShowGuide();
    return {gridX: mx, gridY: my, gridScale: scale};
}

/////////////////////////////////////////////////////////////////////////////
// 전체 아이템들이 위치한 좌표 정보
/////////////////////////////////////////////////////////////////////////////
function _getItemsViewPosition()
{
    var lx, rx, ty, by;

    for (var i = 0 ; i < gridItems.length ; i ++) {
        var item = gridItems[i];
        if (item) {
            if (lx === undefined) {
                lx = item.column;
                rx = item.column + item.columnCount;
                ty = item.row;
                by = item.row + item.rowCount;
                continue;
            }

            if (item.column < lx)
                lx = item.column;
            if (item.column + item.columnCount > rx)
                rx = item.column + item.columnCount;
            if (item.row < ty)
                ty = item.row;
            if (item.row + item.rowCount > by)
                by = item.row + item.rowCount;
        }
    }

    if (lx === undefined) {
        lx = 0; rx = 1; ty = 0 ; by = 1;
    }

//    console.log("left=",lx, "right=",rx, "top=",ty, "bottom=",by)

    return {left: lx, right: rx, top: ty, bottom: by};
}


/////////////////////////////////////////////////////////////////////////////
// 마우스 좌표에서 그리드 행/열의인덱스 정보값 획득
/////////////////////////////////////////////////////////////////////////////
// 열
function _getGridColumn(mx)
{
    var ex = (mx - viewGridLines.gridX) / viewGridLines.cellWidth;
    return Math.floor(ex);
}

// 행
function _getGridRow(my)
{
    var ey = (my - viewGridLines.gridY) / viewGridLines.cellHeight;
    return Math.floor(ey);
}



/////////////////////////////////////////////////////////////////////////////
// 충돌 테스트
/////////////////////////////////////////////////////////////////////////////
// 사각영역간 충돌 테스트
function _rectCollisionTest(r1, r2)
{
    if (r1.x < r2.x+r2.width && r1.x+r1.width > r2.x &&
        r1.y < r2.y+r2.height && r1.y+r1.height > r2.y)
        return true;
    return false;
}

// grid cell 충돌 테스트
function _cellCollistionTest(col, colCount, row, rowCount,
                            col2, colCount2, row2, rowCount2)
{
    if (col < col2 + colCount2 && col+colCount > col2 &&
            row < row2 + rowCount2 && row+rowCount > row2) {
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// 셀 column, row의 중점 x, y 좌표
/////////////////////////////////////////////////////////////////////////////
function _getCenterOfColumn(lX)
{
    return (lX*viewGridLines.cellWidth)+viewGridLines.gridX + viewGridLines.cellWidth/2;
}

function _getCenterOfRow(lY)
{
    return (lY*viewGridLines.cellHeight)+viewGridLines.gridY + viewGridLines.cellHeight/2;
}

/////////////////////////////////////////////////////////////////////////////
// 마우스가 아이템 크기 조정 영역에 있는지 확인
/////////////////////////////////////////////////////////////////////////////
function _checkMouseResizable(mx, my, item)
{
    var offset = item.width >= 640 ? 8 : (item.width <= 100 ? 4 : 6);

    // 안쪽 사각 체크
    if (mx <= (item.x+offset) || mx >= (item.x+item.width-offset) ||
        my <= (item.y+offset) || my >= (item.y+item.height-offset)) {

        // 3등분한 9구역에 대해서 계산
        var base = item.width >= item.height ? item.height : item.width;
        var div = base / 6; // 모서리 기준 값

        var x_1 = item.x + div;
        var x_2 = item.x+item.width - div;
        var y_1 = item.y + div;
        var y_2 = item.y+item.height - div;

        // left top
        if (mx <= x_1 && my <= y_1) {
            resizingStatus.enable_x0 = true;
            resizingStatus.enable_y0 = true;
            gridMouseArea.cursorShape = Qt.SizeFDiagCursor;
            return true;
        }
        // right bottom
        if (mx >= x_2 && my >= y_2) {
            resizingStatus.enable_x1 = true;
            resizingStatus.enable_y1 = true;
            gridMouseArea.cursorShape = Qt.SizeFDiagCursor;
            return true;
        }

        // right top
        if (mx >= x_2 && my <= y_1) {
            resizingStatus.enable_x1 = true;
            resizingStatus.enable_y0 = true;
            gridMouseArea.cursorShape = Qt.SizeBDiagCursor;
            return true;
        }

        // left bottom
        if (mx <= x_1 && my >= y_2) {
            resizingStatus.enable_x0 = true;
            resizingStatus.enable_y1 = true;
            gridMouseArea.cursorShape = Qt.SizeBDiagCursor;
            return true;
        }

        // top
        if (my <= y_1) {
            resizingStatus.enable_y0 = true;
            gridMouseArea.cursorShape = Qt.SizeVerCursor;
            return true;
        }

        // bottom
        if (my >= y_2) {
            resizingStatus.enable_y1 = true;
            gridMouseArea.cursorShape = Qt.SizeVerCursor;
            return true;
        }

        // left
        if (mx <= x_1) {
            resizingStatus.enable_x0 = true;
            gridMouseArea.cursorShape = Qt.SizeHorCursor;
            return true;
        }

        // right
        resizingStatus.enable_x1 = true;
        gridMouseArea.cursorShape = Qt.SizeHorCursor;
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
// 아이템 위치 및 크기 변경
/////////////////////////////////////////////////////////////////////////////
function _moveItemGeometry(items)
{
    var i = 0;
    var j = 0;
    var updatedItems = [];
    var newArrangeItems = [];

    for (i = 0 ; i < items.length ; i++) {
        var sourceItem = items[i];

        if (sourceItem.setPositionChanging(false)) {
            updatedItems.push(sourceItem.itemModel);

            // 소스가 백그라운드 이미지인 경우 제외
            if (sourceItem.isBackground) {
                continue;
            }

            for (j = 0 ; j < gridItems.length ; j++) {
                var targetItem = gridItems[j];

                // 자기 자신은 제외
                if (sourceItem === targetItem) {
                    continue;
                }

                // 유효하지 않은 다른 아이템 제외
                if (!_isValidPositionItem(targetItem)) {
                    continue;
                }

                // 기존 아이템들과 위치 중복되는지 확인
                if (_cellCollistionTest(sourceItem.column, sourceItem.columnCount,
                                       sourceItem.row, sourceItem.rowCount,
                                       targetItem.column, targetItem.columnCount,
                                       targetItem.row, targetItem.rowCount)) {
                    targetItem.itemModel.invalid = true;
                    newArrangeItems.push(targetItem);
                }
            }
        }
    }

    // 충돌 아이템 재배치
    for (i = 0 ; i < newArrangeItems.length ; i++) {
        var aItem = newArrangeItems[i];
        var pos = layoutModel.getNewItemPosition(aItem.columnCount, aItem.rowCount);
        aItem.itemModel.invalid = false;
        aItem.movePosition(pos.x, pos.width, pos.y, pos.height);
        updatedItems.push(aItem.itemModel);
    }

    if (updatedItems.length > 0) {
        itemsUpdated(updatedItems);
        return true;
    }
    return false;
}



// 마우스가 해당 아이템 상에 위치하는지 여부
function _isMouseIn(mx, my, item) {
    if (mx > item.x && mx < (item.x+item.width) && my > item.y && my < (item.y+item.height))
        return true;
    return false;
}

// 충돌체크 등 비교아이템이 유효한지 판단
function _isValidPositionItem(targetItem) {
    // 이동중인 다른 아이템 및 백그라운드 이미지 제외
    if (targetItem.positionChanging ||
        targetItem.itemModel.invalid ||
        targetItem.isBackground) {
        return false;
    }
    return true;
}

// 새로운 아이템리스트로 리셋
function _resetNewItems(newItems)
{
    if (newItems.length > 0) {
        gridItems = newItems;
        var newSelectedItems = [];
        var newFocusedItem = null;

        for (var i = 0 ; i < gridItems.length ; i++) {
            var item = gridItems[i];
            if (item.selected || item.focusedTarget) {
                newSelectedItems.push(item);
                if (item.focusedTarget) {
                    newFocusedItem = item;
                }
            }
        }

        if (newSelectedItems.length === 0) {
            newSelectedItems.push(gridItems[0]);
            newFocusedItem = gridItems[0];
        }

        if (!newFocusedItem)
            newFocusedItem = newSelectedItems[0];

        selectedItems = newSelectedItems;
        _setFocusedItem(newFocusedItem);

        mouseCanceled();
        fitAndMediaCheck();
        _resetItemCount();
    }
    else {
        gridItems = [];
        cleanUp();
    }
}

// 현재 비디오 아이템 갯수
function _getVideoItemsCount()
{
    var count = 0;
    for (var i = 0 ; i < gridItems.length ; i++) {
        var item = gridItems[i];
        if (item && item.isVideoItem) {
            count++;
        }
    }
    return count;
}

// 현재 웹 아이템 갯수
function _getWebItemsCount()
{
    var count = 0;
    for (var i = 0 ; i < gridItems.length ; i++) {
        var item = gridItems[i];
        if (item && item.isWebpage) {
            count++;
        }
    }
    return count;
}
