import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"

ApplicationWindow {
    id: dashboard
    width: WisenetGui.eventSearchWindowDefaultWidth
    height: WisenetGui.eventSearchWindowDefaultHeight
    minimumWidth: WisenetGui.eventSearchWindowMinimumWidht
    minimumHeight: WisenetGui.eventSearchWindowMinimumHeight
    visible: true
    color: WisenetGui.contrast_11_bg
    flags: Qt.FramelessWindowHint | Qt.Window

    title: WisenetLinguist.dashboard

    property int cardWidth: 260
    property int cardHeight: 430
    property int leftRightMargin: 60
    property int cardMargin: 10
    property int errorDetailMargin: 12

    Rectangle{

        anchors.fill: parent
        border.color: WisenetGui.contrast_07_grey
        border.width: 1

        color: "transparent"

        /*
        //for test
        Button{
            id: printButton

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 60
            anchors.leftMargin: 30

            width: 100
            height: 20

            text: "Test"

            onClicked: {
                dashboardViewModel.printStatus()
                dashboardViewModel.refreshData()
            }
        }*/

        // Window control bar
        Rectangle{
            id: dashboardBar

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 1
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            color: WisenetGui.contrast_10_dark
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
                    dashboard.setX(dashboard.x + dx)
                }

                onMouseYChanged: {
                    if(windowState == "maximized")
                    {
                        return
                    }

                    var dy = mouseY - previousY
                    dashboard.setY(dashboard.y + dy)
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
                        console.log("Dashboard minimizeButton clicked")
                        if(Qt.platform.os === "windows") {
                            dashboard.showMinimized()
                        }
                        else if (Qt.platform.os === "linux") {
                            // 2023.01.04. 우분투에서 최소화 안되는 현상
                            dashboard.visible = false;
                        }
                        else {
                            // mac에서는 FramelessWindowHint set시 정상동작하지 않으므로, flag값을 변경한다.
                            if(Qt.platform.os === "osx") {
                                flags = Qt.Window | Qt.CustomizeWindowHint | Qt.WindowMinMaxButtonsHint
                            }
                            dashboard.visibility = Window.Windowed
                            dashboard.visibility = Window.Minimized
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
                        console.log("Dashboard maximizeButton clicked")
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
                        console.log("dashboard closeButton clicked guid : " + windowGuid)
                        dashboard.close()
                    }
                }
            }

        }

        // Main view
        Rectangle{
            id: dashboardMain

            anchors.top: dashboardBar.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            color: "transparent"

            // Logo
            Image{
                id: dashboardLogo

                anchors.top: parent.top
                anchors.topMargin: (dashboardContent.y - dashboardLogo.height) / 2
                anchors.horizontalCenter: parent.horizontalCenter

                width: 288
                height: 23
                sourceSize.width: 288
                sourceSize.height: 23

                source: "qrc:/Assets/Icon/Dashboard/dashboard_logo.svg"
            }

            // Content
            Rectangle{
                id: dashboardContent

                height: parent.height* 3/5
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right

                color: "transparent"

                // Recorder normal
                NormalStatusCard{
                    id: recorderNormalStatus

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.leftMargin: leftRightMargin

                    objectType: WisenetLinguist.device
                    objectCount: dashboardViewModel.recorderCount

                    visible: dashboardViewModel.recorderErrorCount === 0
                }

                // Camera normal
                NormalStatusCard{
                    id: cameraNormalStatus

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: recorderNormalStatus.right
                    anchors.leftMargin: cardMargin

                    objectType: WisenetLinguist.camera
                    isCamera: true
                    objectCount: dashboardViewModel.connectedCameraCount

                    visible: dashboardViewModel.disconnectedCameraCount === 0
                }

                // Recorder error
                Rectangle{
                    id: recorderErrorDisplayRect

                    width: state === "SUMMARY" ? cardWidth : parent.width - (leftRightMargin * 2)
                    //height: cardHeight

                    z: 1

                    border.width: 1
                    border.color: WisenetGui.contrast_08_dark_grey

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.leftMargin: leftRightMargin

                    visible: !recorderNormalStatus.visible

                    //color: recorderAbnormalRectMouseArea.containsMouse ? WisenetGui.contrast_07_grey : WisenetGui.contrast_08_dark_grey
                    color: WisenetGui.contrast_10_dark
                    radius: 4

                    state: "SUMMARY"
                    onStateChanged: {
                        if(state === "DETAIL")
                        {
                            recorderErrorDisplayRect.z = 2
                            cameraErrorDisplayRect.z = 1
                        }
                    }

                    Behavior on width {
                        NumberAnimation {
                            duration: 300; easing.type: Easing.InOutQuad
                        }
                    }

                    ErrorStatusCard{
                        id: recorderErrorStatus

                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        objectType: WisenetLinguist.device

                        recorderCount : dashboardViewModel.recorderCount

                        powerErrorCount : dashboardViewModel.powerErrorCount
                        diskErrorCount : dashboardViewModel.diskErrorCount
                        overloadErrorCount : dashboardViewModel.overloadErrorCount
                        connectionErrorCount : dashboardViewModel.connectionErrorCount
                        fanErrorCount : dashboardViewModel.fanErrorCount
                    }

                    MouseArea{
                        id: recorderAbnormalRectMouseArea

                        anchors.fill: parent
                        hoverEnabled: true

                        cursorShape: containsMouse && recorderErrorDisplayRect.state === "SUMMARY" ? Qt.PointingHandCursor : Qt.ArrowCursor

                        onClicked: recorderErrorDisplayRect.state = "DETAIL"
                    }

                    Rectangle{
                        id: recorderErrorLine1

                        width: 1
                        anchors.top: parent.top
                        anchors.left: recorderErrorStatus.right
                        anchors.bottom: parent.bottom

                        color: WisenetGui.contrast_08_dark_grey
                        visible: recorderErrorDisplayRect.state === "DETAIL"
                    }

                    // List
                    RecorderErrorDetailView{
                        id: recorderErrorDetails

                        flowWidth: dashboardContent.width - (leftRightMargin*2) - recorderErrorStatus.width - (errorDetailMargin*2) - 20
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.left: recorderErrorLine1.right
                        anchors.bottom: parent.bottom
                        anchors.topMargin: errorDetailMargin
                        anchors.leftMargin: errorDetailMargin
                        anchors.rightMargin: errorDetailMargin + 10
                        anchors.bottomMargin: errorDetailMargin

                        visible: recorderErrorDisplayRect.state === "DETAIL"

                        powerErrorListModel: dashboardViewModel.powerErrorListModel
                        diskErrorListModel: dashboardViewModel.diskErrorListModel
                        overloadErrorListModel: dashboardViewModel.overloadErrorListModel
                        connectionErrorListModel: dashboardViewModel.connectionErrorListModel
                        fanErrorListModel: dashboardViewModel.fanErrorListModel

                        powerErrorVisible: dashboardViewModel.powerErrorCount > 0
                        diskErrorVisible: dashboardViewModel.diskErrorCount > 0
                        overloadErrorVisible: dashboardViewModel.overloadErrorCount > 0
                        connectionErrorVisible: dashboardViewModel.connectionErrorCount > 0
                        fanErrorVisible: dashboardViewModel.fanErrorCount > 0
                    }

                    Rectangle{
                        id: recorderErrorLine2

                        width: 1
                        anchors.top: parent.top
                        anchors.right: recorderAbnormalRectFoldButton.left
                        anchors.bottom: parent.bottom

                        visible: recorderErrorDisplayRect.state === "DETAIL"
                        color: WisenetGui.contrast_08_dark_grey
                    }

                    Rectangle{
                        id: recorderAbnormalRectFoldButton

                        width: 18

                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.topMargin: 8
                        anchors.rightMargin: 1
                        anchors.bottomMargin: 8

                        color: "transparent"
                        visible: recorderErrorDisplayRect.state === "DETAIL"

                        Image{
                            width: 18
                            height: 20

                            sourceSize.width: 18
                            sourceSize.height: 20

                            anchors.centerIn: parent
                            source: recorderFoldButtonMouseArea.containsMouse ? "qrc:/Assets/Icon/Dashboard/fold_hover.svg" : "qrc:/Assets/Icon/Dashboard/fold_normal.svg"
                        }

                        MouseArea{
                            id: recorderFoldButtonMouseArea
                            anchors.fill: parent

                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: recorderErrorDisplayRect.state = "SUMMARY"
                        }
                    }
                }

                // Camera error
                Rectangle{
                    id: cameraErrorDisplayRect

                    x: cameraErrorDisplayRect.state === "SUMMARY" ? recorderErrorDisplayRect.x + cardWidth + cardMargin : recorderErrorDisplayRect.x
                    z: 1
                    width: cameraErrorDisplayRect.state === "SUMMARY" ? cardWidth : parent.width - (leftRightMargin * 2)
                    //height: cardHeight

                    border.width: 1
                    border.color: WisenetGui.contrast_08_dark_grey

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    visible: !cameraNormalStatus.visible

                    color: WisenetGui.contrast_10_dark
                    radius: 4

                    state: "SUMMARY"
                    onStateChanged: {
                        if(state === "DETAIL")
                        {
                            recorderErrorDisplayRect.z = 1
                            cameraErrorDisplayRect.z = 2
                        }
                    }

                    Behavior on x {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Behavior on width {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.InOutQuad
                        }
                    }

                    ErrorStatusCard{
                        id: cameraErrorStatus

                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        objectType: WisenetLinguist.camera

                        isCamera: true
                        disconnectedCount : dashboardViewModel.disconnectedCameraCount
                        connectedCount : dashboardViewModel.connectedCameraCount
                    }

                    MouseArea{
                        id: cameraErrorDisplayRectMouseArea

                        anchors.fill: parent
                        hoverEnabled: true

                        cursorShape: containsMouse && cameraErrorDisplayRect.state === "SUMMARY" ? Qt.PointingHandCursor : Qt.ArrowCursor

                        onClicked: cameraErrorDisplayRect.state = "DETAIL"
                    }

                    Rectangle{
                        id: cameraErrorLine1

                        width: 1
                        anchors.top: parent.top
                        anchors.left: cameraErrorStatus.right
                        anchors.bottom: parent.bottom

                        color: WisenetGui.contrast_08_dark_grey
                        visible: cameraErrorDisplayRect.state === "DETAIL"
                    }

                    CameraErrorDetailView{
                        id: cameraErrorDetailView

                        flowWidth: dashboardContent.width - (leftRightMargin*2) - cameraErrorStatus.width - (errorDetailMargin * 2) - 20
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.left: cameraErrorStatus.right
                        anchors.bottom: parent.bottom
                        anchors.topMargin: errorDetailMargin
                        anchors.leftMargin: errorDetailMargin
                        anchors.rightMargin: errorDetailMargin + 10
                        anchors.bottomMargin: errorDetailMargin

                        visible: cameraErrorDisplayRect.state === "DETAIL"

                        disconnectedCameraListModel: dashboardViewModel.disconnectedCameraListSortFilterProxyModel
                        connectedCameraListModel: dashboardViewModel.connectedCameraListSortFilterProxyModel

                        groupTreeProxyModel: dashboardViewModel.groupTreeProxyModel
                        deviceTreeProxyModel: dashboardViewModel.deviceTreeProxyModel

                        onSelectedGroupChanged: {
                            console.log("cameraErrorDetailView.onSelectedGroupChanged", groupId)
                            dashboardViewModel.disconnectedCameraListSortFilterProxyModel.groupId = groupId
                            dashboardViewModel.connectedCameraListSortFilterProxyModel.groupId = groupId

                            dashboardViewModel.disconnectedCameraListSortFilterProxyModel.invalidate()
                            dashboardViewModel.connectedCameraListSortFilterProxyModel.invalidate()
                        }

                        onSelectedDeviceChanged: {
                            console.log("cameraErrorDetailView.onSelectedDeviceChanged", deviceId)
                            dashboardViewModel.disconnectedCameraListSortFilterProxyModel.deviceId = deviceId
                            dashboardViewModel.connectedCameraListSortFilterProxyModel.deviceId = deviceId

                            dashboardViewModel.disconnectedCameraListSortFilterProxyModel.invalidate()
                            dashboardViewModel.connectedCameraListSortFilterProxyModel.invalidate()
                        }

                        onFilterChanged: {
                            console.log("cameraErrorDetailView.onFilterChanged", filter)
                            if(filter === "STATUS")
                            {
                                dashboardViewModel.disconnectedCameraListSortFilterProxyModel.useGroupFilter = false
                                dashboardViewModel.disconnectedCameraListSortFilterProxyModel.useDeviceFilter = false
                                dashboardViewModel.connectedCameraListSortFilterProxyModel.useGroupFilter = false
                                dashboardViewModel.connectedCameraListSortFilterProxyModel.useDeviceFilter = false
                            }
                            else if(filter === "GROUP")
                            {
                                dashboardViewModel.disconnectedCameraListSortFilterProxyModel.useGroupFilter = true
                                dashboardViewModel.disconnectedCameraListSortFilterProxyModel.useDeviceFilter = false
                                dashboardViewModel.connectedCameraListSortFilterProxyModel.useGroupFilter = true
                                dashboardViewModel.connectedCameraListSortFilterProxyModel.useDeviceFilter = false
                            }
                            else if(filter === "NVR")
                            {
                                dashboardViewModel.disconnectedCameraListSortFilterProxyModel.useGroupFilter = false
                                dashboardViewModel.disconnectedCameraListSortFilterProxyModel.useDeviceFilter = true
                                dashboardViewModel.connectedCameraListSortFilterProxyModel.useGroupFilter = false
                                dashboardViewModel.connectedCameraListSortFilterProxyModel.useDeviceFilter = true
                            }

                            dashboardViewModel.disconnectedCameraListSortFilterProxyModel.invalidate()
                            dashboardViewModel.connectedCameraListSortFilterProxyModel.invalidate()
                        }
                    }

                    Rectangle{
                        id: cameraErrorLine2

                        width: 1
                        anchors.top: parent.top
                        anchors.right: cameraErrorDisplayRectFoldButton.left
                        anchors.bottom: parent.bottom

                        visible: cameraErrorDisplayRect.state === "DETAIL"
                        color: WisenetGui.contrast_08_dark_grey
                    }

                    Rectangle{
                        id: cameraErrorDisplayRectFoldButton

                        width: 18

                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.topMargin: 8
                        anchors.rightMargin: 1
                        anchors.bottomMargin: 8

                        color: "transparent"
                        visible: cameraErrorDisplayRect.state === "DETAIL"

                        Image{
                            width: 18
                            height: 20

                            sourceSize.width: 18
                            sourceSize.height: 20

                            anchors.centerIn: parent
                            source: cameraFoldButtonMouseArea.containsMouse ? "qrc:/Assets/Icon/Dashboard/fold_hover.svg" : "qrc:/Assets/Icon/Dashboard/fold_normal.svg"
                        }

                        MouseArea{
                            id: cameraFoldButtonMouseArea
                            anchors.fill: parent

                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: cameraErrorDisplayRect.state = "SUMMARY"
                        }
                    }
                }

                // Chart
                ChartView{
                    id: chartView

                    anchors.left: cameraNormalStatus.right
                    anchors.leftMargin: cardMargin
                    anchors.right: parent.right
                    anchors.rightMargin: leftRightMargin
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    //height: cardHeight
                    color: WisenetGui.contrast_10_dark
                    radius: 4

                    border.width: 1
                    border.color: WisenetGui.contrast_08_dark_grey
                }
            }
        }
    }

    onVisibleChanged: {
        console.log("DashboardView.onVisibleChanged", visible)

        if(visible === true){
            dashboardViewModel.refreshData()
            //chartView.depth1Data = dashboardViewModel.getChartData()
        }
    }

    Connections{
        target: dashboardViewModel.groupTreeModel
        onExpandRoot: cameraErrorDetailView.expandGroupTree()
    }

    Connections{
        target: dashboardViewModel.deviceTreeModel
        onExpandRoot: cameraErrorDetailView.expandDeviceTree()
    }

    Connections{
        target: dashboardViewModel
        onChartDataUpdated:{
            if(!userGroupModel.isAdmin)
                return;

            chartView.depth1Data = vMap;
            chartView.depth2Data = vMap2;
            chartView.depth3Data = vMap3;
            chartView.depth4Data = vMap4;
        }
    }

    // 최대화/복귀 함수
    function doMaximize()
    {
        console.log("doMaximize start state: " + windowState)
        if(windowState != "maximized")
        {
            dashboard.showMaximized()
            windowState = "maximized"
        }
        else
        {
            dashboard.showNormal()
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

            if(dashboard.height - dy >= dashboard.minimumHeight)
            {
                dashboard.setY(dashboard.y + dy)
                dashboard.setHeight(dashboard.height - dy)
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

            if(dashboard.height + dy >= dashboard.minimumHeight)
                dashboard.setHeight(dashboard.height + dy)
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

            if(dashboard.width - dx >= dashboard.minimumWidth)
            {
                dashboard.setX(dashboard.x + dx)
                dashboard.setWidth(dashboard.width - dx)
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

            if(dashboard.width + dx >= dashboard.minimumWidth)
            {
                dashboard.setWidth(dashboard.width + dx)
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

            if(dashboard.width - dx >= dashboard.minimumWidth)
            {
                dashboard.setX(dashboard.x + dx)
                dashboard.setWidth(dashboard.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(dashboard.height - dy >= dashboard.minimumHeight)
            {
                dashboard.setY(dashboard.y + dy)
                dashboard.setHeight(dashboard.height - dy)
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

            if(dashboard.width + dx >= dashboard.minimumWidth)
                dashboard.setWidth(dashboard.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(dashboard.height - dy >= dashboard.minimumHeight)
            {
                dashboard.setY(dashboard.y + dy)
                dashboard.setHeight(dashboard.height - dy)
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

            if(dashboard.width - dx >= dashboard.minimumWidth)
            {
                dashboard.setX(dashboard.x + dx)
                dashboard.setWidth(dashboard.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(dashboard.height + dy >= dashboard.minimumHeight)
                dashboard.setHeight(dashboard.height + dy)
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

            if(dashboard.width + dx >= dashboard.minimumWidth)
                dashboard.setWidth(dashboard.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(dashboard.height + dy >= dashboard.minimumHeight)
                dashboard.setHeight(dashboard.height + dy)
        }
    }
}
