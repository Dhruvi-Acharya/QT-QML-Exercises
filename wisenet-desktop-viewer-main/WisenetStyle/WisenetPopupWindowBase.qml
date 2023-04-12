import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetLanguage 1.0

ApplicationWindow {
    id: baseWindow
    width: 530
    height: 422
    modality: Qt.ApplicationModal
    flags: Qt.Window | Qt.FramelessWindowHint
    background: Rectangle{
        anchors.fill: parent
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1
    }

    property bool minmaxButtonVisible: true
    property bool resizeEnabled: true
    property bool applyButtonVisible: true
    property bool cancelButtonVisible: true
    property alias applyButtonText: applyButton.text
    property alias cancelButtonText: cancelButton.text
    property int buttonHeightSize : 28
    property int buttonWidthSize : 120

    signal applyButtonClicked()
    signal cancelButtonClicked()

    property string windowState: "normal"   // normar or maximized
    property int pressedX: 0
    property int pressedY: 0

    function doMaximize() {
        if(!minmaxButtonVisible)
            return

        if(windowState == "maximized") {
            windowState = "normal"
            showNormal()
        }
        else {
            windowState = "maximized"
            showMaximized()
        }
    }

    function setTitle(titleText){
        baseWindow.title = titleText
    }

    onVisibilityChanged: {
        if(windowState == "maximized" && visibility == 2)
            showMaximized()
    }

    header: Rectangle {
        color: WisenetGui.transparent
        height: 70
        Text {
            font.pixelSize: 24
            color: WisenetGui.contrast_00_white
            anchors.left: parent.left
            anchors.leftMargin: 34
            anchors.right: minmaxButtonVisible ? minimizeButton.left : closeButton.left
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            text: baseWindow.title
            elide: Text.ElideRight
        }
        Rectangle {
            color: WisenetGui.contrast_08_dark_grey
            height: 1
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 35
            anchors.right: parent.right
            anchors.rightMargin: 35
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onPressed: {
                pressedX = mouseX
                pressedY = mouseY
            }
            onPositionChanged: {
                if(windowState == "maximized")
                    return

                var newX = baseWindow.x + mouseX - pressedX
                var newY = baseWindow.y + mouseY - pressedY

                if(newX+10 > Screen.desktopAvailableWidth)
                    newX = Screen.desktopAvailableWidth - 10
                if(newY+10 > Screen.desktopAvailableHeight)
                    newY = Screen.desktopAvailableHeight - 10

                baseWindow.x = newX
                baseWindow.y = newY
            }
            onDoubleClicked: {
                doMaximize()
            }
        }
        WindowButton {
            id: minimizeButton
            anchors.right: maximizeButton.left
            anchors.rightMargin: 6
            visible: baseWindow.minmaxButtonVisible
            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: parent.pressed ? WisenetImage.popup_minimize_press :
                        parent.hovered ? WisenetImage.popup_minimize_hover : WisenetImage.popup_minimize_default
            }
            onClicked: {
                baseWindow.showMinimized()
            }
        }
        WindowButton {
            id: maximizeButton
            anchors.right: closeButton.left
            anchors.rightMargin: 6
            visible: baseWindow.minmaxButtonVisible
            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: {
                    if(windowState != "maximized") {
                        return parent.pressed ? WisenetImage.popup_maximize_press :
                               parent.hovered ? WisenetImage.popup_maximize_hover : WisenetImage.popup_maximize_default
                    }
                    else {
                        return parent.pressed ? WisenetImage.popup_normalize_press :
                               parent.hovered ? WisenetImage.popup_normalize_hover : WisenetImage.popup_normalize_default
                    }
                }
            }
            onClicked: {
                doMaximize()
            }
        }
        WindowButton {
            id: closeButton
            anchors.right: parent.right
            anchors.rightMargin: 9

            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: parent.pressed ? WisenetImage.popup_close_press :
                        parent.hovered ? WisenetImage.popup_close_hover : WisenetImage.popup_close_default
            }
            onClicked: {
                baseWindow.close()
            }
        }

        // Resize MouseArea
        MouseArea {
            // 상
            enabled: resizeEnabled
            width: parent.width
            height: 5
            acceptedButtons: Qt.LeftButton
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeVerCursor : Qt.ArrowCursor
            onPressed: pressedY = mouseY
            onMouseYChanged: resizeUp(mouseY)
        }
        MouseArea {
            // 좌
            enabled: resizeEnabled
            width: 5
            height: parent.height
            acceptedButtons: Qt.LeftButton
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeHorCursor : Qt.ArrowCursor
            onPressed: pressedX = mouseX
            onMouseXChanged: resizeLeft(mouseX)
        }
        MouseArea {
            // 우
            enabled: resizeEnabled
            width: 5
            height: parent.height
            anchors.right: parent.right
            acceptedButtons: Qt.LeftButton
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeHorCursor : Qt.ArrowCursor
            onPressed: pressedX = mouseX
            onMouseXChanged: resizeRight(mouseX)
        }
        MouseArea {
            // 좌상
            enabled: resizeEnabled
            height: 5
            width: 5
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeFDiagCursor : Qt.ArrowCursor
            onPressed: {
                pressedX = mouseX
                pressedY = mouseY
            }
            onPositionChanged: {
                resizeLeft(mouseX)
                resizeUp(mouseY)
            }
        }
        MouseArea {
            // 우상
            enabled: resizeEnabled
            height: 5
            width: 5
            anchors.right: parent.right
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeBDiagCursor : Qt.ArrowCursor
            onPressed: {
                pressedX = mouseX
                pressedY = mouseY
            }
            onPositionChanged: {
                resizeRight(mouseX)
                resizeUp(mouseY)
            }
        }

        component WindowButton: Button{
            y: 11
            width: 16
            height: 16
            hoverEnabled: true
            background: Rectangle {
                color: WisenetGui.transparent
            }
        }
    }

    footer: Rectangle {
        color: WisenetGui.transparent
        height: 70
        WisenetOrangeButton {
            id: applyButton
            visible: applyButtonVisible
            x: parent.width / 2 - width - 5
            width: baseWindow.buttonWidthSize
            height: baseWindow.buttonHeightSize
            anchors.verticalCenter: parent.verticalCenter
            text: WisenetLinguist.apply
            onClicked: applyButtonClicked()
        }

        WisenetGrayButton {
            id: cancelButton
            visible: cancelButtonVisible
            x: applyButtonVisible ? parent.width / 2 + 5 : (parent.width - width) / 2
            width: baseWindow.buttonWidthSize
            height: baseWindow.buttonHeightSize
            anchors.verticalCenter: parent.verticalCenter
            text: WisenetLinguist.cancel
            onClicked: {
                baseWindow.close()
                baseWindow.cancelButtonClicked()
            }
        }

        // Resize MouseArea
        MouseArea {
            // 하
            enabled: resizeEnabled
            width: parent.width
            height: 5
            anchors.bottom: parent.bottom
            acceptedButtons: Qt.LeftButton
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeVerCursor : Qt.ArrowCursor
            onPressed: pressedY = mouseY
            onMouseYChanged: resizeDown(mouseY)
        }
        MouseArea {
            // 좌
            enabled: resizeEnabled
            width: 5
            height: parent.height
            acceptedButtons: Qt.LeftButton
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeHorCursor : Qt.ArrowCursor
            onPressed: pressedX = mouseX
            onMouseXChanged: resizeLeft(mouseX)
        }
        MouseArea {
            // 우
            enabled: resizeEnabled
            width: 5
            height: parent.height
            anchors.right: parent.right
            acceptedButtons: Qt.LeftButton
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeHorCursor : Qt.ArrowCursor
            onPressed: pressedX = mouseX
            onMouseXChanged: resizeRight(mouseX)
        }
        MouseArea {
            // 좌하
            enabled: resizeEnabled
            height: 5
            width: 5
            anchors.bottom: parent.bottom
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeBDiagCursor : Qt.ArrowCursor
            onPressed: {
                pressedX = mouseX
                pressedY = mouseY
            }
            onPositionChanged: {
                resizeLeft(mouseX)
                resizeDown(mouseY)
            }
        }
        MouseArea {
            // 우하
            enabled: resizeEnabled
            height: 5
            width: 5
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            cursorShape: windowState != "maximized" && enabled ? Qt.SizeFDiagCursor : Qt.ArrowCursor
            onPressed: {
                pressedX = mouseX
                pressedY = mouseY
            }
            onPositionChanged: {
                resizeRight(mouseX)
                resizeDown(mouseY)
            }
        }
    }

    // Resize MouseArea
    MouseArea {
        // 좌
        enabled: resizeEnabled
        width: 5
        height: parent.height
        acceptedButtons: Qt.LeftButton
        cursorShape: windowState != "maximized" && enabled ? Qt.SizeHorCursor : Qt.ArrowCursor
        onPressed: pressedX = mouseX
        onMouseXChanged: resizeLeft(mouseX)
    }
    MouseArea {
        // 우
        enabled: resizeEnabled
        width: 5
        height: parent.height
        anchors.right: parent.right
        acceptedButtons: Qt.LeftButton
        cursorShape: windowState != "maximized" && enabled ? Qt.SizeHorCursor : Qt.ArrowCursor
        onPressed: pressedX = mouseX
        onMouseXChanged: resizeRight(mouseX)
    }

    function resizeLeft(currentX) {
        if(windowState == "maximized" || !resizeEnabled)
            return

        var dx = currentX - pressedX
        if(baseWindow.width - dx < baseWindow.minimumWidth)
            dx = baseWindow.width - baseWindow.minimumWidth
        if(baseWindow.width - dx > baseWindow.maximumWidth)
            dx = baseWindow.width - baseWindow.maximumWidth

        baseWindow.setX(baseWindow.x + dx)
            baseWindow.setWidth(baseWindow.width - dx)
    }

    function resizeRight(currentX) {
        if(windowState == "maximized" || !resizeEnabled)
            return

        var dx = currentX - pressedX
        if(baseWindow.width + dx < baseWindow.minimumWidth)
            dx = baseWindow.minimumWidth - baseWindow.width
        if(baseWindow.width + dx > baseWindow.maximumWidth)
            dx = baseWindow.maximumWidth - baseWindow.width

        baseWindow.setWidth(baseWindow.width + dx)
    }

    function resizeUp(currentY) {
        if(windowState == "maximized" || !resizeEnabled)
            return

        var dy = currentY - pressedY
        if(baseWindow.height - dy < baseWindow.minimumHeight)
            dy = baseWindow.height - baseWindow.minimumHeight
        if(baseWindow.height - dy > baseWindow.maximumHeight)
            dy = baseWindow.height - baseWindow.maximumHeight

        baseWindow.setY(baseWindow.y + dy)
        baseWindow.setHeight(baseWindow.height - dy)
    }

    function resizeDown(currentY) {
        if(windowState == "maximized" || !resizeEnabled)
            return

        var dy = currentY - pressedY
        if(baseWindow.height + dy < baseWindow.minimumHeight)
            dy = baseWindow.minimumHeight - baseWindow.height
        if(baseWindow.height + dy > baseWindow.maximumHeight)
            dy = baseWindow.maximumHeight - baseWindow.height

        baseWindow.setHeight(baseWindow.height + dy)
    }
}
