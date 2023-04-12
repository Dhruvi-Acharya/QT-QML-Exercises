import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3

import "tree"
import "setup"
import "login"
import "layoutNavigator"

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1280
    height: 720

    title: qsTr("Wisenet Ui Test")

    minimumWidth: 1280
    minimumHeight: 720

    // signal 정의
    signal layoutChanged(string msg);

    // Qt.FramelessWindowHint : Frame 없이, Qt.Window : 최소화시에 트레이에 아이콘 안나오는 문제 해결
    flags: Qt.FramelessWindowHint | Qt.Window

    //property string windowGuid

    // Declare properties that will store the position of the mouse cursor
    property int previousX
    property int previousY

    // maximized or normal 상태임을 저장
    property string windowState
    property int previousVisibility

    onWindowStateChanged: {
        console.log("onWindowStateChanged state: " + mainWindow.windowState)
    }

    onVisibleChanged:{
        console.log("onVisibleChanged")
    }

    onVisibilityChanged: {

        if(mainWindow.visibility == 2)
        {
            if(windowState == "maximized" && previousVisibility == 3)
            {
                mainWindow.showMaximized()
            }
        }

        previousVisibility = mainWindow.visibility

        console.log("onVisibilityChanged visibility: " + mainWindow.visibility)
    }


    /*
    data: [
        Connections {
            target: mainWindow
            onClicked: console.log("clicked")
        },
        Connections {
            target: mainWindow
            onWindowStateChanged: console.log("onWindowStateChanged state: " + mainWindow.windowState)
        },
        Connections {
            target: mainWindow
            onVisibleChanged: console.log("onVisibleChanged")
        },
        Connections {
            target: mainWindow
            onVisibilityChanged: {

                if(mainWindow.visibility == 2)
                {
                    if(windowState == "maximized" && previousVisibility == 3)
                    {
                        mainWindow.showMaximized()
                    }
                }

                previousVisibility = mainWindow.visibility

                console.log("onVisibilityChanged visibility: " + mainWindow.visibility)
            }
        }
    ]*/



    // 최대화/복귀 함수
    function doMaximize()
    {
        console.log("doMaximize start state: " + windowState)
        if(windowState != "maximized")
        {
            mainWindow.showMaximized()
            windowState = "maximized"
        }
        else
        {
            mainWindow.showNormal()
            windowState = "normal"
        }
        console.log("doMaximize end state: " + windowState)
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

            if(mainWindow.height - dy >= mainWindow.minimumHeight)
            {
                mainWindow.setY(mainWindow.y + dy)
                mainWindow.setHeight(mainWindow.height - dy)
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

            if(mainWindow.height + dy >= mainWindow.minimumHeight)
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

            if(mainWindow.width - dx >= mainWindow.minimumWidth)
            {
                mainWindow.setX(mainWindow.x + dx)
                mainWindow.setWidth(mainWindow.width - dx)
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

            if(mainWindow.width + dx >= mainWindow.minimumWidth)
            {
                mainWindow.setWidth(mainWindow.width + dx)
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

            if(mainWindow.width - dx >= mainWindow.minimumWidth)
            {
                mainWindow.setX(mainWindow.x + dx)
                mainWindow.setWidth(mainWindow.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(mainWindow.height - dy >= mainWindow.minimumHeight)
            {
                mainWindow.setY(mainWindow.y + dy)
                mainWindow.setHeight(mainWindow.height - dy)
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

            if(mainWindow.width + dx >= mainWindow.minimumWidth)
                mainWindow.setWidth(mainWindow.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(mainWindow.height - dy >= mainWindow.minimumHeight)
            {
                mainWindow.setY(mainWindow.y + dy)
                mainWindow.setHeight(mainWindow.height - dy)
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

            if(mainWindow.width - dx >= mainWindow.minimumWidth)
            {
                mainWindow.setX(mainWindow.x + dx)
                mainWindow.setWidth(mainWindow.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(mainWindow.height + dy >= mainWindow.minimumHeight)
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

            if(mainWindow.width + dx >= mainWindow.minimumWidth)
                mainWindow.setWidth(mainWindow.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(mainWindow.height + dy >= mainWindow.minimumHeight)
                mainWindow.setHeight(mainWindow.height + dy)
        }
    }

    // 위쪽 윈도우 바 MouseArea
    MouseArea {
        height: 30
        anchors {
            top: topBorder.bottom
            left: leftBorder.right
            leftMargin: 50
            right: rightBorder.left
            rightMargin: 110
        }

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
            mainWindow.setX(mainWindow.x + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY
            mainWindow.setY(mainWindow.y + dy)
        }

        onDoubleClicked:
        {
            doMaximize()
            console.log("double clicked")
        }
    }

    //view변경함수
    function navigate(page){
        pageLoader.source = page + ".qml";
    }

    AddDeviceWindow {
        id : addDeviceWindow
        transientParent: mainWindow
        visible: false
        //x: Math.round((parent.width - width) / 2)
        //y: Math.round((parent.height - height) / 2)
    }

    ColumnLayout{
        anchors{
            fill: parent
            topMargin: 3
            bottomMargin: 3
            leftMargin: 3
            rightMargin: 3
        }
        spacing: 0

        // Header (Upper Bar)
        Rectangle {
            id: upperBar
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            Layout.preferredWidth: 1280
            Layout.preferredHeight: 26
            Layout.minimumHeight: 26
            Layout.maximumHeight: 26

            border.color: "#444444"
            border.width: 1

            Rectangle{
                id: upperLayout
                anchors.fill: parent
                color: "#303030"

                // MainPopup 보다 뒤에 하면 위로올라옴.
                LayoutNavigator{
                    id: layoutNavigator
                    x: 57
                    y: 1

                    height: 24
                }

                MainPopupMenu {
                    id: mainPopupMenu
                    x: 1
                    y: 1
                    width: 55
                    height: 24
                }

                RowLayout{
                    id: windowResizeButtonsLayout
                    Layout.fillHeight: true

                    anchors {
                        top: parent.top
                        topMargin: 1
                        right: parent.right
                        rightMargin: 1
                    }

                    Image {
                        id: minimizeButton
                        Layout.maximumHeight: 24
                        Layout.maximumWidth: 34

                        source: minimizeButtonMouseArea.containsMouse ? "icon/minimize_button_o.png" : "icon/minimize_button.png"

                        MouseArea
                        {
                            id: minimizeButtonMouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                console.log("minimizeButton clicked")
                                mainWindow.showMinimized()
                            }
                        }
                    }

                    Image {
                        id: maximizeButton
                        Layout.maximumHeight: 24
                        Layout.maximumWidth: 34

                        source: {
                            if(maximizeButtonMouseArea.containsMouse)
                            {
                                if(  windowState == "maximized")
                                    return "icon/normalize_button_o.png"
                                else
                                    return "icon/maximize_button_o.png"
                            }
                            else
                            {
                                if(  windowState == "maximized")
                                    return "icon/normalize_button.png"
                                else
                                    return "icon/maximize_button.png"
                            }
                        }

                        MouseArea
                        {
                            id: maximizeButtonMouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                console.log("maximizeButton clicked")
                                doMaximize()
                            }
                        }
                    }

                    Image {
                        id: closeButton
                        Layout.maximumHeight: 24
                        Layout.maximumWidth: 34

                        source: closeButtonMouseArea.containsMouse ? "icon/x_button_o.png" : "icon/x_button.png"

                        MouseArea
                        {
                            id: closeButtonMouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                console.log("closeButton clicked guid : " + windowGuid)
                                mainWindow.close()

                                console.log("onClosing 1 guid : " + windowGuid)
                                windowHanlder.destroyWindow(windowGuid)
                                console.log("onClosing 2 guid : " + windowGuid)
                            }
                        }
                    }
                }
            }
        }

        // Content Area
        Rectangle {
            Layout.alignment: Qt.AlignTop
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1280
            Layout.preferredHeight: 650

            // main page loader
            Loader{
                id : pageLoader
                anchors.fill: parent
                source: "login/LoginView.qml"
            }
        }

        // Footer
        Rectangle {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            Layout.preferredWidth: 1280
            Layout.preferredHeight: 20

            border.color: "#444444"
            border.width: 1
        }
    }
}
