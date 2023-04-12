import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"

ApplicationWindow {
    id: healthMonitoringView
    width: WisenetGui.eventSearchWindowDefaultWidth
    height: WisenetGui.eventSearchWindowDefaultHeight
    minimumWidth: WisenetGui.eventSearchWindowMinimumWidht
    minimumHeight: WisenetGui.eventSearchWindowMinimumHeight
    visible: true
    color: WisenetGui.contrast_11_bg
    flags: Qt.FramelessWindowHint | Qt.Window

    title: "Health monitoring"

    Rectangle{

        anchors.fill: parent
        border.color: WisenetGui.contrast_07_grey
        border.width: 1

        color: "transparent"

        // Window control bar
        Rectangle{
            id: healthMonitoringBar

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 1
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            color: WisenetGui.contrast_09_dark
            height: 40

            Text{
                id: title

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 13
                anchors.leftMargin: 30

                text: WisenetLinguist.dashboard
                color: WisenetGui.contrast_02_light_grey

                font.pixelSize: 12
            }

            MouseArea {
                height: 30
                anchors.fill: parent
                anchors.rightMargin: 45

                onPressed: {
                    previousX = mouseX
                    previousY = mouseY
                }

                onMouseXChanged: {
                    if(windowState == "maximized")
                    {
                        return
                    }

                    var dx = mouseX - previousX
                    healthMonitoringView.setX(healthMonitoringView.x + dx)
                }

                onMouseYChanged: {
                    if(windowState == "maximized")
                    {
                        return
                    }

                    var dy = mouseY - previousY
                    healthMonitoringView.setY(healthMonitoringView.y + dy)
                }

                onDoubleClicked:
                {
                    doMaximize()
                    console.log("double clicked")
                }
            }

            Image {
                id: minimizeButton
                height: 16
                width: 16
                anchors.top: parent.top
                anchors.right: maximizeButton.left
                anchors.topMargin: 12
                anchors.rightMargin: 6
                sourceSize: Qt.size(width, height)
                source: minimizeButtonMouseArea.containsPress ? WisenetImage.popup_minimize_press :
                                                                minimizeButtonMouseArea.containsMouse ? WisenetImage.popup_minimize_hover : WisenetImage.popup_minimize_default
                MouseArea
                {
                    id: minimizeButtonMouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        console.log("HealthMonitoring minimizeButton clicked")
                        if(Qt.platform.os === "windows") {
                            healthMonitoringView.showMinimized()
                        }
                        else if (Qt.platform.os === "linux") {
                            // 2023.01.04. 우분투에서 최소화 안되는 현상
                            healthMonitoringView.visible = false;
                        }
                        else {
                            // mac에서는 FramelessWindowHint set시 정상동작하지 않으므로, flag값을 변경한다.
                            if(Qt.platform.os === "osx") {
                                flags = Qt.Window | Qt.CustomizeWindowHint | Qt.WindowMinMaxButtonsHint
                            }
                            healthMonitoringView.visibility = Window.Windowed
                            healthMonitoringView.visibility = Window.Minimized
                        }
                    }
                }
            }

            Image {
                id: maximizeButton
                height: 16
                width: 16
                anchors.top: parent.top
                anchors.right: closeButton.left
                anchors.topMargin: 12
                anchors.rightMargin: 6
                sourceSize: Qt.size(width, height)
                source: {
                    if(windowState != "maximized") {
                        return maximizeButtonMouseArea.containsPress ? WisenetImage.popup_maximize_press :
                                                                       maximizeButtonMouseArea.containsMouse ? WisenetImage.popup_maximize_hover : WisenetImage.popup_maximize_default
                    }
                    else {
                        return maximizeButtonMouseArea.containsPress ? WisenetImage.popup_normalize_press :
                                                                       maximizeButtonMouseArea.containsMouse ? WisenetImage.popup_normalize_hover : WisenetImage.popup_normalize_default
                    }
                }
                MouseArea
                {
                    id: maximizeButtonMouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        console.log("HealthMonitoring maximizeButton clicked")
                        doMaximize()
                    }
                }
            }

            Image {
                id: closeButton
                height: 16
                width: 16
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 12
                anchors.rightMargin: 9
                sourceSize: Qt.size(width, height)
                source: closeButtonMouseArea.containsPress ? WisenetImage.popup_close_press :
                                                             closeButtonMouseArea.containsMouse ? WisenetImage.popup_close_hover : WisenetImage.popup_close_default
                MouseArea
                {
                    id: closeButtonMouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        healthMonitoringView.close()
                    }
                }
            }

        }

        // Main view
        Rectangle{
            id: healthMonitoringMain

            anchors.top: healthMonitoringBar.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            color: "transparent"

            // Logo
            Rectangle{
                id: logoRect

                anchors.top: parent.top
                anchors.topMargin: 30
                anchors.horizontalCenter: parent.horizontalCenter
                width: wisenetviewerLogo.sourceSize.width + 6 + healthMonitoringTitle.contentWidth
                height: 28
                color: "transparent"

                Image{
                    id: wisenetviewerLogo

                    height: 25
                    source: logoManager.useCustomLogo? "file:///" + logoManager.customLogoPath : WisenetImage.wisenet_viewer_logo
                    sourceSize.width: 185
                    sourceSize.height: 19
                    fillMode: Image.PreserveAspectFit

                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 3
                }

                Text{
                    id: healthMonitoringTitle

                    anchors.left: wisenetviewerLogo.right
                    anchors.leftMargin: 6
                    anchors.bottom: parent.bottom

                    color: WisenetGui.contrast_00_white
                    text: "Health monitoring"
                    font.pixelSize: 20
                    bottomPadding: 0

                    verticalAlignment: Text.AlignBottom
                }
            }

            // Content
            Rectangle{
                id: healthMonitoringContent

                anchors.top: logoRect.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 30
                anchors.bottom: parent.bottom

                color: "transparent"

                HealthOverview{
                    id: healthOverview

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 20

                    width: 613
                    height: 190
                }

                AlertLog{
                    id: alertLog

                    anchors.top: healthOverview.bottom
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.topMargin: 20
                    anchors.leftMargin: 20
                    anchors.bottomMargin: 20

                    width: 613
                }

                DeviceInformation{
                    id: deviceInformation

                    deviceListProxy: healthMonitoringViewModel.deviceListProxy

                    anchors.top: parent.top
                    anchors.left: healthOverview.right
                    anchors.leftMargin: 20
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 20
                }
            }
        }
    }

    onVisibleChanged: {
        console.log("HealthMonitoringView.onVisibleChanged", visible)
    }

    // 최대화/복귀 함수
    function doMaximize()
    {
        console.log("doMaximize start state: " + windowState)
        if(windowState != "maximized")
        {
            healthMonitoringView.showMaximized()
            windowState = "maximized"
        }
        else
        {
            healthMonitoringView.showNormal()
            windowState = "normal"
        }
        console.log("doMaximize end state: " + windowState)
    }

    // Declare properties that will store the position of the mouse cursor
    property int previousX
    property int previousY

    // maximized or normal 상태임을 저장
    property string windowState: "normal"
    property int previousVisibility

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
        cursorShape: windowState != "maximized" ? Qt.SizeVerCursor : Qt.ArrowCursor

        onPressed: {
            // We memorize the position along the Y axis
            previousY = mouseY
        }

        // When changing a position, we recalculate the position of the window, and its height
        onMouseYChanged: {
            // 최대화 모드일때 동작하지 않도록 수정
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(healthMonitoringView.height - dy >= healthMonitoringView.minimumHeight)
            {
                healthMonitoringView.setY(healthMonitoringView.y + dy)
                healthMonitoringView.setHeight(healthMonitoringView.height - dy)
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
        cursorShape: windowState != "maximized" ? Qt.SizeVerCursor : Qt.ArrowCursor

        onPressed: {
            previousY = mouseY
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(healthMonitoringView.height + dy >= healthMonitoringView.minimumHeight)
                healthMonitoringView.setHeight(healthMonitoringView.height + dy)
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

        cursorShape: windowState != "maximized" ? Qt.SizeHorCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(healthMonitoringView.width - dx >= healthMonitoringView.minimumWidth)
            {
                healthMonitoringView.setX(healthMonitoringView.x + dx)
                healthMonitoringView.setWidth(healthMonitoringView.width - dx)
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

        cursorShape: windowState != "maximized" ? Qt.SizeHorCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(healthMonitoringView.width + dx >= healthMonitoringView.minimumWidth)
            {
                healthMonitoringView.setWidth(healthMonitoringView.width + dx)
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
        cursorShape: windowState != "maximized" ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(healthMonitoringView.width - dx >= healthMonitoringView.minimumWidth)
            {
                healthMonitoringView.setX(healthMonitoringView.x + dx)
                healthMonitoringView.setWidth(healthMonitoringView.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(healthMonitoringView.height - dy >= healthMonitoringView.minimumHeight)
            {
                healthMonitoringView.setY(healthMonitoringView.y + dy)
                healthMonitoringView.setHeight(healthMonitoringView.height - dy)
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
        cursorShape: windowState != "maximized" ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(healthMonitoringView.width + dx >= healthMonitoringView.minimumWidth)
                healthMonitoringView.setWidth(healthMonitoringView.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(healthMonitoringView.height - dy >= healthMonitoringView.minimumHeight)
            {
                healthMonitoringView.setY(healthMonitoringView.y + dy)
                healthMonitoringView.setHeight(healthMonitoringView.height - dy)
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
        cursorShape: windowState != "maximized" ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(healthMonitoringView.width - dx >= healthMonitoringView.minimumWidth)
            {
                healthMonitoringView.setX(healthMonitoringView.x + dx)
                healthMonitoringView.setWidth(healthMonitoringView.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(healthMonitoringView.height + dy >= healthMonitoringView.minimumHeight)
                healthMonitoringView.setHeight(healthMonitoringView.height + dy)
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
        cursorShape: windowState != "maximized" ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(healthMonitoringView.width + dx >= healthMonitoringView.minimumWidth)
                healthMonitoringView.setWidth(healthMonitoringView.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(healthMonitoringView.height + dy >= healthMonitoringView.minimumHeight)
                healthMonitoringView.setHeight(healthMonitoringView.height + dy)
        }
    }
}
