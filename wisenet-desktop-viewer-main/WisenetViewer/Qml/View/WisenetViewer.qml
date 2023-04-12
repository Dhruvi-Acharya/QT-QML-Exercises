import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "MainWindow/Content/InitialPasswordSetting"
import "MainWindow/Content/LicenseAgreement"
import "MainWindow/Content/Login"
import "MainWindow/Content/Monitoring"
import "MainWindow/Header"
import "Popup"
import "WisenetViewer.js" as ViewingGridManager
import "qrc:/"

ApplicationWindow {
    id: wisenetViewerMainWindow
    width: 1280
    height: 720
    visible: true
    title: qsTr("Wisenet Viewer")

    visibility: Window.Windowed
    minimumWidth: 1280
    minimumHeight: 720

    // Qt.FramelessWindowHint : Frame 없이, Qt.Window : 최소화시에 트레이에 아이콘 안나오는 문제 해결
    // Window : window
    // WindowFullscreenButtonHint : On macOS adds a fullscreen button.
    flags: Qt.FramelessWindowHint | Qt.Window

    // Declare properties that will store the position of the mouse cursor
    property int previousMouseX
    property int previousMouseY

//    onActiveFocusItemChanged: console.log("activeFocusItem", activeFocusItem)
//    onFocusedViewingGridItemChanged: console.log("focusedViewingGridItem", focusedViewingGridItem)

    property var selectedViewingGrid: headerView.currentLayout
    property var focusedViewingGridItem: (selectedViewingGrid && selectedViewingGrid.focusedItem) ? selectedViewingGrid.focusedItem : null

    signal selectLayout(string layoutName, string uuid, var layoutViewModel);
    signal selectSequence(string layoutName, string uuid);

    property int viewerMode : MainViewModel.Windowed
    property int lastViewerMode : MainViewModel.Windowed
    property int lastWindowX : x // window 모드일때 마지막 x
    property int lastWindowY : y // window 모드일때 마지막 y
    property int lastWindowWidth : width // window 모드일때 마지막 width
    property int lastWindowHeight : height // window 모드일때 마지막 height
    property int setupHomeStartX : 70
    property int setupHomeStartY : 50
    property bool displayOsd : true
    property bool displayVideoStatus : false
    property var windowPanelFlipList : [false, false, false]
    property bool isPanelFlipApplied : false
    property bool isLinkedMode: true   // 동기화 재생 모드
    property bool isThumbnailMode: false    // 타임라인 썸네일 표시 모드
    property bool isMultiTimelineMode: false    // 멀티 타임라인 표시 모드
    property real timelineHeight: 100   // showHide 및 isThumbnailMode, isMultiTimelineMode on/off에 따라 복귀 할 컨트롤의 height

    background: Rectangle{
        color: WisenetGui.contrast_11_bg
    }

    onVisibilityChanged: {
        if (Qt.platform.os === "osx") {
            if (visibility !== Window.Minimized) {
                console.log("WisenetViewer visibility changed for mac", visibility)
                flags = Qt.Window | Qt.FramelessWindowHint;
            }

            if (visibility === Window.Maximized && viewerMode !== MainViewModel.Maximized) {
                viewerMode = MainViewModel.Maximized
            }
            else if (visibility === Window.Windowed && viewerMode !== MainViewModel.Windowed) {
                viewerMode = MainViewModel.Windowed
            }
        }
        else if (Qt.platform.os === "linux") {
            if (visibility === Window.Windowed && (viewerMode === MainViewModel.Minimized || viewerMode === MainViewModel.FullScreen))
                viewerMode = lastViewerMode;
        }
        else {
            if (visibility === Window.Windowed && viewerMode === MainViewModel.Minimized) {
                console.log("WisenetViewer visibility changed, should be recovered from minimized", visibility)
                viewerMode = lastViewerMode;
            }
        }
        console.log("ApplicationWindow : " + displayOsd)
    }
    onDisplayOsdChanged: {
        console.log("WisenetViewer.qml onDisplayOsdChanged" + displayOsd)
        mainViewModel.displayOsd = displayOsd
    }
    onDisplayVideoStatusChanged: {
        mainViewModel.displayVideoStatus = displayVideoStatus
    }
    onWindowPanelFlipListChanged: {
        console.log("WisenetViewer onWindowPanelFlipListChanged : ", windowPanelFlipList[0], windowPanelFlipList[1], windowPanelFlipList[2])
        mainViewModel.panelFlipList = windowPanelFlipList
    }
    onIsLinkedModeChanged: {
        console.log("WisenetViewer onIsLinkedModeChanged :", isLinkedMode)
        mainViewModel.isLinkedMode = isLinkedMode
    }
    onIsThumbnailModeChanged: {
        console.log("WisenetViewer onIsThumbnailModeChanged :", isThumbnailMode)
        mainViewModel.isThumbnailMode = isThumbnailMode
    }
    onIsMultiTimelineModeChanged: {
        console.log("WisenetViewer onIsMultiTimelineModeChanged :", isMultiTimelineMode)
        mainViewModel.isMultiTimelineMode = isMultiTimelineMode
    }
    onTimelineHeightChanged: {
        //console.log("WisenetViewer onTimelineHeightChanged :", timelineHeight)
        mainViewModel.timelineHeight = timelineHeight
    }

    // windows os에서 fullscreen/maximized일 때 멀티모니터에서 깜빡이는 문제해결하기 위해서,
    // window mode 별도로 구현
    onViewerModeChanged: {
        console.log("WisenetViewer onViewerModeChanged", viewerMode, visibility);

        if (viewerMode === MainViewModel.Maximized || viewerMode === MainViewModel.FullScreen) {
            console.log("WisenetViewer Maximized || FullScreen", viewerMode, visibility," width=",width,"height=",height, "lastWindowWidth=",lastWindowWidth,"lastWindowHeight=",lastWindowHeight);            
            if (Qt.platform.os === "osx" || Qt.platform.os === "linux") {
                if (viewerMode === MainViewModel.Maximized) {
                    visibility = Window.Maximized
                }
                else if (viewerMode === MainViewModel.FullScreen) {
                    visibility = Window.FullScreen
                }
            }            
            else {
                // windows인 경우 멀티모니터 최대화/fullscreen에서 깜빡임 수정
                if(lastViewerMode == MainViewModel.Windowed) {
                    lastWindowX = x
                    lastWindowY = y
                    lastWindowWidth = width
                    lastWindowHeight = height
                }

                x = Screen.virtualX
                y = Screen.virtualY
                width = Screen.width
                height = Screen.height+1;

                if (visibility != Window.Windowed) {
                    visibility = Window.Windowed;
                }
                if (viewerMode === MainViewModel.FullScreen) {
                    monitoringView.changeFullScreenMode(true)
                }
            }
        }
        else if (viewerMode === MainViewModel.Windowed) {
            console.log("WisenetViewer Windowed", viewerMode, visibility," width=",width,"height=",height, "lastWindowWidth=",lastWindowWidth,"lastWindowHeight=",lastWindowHeight);
            if (Qt.platform.os === "osx" || Qt.platform.os === "linux") {
                visibility = Window.Windowed;
            }
            else {
                x = lastWindowX
                y = lastWindowY
                width = lastWindowWidth
                height = lastWindowHeight

                if (visibility != Window.Windowed) {
                    visibility = Window.Windowed;
                }
            }

            lastViewerMode = viewerMode
        }
        else if (viewerMode === MainViewModel.Minimized) {
            console.log("WisenetViewer Minimized", viewerMode, visibility);
            if (Qt.platform.os === "osx") {
                // mac에서는 FramelessWindowHint set시 정상동작하지 않으므로, flag값을 변경한다.
                flags = Qt.Window | Qt.CustomizeWindowHint | Qt.WindowMinMaxButtonsHint
                visibility = Window.Minimized;
            }
            else {
                if(lastViewerMode == MainViewModel.Windowed) {
                    lastWindowX = x
                    lastWindowY = y
                    lastWindowWidth = width
                    lastWindowHeight = height
                }

                visibility = Window.Minimized;
            }
        }

        console.log("WisenetViewer onViewerModeChanged finished", viewerMode, visibility);
    }

    Component.onCompleted: {
        console.log("wisenetViewerMainWindow onCompleted, viewMode=", viewerMode, "lastViewerMode=",lastViewerMode)
        viewerMode = mainViewModel.viewerMode
        lastViewerMode = viewerMode
        mainViewModel.getServiceInformation()
        //        console.log("wisenetViewerMainWindow onCompleted, viewMode=", viewerMode)

        var screens = Qt.application.screens;
        for (var i = 0; i < screens.length; ++i) {
            console.log("screen " + screens[i].name + " has geometry " +
                        screens[i].virtualX + ", " + screens[i].virtualY + " " +
                        screens[i].width + "x" + screens[i].height + ", pixelDensity " +
                        screens[i].pixelDensity + ", devicePixelRatio " + screens[i].devicePixelRatio)
        }
    }

    function openSetup(index)
    {
        if( index === WisenetSetupTree.SubPageIndex.DdnsP2pRegister) {
            windowService.openP2pView()
            return;
        }

        windowService.setupMainView_GotoSetup(index);
    }

    WisenetMessageDialog{
        id: errorDialog

        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    MonitoringView {
        id: monitoringView
        color: "transparent"
        anchors.fill: parent
        anchors.topMargin: headerView.visible ? headerView.height+1 : 1
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        anchors.bottomMargin: 1
        selectedViewingGrid: wisenetViewerMainWindow.selectedViewingGrid
        focusedViewingGridItem: wisenetViewerMainWindow.focusedViewingGridItem

        visible: false

        onEventSearchPopupOpen : windowService.eventSearchPopupOpenShortcutClicked(menuNumber)

        onOpenSetup: {
            //windowService.openSetup()
            windowService.setupMainView_SetSetupVisible(true)
            windowService.setupMainView_GotoHome()
            /*
            if(setupMainView.visible && eventSearchView.visible){
                eventSearchView.raise()
            }*/
        }

        onOpenDashboard: {
            windowService.healthMonitoringView_SetVisible(true)
        }

        onOpenMaking: {
            windowService.maskingView_Open()
        }

        onSelectLayout: {
            console.log("wisenetViewerMainWindow.item onSelectLayout()")
            headerView.layoutNavigationView.selectLayout(type, name, uuid, layoutViewModel)
        }

        onSelectSequence: {
            console.log("wisenetViewerMainWindow.item onSelectSequence()")
            headerView.layoutNavigationView.selectSequence(type, name, uuid)
        }

        onSequencePause: {
            console.log("wisenetViewerMainWindow.item onSequencePause()")
            headerView.layoutNavigationView.sequencePause(tabItemId)
        }

        onSequencePlay: {
            console.log("wisenetViewerMainWindow.item onSequencePlay()")
            headerView.layoutNavigationView.sequencePlay(tabItemId)
        }

        onAddTabCurrentLayout: {
            headerView.layoutNavigationView.addTabCurrentLayout()
        }

        onOpenInNewTabTriggered: headerView.openInNewTab(items)
        onOpenNewTabTriggered: headerView.layoutNavigationView.makeNewLayout();
        onFullScreenModeChanged: wisenetViewerMainWindow.changeFullScreenMode(onOff)
        onCloseTabTriggered: headerView.layoutNavigationView.closeCurrentTab();

        onOpenInitializeDeviceCredential: windowService.openInitializeDeviceCredentialView()
        onOpenP2pRegister: windowService.openP2pView()
    }

    HeaderView{
        id: headerView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.leftMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.rightMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1

        isFocusedWindow: wisenetViewerMainWindow.activeFocusItem != null

        height: 40

        onTabChanged: monitoringView.tabChanged(itemType, id, name)
        onTabClosedWithoutSave: monitoringView.tabClosedWithoutSave(closedLayouts)

        onSequencePlayStatusChanged: monitoringView.sequencePlayStatusChanged(isPlaying)

        onCloseButtonClicked : {
            if(windowHandler.windowCount() > 1){
                programExitDialog.message = WisenetLinguist.closeMessage1 + " " + WisenetLinguist.closeMessage2
                programExitDialog.applyButtonVisible = true
            }
            else{
                programExitDialog.message = WisenetLinguist.closeMessage2
                programExitDialog.applyButtonVisible = false
            }

            programExitDialog.show()
        }

        onCloseRightPanelFilterPopups: monitoringView.closeRightPanelFilterPopups()
        onOpenNotificationSetup: openSetup( WisenetSetupTree.SubPageIndex.EventNotifications )
    }

    InitialPasswordSettingView{
        id: initialPasswordSettingView

        anchors.centerIn: parent
        anchors.topMargin: headerView.visible ? headerView.height + 1 : 0
        anchors.leftMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.rightMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.bottomMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1

        visible: false
    }

    LoginView{
        id: loginView

        anchors.centerIn: parent
        anchors.topMargin: headerView.visible ? headerView.height + 1 : 0
        anchors.leftMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.rightMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.bottomMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1

        //visible: true
    }

    LicenseAgreementView{
        id: licenseAgreementView

        anchors.centerIn: parent
        anchors.topMargin: headerView.visible ? headerView.height + 1 : 0
        anchors.leftMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.rightMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.bottomMargin: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1

        visible: false
    }

    Connections{
        target: mainViewModel

        function onShowPasswordChangeView(result){
            console.log("wisenetViewerMainWindow mainViewModel onShowPasswordChangeView")

            headerView.changeLayoutNavigatorVisible(false)

            initialPasswordSettingView.visible = true
            loginView.visible = false
            licenseAgreementView.visible = false
            monitoringView.visible = false
        }

        function onShowMonitoringView(result){
            console.log("wisenetViewerMainWindow mainViewModel onShowMonitoringView")

            headerView.changeLayoutNavigatorVisible(true)
            headerView.checkEmptyNavigator()

            initialPasswordSettingView.visible = false
            loginView.visible = false
            licenseAgreementView.visible = false
            monitoringView.visible = true
        }

        function onShowLoginView(result){
            console.log("wisenetViewerMainWindow mainViewModel onShowLoginView")

            headerView.changeLayoutNavigatorVisible(false)

            initialPasswordSettingView.visible = false
            loginView.visible = true
            licenseAgreementView.visible = false
            monitoringView.visible = false

            loginViewModel.loadLocalSetting()

            windowService.setupMainView_SetSetupVisible(false)
            windowService.eventSearchView_SetVisible(false)

            if(loginViewModel.autologinChecked && !loginViewModel.logout && !mainViewModel.LoginStatus){
                loginView.showIndicator()
                loginViewModel.loginExecute()
            }
        }

        function onShowLicenseAgreementView(result){
            console.log("wisenetViewerMainWindow mainViewModel onShowLicenseAgreementView")

            headerView.changeLayoutNavigatorVisible(false)

            initialPasswordSettingView.visible = false
            loginView.visible = false
            licenseAgreementView.visible = true
            monitoringView.visible = false
        }

        // 멀티 윈도우의 포커스가 바뀔때 emit되는 signal 처리
        function onFocusWisenetViewer(onOff){
            console.log("wisenetViewerMainWindow mainViewModel onFocusWisenetViewer onOff=",onOff)
            headerView.isFocusedWindow = onOff
            if(onOff) {
//                selectedViewingGrid = headerView.currentLayout
//                focusedViewingGridItem = (selectedViewingGrid && selectedViewingGrid.focusedItem) ? selectedViewingGrid.focusedItem : null
                if(focusedViewingGridItem) focusedViewingGridItem.updateFocusedRect(true)
            }
            else {
//                selectedViewingGrid = null
//                focusedViewingGridItem = null
                if(focusedViewingGridItem) focusedViewingGridItem.updateFocusedRect(false)
            }
        }

        function onClose(){
            wisenetViewerMainWindow.close()
        }

        function onRefreshScreen(){
            var screens = Qt.application.screens;
            var list = []


            for (var i = 0; i < screens.length; ++i)
            {
                var screenInfo = screens[i].name + ":" + screens[i].virtualX + ":" + screens[i].virtualY + ":" + screens[i].width + ":" + screens[i].height;
                console.log(screens[i].name + ":" + screens[i].virtualX + ":" + screens[i].virtualY + ":" + screens[i].width + ":" + screens[i].height)
                list.push(screenInfo)
            }
            mainViewModel.setScreen(list);
        }

        function onViewerModeChanged(mode){
            console.log("mainViewModel signal - onViewerModeChanged wisenetViewerMainWindow.viewerMode=", wisenetViewerMainWindow.viewerMode, "mode=", mode)
            wisenetViewerMainWindow.lastViewerMode = wisenetViewerMainWindow.viewerMode
            wisenetViewerMainWindow.viewerMode = mode
        }

        function onRefreshWindowPosition(){
            mainViewModel.setWindowPosition(wisenetViewerMainWindow.x, wisenetViewerMainWindow.y, viewerMode)

            if(viewerMode == MainViewModel.Windowed){
                mainViewModel.setWidth(wisenetViewerMainWindow.width)
                mainViewModel.setHeight(wisenetViewerMainWindow.height)
            }
        }

        function onSetPosition(x, y, width, height){
            console.log("mainViewModel signal - onSetPosition", x, y, width, height)
            wisenetViewerMainWindow.setX(x)
            wisenetViewerMainWindow.setY(y)

            if(width > 0)
                wisenetViewerMainWindow.setWidth(width)

            if(height > 0)
                wisenetViewerMainWindow.setHeight(height)
        }
        function onDisplayOsdChanged(osd) {
            displayOsd = osd;
            wisenetViewerMainWindow.displayOsd = osd
        }
        function onDisplayVideoStatusChanged(videoStatus) {
            displayVideoStatus = videoStatus;
            wisenetViewerMainWindow.displayVideoStatus = videoStatus
        }
        function onPanelFlipListChanged(panelFlipList) {
            console.log("mainViewModel signal - onPanelFlipListChanged newPanelFlip:", panelFlipList[0], panelFlipList[1], panelFlipList[2], "curPanelFlip:", windowPanelFlipList[0], windowPanelFlipList[1], windowPanelFlipList[2])

            var newLeftPanelFlip = panelFlipList[0]
            var newRightPanelFlip = panelFlipList[1]
            var newBottomPanelFlip = panelFlipList[2]

            var curLeftPanelFlip = windowPanelFlipList[0]
            var curRightPanelFlip = windowPanelFlipList[1]
            var curBottomPanelFlip = windowPanelFlipList[2]

            if(newLeftPanelFlip === curLeftPanelFlip && newRightPanelFlip === curRightPanelFlip && newBottomPanelFlip === curBottomPanelFlip) {
//                console.log("mainViewModel signal - onPanelFlipListChanged : already same -> return")
                return
            }

            wisenetViewerMainWindow.windowPanelFlipList = panelFlipList

            if(!isPanelFlipApplied) {
                if(newLeftPanelFlip) { // left panel flip
                    monitoringView.flipLeftView()
                }

                if(newRightPanelFlip) { // right panel flip
                    monitoringView.flipRightView()
                }

                monitoringView.mediaControllerView.showAndHide(!newBottomPanelFlip) // bottom panel flip

                isPanelFlipApplied = true
            }
        }

        function onIsLinkedModeChanged() {
            isLinkedMode = mainViewModel.isLinkedMode
            monitoringView.mediaControllerView.setIsLinkedMode(isLinkedMode)
        }

        function onIsThumbnailModeChanged() {
            isThumbnailMode = mainViewModel.isThumbnailMode
            monitoringView.mediaControllerView.setIsThumbnailMode(isThumbnailMode)
        }

        function onIsMultiTimelineModeChanged() {
            isMultiTimelineMode = mainViewModel.isMultiTimelineMode
            monitoringView.mediaControllerView.setIsMultiTimelineMode(isMultiTimelineMode)
        }

        function onTimelineHeightChanged() {
            timelineHeight = mainViewModel.timelineHeight
            monitoringView.mediaControllerView.setTimelineHeight(timelineHeight)
        }
    }

    /*
    DashboardView{
        id: dashboardView
        transientParent: wisenetViewerMainWindow
        visible: false
    }*/

    LayoutSaveAsMediaDialog {
        id: layoutSaveAsDialog

        title: saveAs ? WisenetLinguist.saveLayoutAs : WisenetLinguist.saveLayout
        property bool saveAs : true
        property var layout : null
        onAccepted: {
            console.log("layoutNavigationView::SaveAsDialog onAccepted()", layoutSaveAsDialog.text);

            if(layoutSaveAsDialog.text.trim() === ""){
                errorDialog.message = WisenetLinguist.enterNameMessage
                errorDialog.visible = true
                return;
            }

            if(saveAs)
                layout.saveAsToCoreService(layoutSaveAsDialog.text);
            else
            {
                layout.name = layoutSaveAsDialog.text
                layout.ownerId = userGroupModel.userId
                layout.saveToCoreService()
            }
        }
    }

    property bool closeByButton : false
    WisenetMessageDialog{
        id  : programExitDialog
        message: WisenetLinguist.closeMessage1 + " " + WisenetLinguist.closeMessage2
        applyButtonText: WisenetLinguist.closeWindow
        cancelButtonText: WisenetLinguist.exitProgram
        buttonWidthSize : 140
        buttonHeightSize: 40

        onApplyButtonClicked: {
            console.log("Window closeButton clicked window Id : " + windowGuid)

            closeByButton = true
            wisenetViewerMainWindow.close()
            console.log("onClosing 1 window Id : " + windowGuid)
            windowHandler.destroyWindow(windowGuid)
            console.log("onClosing 2 window Id : " + windowGuid)
        }

        onCancelButtonClicked: {
            console.log("WisenetViewer exit program");
            closeByButton = true
            windowService.exportProgressView_ForceClose()
            windowHandler.exitProgram()
        }
    }

    Connections{
        target: windowHandler

        onSetCloseByButton: {
            closeByButton = close
        }
    }

    onClosing:{
        console.log("onClosing closeByButton", closeByButton)

        if(!closeByButton){
            close.accepted = false

            if(windowHandler.windowCount() > 1){
                programExitDialog.message = WisenetLinguist.closeMessage1 + " " + WisenetLinguist.closeMessage2
                programExitDialog.applyButtonVisible = true
            }
            else{
                programExitDialog.message = WisenetLinguist.closeMessage2
                programExitDialog.applyButtonVisible = false
            }

            programExitDialog.show()
        }
    }

    function changeFullScreenMode(onOff)
    {
        console.log("changeFullScreenMode: " + onOff)
        if(onOff)
        {
            headerView.visible = false
            topBorder.visible = false
            bottomBorder.visible = false
            leftBorder.visible = false
            rightBorder.visible = false
            topLeftPoint.visible = false
            topRightPoint.visible = false
            bottomLeftPoint.visible = false
            bottomRightPoint.visible = false
            windowBarRectangle.visible = false

            if(wisenetViewerMainWindow.viewerMode != MainViewModel.FullScreen) {
                // 현재 FullScreen이 아닐 때만 Mode 업데이트
                wisenetViewerMainWindow.lastViewerMode = wisenetViewerMainWindow.viewerMode
                wisenetViewerMainWindow.viewerMode = MainViewModel.FullScreen
            }
        }
        else
        {
            headerView.visible = true
            topBorder.visible = true
            bottomBorder.visible = true
            leftBorder.visible = true
            rightBorder.visible = true
            topLeftPoint.visible = true
            topRightPoint.visible = true
            bottomLeftPoint.visible = true
            bottomRightPoint.visible = true
            windowBarRectangle.visible = true

            if(wisenetViewerMainWindow.lastViewerMode == MainViewModel.FullScreen)
                wisenetViewerMainWindow.lastViewerMode = MainViewModel.Windowed

            if(Qt.platform.os != "linux")
                wisenetViewerMainWindow.viewerMode = wisenetViewerMainWindow.lastViewerMode
            else
                wisenetViewerMainWindow.visibility = Window.Windowed
        }
    }


    // 위쪽 크기 변경
    MouseArea {
        id: topBorder
        height: 3
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right

            // 모퉁이 비우기 위함.
            leftMargin: 3
            rightMargin: 3
        }

        // 커서 모양 변경
        // https://doc.qt.io/qt-5/qcursor.html
        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeVerCursor : Qt.ArrowCursor

        onPressed: {
            // We memorize the position along the Y axis
            previousMouseY = mouseY
        }

        // When changing a position, we recalculate the position of the window, and its height
        onMouseYChanged: {
            // 최대화 모드일때 동작하지 않도록 수정
            if(viewerMode !== MainViewModel.Windowed)
            {
                return
            }

            var dy = mouseY - previousMouseY

            if(wisenetViewerMainWindow.height - dy >= wisenetViewerMainWindow.minimumHeight){
                wisenetViewerMainWindow.setY(wisenetViewerMainWindow.y + dy)
                wisenetViewerMainWindow.setHeight(wisenetViewerMainWindow.height - dy)
                mainViewModel.setHeight(wisenetViewerMainWindow.height)
            }
        }
    }

    // 아래쪽 크기 변경
    MouseArea {
        id: bottomBorder
        height: 3
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right

            leftMargin: 3
            rightMargin: 3
        }
        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeVerCursor : Qt.ArrowCursor

        onPressed: {
            previousMouseY = mouseY
        }

        onMouseYChanged: {
            if(viewerMode !== MainViewModel.Windowed)
            {
                return
            }

            var dy = mouseY - previousMouseY

            if(wisenetViewerMainWindow.height + dy >= wisenetViewerMainWindow.minimumHeight){
                wisenetViewerMainWindow.setHeight(wisenetViewerMainWindow.height + dy)
                mainViewModel.setHeight(wisenetViewerMainWindow.height)
            }
        }
    }

    // 왼쪽 크기 변경
    MouseArea {
        id: leftBorder
        width: 3
        anchors {
            top: topBorder.bottom
            bottom: bottomBorder.top
            left: parent.left

            topMargin: 3
            bottomMargin: 3
        }

        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeHorCursor : Qt.ArrowCursor

        onPressed: {
            previousMouseX = mouseX
        }

        onMouseXChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX

            if(wisenetViewerMainWindow.width - dx >= wisenetViewerMainWindow.minimumWidth){
                wisenetViewerMainWindow.setX(wisenetViewerMainWindow.x + dx)
                wisenetViewerMainWindow.setWidth(wisenetViewerMainWindow.width - dx)
                mainViewModel.setWidth(wisenetViewerMainWindow.width)
            }
        }
    }

    // 오른쪽 크기 변경
    MouseArea {
        id: rightBorder
        width: 3
        anchors {
            top: topBorder.bottom
            bottom: bottomBorder.top
            right: parent.right

            topMargin: 3
            bottomMargin: 3
        }

        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeHorCursor : Qt.ArrowCursor

        onPressed: {
            previousMouseX = mouseX
        }

        onMouseXChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX

            if(wisenetViewerMainWindow.width + dx >= wisenetViewerMainWindow.minimumWidth){
                wisenetViewerMainWindow.setWidth(wisenetViewerMainWindow.width + dx)
                mainViewModel.setWidth(wisenetViewerMainWindow.width)
            }
        }
    }

    // 좌상
    MouseArea {
        id: topLeftPoint
        height: 3
        width: 3
        anchors {
            top: parent.top
            left: parent.left
        }

        // 커서 모양 변경
        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousMouseX = mouseX
            previousMouseY = mouseY
        }

        onMouseXChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX

            if(wisenetViewerMainWindow.width - dx >= wisenetViewerMainWindow.minimumWidth){
                wisenetViewerMainWindow.setX(wisenetViewerMainWindow.x + dx)
                wisenetViewerMainWindow.setWidth(wisenetViewerMainWindow.width - dx)
                mainViewModel.setWidth(wisenetViewerMainWindow.width)
            }
        }

        onMouseYChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dy = mouseY - previousMouseY

            if(wisenetViewerMainWindow.height - dy >= wisenetViewerMainWindow.minimumHeight){
                wisenetViewerMainWindow.setY(wisenetViewerMainWindow.y + dy)
                wisenetViewerMainWindow.setHeight(wisenetViewerMainWindow.height - dy)
                mainViewModel.setHeight(wisenetViewerMainWindow.height)
            }
        }
    }

    // 우상
    MouseArea {
        id: topRightPoint
        height: 3
        width: 3
        anchors {
            top: parent.top
            right: parent.right
        }

        // 커서 모양 변경
        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousMouseX = mouseX
            previousMouseY = mouseY
        }

        onMouseXChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX

            if(wisenetViewerMainWindow.width + dx >= wisenetViewerMainWindow.minimumWidth){
                wisenetViewerMainWindow.setWidth(wisenetViewerMainWindow.width + dx)
                mainViewModel.setWidth(wisenetViewerMainWindow.width)
            }
        }

        onMouseYChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dy = mouseY - previousMouseY

            if(wisenetViewerMainWindow.height - dy >= wisenetViewerMainWindow.minimumHeight){
                wisenetViewerMainWindow.setY(wisenetViewerMainWindow.y + dy)
                wisenetViewerMainWindow.setHeight(wisenetViewerMainWindow.height - dy)
                mainViewModel.setHeight(wisenetViewerMainWindow.height)
            }
        }
    }

    // 좌하
    MouseArea {
        id: bottomLeftPoint
        height: 3
        width: 3
        anchors {
            bottom: parent.bottom
            left: parent.left
        }

        // 커서 모양 변경
        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousMouseX = mouseX
            previousMouseY = mouseY
        }

        onMouseXChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX

            if(wisenetViewerMainWindow.width - dx >= wisenetViewerMainWindow.minimumWidth){
                wisenetViewerMainWindow.setX(wisenetViewerMainWindow.x + dx)
                wisenetViewerMainWindow.setWidth(wisenetViewerMainWindow.width - dx)
                mainViewModel.setWidth(wisenetViewerMainWindow.width)
            }
        }

        onMouseYChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dy = mouseY - previousMouseY

            if(wisenetViewerMainWindow.height + dy >= wisenetViewerMainWindow.minimumHeight){
                wisenetViewerMainWindow.setHeight(wisenetViewerMainWindow.height + dy)
                mainViewModel.setHeight(wisenetViewerMainWindow.height)
            }
        }
    }

    // 우하
    MouseArea {
        id: bottomRightPoint
        height: 3
        width: 3
        anchors {
            bottom: parent.bottom
            right: parent.right
        }

        // 커서 모양 변경
        cursorShape: viewerMode === MainViewModel.Windowed ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousMouseX = mouseX
            previousMouseY = mouseY
        }

        onMouseXChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX

            if(wisenetViewerMainWindow.width + dx >= wisenetViewerMainWindow.minimumWidth){
                wisenetViewerMainWindow.setWidth(wisenetViewerMainWindow.width + dx)
                mainViewModel.setWidth(wisenetViewerMainWindow.width)
            }

        }

        onMouseYChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dy = mouseY - previousMouseY

            if(wisenetViewerMainWindow.height + dy >= wisenetViewerMainWindow.minimumHeight){
                wisenetViewerMainWindow.setHeight(wisenetViewerMainWindow.height + dy)
                mainViewModel.setHeight(wisenetViewerMainWindow.height)
            }
        }
    }

    // 위쪽 윈도우 바 MouseArea
    Rectangle{
        id: windowBarRectangle
        z:5
        color: WisenetGui.layoutTabColor
        //color: "red" // for test
        anchors.fill : headerView
        anchors.leftMargin: headerView.menuPopupViewWidth + headerView.layoutNavigationViewWidth
        anchors.rightMargin: headerView.timeTextWidth + headerView.utilityViewWidth + headerView.windowsControlViewWidth

        MouseArea {
            id: windowBarMouseArea
            height: 30
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width : parent.width + headerView.timeTextWidth
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed: {
                if (mouse.button === Qt.RightButton){
                    if(mainViewModel.LoginStatus)
                        headerView.openNavigationBarContext()
                }
                else{
                    previousMouseX = mouseX
                    previousMouseY = mouseY
                }
            }

            onMouseXChanged: {
                if(windowBarMouseArea.pressedButtons & Qt.RightButton)
                    return

                if(viewerMode !== MainViewModel.Windowed)
                    return

                var dx = mouseX - previousMouseX
                wisenetViewerMainWindow.setX(wisenetViewerMainWindow.x + dx)
            }

            onMouseYChanged: {
                if(windowBarMouseArea.pressedButtons & Qt.RightButton)
                    return

                if(viewerMode !== MainViewModel.Windowed)
                    return

                var dy = mouseY - previousMouseY
                wisenetViewerMainWindow.setY(wisenetViewerMainWindow.y + dy)
            }

            onDoubleClicked:
            {
                if (mouse.button === Qt.RightButton)
                    return

                if (viewerMode === MainViewModel.Windowed)
                    viewerMode = MainViewModel.Maximized
                else if (viewerMode === MainViewModel.Maximized)
                    viewerMode = MainViewModel.Windowed
                console.log("double clicked")
            }
        }
    }

    MouseArea {
        id: logoMouseArea

        anchors.top : headerView.top
        anchors.topMargin: 3
        anchors.bottom : headerView.bottom
        anchors.left : headerView.left
        anchors.leftMargin: 3
        width: headerView.menuPopupViewWidth

        onPressed: {
            previousMouseX = mouseX
            previousMouseY = mouseY
        }

        onMouseXChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX
            wisenetViewerMainWindow.setX(wisenetViewerMainWindow.x + dx)
        }

        onMouseYChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dy = mouseY - previousMouseY
            wisenetViewerMainWindow.setY(wisenetViewerMainWindow.y + dy)
        }

        onDoubleClicked:
        {
            if (viewerMode === MainViewModel.Windowed)
                viewerMode = MainViewModel.Maximized
            else if (viewerMode === MainViewModel.Maximized)
                viewerMode = MainViewModel.Windowed
            console.log("logo mouse area double clicked")
        }
    }

    /* background border */
    Rectangle {
        anchors.fill: parent
        color:"transparent"
        border.width: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        border.color: WisenetGui.contrast_07_grey
    }
}
