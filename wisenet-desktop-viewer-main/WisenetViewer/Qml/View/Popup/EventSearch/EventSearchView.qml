import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import QtQuick.Dialogs 1.3

import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"

ApplicationWindow {
    id: eventSearch
    width: WisenetGui.eventSearchWindowDefaultWidth
    height: WisenetGui.eventSearchWindowDefaultHeight
    minimumWidth: WisenetGui.eventSearchWindowMinimumWidht
    minimumHeight: WisenetGui.eventSearchWindowMinimumHeight
    visible: true
    color: WisenetGui.contrast_09_dark
    flags: Qt.FramelessWindowHint | Qt.Window

    title: WisenetLinguist.eventSearch

    component CustomToolTip: ToolTip {
        id: reverseTooltip
        visible: tabArea.containsMouse && innerText.truncated
        delay: 200
        text: innerText.text

        leftPadding: 5
        rightPadding: 5
        topPadding: 3 + 5
        bottomPadding: 3
        font.pixelSize: 11

        contentItem: Text {
            text: reverseTooltip.text
            font: reverseTooltip.font
            color: WisenetGui.contrast_08_dark_grey
            padding: 2
        }

        background: Rectangle {
            color: WisenetGui.transparent
            Rectangle {
                anchors.fill: parent
                anchors.topMargin: 5
                color: WisenetGui.contrast_02_light_grey
                radius: 2
            }
            Canvas {
                width: 8
                height: 8
                x: -reverseTooltip.x + reverseTooltip.parent.width/2 - width/2
                anchors.top: parent.top
                contextType: "2d"
                onPaint: {
                    var context = getContext("2d")
                    context.reset();
                    context.moveTo(0, height);
                    context.lineTo(width, height);
                    context.lineTo(width / 2, 0);
                    context.closePath();
                    context.fillStyle = WisenetGui.contrast_02_light_grey
                    context.fill();
                }
            }
        }
    }


    // 최대화/복귀 함수
    function doMaximize()
    {
        console.log("doMaximize start state: " + windowState)
        if(windowState != "maximized")
        {
            eventSearch.showMaximized()
            windowState = "maximized"
        }
        else
        {
            eventSearch.showNormal()
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

    property int eventSearchTitleHeight: 72

    function resetFilter()
    {
        eventSearchContentView.resetFilter()
    }

    function setChannelFilter(channels)
    {
        eventSearchContentView.setChannelFilter(channels)
    }

    function setDateFilter(from, to)
    {
        eventSearchContentView.setDateFilter(from, to)
    }

    function searchByFilter()
    {
        eventSearchContentView.searchByFilter()
    }

    function initializeTree()
    {
        eventSearchContentView.initializeTree()
        objectSearchContentView.initializeTree()
        smartSearchContentView.initializeTree()
        textSearchContentView.initializeTree()
    }

    function setSearchMenu(menuNumber){
        searchStackLayout.currentIndex = menuNumber
    }

    onVisibleChanged: {
        if(visible === false) {
            objectSearchContentView.clear()
            smartSearchContentView.clear()
            textSearchContentView.clear()
        }
    }

    Rectangle{
        id: eventSearchBorder
        anchors.fill: parent
        border.color: WisenetGui.contrast_07_grey
        border.width: 1
        color: WisenetGui.contrast_09_dark

        Rectangle{
            id: eventSearchTitle

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 1
            anchors.leftMargin: 1
            anchors.rightMargin: 1

            height: eventSearchTitleHeight
            color: WisenetGui.contrast_09_dark

            //Move EventSearch Window
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
                    eventSearch.setX(eventSearch.x + dx)
                }

                onMouseYChanged: {
                    if(windowState == "maximized")
                    {
                        return
                    }

                    var dy = mouseY - previousY
                    eventSearch.setY(eventSearch.y + dy)
                }

                onDoubleClicked:
                {
                    doMaximize()
                    console.log("double clicked")
                }
            }

            Rectangle{
                id: eventSearchText

                anchors.top: parent.top
                anchors.left: parent.left

                width: WisenetGui.eventSearchFilterDefaultWidth - 1
                height: 72
                color: WisenetGui.contrast_09_dark

                Text{
                    id: searchTitle
                    anchors.fill: parent
                    anchors.topMargin: 24
                    anchors.leftMargin: 28
                    anchors.rightMargin: 47

                    text: searchStackLayout.currentIndex == 0 ? WisenetLinguist.eventSearch :
                                                                searchStackLayout.currentIndex == 1 ? WisenetLinguist.objectSearch :
                                                                                                      searchStackLayout.currentIndex == 2 ? WisenetLinguist.smartSearch : WisenetLinguist.textSearch
                    color: WisenetGui.contrast_00_white
                    font.pixelSize: 24
                    minimumPixelSize: 10
                    fontSizeMode: Text.VerticalFit
                    wrapMode: Text.Wrap
                }

                WisenetImageButton{
                    id: menuOpenButton

                    width: 24
                    height: 24
                    sourceWidth: 24
                    sourceHeight: 24
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.bottomMargin: 18
                    anchors.rightMargin: 16

                    checked: contentSelectPopup.opened

                    imageNormal: WisenetImage.object_search_open_menu_default
                    imageHover: WisenetImage.object_search_open_menu_hover
                    imageSelected: WisenetImage.object_search_open_menu_selected

                    onClicked: {
                        contentSelectPopup.x = eventSearchText.x + eventSearchText.width - 10
                        contentSelectPopup.y = eventSearchText.y + 18
                        contentSelectPopup.open()
                    }
                }
            }

            Rectangle{
                id: eventSearchRightBorder
                anchors.left: eventSearchText.right
                anchors.top: parent.top
                width: 1
                height: eventSearchTitleHeight
                color: WisenetGui.contrast_08_dark_grey
                visible : (searchStackLayout.currentIndex == 0 && eventSearchContentView.eventSearchFilterViewState === "OPEN") ||
                          (searchStackLayout.currentIndex == 1 && objectSearchContentView.aiSearchFilterViewState === "OPEN") ||
                          (searchStackLayout.currentIndex == 2 && smartSearchContentView.smartSearchFilterViewState === "OPEN") ||
                          (searchStackLayout.currentIndex == 3 && textSearchContentView.textSearchFilterViewState === "OPEN")
            }

            Rectangle{
                id: objectSearchRightPanel

                anchors.left: eventSearchRightBorder.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10

                color: "transparent"

                Rectangle{
                    id: objectSearchControlPanel
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 8

                    height: 25
                    color: "transparent"

                    visible: searchStackLayout.currentIndex == 1

                    Rectangle{
                        id: objectSearchTopLeftMenu

                        width: 80
                        height: 20

                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.leftMargin: 32

                        color: "transparent"

                        Image{
                            id: showBestShotViewButton

                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                            width: 20
                            height: 20

                            source: objectSearchContentView.currentIndex === 0 ? WisenetImage.object_search_thumbnail_view_selected :
                                                                                 showBestShotViewButtonMouseArea.containsMouse ? WisenetImage.object_search_thumbnail_view_hover : WisenetImage.object_search_thumbnail_view_default

                            MouseArea{
                                id: showBestShotViewButtonMouseArea
                                anchors.fill: parent

                                hoverEnabled: true

                                onClicked:{
                                    objectSearchContentView.currentIndex = 0
                                }
                            }

                            CustomToolTip {
                                text: WisenetLinguist.bestShotView
                                delay: 500
                                visible: showBestShotViewButtonMouseArea.containsMouse
                            }
                        }

                        Image{
                            id: showGridButton

                            anchors.left: showBestShotViewButton.right
                            anchors.leftMargin: 10
                            anchors.bottom: parent.bottom
                            width: 20
                            height: 20

                            source: objectSearchContentView.currentIndex === 1 ? WisenetImage.object_search_grid_view_selected :
                                                                                 showGridButtonMouseArea.containsMouse ? WisenetImage.object_search_grid_view_hover : WisenetImage.object_search_grid_view_default

                            MouseArea{
                                id: showGridButtonMouseArea
                                anchors.fill: parent

                                hoverEnabled: true

                                onClicked:{
                                    objectSearchContentView.currentIndex = 1
                                }
                            }

                            CustomToolTip {
                                text: WisenetLinguist.listView
                                delay: 500
                                visible: showGridButtonMouseArea.containsMouse
                            }
                        }

                        Image{
                            id: showflagViewButton

                            anchors.left: showGridButton.right
                            anchors.leftMargin: 10
                            anchors.bottom: parent.bottom
                            width: 20
                            height: 20

                            source: objectSearchContentView.currentIndex === 2 ? WisenetImage.object_search_flag_view_selected :
                                                                                 showflagViewButtonMouseArea.containsMouse ? WisenetImage.object_search_flag_view_hover : WisenetImage.object_search_flag_view_default

                            MouseArea{
                                id: showflagViewButtonMouseArea
                                anchors.fill: parent

                                hoverEnabled: true

                                onClicked:{
                                    objectSearchContentView.currentIndex = 2
                                }
                            }

                            CustomToolTip {
                                text: WisenetLinguist.flagView
                                delay: 500
                                visible: showflagViewButtonMouseArea.containsMouse
                            }
                        }
                    }

                    Rectangle{
                        id: objectSearchExportImageMenu

                        width: 42
                        height: 20

                        color: "transparent"

                        visible: objectSearchContentView.currentIndex === 0 || objectSearchContentView.currentIndex === 2

                        anchors.bottom: parent.bottom
                        anchors.left: objectSearchTopLeftMenu.right
                        anchors.leftMargin: 14

                        Rectangle{
                            id: exportImageSeparator
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            width: 1
                            height: 14
                            color: WisenetGui.contrast_07_grey
                        }

                        WisenetImageButton{
                            id: exportImageButton

                            anchors.top: parent.top
                            anchors.left: exportImageSeparator.left
                            anchors.leftMargin: 10
                            width: 20
                            height: 20
                            sourceWidth: 20
                            sourceHeight: 20

                            imageNormal: WisenetImage.object_search_image_export_default
                            imageHover: WisenetImage.object_search_image_export_hover

                            onClicked: {
                                console.log("export image")
                                exportFolderDialog.visible = true
                            }

                            CustomToolTip {
                                text: WisenetLinguist.saveBestShotImageFile
                                delay: 500
                                visible: parent.hovered
                            }
                        }

                        Rectangle{
                            id: exportImageSeparator2
                            anchors.left: exportImageButton.right
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            width: 1
                            height: 14
                            color: WisenetGui.contrast_07_grey
                        }
                    }

                    Rectangle{
                        id: objectSearchTopCenterMenu

                        visible: objectSearchContentView.currentIndex === 0 || objectSearchContentView.currentIndex === 2

                        width: 190
                        height: 20
                        anchors.left: objectSearchExportImageMenu.right
                        anchors.leftMargin: 14
                        //anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom

                        color: "transparent"

                        Image{
                            id: zoomoutImage

                            width: 20
                            height: 20
                            sourceSize: "20x20"
                            anchors.left: parent.left
                            anchors.top: parent.top

                            source: WisenetImage.object_search_zoom_out
                        }

                        WisenetSlider{
                            id: bestShotHeightSlider

                            anchors.left: zoomoutImage.right
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter

                            height: 11
                            width: 130
                            stepSize: 10

                            from : 100
                            to : 500
                            snapMode: Slider.SnapOnRelease
                            value: 200

                            onValueChanged: {
                                console.log("imageSizeSlider.onValueChanged",  value)
                                objectSearchContentView.setBestShotHeight(value)
                            }
                        }

                        Image{
                            id: zoominImage

                            width: 20
                            height: 20
                            sourceSize: "20x20"
                            anchors.left: bestShotHeightSlider.right
                            anchors.leftMargin: 10
                            anchors.top: parent.top

                            source: WisenetImage.object_search_zoom_in
                        }
                    }
                }

                Image {
                    id: minimizeButton
                    height: 16
                    width: 16
                    anchors.top: parent.top
                    anchors.right: maximizeButton.left
                    anchors.topMargin: 10
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
                            console.log("EventSearch minimizeButton clicked")
                            if(Qt.platform.os === "windows") {
                                eventSearch.showMinimized()
                            }
                            else if (Qt.platform.os === "linux") {
                                // 2023.01.04. 우분투에서 최소화 안되는 현상
                                eventSearch.visible = false;
                            }
                            else {
                                // mac에서는 FramelessWindowHint set시 정상동작하지 않으므로, flag값을 변경한다.
                                if(Qt.platform.os === "osx") {
                                    flags = Qt.Window | Qt.CustomizeWindowHint | Qt.WindowMinMaxButtonsHint
                                }
                                eventSearch.visibility = Window.Windowed
                                eventSearch.visibility = Window.Minimized
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
                    anchors.topMargin: 11
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
                            console.log("EventSearch maximizeButton clicked")
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
                    anchors.topMargin: 11
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
                            eventSearch.close()
                        }
                    }
                }
            }

            Rectangle{
                id: textSearchRightPanel

                anchors.left: eventSearchRightBorder.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10

                color: "transparent"

                Rectangle{
                    id: textSearchControlPanel
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 8

                    height: 25
                    color: "transparent"

                    visible: searchStackLayout.currentIndex == 3

                    Rectangle{
                        id: textSearchTopLeftMenu

                        width: 60
                        height: 20

                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.leftMargin: 32

                        color: "transparent"

                        WisenetImageButton{
                            id: showReceiptViewButton

                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                            width: 20
                            height: 20
                            sourceWidth: 20
                            sourceHeight: 20

                            checkable: true
                            checked: textSearchContentView.currentIndex === 0

                            imageNormal: WisenetImage.object_search_thumbnail_view_default
                            imageSelected: WisenetImage.object_search_thumbnail_view_selected
                            imageHover: WisenetImage.object_search_thumbnail_view_hover

                            onClicked: {
                                textSearchContentView.currentIndex = 0
                            }

                            CustomToolTip {
                                text: WisenetLinguist.receiptView
                                delay: 500
                                visible: parent.hovered
                            }
                        }

                        WisenetImageButton{
                            id: showReceiptGridButton

                            anchors.left: showReceiptViewButton.right
                            anchors.leftMargin: 10
                            anchors.bottom: parent.bottom
                            width: 20
                            height: 20
                            sourceWidth: 20
                            sourceHeight: 20

                            checkable: true
                            checked: textSearchContentView.currentIndex === 1

                            imageNormal: WisenetImage.object_search_grid_view_default
                            imageSelected: WisenetImage.object_search_grid_view_selected
                            imageHover: WisenetImage.object_search_grid_view_hover

                            onClicked: {
                                textSearchContentView.currentIndex = 1
                            }

                            CustomToolTip {
                                text: WisenetLinguist.listView
                                delay: 500
                                visible: parent.hovered
                            }
                        }


                        Rectangle{
                            id: showReceiptSeparator
                            anchors.left: showReceiptGridButton.right
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            width: 1
                            height: 14
                            color: WisenetGui.contrast_07_grey
                        }
                    }

                    Rectangle{
                        id: textSearchTopCenterMenu

                        visible: textSearchContentView.currentIndex === 0 || textSearchContentView.currentIndex === 2

                        width: 190
                        height: 20
                        anchors.left: textSearchTopLeftMenu.right
                        anchors.leftMargin: 14
                        //anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom

                        color: "transparent"

                        Image{
                            id: showReceiptZoomoutImage

                            width: 20
                            height: 20
                            sourceSize: "20x20"
                            anchors.left: parent.left
                            anchors.top: parent.top

                            source: WisenetImage.object_search_zoom_out
                        }

                        WisenetSlider{
                            id: showReceiptSlider

                            anchors.left: showReceiptZoomoutImage.right
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter

                            height: 11
                            width: 130

                            from : 0
                            to : 100
                            snapMode: Slider.SnapOnRelease
                            value: 50
                            stepSize: 10

                            onValueChanged: {
                                console.log("imageSizeSlider.onValueChanged",  value)
                                textSearchContentView.setZoomInOutStepValue(value)
                            }
                        }

                        Image{
                            id: showReceiptZoominImage

                            width: 20
                            height: 20
                            sourceSize: "20x20"
                            anchors.left: showReceiptSlider.right
                            anchors.leftMargin: 10
                            anchors.top: parent.top

                            source: WisenetImage.object_search_zoom_in
                        }
                    }
                }

                Image {
                    id: receiptMinimizeButton
                    height: 16
                    width: 16
                    anchors.top: parent.top
                    anchors.right: receiptMaximizeButton.left
                    anchors.topMargin: 10
                    anchors.rightMargin: 6
                    sourceSize: Qt.size(width, height)
                    source: receiptMinimizeButtonMouseArea.containsPress ? WisenetImage.popup_minimize_press :
                                                                           receiptMinimizeButtonMouseArea.containsMouse ? WisenetImage.popup_minimize_hover : WisenetImage.popup_minimize_default
                    MouseArea
                    {
                        id: receiptMinimizeButtonMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            console.log("EventSearch minimizeButton clicked")
                            if(Qt.platform.os === "windows") {
                                eventSearch.showMinimized()
                            }
                            else if (Qt.platform.os === "linux") {
                                // 2023.01.04. 우분투에서 최소화 안되는 현상
                                eventSearch.visible = false;
                            }
                            else {
                                // mac에서는 FramelessWindowHint set시 정상동작하지 않으므로, flag값을 변경한다.
                                if(Qt.platform.os === "osx") {
                                    flags = Qt.Window | Qt.CustomizeWindowHint | Qt.WindowMinMaxButtonsHint
                                }
                                eventSearch.visibility = Window.Windowed
                                eventSearch.visibility = Window.Minimized
                            }
                        }
                    }
                }

                Image {
                    id: receiptMaximizeButton
                    height: 16
                    width: 16
                    anchors.top: parent.top
                    anchors.right: receiptCloseButton.left
                    anchors.topMargin: 11
                    anchors.rightMargin: 6
                    sourceSize: Qt.size(width, height)
                    source: {
                        if(windowState != "maximized") {
                            return receiptMaximizeButtonMouseArea.containsPress ? WisenetImage.popup_maximize_press :
                                                                                  receiptMaximizeButtonMouseArea.containsMouse ? WisenetImage.popup_maximize_hover : WisenetImage.popup_maximize_default
                        }
                        else {
                            return receiptMaximizeButtonMouseArea.containsPress ? WisenetImage.popup_normalize_press :
                                                                                  receiptMaximizeButtonMouseArea.containsMouse ? WisenetImage.popup_normalize_hover : WisenetImage.popup_normalize_default
                        }
                    }
                    MouseArea
                    {
                        id: receiptMaximizeButtonMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            console.log("EventSearch maximizeButton clicked")
                            doMaximize()
                        }
                    }
                }

                Image {
                    id: receiptCloseButton
                    height: 16
                    width: 16
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 11
                    anchors.rightMargin: 9
                    sourceSize: Qt.size(width, height)
                    source: receiptClloseButtonMouseArea.containsPress ? WisenetImage.popup_close_press :
                                                                         receiptClloseButtonMouseArea.containsMouse ? WisenetImage.popup_close_hover : WisenetImage.popup_close_default
                    MouseArea
                    {
                        id: receiptClloseButtonMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            eventSearch.close()
                        }
                    }
                }
            }

        }

        StackLayout{
            id: searchStackLayout
            anchors.top: eventSearchTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            anchors.bottomMargin: 1

            EventSearchContentView {
                id: eventSearchContentView

                isFocusedWindow: eventSearch.activeFocusItem != null
            }

            ObjectSearchContentView{
                id: objectSearchContentView

                onBestShotHeightChanged: {
                    bestShotHeightSlider.value = height
                }
            }

            SmartSearchContentView {
                id: smartSearchContentView
            }

            TextSearchContentView {
                id: textSearchContentView
            }

            onCurrentIndexChanged: {
                objectSearchContentView.closeAnchoredPopup()
            }
        }
    }

    ContentSelectPopup{
        id: contentSelectPopup

        onCurrentIndexChanged: searchStackLayout.currentIndex = idx
    }

    FileDialog {
        id: exportFolderDialog
        visible: false
        //flags: FolderDialog.ShowDirsOnly
        folder: shortcuts.documents
        selectFolder: true
        onAccepted: {
            var folderPath = filePathConveter.getPathByUrl(fileUrl)
            console.log("exportFolderDialog ", fileUrl, folderPath)

            if(objectSearchContentView.currentIndex == 0)
            {
                objectSearchViewModel.bestShotViewModel.exportImage(folderPath)
            }
            else if(objectSearchContentView.currentIndex == 2)
            {
                objectSearchViewModel.flagViewModel.exportImage(folderPath)
            }
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

            if(eventSearch.height - dy >= eventSearch.minimumHeight)
            {
                eventSearch.setY(eventSearch.y + dy)
                eventSearch.setHeight(eventSearch.height - dy)
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

            if(eventSearch.height + dy >= eventSearch.minimumHeight)
                eventSearch.setHeight(eventSearch.height + dy)
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

            if(eventSearch.width - dx >= eventSearch.minimumWidth)
            {
                eventSearch.setX(eventSearch.x + dx)
                eventSearch.setWidth(eventSearch.width - dx)
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

            if(eventSearch.width + dx >= eventSearch.minimumWidth)
            {
                eventSearch.setWidth(eventSearch.width + dx)
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

            if(eventSearch.width - dx >= eventSearch.minimumWidth)
            {
                eventSearch.setX(eventSearch.x + dx)
                eventSearch.setWidth(eventSearch.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(eventSearch.height - dy >= eventSearch.minimumHeight)
            {
                eventSearch.setY(eventSearch.y + dy)
                eventSearch.setHeight(eventSearch.height - dy)
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

            if(eventSearch.width + dx >= eventSearch.minimumWidth)
                eventSearch.setWidth(eventSearch.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(eventSearch.height - dy >= eventSearch.minimumHeight)
            {
                eventSearch.setY(eventSearch.y + dy)
                eventSearch.setHeight(eventSearch.height - dy)
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

            if(eventSearch.width - dx >= eventSearch.minimumWidth)
            {
                eventSearch.setX(eventSearch.x + dx)
                eventSearch.setWidth(eventSearch.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(eventSearch.height + dy >= eventSearch.minimumHeight)
                eventSearch.setHeight(eventSearch.height + dy)
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

            if(eventSearch.width + dx >= eventSearch.minimumWidth)
                eventSearch.setWidth(eventSearch.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(eventSearch.height + dy >= eventSearch.minimumHeight)
                eventSearch.setHeight(eventSearch.height + dy)
        }
    }
}
