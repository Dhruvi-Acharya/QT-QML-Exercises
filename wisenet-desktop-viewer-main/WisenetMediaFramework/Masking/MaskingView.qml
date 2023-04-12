import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import WisenetLanguage 1.0
import Wisenet.MediaController 1.0
import WisenetMediaFramework 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"
import "qrc:/Masking"
import "qrc:/MediaController"

ApplicationWindow {
    id: rootWindow
    width: 1600
    height: 900
    minimumWidth: 1280
    minimumHeight: 720
    flags: Qt.Window | Qt.FramelessWindowHint
    background: Rectangle{
        anchors.fill: parent
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_07_grey
        border.width: 1
    }

    property string windowState: "normal"   // normar or maximized
    property int leftAreaWidth: 244
    property int rightAreaWidth: 392
    property int pressedX: 0
    property int pressedY: 0

    function doMaximize() {
        if(windowState == "maximized") {
            windowState = "normal"
            showNormal()
        }
        else {
            windowState = "maximized"
            showMaximized()
        }
    }

    function startMaskingExport(startTime, endTime, filePath, fileName,
                                usePassword, password, useDigitalSignature) {
        console.log("MaskingView::startExport()", new Date(startTime), new Date(endTime), filePath, fileName)

        if(viewingGrid.focusedItem.objectName != "videoItem")
            return

        if(!viewingGrid.focusedItem || !viewingGrid.focusedItem.isLocalResource)
            return

        viewingGrid.focusedItem.player.startMaskingExport(startTime, endTime, filePath, fileName,
                                                          usePassword, password, useDigitalSignature)
    }

    function stopMaskingExport() {
        console.log("MaskingView::stopMaskingExport()")

        if(viewingGrid.focusedItem.objectName != "videoItem")
            return

        if(!viewingGrid.focusedItem || !viewingGrid.focusedItem.isLocalResource)
            return

        viewingGrid.focusedItem.player.stopMaskingExport()
    }

    onVisibilityChanged: {
        if(windowState == "maximized" && visibility == 2)
            showMaximized()
    }

    // Header (title & window buttons)
    Rectangle {
        id: headerRect
        color: WisenetGui.contrast_09_dark
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 70
        anchors.margins: 1

        Rectangle {
            width: leftAreaWidth
            height: parent.height
            color: WisenetGui.transparent
            border.color: WisenetGui.contrast_08_dark_grey
            border.width: 1

            Text {
                font.pixelSize: 24
                color: WisenetGui.contrast_00_white
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                text: WisenetLinguist.masking
                elide: Text.ElideRight
            }
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

                var newX = rootWindow.x + mouseX - pressedX
                var newY = rootWindow.y + mouseY - pressedY

                if(newX+10 > Screen.desktopAvailableWidth)
                    newX = Screen.desktopAvailableWidth - 10
                if(newY+10 > Screen.desktopAvailableHeight)
                    newY = Screen.desktopAvailableHeight - 10

                rootWindow.x = newX
                rootWindow.y = newY
            }
            onDoubleClicked: doMaximize()
        }

        WindowButton {
            id: minimizeButton
            anchors.right: maximizeButton.left
            anchors.rightMargin: 6
            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: parent.pressed ? WisenetImage.popup_minimize_press :
                                         parent.hovered ? WisenetImage.popup_minimize_hover : WisenetImage.popup_minimize_default
            }
            onClicked: rootWindow.showMinimized()
        }
        WindowButton {
            id: maximizeButton
            anchors.right: closeButton.left
            anchors.rightMargin: 6
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
            onClicked: doMaximize()
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
            onClicked: rootWindow.close()
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

    // Left Area (mask setting & list)
    Rectangle {
        id: leftArea
        width: leftAreaWidth
        anchors.top: headerRect.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 1
        anchors.bottomMargin: 1
        color: WisenetGui.contrast_09_dark
        enabled: !exportSettingView.isInProgress

        MaskSettingView {
            id: maskSettingView
            width: parent.width
            height: 160
            anchors.top: parent.top
            anchors.bottom: spliter.top

            onOpenDialog:{
                viewingGrid.openMediaDialog()
            }

            onAutoButtonClicked:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.startAutoMasking(isOn)
                maskItemListView.startAutoMasking(isOn)
            }
            onFixedButtonClicked:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.startFixedMasking(isOn)
                maskItemListView.startFixedMasking(isOn)
            }
            onManualButtonClicked:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.startManualMasking(isOn)
                maskItemListView.startManualMasking(isOn)
            }
            onPreviewButtonClicked:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.player.maskPreviewOn = isOn
            }

            onMaskPaintTypeChanged:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.player.setMaskPaintType(maskPaintType)
            }

            onMosaicCellSizeChanged:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.player.setMaskMosaicCellSize(mosaicCellSize)
            }

            onSolidColorChanged:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.player.setMaskRGBColor(solidColor)
            }

            onAreaTypeChanged:{
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.player.setMaskInclude(areaType)
            }
        }

        Rectangle {
            id: spliter

            width: parent.width
            anchors.top: maskSettingView.bottom
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        MaskItemListView {
            id: maskItemListView
            width: parent.width
            anchors.top: spliter.bottom
            anchors.bottom: parent.bottom
            listModel: viewingGrid.focusedItem.player.maskItemListModel

            onSelectedMaskItemChanged:{
                console.log("maskItemListView.onSelectedMaskItemChanged", maskId, startTime, endTime)

                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.selectMaskItem(maskId)
            }

            onMaskTypeChanged:{
                console.log("maskItemListView.onMaskTypeChanged", maskType)
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                maskSettingView.selectMaskType(maskType)
            }

            onMaskItemVisibleChanged:{
                console.log("maskItemListView.onMaskItemVisibleChanged", maskId, isVisible)

                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.changeMaskItemVisible(maskId, isVisible)
                viewingGrid.focusedItem.player.refreshMaskResult()
            }

            onMaskItemNameChanged:{
                console.log("maskItemListView.onMaskItemVisibleChanged", maskId, maskName)
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.changeMaskItemName(maskId, maskName)
            }

            onMaskItemRemoved:{
                console.log("maskItemListView.onMaskItemRemoved", maskId)
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.removeMaskItem(maskId)
            }

            onRequestSeek:{
                mediaControllerView.requestSeek(time)
            }

            onSelectedStartTimeChanged: {
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.player.refreshMaskResult()
            }

            onSelectedEndTimeChanged: {
                if(viewingGrid.focusedItem.objectName != "videoItem")
                    return

                viewingGrid.focusedItem.player.refreshMaskResult()
            }
        }

        Rectangle {
            width: 1
            height: parent.height
            anchors.right: parent.right
            color: WisenetGui.contrast_08_dark_grey
        }
    }

    // Right Area (export setting)
    Rectangle {
        id: rightArea
        width: rightAreaWidth
        anchors.top: headerRect.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 1
        anchors.bottomMargin: 1
        color: WisenetGui.contrast_09_dark

        MaskingExportSettingView {
            id: exportSettingView
            anchors.fill: parent
            focusedViewingGridItem: viewingGrid.focusedItem
        }
    }

    // Viewing Grid
    ViewingGrid {
        id: viewingGrid
        z: -1
        anchors.top: headerRect.bottom
        anchors.bottom: mediaControllerView.top
        anchors.left: leftArea.right
        anchors.right: rightArea.left
        isFocusedWindow: rootWindow.activeFocusItem != null
        focus: true
        enabled: !exportSettingView.isInProgress

        viewX : 0
        viewY : 0
        viewWidth: width
        viewHeight: height
        standalone: true
        sleepModeDuration: 0
        isMaskingMode: true

        onItemsAdded: {
            console.log("MaskingView.viewingGrid.onItemsAdded", maskSettingView.previewOn, maskSettingView.fixedOn,
                        maskSettingView.manualOn, maskSettingView.autoOn)

            maskSettingView.previewOn = true
            viewingGrid.focusedItem.player.maskPreviewOn = maskSettingView.previewOn

            maskSettingView.fixedOn = true

            if(maskSettingView.fixedOn)
                viewingGrid.focusedItem.startFixedMasking(maskSettingView.fixedOn)
            if(maskSettingView.manualOn)
                viewingGrid.focusedItem.startManualMasking(maskSettingView.manualOn)
            if(maskSettingView.autoOn)
                viewingGrid.focusedItem.startAutoMasking(maskSettingView.autoOn)

            viewingGrid.focusedItem.player.setMaskPaintType(maskSettingView.maskPaintType)
            viewingGrid.focusedItem.player.setMaskMosaicCellSize(maskSettingView.mosaicCellSize)
            viewingGrid.focusedItem.player.setMaskRGBColor(maskSettingView.solidColor)
            viewingGrid.focusedItem.player.setMaskInclude(maskSettingView.areaType)
        }

        onSelectedMaskItemChanged: {
            console.log("viewingGrid.onSelectedMaskItemChanged", maskId)
            maskItemListView.selectMaskItem(maskId)
        }

        onShowTrackingFailMessage: {
            console.log("viewingGrid.onShowTrackingFailMessage")
            viewingGrid.trackingFailed()
        }
    }

    // Media Controller (Timeline)
    MediaControllerView {
        id: mediaControllerView
        anchors.right: rightArea.left
        anchors.left: leftArea.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        enabled: !exportSettingView.isInProgress

        simpleTimeline: true
        selectedViewingGrid: viewingGrid
        focusedViewingGridItem: viewingGrid.focusedItem
        selectedMaskStartTime: maskItemListView.selectedStartTime
        selectedMaskEndTime: maskItemListView.selectedEndTime
        enableSpeedControl: focusedViewingGridItem && focusedViewingGridItem.hasPlayer ? !focusedViewingGridItem.player.isTracking : true

        onSelectedTimelineUpdated: {
            // default selection range = all period
            selectionStartTime = timelineStartTime
            selectionEndTime = timelineEndTime

            maskItemListView.timelineStartTime = timelineStartTime
            maskItemListView.timelineEndTime = timelineEndTime
        }
    }


    // Resize MouseArea
    MouseArea {
        // 상
        width: parent.width
        height: 5
        acceptedButtons: Qt.LeftButton
        cursorShape: windowState != "maximized" && enabled ? Qt.SizeVerCursor : Qt.ArrowCursor
        onPressed: pressedY = mouseY
        onMouseYChanged: resizeUp(mouseY)
    }
    MouseArea {
        // 하
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
        width: 5
        height: parent.height
        acceptedButtons: Qt.LeftButton
        cursorShape: windowState != "maximized" && enabled ? Qt.SizeHorCursor : Qt.ArrowCursor
        onPressed: pressedX = mouseX
        onMouseXChanged: resizeLeft(mouseX)
    }
    MouseArea {
        // 우
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
    MouseArea {
        // 좌하
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

    // Resize functions
    function resizeLeft(currentX) {
        if(windowState == "maximized")
            return

        var dx = currentX - pressedX
        if(rootWindow.width - dx < rootWindow.minimumWidth)
            dx = rootWindow.width - rootWindow.minimumWidth
        if(rootWindow.width - dx > rootWindow.maximumWidth)
            dx = rootWindow.width - rootWindow.maximumWidth

        rootWindow.setX(rootWindow.x + dx)
        rootWindow.setWidth(rootWindow.width - dx)
    }
    function resizeRight(currentX) {
        if(windowState == "maximized")
            return

        var dx = currentX - pressedX
        if(rootWindow.width + dx < rootWindow.minimumWidth)
            dx = rootWindow.minimumWidth - rootWindow.width
        if(rootWindow.width + dx > rootWindow.maximumWidth)
            dx = rootWindow.maximumWidth - rootWindow.width

        rootWindow.setWidth(rootWindow.width + dx)
    }
    function resizeUp(currentY) {
        if(windowState == "maximized")
            return

        var dy = currentY - pressedY
        if(rootWindow.height - dy < rootWindow.minimumHeight)
            dy = rootWindow.height - rootWindow.minimumHeight
        if(rootWindow.height - dy > rootWindow.maximumHeight)
            dy = rootWindow.height - rootWindow.maximumHeight

        rootWindow.setY(rootWindow.y + dy)
        rootWindow.setHeight(rootWindow.height - dy)
    }
    function resizeDown(currentY) {
        if(windowState == "maximized")
            return

        var dy = currentY - pressedY
        if(rootWindow.height + dy < rootWindow.minimumHeight)
            dy = rootWindow.minimumHeight - rootWindow.height
        if(rootWindow.height + dy > rootWindow.maximumHeight)
            dy = rootWindow.maximumHeight - rootWindow.height

        rootWindow.setHeight(rootWindow.height + dy)
    }
}
