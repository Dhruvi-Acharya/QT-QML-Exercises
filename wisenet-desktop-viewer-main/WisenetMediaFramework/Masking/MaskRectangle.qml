import QtQuick 2.15
import QtQuick.Shapes 1.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import Wisenet.Define 1.0
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0
import "qrc:/"
import "qrc:/SmartSearch"


Item {
    id: rectItem

    property bool selected: false
    property bool drawing: false
    property bool editing: moveMouseArea.pressed || resizing
    property int maskType: MaskType.Fixed
    property int maskId: 0
    property string maskName: ""
    property alias maskRectangle: maskRect
    property int minimumSize: 20

    // (0,0) ~ (1,1) 좌표계로 정규화 된 좌표
    property real normalX: 0
    property real normalY: 0
    property real normalWidth: 0
    property real normalHeight: 0

    signal deleteButtonClicked()

    function updateRectangle(x, y, width, height) {
        normalX = x / rectItem.width
        normalY = y / rectItem.height
        normalWidth = width / rectItem.width
        normalHeight = height / rectItem.height
    }

    function updateNormalRectangle(normalRect) {
        normalX = normalRect.x
        normalY = normalRect.y
        normalWidth = normalRect.width
        normalHeight = normalRect.height
    }

    function getRectCoordinates() {
        var rectCoordinates = []

        // push [x, y, width, height] data
        rectCoordinates.push(normalX)
        rectCoordinates.push(normalY)
        rectCoordinates.push(normalWidth)
        rectCoordinates.push(normalHeight)

        return rectCoordinates
    }

    function moveArea(dx, dy) {
        var nx = maskRect.x + dx
        var ny = maskRect.y + dy

        if(nx + maskRect.width >= rootItem.width)
            nx = rootItem.width - maskRect.width
        else if(nx < 0)
            nx = 0

        if(ny + maskRect.height >= rootItem.height)
            ny = rootItem.height - maskRect.height
        else if(ny < 0)
            ny =0

        rectItem.normalX = nx / rectItem.width
        rectItem.normalY = ny / rectItem.height
    }

    function resize(dx, dy, fixedTop, fixedLeft) {
        if(fixedTop)
            dy = -dy
        if(fixedLeft)
            dx = -dx

        var top = fixedTop ? maskRect.y : maskRect.y + dy
        var left = fixedLeft ? maskRect.x : maskRect.x + dx
        var newWidth = maskRect.width - dx > minimumSize ? maskRect.width - dx : minimumSize
        var newHeight= maskRect.height - dy > minimumSize ? maskRect.height - dy : minimumSize
        if(!fixedTop && newHeight === minimumSize)
            top = maskRect.y + maskRect.height - minimumSize
        if(!fixedLeft && newWidth === minimumSize)
            left = maskRect.x + maskRect.width - minimumSize
        var bottom = top + newHeight
        var right = left + newWidth

        if(top < 0)
            top = 0
        if(left < 0)
            left = 0
        if(bottom > rectItem.height)
            bottom = rectItem.height
        if(right > rectItem.width)
            right = rectItem.width

        rectItem.normalX = left / rectItem.width
        rectItem.normalY = top / rectItem.height
        rectItem.normalWidth = (right - left) / rectItem.width
        rectItem.normalHeight = (bottom - top) / rectItem.height
    }

    Rectangle {
        id: maskRect
        x: normalX * rectItem.width
        y: normalY * rectItem.height
        z: 1
        width: normalWidth * rectItem.width
        height: normalHeight * rectItem.height
        color: selected ? WisenetGui.color_secondary_dark_opacity30 : WisenetGui.transparent
        border.color: selected ? WisenetGui.color_primary :
                      maskType === MaskType.Fixed ? WisenetGui.color_masking_fixed :
                      maskType === MaskType.Manual ? WisenetGui.color_masking_manual : WisenetGui.color_masking_auto
        border.width: 2

        MouseDragArea {
            id: moveMouseArea
            onPressed: {
                if(fixedDrawControl.currentRect != null || manualDrawControl.currentRect !== null || autoDrawControl.currentRect !== null)
                    return // 현재 그리고 있는 Rectangle이 있으면 리턴

                fixedDrawControl.selectItem(rectItem)
                manualDrawControl.selectItem(rectItem)
                autoDrawControl.selectItem(rectItem)
            }
            onDragPositionChanged: {
                if(rectItem.selected == false)
                    return

                moveArea(dx,dy)
                if(rectItem.maskType == MaskType.Manual) {
                    manualDrawControl.manualBoxUpdated(maskId, getRectCoordinates())
                }
            }
            onClicked :{
                // 현재 그리고 있는 영역이 없으면 click 이벤트를 전파하지 않음 => click은 영역 선택 우선순위로 동작
                if(fixedDrawControl.currentRect === null && manualDrawControl.currentRect === null && autoDrawControl.currentRect === null)
                    mouse.accepted = true
                rootItem.updateFinished(maskId, getRectCoordinates())
            }
        }
    }

    Text {
        id: nameText
        height: 16
        font.pixelSize: 12
        color: WisenetGui.white
        style: Text.Outline
        styleColor: WisenetGui.black
        verticalAlignment: Text.AlignVCenter
        leftPadding: 2
        rightPadding: 2
        text: maskName
        visible: maskRect.visible
        x: maskRect.x + width < rectItem.width ? maskRect.x : rectItem.width - width
        y: maskRect.y > height ? maskRect.y - height : 0
    }

    component ResizeMouseArea: Rectangle {
        id: resizeRect
        z: 3
        width: 14
        height: 14
        color: WisenetGui.transparent
        enabled: rectItem.selected && maskRect.visible

        property alias cursorShape: dragArea.cursorShape
        property alias pressed: dragArea.pressed
        property bool fixedTop: false
        property bool fixedLeft: false

        MouseDragArea {
            id: dragArea
            hoverEnabled: true
            onPressed: {
                if(fixedDrawControl.currentRect !== null || manualDrawControl.currentRect !== null || autoDrawControl.currentRect !== null)
                    return // 현재 그리고 있는 Rectangle이 있으면 리턴

                // select item
                if(rectItem.maskType == MaskType.Fixed)
                    fixedDrawControl.selectItem(rectItem)
                if(rectItem.maskType == MaskType.Manual)
                    manualDrawControl.selectItem(rectItem)
                else if(rectItem.maskType == MaskType.Auto)
                    autoDrawControl.selectItem(rectItem)
            }
            onDragPositionChanged: {
                if(rectItem.selected == false)
                    return

                resize(dx, dy, resizeRect.fixedTop, resizeRect.fixedLeft)

                if(rectItem.maskType == MaskType.Manual) {
                    manualDrawControl.manualBoxUpdated(maskId, getRectCoordinates())
                }
            }
            onClicked: {
                // 현재 그리고 있는 영역이 없으면 click 이벤트를 전파하지 않음 => click은 영역 선택 우선순위로 동작
                if(fixedDrawControl.currentRect === null && manualDrawControl.currentRect === null && autoDrawControl.currentRect === null)
                    mouse.accepted = true
                rootItem.updateFinished(maskId, getRectCoordinates())
            }
        }
    }

    property bool resizing: topLeft1.pressed || topLeft2.pressed || topRight1.pressed || topRight2.pressed ||
                            bottomLeft1.pressed || bottomLeft2.pressed || bottomRight1.pressed || bottomRight2.pressed
    property int resizeAreaSize: 14

    ResizeMouseArea {
        id: topLeft1
        width: Math.min(maskRect.width/2, resizeAreaSize*3)
        height: Math.min(maskRect.height/2, resizeAreaSize)
        x: maskRect.x - resizeAreaSize/2
        y: maskRect.y - resizeAreaSize/2
        cursorShape: Qt.SizeFDiagCursor
        fixedTop: false
        fixedLeft: false
    }
    ResizeMouseArea {
        id: topLeft2
        width: Math.min(maskRect.width/2, resizeAreaSize)
        height: Math.min(maskRect.height/2, resizeAreaSize*3)
        x: maskRect.x - resizeAreaSize/2
        y: maskRect.y - resizeAreaSize/2
        cursorShape: Qt.SizeFDiagCursor
        fixedTop: false
        fixedLeft: false
    }
    ResizeMouseArea {
        id: topRight1
        width: Math.min(maskRect.width/2, resizeAreaSize*3)
        height: Math.min(maskRect.height/2, resizeAreaSize)
        x: maskRect.x + maskRect.width - width + resizeAreaSize/2
        y: maskRect.y - resizeAreaSize/2
        cursorShape: Qt.SizeBDiagCursor
        fixedTop: false
        fixedLeft: true
    }
    ResizeMouseArea {
        id: topRight2
        width: Math.min(maskRect.width/2, resizeAreaSize)
        height: Math.min(maskRect.height/2, resizeAreaSize*3)
        x: maskRect.x + maskRect.width - resizeAreaSize/2
        y: maskRect.y - resizeAreaSize/2
        cursorShape: Qt.SizeBDiagCursor
        fixedTop: false
        fixedLeft: true
    }
    ResizeMouseArea {
        id: bottomLeft1
        width: Math.min(maskRect.width/2, resizeAreaSize*3)
        height: Math.min(maskRect.height/2, resizeAreaSize)
        x: maskRect.x - resizeAreaSize/2
        y: maskRect.y + maskRect.height - resizeAreaSize/2
        cursorShape: Qt.SizeBDiagCursor
        fixedTop: true
        fixedLeft: false
    }
    ResizeMouseArea {
        id: bottomLeft2
        width: Math.min(maskRect.width/2, resizeAreaSize)
        height: Math.min(maskRect.height/2, resizeAreaSize*3)
        x: maskRect.x - resizeAreaSize/2
        y: maskRect.y + maskRect.height - height + resizeAreaSize/2
        cursorShape: Qt.SizeBDiagCursor
        fixedTop: true
        fixedLeft: false
    }
    ResizeMouseArea {
        id: bottomRight1
        width: Math.min(maskRect.width/2, resizeAreaSize*3)
        height: Math.min(maskRect.height/2, resizeAreaSize)
        x: maskRect.x + maskRect.width - width + resizeAreaSize/2
        y: maskRect.y + maskRect.height - resizeAreaSize/2
        cursorShape: Qt.SizeFDiagCursor
        fixedTop: true
        fixedLeft: true
    }
    ResizeMouseArea {
        id: bottomRight2
        width: Math.min(maskRect.width/2, resizeAreaSize)
        height: Math.min(maskRect.height/2, resizeAreaSize*3)
        x: maskRect.x + maskRect.width - resizeAreaSize/2
        y: maskRect.y + maskRect.height - height + resizeAreaSize/2
        cursorShape: Qt.SizeFDiagCursor
        fixedTop: true
        fixedLeft: true
    }
}

