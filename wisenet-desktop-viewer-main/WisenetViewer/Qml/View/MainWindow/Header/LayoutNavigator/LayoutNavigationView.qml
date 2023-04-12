import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml 2.15
import QtQml.Models 2.15
import QtQuick.Controls.Universal 2.0
import WisenetStyle 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

Rectangle {
    id: layoutNavigationView

    color: WisenetGui.layoutTabColor

    height: 40
    property int menuButtonWidth
    property int clockWidth
    property int utilityButtonsWidth
    property int windowButtonsWidth

    property int minimumTitleBarWidth : 100
    property int maxTabAreaWidth: parent.width - menuButtonWidth - clockWidth - utilityButtonsWidth - windowButtonsWidth - minimumTitleBarWidth

    property int maxTabWidth : 130

    width: tabListView.width + buttonArea.width

    property alias layoutList : tabListView
    property alias currentLayout: tabListView.currentViewingGrid

    signal tabChanged(var itemType, var id, var name)
    signal tabClosedWithoutSave(var closedLayouts)

    signal sequencePlayStatusChanged(var isPlaying) // 기존 시퀀스 탭의 상태 변경 전달

    property bool isFocusedWindow : false

    property bool isWaiting: false

    property int savedCurrentIndex: 0

    property var sequenceInfoMap: (new Map([])) // 시퀀스 탭 별 제어를 위한 InfoMap

    Connections{
        target: layoutNavigationViewModel

        function onMediaOpen(itemType, itemId, channelNumber){
            console.log("layoutNavigationView::onMediaOpen() ItemType[" + itemType + "] ItemId[" + itemId + "] ChannelNumber[" + channelNumber + "]")

            if(itemType === ItemType.Camera){
                currentLayout.addNewVideoChannel(itemId, channelNumber)
            }
            else if(itemType === ItemType.WebPage){
                currentLayout.addNewWebpage(itemId)
            }
            else if(itemType === ItemType.LocalFile){
                currentLayout.addNewLocalFile(itemId)
            }
        }

        function onListMediaOpen(dragItemListModel){
            currentLayout.addNewDragItems(dragItemListModel)
        }

        function onGridItemsMediaOpen(items){
            currentLayout.addNewGridItems(items)
        }

        function onLayoutNameChanged(layoutId, layoutName){
            tabListView.layoutNameChange(layoutId, layoutName)
        }

        function onLayoutOpen(layoutId, layoutName, layoutViewModel){
            console.log("layoutNavigationView::onLayoutOpen() ItemId[" + layoutId + "] name[" + layoutName + "]")
            layoutNavigationView.selectLayout(ItemType.Layout, layoutName, layoutId, layoutViewModel)
        }

        function onSequenceOpen(sequenceId, sequenceName){
            console.log("layoutNavigationView::onSequenceOpen() ItemId[" + sequenceId + "] name[" + sequenceName + "]")
            layoutNavigationView.selectSequence(ItemType.Sequence, sequenceName, sequenceId)
        }
    }

    Connections{
        target: sequenceAddViewModel

        function onSequenceInfoChanged(sequenceName, sequenceId){
            console.log("sequenceAddViewModel->layoutNavigationView::onSequenceInfoChanged() sequenceName[" + sequenceName + "] sequenceId[" + sequenceId + "]")

            updateSequence(sequenceName, sequenceId)
        }
    }

    Connections{
        target: (resourceViewModel && resourceViewModel.layoutTreeModel) ? resourceViewModel.layoutTreeModel : null

        function onSequenceInfoChanged(sequenceName, sequenceId){
            console.log("layoutTreeModel->layoutNavigationView::onSequenceInfoChanged() sequenceName[" + sequenceName + "] sequenceId[" + sequenceId + "]")

            updateSequence(sequenceName, sequenceId)
        }
    }

    Connections{
        target: gControllerManager
        enabled : isFocusedWindow

        function onButtonChanged(controllerMode, controllerButtonAction){
            switch(controllerMode) {
            case ControllerModel.ScreenMode:
                switch(controllerButtonAction) {
                case ControllerSpc2000.MoveLayoutTab1:
                    if(tabListView.count > 0) {
                        tabListView.currentIndex = 0
                    }
                    break
                case ControllerSpc2000.MoveLayoutTab2:
                    if(tabListView.count > 1) {
                        tabListView.currentIndex = 1
                    }
                    break
                case ControllerSpc2000.MoveLayoutTab3:
                    if(tabListView.count > 2) {
                        tabListView.currentIndex = 2
                    }
                    break
                case ControllerSpc2000.MoveLayoutTab4:
                    if(tabListView.count > 3) {
                        tabListView.currentIndex = 3
                    }
                    break
                case ControllerSpc2000.MoveLayoutTab5:
                    if(tabListView.count > 4) {
                        tabListView.currentIndex = 4
                    }
                    break
                case ControllerSpc2000.MoveLayoutTab6:
                    if(tabListView.count > 5) {
                        tabListView.currentIndex = 5
                    }
                    break
                case ControllerSpc2000.MoveLayoutTab7:
                    if(tabListView.count > 6) {
                        tabListView.currentIndex = 6
                    }
                    break
                case ControllerSpc2000.MoveLayoutTab8:
                    if(tabListView.count > 7) {
                        tabListView.currentIndex = 7
                    }
                    break
                default:
                    break
                }
            }
        }
    }

    function selectLayout(type, name, uuid, layoutViewModel){
        console.log("layoutNavigationView::selectLayout()")
        tabListView.selectLayout(type, name, uuid, layoutViewModel)
    }

    function selectSequence(type, name, uuid){
        console.log("layoutNavigationView::selectSequence()")
        tabListView.selectSequence(type, name, uuid)
    }

    function addTabCurrentLayout() {
        // 시퀀스의 현재 레이아웃의 탭을 새 탭에서 띄움
        var targetTab = continueSequenceTimer.tab
        var currentLayoutInfo = targetTab.sequenceLayoutList[continueSequenceTimer.currentSequenceIdx] // 해당 순번의 레이아웃 정보 얻어옴

        var layoutId = currentLayoutInfo["layoutId"]

        layoutNavigationViewModel.loadFromCoreService(layoutId)
        var layoutViewModel = layoutNavigationViewModel.layoutViewModel

        tabListView.selectLayout(ItemType.Layout, layoutViewModel.name, layoutId, layoutViewModel)
    }

    function makeNewLayout(){
        console.log("layoutNavigationView::makeNewLayout()")
        layoutNavigationViewModel.makeNewTab()
        tabListView.currentIndex = tabListView.count - 1
    }

    function makeNewLayoutwithItems(itemType, itemId, channelNumber){
        console.log("layoutNavigationView::makeNewLayoutwithItems() ItemType[" + itemType + "] ItemId[" + itemId + "] ChannelNumber[" + channelNumber + "]")
        layoutNavigationViewModel.makeNewTab()
        tabListView.currentIndex = tabListView.count - 1

        if(itemType === ItemType.Camera){
            currentLayout.addNewVideoChannel(itemId, channelNumber)
        }
        else if(itemType === ItemType.WebPage){
            currentLayout.addNewWebpage(itemId)
        }
        else if(itemType === ItemType.LocalFile){
            currentLayout.addNewLocalFile(itemId)
        }
    }

    function checkEmptyNavigator(){
        console.log("layoutNavigationView::checkEmptyNavigator()")
        if(tabListView.count == 0)
            makeNewLayout()
    }

    function openInNewTab(items){
        makeNewLayout()
        currentLayout.addNewGridItems(items)
    }
    function closeCurrentTab()
    {
        console.log("layoutNavigationView::closeCurrentTab()")
        continueSequenceTimer.stop()
        prepareSequenceTimer.stop()
        var tabItem = tabListView.currentItem;
        if (tabItem) {
            var closedLayouts = [tabItem.uuid];
            layoutNavigationViewModel.closeTab(closedLayouts[0]);
            tabClosedWithoutSave(closedLayouts)
        }
    }

    function sequencePause(tabItemId) {
        var targetTab = sequenceInfoMap[tabItemId].tab // 일시정지할 탭 정보 얻어옴
        targetTab.setSeqPlayingStatus(false) // 해당 탭 일시정지

        // 재생 관련 타이머는 모두 일시정지
        continueSequenceTimer.running = false
        prepareSequenceTimer.running = false

        decoderManager.switchFullFrameMode()
    }

    function sequencePlay(tabItemId) {
        var targetTab = sequenceInfoMap[tabItemId].tab // 재생할 탭 정보 얻어옴
        targetTab.setSeqPlayingStatus(true) // 해당 탭 재생
        targetTab.sequenceLayoutList = layoutNavigationViewModel.getLayoutListOfSequence(tabItemId)

        // 현재 띄워져있는 레이아웃은 그대로 두고 타이머만 재시작
        startPrepareTimer(sequenceInfoMap[tabItemId].tab, sequenceInfoMap[tabItemId].interval-5, sequenceInfoMap[tabItemId].currentSequenceIdx+1, sequenceInfoMap[tabItemId].isMainViewingGrid)
        startSequenceTimer(sequenceInfoMap[tabItemId].tab, sequenceInfoMap[tabItemId].interval, sequenceInfoMap[tabItemId].currentSequenceIdx, sequenceInfoMap[tabItemId].isMainViewingGrid)
    }

    function updateSequence(sequenceName, sequenceId) {

        if(sequenceInfoMap[sequenceId] === null) {
            console.log("sequence is not on the header")
            return null
        }

        var previousTab = tabListView.currentItem

        // 탭 이름 업데이트
        tabListView.layoutNameChange(sequenceId, sequenceName)

        // 기존 시퀀스를 위한 타이머와 뷰잉그리드 순서 초기화
        continueSequenceTimer.running = false
        prepareSequenceTimer.running = false

        // 업데이트 된 시퀀스 정보 get
        if(sequenceInfoMap[sequenceId] === undefined) {
            console.log("sequence[",sequenceId,"] is undefined -> pass!")
            return
        }
        var targetTab = sequenceInfoMap[sequenceId].tab
        targetTab.sequenceLayoutList = layoutNavigationViewModel.getLayoutListOfSequence(sequenceId)

        // sequenceId에 해당하는 시퀀스 재실행
        tabListView.prepareSequence(targetTab, 0, true)
        tabListView.continueSequence(targetTab, 0, true)

        // 기존 일시정지였다면, 처음 레이아웃 띄우고 일시정지
        if(!targetTab.checkSeqPlaying()) {
            sequencePause(sequenceId)
        }

        // 현재 탭으로 돌아와서 원상복구 처리
        if(previousTab.isSequence()) {
            if(previousTab.checkSeqPlaying()) {
                sequencePlay(previousTab.uuid)
            }
            else {
                sequencePause(previousTab.uuid)
            }
        }
    }

    focus: true
    Keys.onPressed: {

        switch (event.key) {
        case Qt.Key_S:
            //if()
            console.log("Layout :: S pressed");
            break;
        }
    }

    Component {
        id: viewingGridComponent
        ViewingGrid {
            focus: true
            anchors.fill:parent
            isFocusedWindow:layoutNavigationView.isFocusedWindow
            isFullScreen: wisenetViewerMainWindow.viewerMode===MainViewModel.FullScreen
            viewX : monitoringView.viewX
            viewY : monitoringView.viewY
            viewWidth: monitoringView.viewWidth
            viewHeight: monitoringView.viewHeight
            itemNameAlwaysVisible : wisenetViewerMainWindow.displayOsd
            videoStatus: wisenetViewerMainWindow.displayVideoStatus
        }
    }

    ListView {
        id: tabListView
        spacing: 1
        height: 40

        width: ((maxTabWidth * tabListView.count) >= maxTabAreaWidth) ? maxTabAreaWidth : (maxTabWidth * tabListView.count)

        //interactive: false
        orientation: ListView.Horizontal

        focus: true

        property var currentViewingGrid : currentItem && currentItem.viewingGrid ? currentItem.viewingGrid : null
        property int positionViewIndex : 0

        onCurrentItemChanged: {
            // hh3.kim : bindning error log 수정 (2021-11-04)
            if (currentItem) {
                layoutNavigationView.tabChanged(currentItem.itemType, currentItem.uuid, currentItem.tabText)
            }
        }

        function layoutNameChange(uuid, name){
            for(var i = 0; i < tabListView.count; i++ )
            {
                var item = tabListView.itemAtIndex(i)

                if(item.uuid === uuid)
                    item.setName(name)
            }
        }

        function selectLayout(type, name, uuid, layoutViewModel){
            var hasLayout = false
            console.log("selectLayout() : arg name[" + name + "] uid[" + uuid + "]" + "type["+type+"]")
            for(var i = 0; i < tabListView.count; i++ )
            {
                var item = tabListView.itemAtIndex(i)
                console.log("selectLayout() item uuid[" + item.uuid + "]")

                if(item === null)
                    console.log("item is null: " + name)

                if(item.uuid === uuid){
                    console.log("selectLayout() selected! item_uuid[" + item.uuid + "]")
                    tabListView.currentIndex = i
                    hasLayout = true

                    if(type === ItemType.Layout) {
                        item.setLayout(layoutViewModel)
                    }

                    break
                }
            }

            if(hasLayout === false){
                layoutNavigationViewModel.addTab(type, name, uuid)
                tabListView.currentIndex = tabListView.count - 1

                var tab = tabListView.itemAtIndex(tabListView.count-1)
                console.log("selectLayout() : ~~~~tab " + tab + " viewmodel " + layoutViewModel)

                if(type === ItemType.Layout) {
                    tab.setLayout(layoutViewModel)
                }
            }
        }

        function selectSequence(type, name, uuid){
            var hasLayout = false
            var idx = 0

            for(var i = 0; i < tabListView.count; i++ )
            {
                var item = tabListView.itemAtIndex(i)

                if(item === null)
                    console.log("item is null: " + name)

                // 현재 HeaderView에 해당 시퀀스가 열려있다면
                if(item.uuid === uuid){
                    console.log("selectSequence() selected! item_uuid[" + item.uuid + "]")
                    tabListView.currentIndex = i
                    hasLayout = true

                    if(type === ItemType.Sequence) {
                        item.sequenceLayoutList = layoutNavigationViewModel.getLayoutListOfSequence(uuid) // 시퀀스가 재생할 레이아웃 리스트를 얻어옴

                        prepareSequence(item, 0, true) // 첫 레이아웃 준비
                        continueSequence(item, 0, true) // 첫 레이아웃 출력
                        sequencePlayStatusChanged(true) // 재생 아이콘으로 표시

                        item.viewingGrid.z = 1 // 시퀀스 재생 종료 후에는 원래 상태대로 viewingGrid만이 보여야함
                        item.viewingGrid2.z = -1
                    }

                    break
                }
            }

            // 새로 여는 탭이라면
            if(hasLayout === false){
                console.log("selectSequence() : addTab : type=",type," name=",name," uuid=",uuid)
                layoutNavigationViewModel.addTab(type, name, uuid)
                tabListView.currentIndex = tabListView.count - 1

                var tab = tabListView.itemAtIndex(tabListView.count-1)
                console.log("selectSequence() : ~~~~tab ",tab)

                if(type === ItemType.Sequence) {
                    tab.sequenceLayoutList = layoutNavigationViewModel.getLayoutListOfSequence(uuid)

                    prepareSequence(tab, 0, true)
                    continueSequence(tab, 0, true)

                    tab.viewingGrid.z = 1
                    tab.viewingGrid2.z = -1
                }
            }
        }

        // 다음 시퀀스 재생
        function continueSequence(tab, currentSequenceIdx, isMainViewingGrid) {
            if(tab === null)
                return

            // 마지막 레이아웃 출력 뒤에는 첫 레이아웃 출력
            if(currentSequenceIdx >= tab.sequenceLayoutList.length) {
                currentSequenceIdx = 0
            }

            var layoutInfo = tab.sequenceLayoutList[currentSequenceIdx] // 해당 순번의 레이아웃 정보 얻어옴

            if(layoutInfo !== undefined) {
                var layoutId = layoutInfo["layoutId"]
                var delayMs = layoutInfo["delayMs"]

                if(tab !== null) {
                    // 시퀀스 재생을 위해 두 개의 viewingGrid가 교차 출력
                    if(isMainViewingGrid) {
                        tab.viewingGrid2.z = -1
                        tab.viewingGrid.z = 1
                        currentViewingGrid = tab.viewingGrid
                    }
                    else {
                        tab.viewingGrid.z = -1
                        tab.viewingGrid2.z = 1
                        currentViewingGrid = tab.viewingGrid2
                    }
                    startPrepareTimer(tab, delayMs - 5, currentSequenceIdx + 1, !isMainViewingGrid) // delayMs - 5 초 전부터 다음 시퀀스를 준비하기 위한 타이머 실행
                    startSequenceTimer(tab, delayMs, currentSequenceIdx, !isMainViewingGrid) // delayMs 초 뒤에 다음 시퀀스를 출력하기 위한 타이머 실행
                }
            }
        }

        // 다음 시퀀스 미리 로드
        function prepareSequence(tab, nextSequenceIdx, isMainViewingGrid) {
            if(tab === null)
                return

            // 마지막 레이아웃 출력 뒤에는 첫 레이아웃 준비
            if(nextSequenceIdx >= tab.sequenceLayoutList.length) {
                nextSequenceIdx = 0
            }

            const nextLayoutInfo = tab.sequenceLayoutList[nextSequenceIdx] // 해당 순번의 레이아웃 정보 얻어옴

            if(nextLayoutInfo !== undefined) {
                const layoutId = nextLayoutInfo["layoutId"]

                layoutNavigationViewModel.loadFromCoreService(layoutId)
                var layoutViewModel = layoutNavigationViewModel.layoutViewModel


                if(tab !== null && layoutViewModel !== null) {
                    // 시퀀스 재생을 위해 두 개의 viewingGrid가 교차 출력
                    if(isMainViewingGrid) {
                        tab.viewingGrid.z = -1
                    }
                    else {
                        tab.viewingGrid2.z = -1
                    }
                    tab.setSequence(layoutViewModel, isMainViewingGrid)
                }
            }
        }

        // https://doc.qt.io/qt-5/qml-qtqml-models-delegatemodel.html
        model: DelegateModel {
            //! [0]
            id: visualModel
            model: (layoutNavigationViewModel && layoutNavigationViewModel.layoutTabModel) ? layoutNavigationViewModel.layoutTabModel : null

            delegate: DropArea {
                id: delegateRoot

                required property string tabText
                required property string uuid
                required property int itemType
                property alias isSequencePlaying: layoutTab.isSequencePlaying // 탭 별 재생 상태 값
                property var sequenceLayoutList: [] // DB에서 가져온 시퀀스의 레이아웃(layoutId, delayMs, order) 리스트. 탭 별로 진행할 레이아웃 리스트 소유

                // Icon width랑 일치시킴
                width: layoutTab.width

                // List view height랑 같게
                height: 40

                property var viewingGrid
                property var layoutViewModel : MediaLayoutViewModel{
                    name: delegateRoot.tabText
                    layoutId: delegateRoot.uuid
                }

                property string layoutId : layoutViewModel.layoutId ? layoutViewModel.layoutId : ""

                // for Sequence
                property var viewingGrid2

                onEntered: function(drag) {
                    // view 영역
                    visualModel.items.move((drag.source as Icon).visualIndex, layoutTab.visualIndex)

                    // model 변경
                    //layoutNavigationViewModel.layoutTabModel.moveTab((drag.source as Icon).visualIndex, layoutTab.visualIndex)
                }

                property int visualIndex: DelegateModel.itemsIndex

                LayoutTab {
                    id: layoutTab

                    width: (tabListView.width / tabListView.count) - 1

                    saveStatus: layoutViewModel.saveStatus ? layoutViewModel.saveStatus : false
                    dragParent: tabListView
                    visualIndex: delegateRoot.visualIndex
                    isFocused: tabListView.currentItem == delegateRoot
                    tabText : delegateRoot.tabText
                    itemType : delegateRoot.itemType
                    uuid: delegateRoot.uuid
                    parentItemCount : visualModel.count
                    layoutModel: layoutViewModel
                    isSequencePlaying: true

                    onMakeNewLayout: layoutNavigationView.makeNewLayout()
                    onTabClosedWithoutSave: layoutNavigationView.tabClosedWithoutSave(closedLayouts)

                    onIsFocusedChanged: {
                        // 시퀀스가 포커싱을 잃을 때는 일시정지 상태로 전환하고 이동
                        if(delegateRoot.itemType == ItemType.Sequence) {
                            if(isFocused) {
                                sequencePlayStatusChanged(isSequencePlaying) // 탭 이동으로 돌아왔을 때는 이전에 저장해둔 isSequencePlaying 값을 토대로 monitoringView.playPauseButton 업데이트
                            }
                            else {
                                sequencePause(uuid) // 탭 이동 시에 기존에 보고 있던 시퀀스는 일시정지 상태로 변경
                                isSequencePlaying = false
                            }
                        }

                        // 포커싱을 받으면 currentViewingGrid 갱신
                        if(isFocused) {
                            // 현재 탭이 어떤 뷰잉그리드를 사용하는지 체크하여 currentViewingGrid 갱신
                            if(tabListView.currentItem.viewingGrid.z >= 0) {
                                tabListView.currentViewingGrid = tabListView.currentItem.viewingGrid
                            }
                            else {
                                tabListView.currentViewingGrid = tabListView.currentItem.viewingGrid2
                            }

                        }

                    }
                }

                Component.onCompleted: {
                    // viewingGrid2는 시퀀스 기능을 위해 사용
                    if(delegateRoot.itemType == ItemType.Sequence){
                        viewingGrid2 = viewingGridComponent.createObject(monitoringView.viewingGridView)
                        //viewingGrid2.z = -1
                        viewingGrid2.visible = Qt.binding(function(){return layoutTab.isFocused})
                        viewingGrid2.itemsAdded.connect(delegateRoot.onItemsAdded)
                        viewingGrid2.itemsUpdated.connect(delegateRoot.onItemsUpdated)
                        viewingGrid2.itemsRemoved.connect(delegateRoot.onItemsRemoved)
                        viewingGrid2.layoutPropertyUpdated.connect(delegateRoot.onLayoutPropertyUpdated)

                        viewingGrid2.setLayout(layoutViewModel)
                    }

                    viewingGrid = viewingGridComponent.createObject(monitoringView.viewingGridView)
                    //viewingGrid.z = -1
                    viewingGrid.visible =  Qt.binding(function(){return layoutTab.isFocused})
                    viewingGrid.itemsAdded.connect(delegateRoot.onItemsAdded)
                    viewingGrid.itemsUpdated.connect(delegateRoot.onItemsUpdated)
                    viewingGrid.itemsRemoved.connect(delegateRoot.onItemsRemoved)
                    viewingGrid.layoutPropertyUpdated.connect(delegateRoot.onLayoutPropertyUpdated)

                    viewingGrid.setLayout(layoutViewModel);

                    console.log("LayoutNavigationView Component.onCompleted 2", viewingGrid, viewingGrid)
                }
                Component.onDestruction: {
                    viewingGrid.destroy()
                    if(delegateRoot.itemType == ItemType.Sequence) {
                        viewingGrid2.destroy()
                    }
                }

                function setLayout(viewModel){
                    // 최초 탭(레이아웃) 생성
                    if(layoutViewModel.uuid !== viewModel.uuid){
                        viewingGrid.setLayout(viewModel)
                    }

                    layoutViewModel = viewModel
                }

                function setSequence(viewModel, isMainViewingGrid) {
                    viewModel.locked = true // 시퀀스는 lock 처리(채널 추가 등의 행위 불가)

                    // 시퀀스 재생을 위해 두 개의 viewingGrid가 교차 출력
                    if(isMainViewingGrid) {
                        viewingGrid.isSequence = true
                        viewingGrid.cleanUpForSequence() // 다음 레이아웃을 띄우기 위해 viewingGrid clean
                        viewingGrid.setLayout(viewModel) // 해당 viewingGrid에 레이아웃 띄움
                    }
                    else {
                        viewingGrid2.isSequence = true
                        viewingGrid2.cleanUpForSequence() // 다음 레이아웃을 띄우기 위해 viewingGrid2 clean
                        viewingGrid2.setLayout(viewModel) // 해당 viewingGrid에 레이아웃 띄움
                    }

                    layoutViewModel = viewModel // 유저에게 보이는 viewingGrid는 하나이므로 layoutViewModel만 사용
                }

                function onItemsAdded(items)
                {
                    console.log("viewingGrid item added", items)
                    layoutViewModel.syncAddedItems(items);
                }

                function onItemsUpdated(items)
                {
                    //console.log("viewingGrid item updated", items)
                    layoutViewModel.syncUpdatedItems(items);
                }

                function onItemsRemoved(itemIds)
                {
                    console.log("viewingGrid item removed", itemIds)
                    layoutViewModel.syncRemovedItems(itemIds);
                }

                function onLayoutPropertyUpdated(layoutModel)
                {
                    console.log("viewingGrid layoutPropertyUpdated", layoutModel)
                    layoutViewModel.syncPropertyUpdatdLayout(layoutModel)
                }

                function setName(name)
                {
                    delegateRoot.tabText = name
                }

                function setSeqPlayingStatus(isPlaying)
                {
                    layoutTab.isSequencePlaying = isPlaying
//                    console.log("viewingGrid setSeqPlayingStatus() layoutTab.isSequencePlaying :", layoutTab.isSequencePlaying, "delegateRoot.isSequencePlaying :", delegateRoot.isSequencePlaying)
                }

                function checkSeqPlaying()
                {
//                    console.log("viewingGrid checkSeqPlaying() layoutTab.isSequencePlaying :", layoutTab.isSequencePlaying, "delegateRoot.isSequencePlaying :", delegateRoot.isSequencePlaying)
                    return delegateRoot.isSequencePlaying
                }

                function isSequence()
                {
                    return viewingGrid.isSequence
                }
            }
        }
    }

    // 버튼 부분
    Rectangle{
        id: buttonArea
        height: 40
        width: 20
        anchors.left: tabListView.right
        color: WisenetGui.layoutTabColor

        Image{
            id: addTabButton

            width:10
            height:10
            sourceSize: "10x10"
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent

            source: "qrc:/Assets/Icon/Header/add_tab.svg"
        }

        MouseArea{
            id: addTabMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                console.log("addTabButton.onClicked")
                makeNewLayout()
            }
        }
    }

    // 다음 시퀀스 재생을 위한 타이머
    Timer {
            id: continueSequenceTimer
            running: false
            repeat: false
            property int currentSequenceIdx: 0
            property var tab
            property bool isMainViewingGrid: true

            onTriggered: {
                tabListView.continueSequence(tab, currentSequenceIdx + 1, isMainViewingGrid)
            }
        }

        function startSequenceTimer(tab, delayMs, currentSequenceIdx, isMainViewingGrid) {
            continueSequenceTimer.interval = delayMs * 1000
            continueSequenceTimer.currentSequenceIdx = currentSequenceIdx
            continueSequenceTimer.tab = tab
            continueSequenceTimer.isMainViewingGrid = isMainViewingGrid
            continueSequenceTimer.running = true

            var seqDataObj = {tab: tab, interval: delayMs, currentSequenceIdx : currentSequenceIdx, isMainViewingGrid : isMainViewingGrid}
            sequenceInfoMap[tab.uuid] = seqDataObj
        }

        // 다음 시퀀스를 미리 로드하기 위한 타이머
        Timer {
            id: prepareSequenceTimer
            running: false
            repeat: false
            property int nextSequenceIdx: 0
            property var tab
            property bool isMainViewingGrid: true

            onTriggered: {
                tabListView.prepareSequence(tab, nextSequenceIdx, isMainViewingGrid)
            }
        }

        function startPrepareTimer(tab, delayMs, nextSequenceIdx, isMainViewingGrid) {
            prepareSequenceTimer.interval = delayMs * 1000
            prepareSequenceTimer.nextSequenceIdx = nextSequenceIdx
            prepareSequenceTimer.tab = tab
            prepareSequenceTimer.isMainViewingGrid = isMainViewingGrid
            prepareSequenceTimer.running = true
        }
}
