import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import WisenetPlayer 1.0
import "qrc:/WisenetStyle"

ApplicationWindow {
    id: mainWindow
    width: 1280
    height: 720
    visible: true
    title: qsTr("Wisenet Player")
    visibility: Window.Windowed
    minimumWidth: 1280
    minimumHeight: 720
    background: Rectangle{
        color: WisenetGui.contrast_11_bg
    }

    // Qt.FramelessWindowHint : Frame 없이, Qt.Window : 최소화시에 트레이에 아이콘 안나오는 문제 해결
    // Window : window
    // WindowFullscreenButtonHint : On macOS adds a fullscreen button.
    flags: Qt.FramelessWindowHint | Qt.Window

    property int viewerMode : MainViewModel.Windowed
    property int lastViewerMode : MainViewModel.Windowed
    property int lastWindowX : x // window 모드일때 마지막 x
    property int lastWindowY : y // window 모드일때 마지막 y
    property int lastWindowWidth : width // window 모드일때 마지막 width
    property int lastWindowHeight : height // window 모드일때 마지막 height

    // Declare properties that will store the position of the mouse cursor
    property int previousMouseX
    property int previousMouseY

    onVisibilityChanged: {
        //console.log("MainWindow::onVisibilityChanged() visibility :", visibility, "viewerMode :",viewerMode, "lastViewerMode :", lastViewerMode)
        if (Qt.platform.os === "osx") {
            if (visibility !== Window.Minimized)
                flags = Qt.Window | Qt.FramelessWindowHint;

            if (visibility === Window.Maximized && viewerMode !== MainViewModel.Maximized)
                viewerMode = MainViewModel.Maximized
            else if (visibility === Window.Windowed && viewerMode !== MainViewModel.Windowed)
                viewerMode = MainViewModel.Windowed
        }
        else if (Qt.platform.os === "linux") {
            if (visibility === Window.Windowed && (viewerMode === MainViewModel.Minimized || viewerMode === MainViewModel.FullScreen))
                viewerMode = lastViewerMode;
        }
        else {
            if (visibility === Window.Windowed && viewerMode === MainViewModel.Minimized)
                viewerMode = lastViewerMode;
        }
    }

    onViewerModeChanged: {
        //console.log("MainWindow::onViewerModeChanged() visibility :", visibility, "viewerMode :",viewerMode, "lastViewerMode :", lastViewerMode)
        if (viewerMode === MainViewModel.Maximized || viewerMode === MainViewModel.FullScreen) {
            if (Qt.platform.os === "osx" || Qt.platform.os === "linux") {
                if (viewerMode === MainViewModel.Maximized)
                    visibility = Window.Maximized
                else if (viewerMode === MainViewModel.FullScreen)
                    visibility = Window.FullScreen
            }
            else {
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
            }
        }
        else if (viewerMode === MainViewModel.Windowed) {
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
    }

    property bool closeByButton: false
    onClosing: {
        if(!closeByButton) {
            close.accepted = false
            programExitDialog.show()
        }
    }

    WisenetMessageDialog {
        id  : programExitDialog
        message: WisenetLinguist.closeMessage2
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.exitProgram
        buttonWidthSize : 140
        buttonHeightSize: 40

        onCancelButtonClicked: {
            closeByButton = true
            monitoringView.selectedViewingGrid.cleanUp()
            Qt.quit()
        }
    }

    MonitoringView {
        id: monitoringView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: headerView.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        anchors.bottomMargin: 1

        onFullScreenModeChanged: mainWindow.changeFullScreenMode(onOff)
    }

    HeaderView{
        id: headerView
        height: 40
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: mainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.leftMargin: mainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        anchors.rightMargin: mainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1

        //isFocusedWindow: mainWindow.activeFocusItem != null

        /*
        onTabChanged: monitoringView.tabChanged(itemType, id)
        onTabClosedWithoutSave: monitoringView.tabClosedWithoutSave(closedLayouts)
        */

        onCloseButtonClicked : {
            programExitDialog.show()
            //Qt.quit()
        }

        MouseArea {
            id: windowBarMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            onPressed: {
                previousMouseX = mouseX
                previousMouseY = mouseY
            }

            onPositionChanged: {
                if(viewerMode !== MainViewModel.Windowed)
                    return

                var dx = mouseX - previousMouseX
                var dy = mouseY - previousMouseY
                mainWindow.setX(mainWindow.x + dx)
                mainWindow.setY(mainWindow.y + dy)
            }

            onDoubleClicked:
            {
                if (viewerMode === MainViewModel.Windowed)
                    viewerMode = MainViewModel.Maximized
                else if (viewerMode === MainViewModel.Maximized)
                    viewerMode = MainViewModel.Windowed
            }
        }
    }

    /* background border */
    Rectangle {
        anchors.fill: parent
        color:"transparent"
        border.width: mainWindow.viewerMode === MainViewModel.FullScreen ? 0 : 1
        border.color: WisenetGui.contrast_07_grey
    }

    function changeFullScreenMode(onOff) {
        console.log("changeFullScreenMode: " + onOff)
        if(onOff) {
            headerView.visible = false
            headerView.height = 0
            topBorder.visible = false
            bottomBorder.visible = false
            leftBorder.visible = false
            rightBorder.visible = false
            topLeftPoint.visible = false
            topRightPoint.visible = false
            bottomLeftPoint.visible = false
            bottomRightPoint.visible = false

            if(mainWindow.viewerMode != MainViewModel.FullScreen) {
                mainWindow.lastViewerMode = mainWindow.viewerMode
                mainWindow.viewerMode = MainViewModel.FullScreen
            }
        }
        else {
            headerView.visible = true
            headerView.height = 40
            topBorder.visible = true
            bottomBorder.visible = true
            leftBorder.visible = true
            rightBorder.visible = true
            topLeftPoint.visible = true
            topRightPoint.visible = true
            bottomLeftPoint.visible = true
            bottomRightPoint.visible = true

            if(mainWindow.lastViewerMode == MainViewModel.FullScreen)
                mainWindow.lastViewerMode = MainViewModel.Windowed

            if(Qt.platform.os != "linux")
                mainWindow.viewerMode = mainWindow.lastViewerMode
            else
                mainWindow.visibility = Window.Windowed
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
                return

            var dy = mouseY - previousMouseY
            if(mainWindow.height - dy < mainWindow.minimumHeight)
                dy = mainWindow.height - mainWindow.minimumHeight

            mainWindow.setY(mainWindow.y + dy)
            mainWindow.setHeight(mainWindow.height - dy)
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
                return

            var dy = mouseY - previousMouseY
            if(mainWindow.height + dy < mainWindow.minimumHeight)
                dy = mainWindow.minimumHeight - mainWindow.height

            mainWindow.setHeight(mainWindow.height + dy)
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
            if(mainWindow.width - dx < mainWindow.minimumWidth)
                dx = mainWindow.width - mainWindow.minimumWidth

            mainWindow.setX(mainWindow.x + dx)
            mainWindow.setWidth(mainWindow.width - dx)
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
            if(mainWindow.width + dx < mainWindow.minimumWidth)
                dx = mainWindow.minimumWidth - mainWindow.width

            mainWindow.setWidth(mainWindow.width + dx)
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

        onPositionChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX
            if(mainWindow.width - dx < mainWindow.minimumWidth)
                dx = mainWindow.width - mainWindow.minimumWidth

            var dy = mouseY - previousMouseY
            if(mainWindow.height - dy < mainWindow.minimumHeight)
                dy = mainWindow.height - mainWindow.minimumHeight

            mainWindow.setX(mainWindow.x + dx)
            mainWindow.setY(mainWindow.y + dy)
            mainWindow.setWidth(mainWindow.width - dx)
            mainWindow.setHeight(mainWindow.height - dy)
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

        onPositionChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX
            if(mainWindow.width + dx < mainWindow.minimumWidth)
                dx = mainWindow.minimumWidth - mainWindow.width

            var dy = mouseY - previousMouseY
            if(mainWindow.height - dy < mainWindow.minimumHeight)
                dy = mainWindow.height - mainWindow.minimumHeight

            mainWindow.setY(mainWindow.y + dy)
            mainWindow.setWidth(mainWindow.width + dx)
            mainWindow.setHeight(mainWindow.height - dy)
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

        onPositionChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX
            if(mainWindow.width - dx < mainWindow.minimumWidth)
                dx = mainWindow.width - mainWindow.minimumWidth

            var dy = mouseY - previousMouseY
            if(mainWindow.height + dy < mainWindow.minimumHeight)
                dy = mainWindow.minimumHeight - mainWindow.height

            mainWindow.setX(mainWindow.x + dx)
            mainWindow.setWidth(mainWindow.width - dx)
            mainWindow.setHeight(mainWindow.height + dy)
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

        onPositionChanged: {
            if(viewerMode !== MainViewModel.Windowed)
                return

            var dx = mouseX - previousMouseX
            if(mainWindow.width + dx < mainWindow.minimumWidth)
                dx = mainWindow.minimumWidth - mainWindow.width

            var dy = mouseY - previousMouseY
            if(mainWindow.height + dy < mainWindow.minimumHeight)
                dy = mainWindow.minimumHeight - mainWindow.height

            mainWindow.setWidth(mainWindow.width + dx)
            mainWindow.setHeight(mainWindow.height + dy)
        }
    }
}
