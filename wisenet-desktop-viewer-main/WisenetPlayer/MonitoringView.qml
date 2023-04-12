import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import WisenetPlayer 1.0
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0
import "Resource"
import "qrc:/"
import "qrc:/WisenetStyle"

/**
import Wisenet.Define 1.0
import "Resource/ContextMenu"
import "Event"
import "MediaController"
import "qrc:/"
**/

Rectangle{
    id: monitoringView
    visible: true
    color: "transparent";

    property int viewX : leftSplitBar.x
    property int viewWidth : width - leftSplitBar.x - leftSplitBar.width
    property int viewY : 0
    property int viewHeight : height - mediaControllerView.height

    property int previousLeftBarX
    readonly property int leftPanelMaxWidth: 600
    readonly property int leftPanelMinWidth: 44 * 6
    property int splitBarMouseAreaWidth : 10

    property var tabItemId
    property bool treeDragStarted

    signal selectLayout(var type, var name, var uuid, var layoutViewModel)

    property var selectedViewingGrid: viewingGrid
    property var focusedViewingGridItem: selectedViewingGrid.focusedItem

    property alias resourceView : resourceTree
    //property alias viewingGridView : viewingGridBackground

    signal fullScreenModeChanged(var onOff)

    // 새탭에서 아이템 열기
    signal openInNewTabTriggered(var items)

    // 새탭만들기
    signal openNewTabTriggered()
    signal closeTabTriggered()

    onSelectedViewingGridChanged: {
        console.log("selected viewing Grid changed=", selectedViewingGrid)
    }
    onFocusedViewingGridItemChanged: {
        console.log("focused viewing Grid item changed=", focusedViewingGridItem)
    }

    function tabChanged(itemType, itemId) {
        monitoringView.tabItemId = itemId
    }

    function showAndHideLeftView(show) {
        if (!show) {
            if(leftView.state == "OPEN")
                leftViewWidthBeforeHide = leftView.width
            leftView.state = "HIDE"
            leftView.width = 0
            shortcutBoxView.visible = false
        }
        else {
            leftView.state = "OPEN"
            leftView.width = leftViewWidthBeforeHide
            shortcutBoxView.visible = true
        }
    }

    property bool leftViewVisibleBeforeFullScreen
    property bool mediaControllerVisibleBeforeFullScreen

    function changeFullScreenMode(onOff) {
        if(onOff) {
            // ON Full screen mode
            leftViewVisibleBeforeFullScreen = leftView.state === "OPEN"
            mediaControllerVisibleBeforeFullScreen = mediaControllerView.state === "show"

            showAndHideLeftView(false)
            mediaControllerView.showAndHide(false)
        }
        else {
            // OFF Full screen mode
            if(leftViewVisibleBeforeFullScreen)
                showAndHideLeftView(true)

            if(mediaControllerVisibleBeforeFullScreen)
                mediaControllerView.showAndHide(true)
        }

        fullScreenModeChanged(onOff)
    }

    // ViewingGrid
    ViewingGrid {
        id: viewingGrid
        focus: true
        anchors.fill: parent
        anchors.bottomMargin: mediaControllerView.height
        isFocusedWindow: mainWindow.activeFocusItem != null
        isFullScreen: mainWindow.viewerMode === MainViewModel.FullScreen
        isMediaFileOnly: true
        isLinkedMode: false
        viewX : monitoringView.viewX
        viewY : monitoringView.viewY
        viewWidth: monitoringView.viewWidth
        viewHeight: monitoringView.viewHeight

        Component.onCompleted: {
            var firstMediaFileUrl = resourceViewModel.treeModel.getFirstMediaFileUrl()
            console.log("viewingGrid Component.onCompleted",  firstMediaFileUrl)
            if(firstMediaFileUrl !== "") {
                addNewLocalFile(firstMediaFileUrl)
            }
            else {
                mediaFolderDialog.open()
            }
        }

        // 전체화면 on/off
        onShowFullscreen: {
            console.log("selectedViewingGrid::onShowFullscreen!!", onOff);
            monitoringView.changeFullScreenMode(onOff)
        }

        onMediaOpenResult: {
            //console.log("selectedViewingGrid::onMediaOpenResult!! Request checkVerifyResult", item.mediaParam.localResource);
            mainViewModel.checkVerifyResult(item.mediaParam.localResource)
        }

        Connections {
            target: mainViewModel
            function onVerifyResultChecked(filePath, verifyResult) {
                //console.log("selectedViewingGrid::onVerifyResultChecked!!", filePath, verifyResult);
                viewingGrid.updateVerifyStatus(filePath, verifyResult)
            }
        }
    }

    FileDialog {
        id: mediaFolderDialog
        title: WisenetLinguist.mediaFileFolder
        selectFolder: true
        folder: shortcuts.documents
        visible: false

        onAccepted: {
            //console.log("mediaFolderDialog.onAccepted() folder:", folder)
            resourceViewModel.changeMediaFolder(fileUrl)

            var firstMediaFileUrl = resourceViewModel.treeModel.getFirstMediaFileUrl()
            if(firstMediaFileUrl !== "") {
                viewingGrid.addNewLocalFile(firstMediaFileUrl)
            }
        }
    }

    Rectangle {
        id: leftView
        state : "OPEN"
        width: leftPanelMinWidth
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: mediaControllerView.top
        color: WisenetGui.contrast_09_dark

        Behavior on width {
            id: leftViewWidthBehavior
            NumberAnimation {
                duration: 300; easing.type: Easing.InOutQuad
            }
        }

        ShortcutBoxView {
            id: shortcutBoxView            
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: WisenetGui.contrast_08_dark_grey

            //volumeControlButtonEnabled : userGroupModel.audio
            videoStatus : selectedViewingGrid ? selectedViewingGrid.videoStatus : false
            channelNameVisible : selectedViewingGrid ? selectedViewingGrid.itemNameAlwaysVisible : false

            onFullScreen : {
                var isFullScreen = mainWindow.viewerMode === MainViewModel.FullScreen
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
        }

        Rectangle {
            id: treePanel
            anchors.top: shortcutBoxView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            color: WisenetGui.contrast_09_dark

            ResourceView {
                id: resourceTree
                anchors.fill: parent
                color: WisenetGui.contrast_09_dark

                Component.onCompleted: {
                    expandAll()
                }

                onAddNewItems: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewDragItems(dragItemListModel)
                }

                onAddNewLocalFile: {
                    if(monitoringView.selectedViewingGrid)
                        monitoringView.selectedViewingGrid.addNewLocalFile(localFileUrl)
                }

                onVerifyFinished: {
                    if(monitoringView.selectedViewingGrid) {
                        monitoringView.selectedViewingGrid.showVerifyFinished();
                        monitoringView.selectedViewingGrid.updateVerifyStatus(fileUuid, verifyResult)
                    }
                }
            }
        }
    }

    Rectangle{
        id: leftSplitBar
        anchors.left: leftView.right
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


    property string arrow_left_default : "qrc:/images/arrow_left_default.svg"
    property string arrow_left_hover : "qrc:/images/arrow_left_hover.svg"
    property string arrow_left_pressed : "qrc:/images/arrow_left_pressed.svg"
    property string arrow_right_default : "qrc:/images/arrow_right_default.svg"
    property string arrow_right_hover : "qrc:/images/arrow_right_hover.svg"
    property string arrow_right_pressed : "qrc:/images/arrow_right_pressed.svg"

    property int leftViewWidthBeforeHide : 0

    Item {
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

        Image {
            id: foldLeftImage
            width: 18
            height: 18
            anchors.centerIn: parent
            sourceSize: "18x18"

            source: {
                if(foldLeftButtonMouseArea.pressed) {
                    if(leftView.state === "OPEN")
                        return arrow_left_pressed
                    else
                        return arrow_right_pressed
                }
                else if(foldLeftButtonMouseArea.containsMouse) {
                    if(leftView.state === "OPEN")
                        return arrow_left_hover
                    else
                        return arrow_right_hover
                }
                else {
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
            onClicked: {
                console.log("foldLeftButton")
                if (leftView.state === "OPEN") {
                    showAndHideLeftView(false)
                }
                else {
                    showAndHideLeftView(true)
                }
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
        isMediaFileOnly: true
        isLinkedMode: false
    }

    property bool changingScreenMode : false

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
            var isFullScreen = (mainWindow.viewerMode === MainViewModel.FullScreen);
            console.log("MonitoringView::onToggleFullScreenTriggred, current=", isFullScreen)
            monitoringView.changeFullScreenMode(!isFullScreen)
        }
    }
}
