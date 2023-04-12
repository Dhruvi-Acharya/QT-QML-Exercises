import QtQuick 2.15
import QtQuick.Controls 2.5
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import "Resource"
import "Resource/ContextMenu"
import "Event"
import "qrc:/"
import "qrc:/MediaController/"
import WisenetMediaFramework 1.0
import Wisenet.DeviceListModel 1.0

Rectangle{
    id: monitoringView
    anchors.fill: parent
    visible: true
    color: "transparent";//WisenetGui.borderColor

    property int viewX : leftSplitBar.x
    property int viewWidth : rightSplitBar.x - leftSplitBar.x - rightSplitBar.width
    property int viewY : 0
    property int viewHeight : height - mediaControllerView.height

    property int previousTreeSplitterY
    property int previousLeftBarX
    property int previousRightBarX
    readonly property int leftPanelMaxWidth: 600
    readonly property int leftPanelMinWidth: 240
    readonly property int rightPanelMaxWidth: 350
    readonly property int rightPanelMinWidth: 240

    property int splitBarMouseAreaWidth : 10

    property bool isSequenceSelected : false
    property var tabItemId
    property var tabName

    property bool treeDragStarted

    signal selectLayout(var type, var name, var uuid, var layoutViewModel)
    signal selectSequence(var type, var name, var uuid)
    signal sequencePause(var tabItemId)
    signal sequencePlay(var tabItemId)
    signal addTabCurrentLayout()

    property var selectedViewingGrid: null
    property var focusedViewingGridItem: null

    property alias resourceView : resourceTree
    property alias viewingGridView : viewingGridBackground
    property alias mediaControllerView: mediaControllerView

    property int shortcutCount: 0

    signal eventSearchPopupOpen(var menuNumber)
    signal openSetup()
    signal fullScreenModeChanged(var onOff)
    signal openDashboard()
    signal openMaking()

    // 새탭에서 아이템 열기
    signal openInNewTabTriggered(var items);

    // 새탭만들기
    signal openNewTabTriggered();
    signal closeTabTriggered();

    // 초기 비밀번호 설정
    signal openInitializeDeviceCredential()

    // P2P register
    signal openP2pRegister()

    signal verifyFinished()

    onSelectedViewingGridChanged: {
    }
    onFocusedViewingGridItemChanged: {
    }

    function tabClosedWithoutSave(closedLayouts){
        layoutTree.tabClosedWithoutSave(closedLayouts)
    }

    function tabChanged(itemType, itemId, name)
    {
        if(itemType === ItemType.Sequence) {
            console.log("MonitoringView::tabChanged() Sequence name :",name," itemId :",itemId)
            monitoringView.isSequenceSelected = true
            mediaControllerView.showAndHide(false)
            mediaControllerView.visible = false
        }
        else {
            console.log("MonitoringView::tabChanged() Layout name :",name," itemId :",itemId)
            monitoringView.isSequenceSelected = false
            mediaControllerView.showAndHide(true)
            mediaControllerView.visible = true
        }

        monitoringView.tabItemId = itemId
        monitoringView.tabName = name
    }

    function sequencePlayStatusChanged(isPlaying) {
        console.log("sequencePlayStatusChanged::isPlaying :",isPlaying)
        if(isPlaying) {
            playPauseButton.state = "playing"
        }
        else {
            playPauseButton.state = "paused"
        }
    }

    function openRenameDialog(itemType, uuid, name)
    {
        renameDialog.itemType = itemType
        renameDialog.text = name
        renameDialog.uuid = uuid

        renameDialog.showDialog()
    }

    function flipLeftView() {
        leftViewWidthBeforeHide = leftView.width
        leftView.state = "HIDE"
        leftView.width = 0
        shortcutBoxView.visible = false

        // hide시 horizontal scroll 남아있는 문제 수정, column width를 0으로 바꾸는 걸로 시도했으나 동작안함.
        resourceTree.horizontalScrollBarPolicy = Qt.ScrollBarAlwaysOff
        layoutTree.horizontalScrollBarPolicy = Qt.ScrollBarAlwaysOff
    }

    function flipRightView() {
        rightViewWidthBeforeHide = rightView.width
        rightView.state = "HIDE"
        rightView.width = 0
    }

    function showAndHideLeftView(show){
        if (!show)
        {
            if(leftView.state == "OPEN")
                leftViewWidthBeforeHide = leftView.width
            leftView.state = "HIDE"
            leftView.width = 0
            shortcutBoxView.visible = false

            // hide시 horizontal scroll 남아있는 문제 수정, column width를 0으로 바꾸는 걸로 시도했으나 동작안함.
            resourceTree.horizontalScrollBarPolicy = Qt.ScrollBarAlwaysOff
            layoutTree.horizontalScrollBarPolicy = Qt.ScrollBarAlwaysOff
        }
        else
        {
            leftView.state = "OPEN"
            leftView.width = leftViewWidthBeforeHide
            shortcutBoxView.visible = true

            // hide시 horizontal scroll 남아있는 문제 수정
            resourceTree.horizontalScrollBarPolicy = Qt.ScrollBarAsNeeded
            layoutTree.horizontalScrollBarPolicy = Qt.ScrollBarAsNeeded
        }
    }

    function showAndHideRightView(show)
    {
        if (!show)
        {
            if(rightView.state == "OPEN")
                rightViewWidthBeforeHide = rightView.width
            rightView.state = "HIDE"
            rightView.width = 0
        }
        else
        {
            rightView.state = "OPEN"
            rightView.width = rightViewWidthBeforeHide
        }
    }

    property bool leftViewVisibleBeforeFullScreen
    property bool rightViewVisibleBeforeFullScreen
    property bool mediaControllerVisibleBeforeFullScreen
    function changeFullScreenMode(onOff)
    {
        if(onOff){
            // ON Full screen mode
            leftViewVisibleBeforeFullScreen = leftView.state === "OPEN"
            rightViewVisibleBeforeFullScreen = rightView.state === "OPEN"
            mediaControllerVisibleBeforeFullScreen = mediaControllerView.state === "show"

            showAndHideLeftView(false)
            showAndHideRightView(false)
            mediaControllerView.showAndHide(false)
        }
        else{
            // OFF Full screen mode
                showAndHideLeftView(true)

                showAndHideRightView(true)

                mediaControllerView.showAndHide(true)
        }

        fullScreenModeChanged(onOff)
    }

    function closeRightPanelFilterPopups(){
        rightPanel.closeFilterPopups()
    }

    onVisibleChanged: {
        if(visible)
            treePanel.adjustTree()
    }

    Component.onCompleted: {
        console.log("MonitoringView Compoonent.onCompleted")
        loadShortcutMenu()
    }

    Connections{
        target: shortcutBoxListModel
        onSettingChanged: loadShortcutMenu()
    }

    function loadShortcutMenu() {
        const useList = shortcutBoxListModel.getData()
        for (var i=0; i<useList.length; i++) {
            shortcutBoxView.setShortcutVisible(i, useList[i])
        }
    }

    Connections{
        target: selectedViewingGrid ? selectedViewingGrid : null

        // 레이아웃, 레이아웃 아이템 변경시 리소스에서 관리하는 레이아웃 모델 데이터 동기화
        // 아이템 추가
        onItemsAdded:{
            console.log("selectedViewingGrid::onItemsAdded!!", items);
            layoutTree.addLayoutItems(selectedViewingGrid.layout.layoutId, items)
        }

        // 아이템 삭제
        onItemsRemoved:{
            console.log("selectedViewingGrid::onItemsRemoved!!", itemIds);
            layoutTree.removeLayoutItems(selectedViewingGrid.layout.layoutId, itemIds)
        }

        // 아이템 변경(이동,리사이징,아이템 속성)
        onItemsUpdated: {
            //console.log("selectedViewingGrid::onItemsUpdated!!", items);
            layoutTree.updateLayoutItems(selectedViewingGrid.layout.layoutId, items)
        }

        // 레이아웃 설정값 변경 (셀종횡비, lock 설정 등)
        onLayoutPropertyUpdated: {
            console.log("selectedViewingGrid::onLayoutPropertyUpdated!!", layoutModel);
            layoutTree.updateLayoutProperty(selectedViewingGrid.layout.layoutId, layoutModel)
        }

        // 전체화면 on/off
        onShowFullscreen: {
            console.log("selectedViewingGrid::onShowFullscreen!!", onOff);
            monitoringView.changeFullScreenMode(onOff)
        }
        // 새탭에서 열기
        onOpenInNewTabTriggered: {
            console.log("selectedViewingGrid::onOpenInNewTabTriggered!!", items);
            monitoringView.openInNewTabTriggered(items);
        }

        // 새창에서 열기
        onOpenInNewWindowTriggered: {
            console.log("selectedViewingGrid::onOpenInNewWindowTriggered!!", items);
            windowHandler.initializeNewWindowWithGridItems(items)
        }

        // 이름바꾸기 (카메라, 웹페이지)
        onEditNameTriggered: {
            console.log("onEditNameTriggered", item, item.itemModel.deviceId, item.itemModel.channelId);
            // 카메라 이름 바꾸기
            if (item.isCamera) {
                var uuid = item.itemModel.deviceId+"_"+item.itemModel.channelId;
                var type = ItemType.Camera;
                var name = item.mediaName;
                monitoringView.openRenameDialog(type, uuid, name);
            }
            // 웹페이지 이름 바꾸기
            else if (item.isWebpage) {
                uuid = item.itemModel.webPageId;
                type = ItemType.WebPage;
                name = item.itemModel.webPageName;
                monitoringView.openRenameDialog(type, uuid, name);
            }
        }
        onNvrSetupOpenTriggered:{
            console.log("selectedViewingGrid::onCameraSetupOpenTriggered!!", items);
            var deviceGuid = [];
            for (var i = 0 ; i < items.length ; i++) {
                var device = items[i].deviceId
                console.log("  deviceId=", items[i].deviceId, ", channelId=", items[i].channelId, ", complexChannel=",device);
                deviceGuid.push(device);
            }
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceList)
            windowService.setupMainView_SelectDeviceListChannel(deviceGuid)
            //emit cameraSetupSelectedChannels()
        }

        // 카메라 설정 페이지 열기
        onCameraSetupOpenTriggered: {
            console.log("selectedViewingGrid::onCameraSetupOpenTriggered!!", items);
            var channelGuid = [];
            for (var i = 0 ; i < items.length ; i++) {
                var channel = items[i].deviceId + "_" + items[i].channelId
                console.log("  deviceId=", items[i].deviceId, ", channelId=", items[i].channelId, ", complexChannel=",channel);
                channelGuid.push(channel);
            }
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceList)
            windowService.setupMainView_SelectDeviceListChannel(channelGuid)
            //emit cameraSetupSelectedChannels()
        }
        // 이벤트 룰 설정 페이지 열기
        onEventRulesSetupOpenTriggered: {
            console.log("selectedViewingGrid::onEventRulesSetupOpenTriggered!!", items);
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.EventRules)
        }

        // 웹페이지 설정 열기
        onWebpageSettingTriggered: {
            webpageAddViewModel.setWebpage(webpageId)

            windowService.webPageAddView_Open(WisenetLinguist.modifyWebpage)
        }

        // Layout drop시 tree로 open 명령
        onLayoutDropped: {
            for (var i = 0 ; i < layoutItems.length ; i ++) {
                console.log("Layout Dropped : ", layoutItems[i].itemId, layoutItems[i].name)
                layoutTree.openLayout(layoutItems[i].itemId)
            }
        }

        // Sequence drop시 tree로 open 명령
        onSequneceDropped: {
            for (var i = 0 ; i < sequenceItems.length ; i ++) {
                console.log("Sequence Dropped : ", sequenceItems[i].itemId, sequenceItems[i].name)
                layoutTree.openSequence(sequenceItems[i].itemId)
            }
        }
    }

    Connections{
        target: manualDiscoveryViewModel
        function onMonitoringPageRegistering(connecting){
            registeringIndicator.visible = connecting
        }
    }
    Connections{
        target: autoDiscoveryViewModel
        function onMonitoringPageRegistering(connecting){
            registeringIndicator.visible = connecting
        }
    }

    WisenetMessageDialog{
        id: errorDialog

        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    // ViewingGrid가 추가되는 Parent 아이템 영역
    // viewingGrid가 뒤에 그려지기 위해서 필요
    Item {
        id: viewingGridBackground
        anchors.fill: parent
    }
    DeviceListModel{
        id : deviceListModel
    }

    ResourceContextMenu{
        id: resourceContextMenu

        onAddNewGroup : {
            if(monitoringView.selectedViewingGrid)
                monitoringView.selectedViewingGrid.addNewGroup(groupId)
        }
        onAddNewVideoChannel : {
            if(monitoringView.selectedViewingGrid)
                monitoringView.selectedViewingGrid.addNewVideoChannel(deviceId, channelId)
        }
        onAddNewWebpage: {
            if(monitoringView.selectedViewingGrid)
                monitoringView.selectedViewingGrid.addNewWebpage(webpageId)
        }
        onAddNewLocalFile: {
            if(monitoringView.selectedViewingGrid)
                monitoringView.selectedViewingGrid.addNewLocalFile(localFileUrl)
        }

        onSelectLayout: monitoringView.selectLayout(type, name, uuid, layoutViewModel)
        onSelectSequence: monitoringView.selectSequence(type, name, uuid)

        onMakeGroup: resourceTree.makeGroup()
        onMoveGroup: resourceTree.moveGroup(groupId)
        onResourceTreeDeleteItems: resourceTree.deleteItems(remove)
        onOpenMediaFileDialog: resourceTree.openMediaFileDialog()

        onLayoutTreeDeleteItems: layoutTree.deleteItems()

        onOpenRenameDialog: monitoringView.openRenameDialog(itemType, uuid, name)

        onOpenResourceTreeItems: resourceTree.openResourceItems()
        onOpenResourceTreeItemsNewWindow: resourceTree.openResourceItemsNewWindow()
        onNewGroupWithSelection: resourceTree.newGroupWithSelection()
        onExportVideoWithSelection: resourceTree.exportVideoWithSelection()

        onOpenLayoutTreeItems: layoutTree.openLayoutItems()
        onOpenLayoutTreeItemsNewWindow: layoutTree.openLayoutItemsNewWindow()

        onOpenResourceTreeCameraSettings: {
            var cameraList = resourceTree.getSelectedCameras()
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceList)
            windowService.setupMainView_SelectDeviceListChannel(cameraList)
        }

        onOpenLayoutTreeCameraSettings: {
            var cameraList = layoutTree.getSelectedCameras()
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceList)
            windowService.setupMainView_SelectDeviceListChannel(cameraList)
        }

        onOpenInitializeDeviceCredential: monitoringView.openInitializeDeviceCredential()
        onOpenP2pRegister: monitoringView.openP2pRegister()

        onExpandResourceTree: resourceTree.expandAll()
        onCollapseResourceTree: resourceTree.collapseAll()
        onExpandLayoutTree: layoutTree.expandAll()
        onCollapseLayoutTree: layoutTree.collapseAll()

        onChangeConnectionStatus : {
            console.log("MonitoringView onChangeConnectionStatus")
            var cameraList = resourceTree.getSelectedCameras()
            var devices = []
            cameraList.forEach(function(item) {
                var ids = item.split('_')
                devices.push(ids[0])

            });
            deviceListModel.connectionChange(devices, connections)
        }

        onVerifyWnmItems: {
            console.log("MonitoringView onVerifyWnmItems")
            resourceTree.verifySignatureWithSelection()
        }

    }

    Rectangle{
        id: leftView
        state : "OPEN"
        width: 240
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: mediaControllerView.top
        //opacity: WisenetGui.gridBackgroundOpacity
        color: WisenetGui.contrast_09_dark

        Behavior on width  {
            id: leftViewWidthBehavior
            NumberAnimation {
                duration: 300; easing.type: Easing.InOutQuad
            }
        }

        ShortcutBoxView{
            id: shortcutBoxView
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            color: WisenetGui.contrast_08_dark_grey

            eventSearchButtonEnabled : userGroupModel && userGroupModel.playback
            addDeviceButtonEnabled : userGroupModel && userGroupModel.isAdmin
            setupButtonEnabled : userGroupModel && userGroupModel.isAdmin
            priorityAlarmButtonEnabled : eventListViewModel.eventListModel.alertAlarmCount > 0 && rightView.state === "HIDE"
            volumeControlButtonEnabled : userGroupModel && userGroupModel.audio
            //dashboardButtonEnabled: userGroupModel && userGroupModel.isAdmin

            videoStatus : selectedViewingGrid ? selectedViewingGrid.videoStatus : false
            channelNameVisible : selectedViewingGrid ? selectedViewingGrid.itemNameAlwaysVisible : false

            onOpenEventSearch: monitoringView.eventSearchPopupOpen(0)
            onOpenAiSearch: monitoringView.eventSearchPopupOpen(1)
            onOpenSmartSearch: monitoringView.eventSearchPopupOpen(2)
            onOpenTextSearch: monitoringView.eventSearchPopupOpen(3)
            onOpenSetup: monitoringView.openSetup()
            onOpenEventPanel: {
                if (rightView.state === "HIDE")
                {
                    showAndHideRightView(true)
                }
            }

            onOpenDashboard: monitoringView.openDashboard()
            onOpenMasking: monitoringView.openMaking()

            onFullScreen : {
                var isFullScreen = (wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen);
                console.log("MonitoringView::onToggleFullScreenTriggred, current=", isFullScreen)
                monitoringView.changeFullScreenMode(!isFullScreen)
            }

            onClearScreen : {
                if(monitoringView.selectedViewingGrid)
                    monitoringView.selectedViewingGrid.cleanUp()
            }
            onToggleVideoStatus : {
                if(selectedViewingGrid) {
                    selectedViewingGrid.videoStatus = !selectedViewingGrid.videoStatus;
                }
            }
            onToggleChannelNameVisible: {
                if(monitoringView.selectedViewingGrid) {
                    selectedViewingGrid.itemNameAlwaysVisible = !selectedViewingGrid.itemNameAlwaysVisible;
                }
            }

            onOpenInitializeDeviceCredential: monitoringView.openInitializeDeviceCredential()
            onOpenP2pRegister: monitoringView.openP2pRegister()

            onShowEditButton: showAnimation.start()
            onHideEditButton: hideAnimationTimer.start()

            NumberAnimation{id: showAnimation; target:shortcutEditButton; property:"opacity"; to: 1.0
                onStarted:shortcutEditButton.visible=true

            }
            NumberAnimation{id: hideAnimation; target:shortcutEditButton; property:"opacity"; to: 0;
                onStarted: console.log("hideAnimation START")
                onStopped: shortcutEditButton.visible=false
            }
        }

        Timer {
            id: hideAnimationTimer
            interval:500
            onTriggered:{
                if(!shortcutBoxView.shortcutMouseHovered && !shortcutEditButton.containsMouse)
                    hideAnimation.start()
                else
                    shortcutEditButton.visible=true

            }
        }

        Rectangle{
            id: shortcutEditButton
            width:20; height:40
            anchors{left:shortcutBoxView.right; top: shortcutBoxView.top}
            visible: editButtonMouseArea.containsMouse ? true : false

            color: WisenetGui.contrast_08_dark_grey
            opacity: editButtonMouseArea.containsMouse ? 1.0 : 0.7

            property alias containsMouse: editButtonMouseArea.containsMouse

            Image{
                id: editButtonImage
                width: 20; height: 40
                anchors.centerIn: parent
                sourceSize:"15x30"
                source: editButtonMouseArea.containsMouse? "qrc:/Assets/Icon/Monitoring/ShortcutBox/editShortcut_select.svg" :
                                                           shortcutBoxEditView.opened? "qrc:/Assets/Icon/Monitoring/ShortcutBox/editShortcut_hover.svg" :
                                                                                       "qrc:/Assets/Icon/Monitoring/ShortcutBox/editShortcut_default.svg"
            }

            MouseArea {
                id: editButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked:{
                    shortcutEditButton.visible = true
                    shortcutBoxEditView.x = shortcutEditButton.x
                    shortcutBoxEditView.y = shortcutEditButton.y + 40
                    shortcutBoxEditView.open()
                }
                onContainsMouseChanged:{
                    console.log("editButtonMouseArea containsMouse changed", containsMouse)
                    if(containsMouse)
                    {
                        hideAnimation.stop()
                        showAnimation.start()
                    }
                    else {
                        if(!shortcutBoxEditView.opened)
                            hideAnimationTimer.start()
                    }
                }
            }
        }

        ShortcutBoxEditView{
            id: shortcutBoxEditView
            onClosed: hideAnimationTimer.start()
        }

        Rectangle{
            id: treePanel
            anchors.top: shortcutBoxView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: sequenceControlBorder.visible ? sequenceControlBorder.top : parent.bottom
            color: WisenetGui.contrast_09_dark

            property int resourceTreeMinimumHeight : 90
            property int layoutTreeMinimumHeight : 60

            onHeightChanged: {
                console.log("treePanel.onHeightChanged() " + treePanel.visible)
                adjustTree()
            }

            function adjustTree(){
                console.log("monitoringView.adjustTree() start ", treePanel.height, resourceTree.height, layoutTree.height)
                if(resourceTree.visible){

                    if(resourceTree.height < 0)
                    {
                        resourceTree.height = (treePanel.height * 2/3) - 30
                    }

                    if(!layoutTree.visible){
                        resourceTree.height = treePanel.height - (resourceBar.height + layoutBar.height)
                    }
                    else{
                        if(resourceTree.height > treePanel.height - (resourceBar.height + layoutBar.height) - treePanel.layoutTreeMinimumHeight){
                            resourceTree.height = treePanel.height - (resourceBar.height + layoutBar.height) - treePanel.layoutTreeMinimumHeight
                        }
                    }

                    if(resourceTree.height < resourceTreeMinimumHeight && layoutTree.visible){
                        resourceTree.height = (treePanel.height * 2/3) - 30
                    }
                }
                console.log("monitoringView.adjustTree() end ", treePanel.height, resourceTree.height, layoutTree.height)
            }

            Rectangle{
                id: resourceBar

                width: parent.width
                height: 34
                color: WisenetGui.contrast_09_dark
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }

                Text {
                    anchors.fill: parent
                    anchors.rightMargin: 34
                    anchors.leftMargin: 11
                    visible: leftView.state === "OPEN"
                    text: WisenetLinguist.resources
                    font.pixelSize: 12
                    color: WisenetGui.white
                    verticalAlignment: Text.AlignVCenter
                }

                Image{
                    id: foldResourceTreeButton

                    anchors.topMargin: 11
                    anchors.bottomMargin: 11
                    anchors.rightMargin: 11
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right

                    source: resourceTree.visible ? WisenetImage.tree_arrow_bottom : WisenetImage.tree_arrow_top

                    MouseArea{
                        anchors.fill: parent

                        onClicked: {
                            resourceTree.visible = !resourceTree.visible
                        }
                    }
                }
            }

            ResourceView{
                id: resourceTree

                width: parent.width
                color: WisenetGui.contrast_09_dark

                height: (parent.height * 2/3) - 30
                property real lastHeight : height

                anchors {
                    top: resourceBar.bottom
                    left: parent.left
                    right: parent.right
                }

                function updateHeight() {
                    console.log("lastHeight: " + lastHeight + " height: " + height + " resourceTree.visible: " + resourceTree.visible + " layoutTree.visible: " + layoutTree.visible)

                    if (!visible) {
                        lastHeight = height;
                    }
                    else {
                        if(layoutTree.visible && (lastHeight > (parent.height - (resourceBar.height + layoutBar.height) - treePanel.layoutTreeMinimumHeight))){
                            lastHeight = parent.height - (resourceBar.height + layoutBar.height) - treePanel.layoutTreeMinimumHeight
                        }

                        height = layoutTree.visible ? lastHeight : (parent.height - (resourceBar.height + layoutBar.height));

                        if(layoutTree.visible && (layoutTree.height < treePanel.layoutTreeMinimumHeight)){
                            layoutTree.height = treePanel.layoutTreeMinimumHeight
                            height = height - treePanel.layoutTreeMinimumHeight
                        }
                    }
                }

                onVisibleChanged: {
                    updateHeight();
                }

                onAddNewGroup: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewGroup(groupId)
                }

                onAddNewVideoChannel: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewVideoChannel(deviceId, channelId)
                }

                onAddNewWebpage: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewWebpage(webpageId)
                }

                onAddNewItems: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewDragItems(dragItemListModel)
                }

                onAddNewLocalFile: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewLocalFile(localFileUrl)
                }

                onOpenRenameDialog: monitoringView.openRenameDialog(itemType, uuid, name)

                onTreeDragStartedChanged: {
                    layoutTree.treeDragStarted = resourceTree.treeDragStarted
                }
                onVerifyFinished: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.showVerifyFinished();
                }
            }

            Item {
                id : registeringIndicator
                anchors.top: treePanel.top
                anchors.right: treePanel.right
                anchors.rightMargin: 30
                anchors.topMargin: 5
                width: 30
                height: 30
                visible : false

                WisenetMediaBusyIndicator {
                    id: streamChaningIndicator
                    anchors.centerIn: parent
                    width: 30
                    height: 30
                    running:true
                }
            }
            Rectangle{
                id: treeSplitBar
                height: 3

                width: parent.width
                color: WisenetGui.contrast_09_dark

                anchors {
                    top : resourceTree.visible ? resourceTree.bottom : resourceBar.bottom
                    left: parent.left
                    right: parent.right
                }

                MouseArea{
                    anchors.fill: parent
                    cursorShape: (resourceTree.visible && layoutTree.visible) ? Qt.SizeVerCursor : Qt.ArrowCursor
                    onPressed: {
                        previousTreeSplitterY = mouseY

                    }
                    onMouseYChanged: {

                        if (resourceTree.visible && layoutTree.visible) {
                            var dy = mouseY - previousTreeSplitterY

                            if(resourceTree.height + dy >= treePanel.resourceTreeMinimumHeight && layoutTree.height - dy >= treePanel.layoutTreeMinimumHeight)
                                resourceTree.height += dy;
                        }
                    }
                }
            }

            Rectangle{
                id: layoutBar

                color: WisenetGui.contrast_09_dark
                width: parent.width
                height: 34
                anchors {
                    top: treeSplitBar.bottom
                    left: parent.left
                    right: parent.right
                }

                Rectangle{
                    id: upperBorder
                    height: 1
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: WisenetGui.contrast_08_dark_grey
                }

                Text {
                    anchors.fill: parent
                    anchors.rightMargin: 34
                    anchors.leftMargin: 11
                    visible: leftView.state === "OPEN"
                    text: WisenetLinguist.layouts
                    font.pixelSize: 12
                    color: WisenetGui.white
                    verticalAlignment: Text.AlignVCenter
                }

                Image{
                    anchors.topMargin: 11
                    anchors.bottomMargin: 11
                    anchors.rightMargin: 11
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right

                    source: layoutTree.visible ? WisenetImage.tree_arrow_bottom : WisenetImage.tree_arrow_top

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            layoutTree.visible = !layoutTree.visible
                        }
                    }
                }
            }

            LayoutView{
                id: layoutTree

                state : "OPEN"
                color: WisenetGui.contrast_09_dark
                width: parent.width

                anchors {
                    top : layoutBar.bottom
                    left : parent.left
                    right: parent.right
                    bottom: parent.bottom
                }

                onVisibleChanged: {
                    resourceTree.updateHeight();
                }

                onAddNewVideoChannel: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewVideoChannel(deviceId, channelId)
                }

                onAddNewWebpage: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewWebpage(webpageId)
                }

                onAddNewLocalFile: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewLocalFile(localFileUrl)
                }

                onAddNewItems: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewDragItems(dragItemListModel)
                }

                onSelectLayout: monitoringView.selectLayout(type, name, uuid, layoutViewModel)
                onSelectSequence: monitoringView.selectSequence(type, name, uuid)

                onOpenRenameDialog: {
                    monitoringView.openRenameDialog(itemType, uuid, name)
                }

                onTreeDragStartedChanged: {
                    resourceTree.treeDragStarted = layoutTree.treeDragStarted
                }
            }
        }

        Rectangle{
            id: sequenceControlBorder
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 35
            width: parent.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
            visible: sequenceControlRect.visible
        }

        Rectangle{
            id: sequenceControlRect

            color: WisenetGui.contrast_09_dark
            height: 30
            visible: isSequenceSelected

            anchors{
                top: sequenceControlBorder.bottom
                left : parent.left
                right: parent.right
                bottom : parent.bottom
            }

            component BaseMediaControlButton: Button{
                flat: true
                width: 22
                height: 16
                //enabled: controlEnabled
                hoverEnabled: true
                background: Rectangle {
                    color: WisenetGui.transparent
                }
            }

            BaseMediaControlButton {
                id: playPauseButton
                anchors.left: parent.left
                anchors.right: addTabCurrentLayoutButton.left
                anchors.top: sequenceControlRect.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 9
                anchors.bottomMargin: 9
                anchors.leftMargin: 11

                state: "playing"

                Image {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    sourceSize: Qt.size(52, 16)
                    source: {
                        if(playPauseButton.state == "playing") {
                            return WisenetImage.sequence_pause
                        }
                        else {
                            return WisenetImage.sequence_play
                        }
                    }
                }

                onClicked: {
                    if(state == "playing") {
                        state = "paused"
                        monitoringView.sequencePause(monitoringView.tabItemId)
                    }
                    else if(state == "paused") {
                        state = "playing"
                        monitoringView.sequencePlay(monitoringView.tabItemId)
                    }
                }

                states: [
                    State { name: "playing" },
                    State { name: "paused" }
                ]
            }

            Rectangle{
                anchors.right: addTabCurrentLayoutButton.left
                anchors.rightMargin: 15
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10

                width: 1
                height: 14
                color: WisenetGui.contrast_07_grey
                visible: sequenceControlRect.visible
            }

            WisenetStateImageButton{
                id: addTabCurrentLayoutButton
                width: 20
                height: 20
                anchors.right: editSequenceButton.left
                anchors.rightMargin: 9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 7

                defaultImageSource: WisenetImage.sequence_new_window_default
                hoverImageSource: WisenetImage.sequence_new_window_hover
                selectImageSource: WisenetImage.sequence_new_window_press
                disableImageSource: WisenetImage.sequence_new_window_dim
                WisenetMediaToolTip {
                    id: openTabTooltip
                    visible: parent.hovered
                    text: WisenetLinguist.openInNewTab
                }

                onClicked: {
                    monitoringView.addTabCurrentLayout()
                }
            }

            WisenetStateImageButton{
                id: editSequenceButton
                width: 20
                height: 20
                anchors.right: parent.right
                anchors.rightMargin: 13
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 7
                defaultImageSource: WisenetImage.sequence_edit_default
                hoverImageSource: WisenetImage.sequence_edit_hover
                selectImageSource: WisenetImage.sequence_edit_select
                disableImageSource: WisenetImage.sequence_edit_dim
                WisenetMediaToolTip {
                    id: editTooltip
                    visible: parent.hovered
                    text: WisenetLinguist.editSequence
                }

                onClicked: {
                    windowService.editSequence(monitoringView.tabName, monitoringView.tabItemId)
                }
            }
        }
    }

    Rectangle{
        id: leftSplitBar
        anchors.left: leftView.right
        //anchors.leftMargin: 1
        anchors.top: parent.top
        anchors.bottom: mediaControllerView.top
        width: 1
        color: WisenetGui.contrast_08_dark_grey
        visible: leftView.state === "OPEN"

        MouseArea{
            width: splitBarMouseAreaWidth
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            cursorShape: Qt.SizeHorCursor

            onPressed: {
                leftViewWidthBehavior.enabled = false
                previousLeftBarX = mouseX
            }

            onMouseXChanged: {
                var dx = mouseX - previousLeftBarX
                if((leftView.width + dx) > leftPanelMaxWidth)
                    leftView.width = leftPanelMaxWidth
                else if ((leftView.width + dx) < leftPanelMinWidth)
                    leftView.width = leftPanelMinWidth
                else
                    leftView.width = leftView.width + dx
            }

            onReleased: {
                leftViewWidthBehavior.enabled = true
            }
        }
    }


    property string arrow_left_default : "qrc:/Assets/Icon/Monitoring/arrow_left_default.svg"
    property string arrow_left_hover : "qrc:/Assets/Icon/Monitoring/arrow_left_hover.svg"
    property string arrow_left_pressed : "qrc:/Assets/Icon/Monitoring/arrow_left_pressed.svg"
    property string arrow_right_default : "qrc:/Assets/Icon/Monitoring/arrow_right_default.svg"
    property string arrow_right_hover : "qrc:/Assets/Icon/Monitoring/arrow_right_hover.svg"
    property string arrow_right_pressed : "qrc:/Assets/Icon/Monitoring/arrow_right_pressed.svg"

    property int leftViewWidthBeforeHide : 0
    property int rightViewWidthBeforeHide : 0
    property int resourceTreeColumnWidthBeforeHide : 0
    property int layoutTreeColumnWidthBeforeHide : 0

    Item{
        id: foldLeftButton
        width: 18
        height: 30
        anchors.left: leftSplitBar.right
        anchors.verticalCenter: leftSplitBar.verticalCenter
        visible: selectedViewingGrid ? selectedViewingGrid.gridLineVisible : true

        Rectangle {
            anchors.fill: parent
            color: WisenetGui.color_live_control_bg
            opacity: foldLeftButtonMouseArea.containsMouse ? 1.0 : 0.7
        }
        Image{
            id: foldLeftImage
            width: 18
            height: 18
            anchors.centerIn: parent
            sourceSize: "18x18"

            source: {
                if(foldLeftButtonMouseArea.pressed)
                {
                    if(leftView.state === "OPEN")
                        return arrow_left_pressed
                    else
                        return arrow_right_pressed
                }
                else if(foldLeftButtonMouseArea.containsMouse)
                {
                    if(leftView.state === "OPEN")
                        return arrow_left_hover
                    else
                        return arrow_right_hover
                }
                else
                {
                    if(leftView.state === "OPEN")
                        return arrow_left_default
                    else
                        return arrow_right_default
                }
            }
        }

        MouseArea
        {
            hoverEnabled: true
            anchors.fill: parent
            id: foldLeftButtonMouseArea
            onClicked:
            {
                console.log("foldLeftButton")
                var panelFlipList = [!wisenetViewerMainWindow.windowPanelFlipList[0], wisenetViewerMainWindow.windowPanelFlipList[1], wisenetViewerMainWindow.windowPanelFlipList[2]]
                wisenetViewerMainWindow.windowPanelFlipList = panelFlipList
//                console.log("foldLeftButton windowPanelFlipList[0] - ", wisenetViewerMainWindow.windowPanelFlipList[0])

                if (leftView.state === "OPEN")
                {
                    showAndHideLeftView(false)
                }
                else
                {
                    showAndHideLeftView(true)
                }
            }
        }
    }



    Item{
        id: foldRightButton
        width: 18
        height: 30
        anchors.verticalCenter: rightSplitBar.verticalCenter
        anchors.right: rightSplitBar.left
        anchors.verticalCenterOffset: 0
        visible: selectedViewingGrid ? selectedViewingGrid.gridLineVisible : true

        Rectangle {
            anchors.fill: parent
            color: WisenetGui.color_live_control_bg
            opacity: foldRightButtonMouseArea.containsMouse ? 1.0 : 0.7
        }
        Image{
            id: foldRightImage

            width: 18
            height: 18
            anchors.centerIn: parent
            sourceSize: "18x18"
            source: {
                if(foldRightButtonMouseArea.pressed)
                {
                    if(rightView.state === "OPEN")
                        return arrow_right_pressed
                    else
                        return arrow_left_pressed
                }
                if(foldRightButtonMouseArea.containsMouse)
                {
                    if(rightView.state === "OPEN")
                        return arrow_right_hover
                    else
                        return arrow_left_hover
                }
                else
                {
                    if(rightView.state === "OPEN")
                        return arrow_right_default
                    else
                        return arrow_left_default
                }
            }
        }

        MouseArea
        {
            hoverEnabled: true
            anchors.fill: parent
            id: foldRightButtonMouseArea
            onClicked:
            {
                console.log("foldRightButton")
                var panelFlipList = [wisenetViewerMainWindow.windowPanelFlipList[0], !wisenetViewerMainWindow.windowPanelFlipList[1], wisenetViewerMainWindow.windowPanelFlipList[2]]
                wisenetViewerMainWindow.windowPanelFlipList = panelFlipList

                if (rightView.state === "OPEN")
                {
                    showAndHideRightView(false)
                }
                else
                {
                    showAndHideRightView(true)
                }
            }
        }

    }


    Rectangle{
        id: rightSplitBar
        anchors.right: rightView.left
        //anchors.rightMargin: 1
        anchors.top: parent.top
        anchors.bottom: mediaControllerView.top
        width:1
        color: WisenetGui.contrast_08_dark_grey
        visible: rightView.state === "OPEN"

        MouseArea{
            width: splitBarMouseAreaWidth
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            cursorShape: Qt.SizeHorCursor

            onPressed: {
                rightViewWidthBehavior.enabled = false
                previousRightBarX = mouseX
            }

            onMouseXChanged: {
                var dx = previousRightBarX - mouseX

                if((rightView.width + dx) > rightPanelMaxWidth)
                    rightView.width  = rightPanelMaxWidth
                else if((rightView.width + dx) < rightPanelMinWidth)
                    rightView.width= rightPanelMinWidth
                else
                    rightView.width = rightView.width + dx
            }

            onReleased: {
                rightViewWidthBehavior.enabled = true
            }
        }
    }

    Rectangle{
        id: rightView
        state : "OPEN"
        width: 240
        //opacity: WisenetGui.gridBackgroundOpacity

        color: WisenetGui.contrast_09_dark

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: mediaControllerView.top

        Rectangle{
            color: WisenetGui.contrast_08_dark_grey
            height: 1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
        }

        RightPanelView{
            id: rightPanel

            anchors.fill: parent
            anchors.topMargin: 1

            selectedViewingGrid: monitoringView.selectedViewingGrid
            onOpenBookmarkPopup : mediaControllerView.openBookmarkPopup(bookmarkId)
            onBookmarkSelected : mediaControllerView.setSelectedBookmarkId(bookmarkId)
            onOpenBookmarkExportVideoPopup: mediaControllerView.openBookmarkExportVideoPopup(bookmarkId)
        }

        Behavior on width  {
            id: rightViewWidthBehavior
            NumberAnimation {
                duration: 300; easing.type: Easing.InOutQuad
            }
        }
    }

    MediaControllerView {
        id: mediaControllerView

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        simpleTimeline: false
        selectedViewingGrid: monitoringView.selectedViewingGrid
        focusedViewingGridItem: monitoringView.focusedViewingGridItem

        onTimelineVisibleTimeChanged: {
            rightPanel.setVisibleTime(startTime, endTime)
        }

        onSelectedBookmarkIdChanged: {
            rightPanel.setSelectedBookmarkId(bookmarkId)
        }

        onExportProgressView_Open: {
            windowService.exportProgressView_Open()
        }

        onStateChanged: {
            var panelFlipList = [wisenetViewerMainWindow.windowPanelFlipList[0], wisenetViewerMainWindow.windowPanelFlipList[1]]
            if(state == "show") {
                panelFlipList.push(false)
            }
            else {
                panelFlipList.push(true)
            }
            console.log("onStateChanged", panelFlipList)
            wisenetViewerMainWindow.windowPanelFlipList = panelFlipList
        }
        onIsLinkedModeChanged: {
            wisenetViewerMainWindow.isLinkedMode = isLinkedMode
        }
        onIsThumbnailModeChanged: {
            wisenetViewerMainWindow.isThumbnailMode = isThumbnailMode
        }
        onIsMultiTimelineModeChanged: {
            wisenetViewerMainWindow.isMultiTimelineMode = isMultiTimelineMode
        }
        onHeightChanged: {
            if(resizeMousePressed)
                wisenetViewerMainWindow.timelineHeight = height
        }
    }

    LayoutSaveAsMediaDialog {
        id: renameDialog

        title: ""
        property int itemType
        property string uuid
        applyText: WisenetLinguist.ok

        onAccepted: {
            console.log("MonitoringView::renameDialog onAccepted()", renameDialog.text);

            if(renameDialog.text.trim() === ""){
                errorDialog.message = WisenetLinguist.enterNameMessage
                errorDialog.visible = true
                return;
            }

            if(itemType === ItemType.Group || itemType === ItemType.Camera || itemType === ItemType.WebPage)
                resourceTree.applyChangeName(itemType, uuid, renameDialog.text)
            else if(itemType === ItemType.Layout || itemType === ItemType.Sequence)
                layoutTree.applyChangeName(itemType, uuid, renameDialog.text)
        }
    }

    property var changingScreenMode : false

    Timer {
        id: changingScreenModeTimer
        interval: 1000
        repeat: false
        running: false
        onTriggered: {
            changingScreenMode = false
        }
    }

    MonitoringViewAction {
        id: monitoringViewAction
        onSaveActionTriggered: {
            if(monitoringView.isSequenceSelected) {
                return
            }

            if(selectedViewingGrid) {
                if(!selectedViewingGrid.isEditable)
                    return  // editable이 아니면 저장하지 않고 리턴

                if(selectedViewingGrid.layout.isNewLayout()){
                    layoutSaveAsDialog.text = "Layout"
                    layoutSaveAsDialog.saveAs = false
                    layoutSaveAsDialog.layout = selectedViewingGrid.layout
                    layoutSaveAsDialog.showDialog();
                }
                else{
                    selectedViewingGrid.saveLayout();
                }
            }
        }
        onSaveAsActionTriggered: {
            if(monitoringView.isSequenceSelected) {
                return
            }

            if(selectedViewingGrid) {
                selectedViewingGrid.saveAsDialog();
            }
        }
        onOpenMediaActionTriggered: {
            if(selectedViewingGrid) {
                selectedViewingGrid.openMediaDialog(true);
            }
        }
        onAutoFitActionTriggered: {
            if(selectedViewingGrid) {
                selectedViewingGrid.setAutoFit(true);
            }
        }
        onToggleFullScreenTriggred: {

            if(changingScreenMode)
            {
                console.log("MonitoringView ScreenModeChanging - changingScreenMode", changingScreenMode)
                return;
            }

            changingScreenMode = true
            changingScreenModeTimer.restart()
            var isFullScreen = (wisenetViewerMainWindow.viewerMode===MainViewModel.FullScreen);
            console.log("MonitoringView::onToggleFullScreenTriggred, current=", isFullScreen)
            monitoringView.changeFullScreenMode(!isFullScreen)
        }
        onNewTabActionTriggered: {
            monitoringView.openNewTabTriggered();
        }
        onCloseTabActionTriggered: {
            monitoringView.closeTabTriggered();
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
