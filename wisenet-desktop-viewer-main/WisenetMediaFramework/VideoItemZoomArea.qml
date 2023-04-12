import QtQuick 2.15
import QtQuick.Shapes 1.15
import WisenetMediaFramework 1.0

Item {
    id: zoomAreaRoot

    // 0~1사이의 normlized value
    property var zoomTargetItem
    property string zoomTargetItemId

    property alias areaX: zoomAreaRect.x
    property alias areaY: zoomAreaRect.y
    property alias areaWidth: zoomAreaRect.width
    property alias areaHeight: zoomAreaRect.height
    property alias borderColor: zoomAreaRect.border.color

    property int orientation: 0
    property bool isDefaultAspect: (orientation%180===0)
    property real contentRectX: 0
    property real contentRectY: 0
    property real contentRectW: 0
    property real contentRectH: 0
    property real normalRectX: 0
    property real normalRectY: 0
    property real normalRectW: 0
    property real normalRectH: 0
    property bool editable: true
    property bool dewarpEnabled: false
    property bool zoomAreaVisible: false
    property bool visibleByZoomTarget: false
    property int fisheyeViewMode: 0
    visible: zoomAreaVisible || visibleByZoomTarget

    signal updateNormalRect(Item zoomArea)

    function onZoomTargetFocused(focused)
    {
        if (dewarpEnabled) {
            if (focused && fisheyeViewMode === MediaLayoutItemViewModel.V_Original)
                visibleByZoomTarget = true;
            else
                visibleByZoomTarget = false;
        }
        else {
            visibleByZoomTarget = focused;
        }
    }
    function onZoomTargetFisheyeViewPortChanged(positions)
    {
        if (positions.length === 32) {
            dewarpViewPath.x1 = positions[0]; dewarpViewPath.y1 = positions[1];
            dewarpViewPath.x2 = positions[2]; dewarpViewPath.y2 = positions[3];
            dewarpViewPath.x3 = positions[4]; dewarpViewPath.y3 = positions[5];
            dewarpViewPath.x4 = positions[6]; dewarpViewPath.y4 = positions[7];
            dewarpViewPath.x5 = positions[8]; dewarpViewPath.y5 = positions[9];
            dewarpViewPath.x6 = positions[10]; dewarpViewPath.y6 = positions[11];
            dewarpViewPath.x7 = positions[12]; dewarpViewPath.y7 = positions[13];
            dewarpViewPath.x8 = positions[14]; dewarpViewPath.y8 = positions[15];
            dewarpViewPath.x9 = positions[16]; dewarpViewPath.y9 = positions[17];
            dewarpViewPath.x10 = positions[18]; dewarpViewPath.y10 = positions[19];
            dewarpViewPath.x11 = positions[20]; dewarpViewPath.y11 = positions[21];
            dewarpViewPath.x12 = positions[22]; dewarpViewPath.y12 = positions[23];
            dewarpViewPath.x13 = positions[24]; dewarpViewPath.y13 = positions[25];
            dewarpViewPath.x14 = positions[26]; dewarpViewPath.y14 = positions[27];
            dewarpViewPath.x15 = positions[28]; dewarpViewPath.y15 = positions[29];
            dewarpViewPath.x16 = positions[30]; dewarpViewPath.y16 = positions[31];
        }
    }

    Item {
        id: videoContentRect
        x: contentRectX
        y: contentRectY
        width: contentRectW
        height: contentRectH

        Rectangle {
            id: zoomAreaRectBg
            anchors.fill: zoomAreaRect
            anchors.margins: 1
            color: zoomAreaRect.border.color
            opacity: mouseArea.containsMouse ? 0.3 : 0.0
            visible: !dewarpEnabled && opacity > 0.0
        }
        // 드와핑 영상에 대한 영역 표시할 때
        Shape {
            id: dewarpViewPath
            anchors.fill: parent
            visible: dewarpEnabled
            opacity: mouseArea.containsMouse ? 0.3 : 0.2

            property real x1
            property real x2
            property real x3
            property real x4
            property real x5
            property real x6
            property real x7
            property real x8
            property real x9
            property real x10
            property real x11
            property real x12
            property real x13
            property real x14
            property real x15
            property real x16
            property real y1
            property real y2
            property real y3
            property real y4
            property real y5
            property real y6
            property real y7
            property real y8
            property real y9
            property real y10
            property real y11
            property real y12
            property real y13
            property real y14
            property real y15
            property real y16

            ShapePath {
                fillColor: zoomAreaRect.border.color
                strokeColor : zoomAreaRect.border.color
                startX: dewarpViewPath.x1*videoContentRect.width; startY: dewarpViewPath.y1*videoContentRect.height
                PathCurve {x: dewarpViewPath.x2*videoContentRect.width; y: dewarpViewPath.y2*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x3*videoContentRect.width; y: dewarpViewPath.y3*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x4*videoContentRect.width; y: dewarpViewPath.y4*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x5*videoContentRect.width; y: dewarpViewPath.y5*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x6*videoContentRect.width; y: dewarpViewPath.y6*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x7*videoContentRect.width; y: dewarpViewPath.y7*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x8*videoContentRect.width; y: dewarpViewPath.y8*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x9*videoContentRect.width; y: dewarpViewPath.y9*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x10*videoContentRect.width; y: dewarpViewPath.y10*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x11*videoContentRect.width; y: dewarpViewPath.y11*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x12*videoContentRect.width; y: dewarpViewPath.y12*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x13*videoContentRect.width; y: dewarpViewPath.y13*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x14*videoContentRect.width; y: dewarpViewPath.y14*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x15*videoContentRect.width; y: dewarpViewPath.y15*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x16*videoContentRect.width; y: dewarpViewPath.y16*videoContentRect.height}
                PathCurve {x: dewarpViewPath.x1*videoContentRect.width; y: dewarpViewPath.y1*videoContentRect.height}
            }
        }

        Rectangle {
            id: zoomAreaRect
            x: {
                if (orientation===90)
                    return normalRectY * contentRectW
                else if (orientation === 180)
                    return contentRectW - ((normalRectW + normalRectX) * contentRectW)
                else if (orientation === 270)
                    return contentRectW - ((normalRectH + normalRectY) * contentRectW)
                return normalRectX * contentRectW
            }
            y: {
                if (orientation===90)
                    return contentRectH - ((normalRectW + normalRectX) * contentRectH)
                else if (orientation === 180)
                    return contentRectH - ((normalRectH + normalRectY) * contentRectH)
                else if (orientation === 270)
                    return (normalRectX * contentRectH)
                return normalRectY * contentRectH
            }
            width: isDefaultAspect ? normalRectW * contentRectW : normalRectH * contentRectW
            height: isDefaultAspect ? normalRectH * contentRectH : normalRectW * contentRectH
            color:"transparent"
            border.color: "white"
            border.width:1

            function updatePosition(nx, ny, nw, nh) {
                if (orientation == 0) {
                    normalRectW = nw/contentRectW
                    normalRectH = nh/contentRectH
                    normalRectX = nx/contentRectW
                    normalRectY = ny/contentRectH
                }
                else if (orientation == 90) {
                    normalRectW = nh/contentRectH
                    normalRectH = nw/contentRectW
                    normalRectX = 1 - normalRectW - (ny/contentRectH)
                    normalRectY = nx/contentRectW
                }
                else if (orientation == 180) {
                    normalRectW = nw/contentRectW
                    normalRectH = nh/contentRectH
                    normalRectX = 1 - normalRectW - (nx/contentRectW)
                    normalRectY = 1 - normalRectH - (ny/contentRectH)
                }
                else if (orientation == 270) {
                    normalRectW = nh/contentRectH
                    normalRectH = nw/contentRectW
                    normalRectX = ny/contentRectH
                    normalRectY = 1 - normalRectH - (nx/contentRectW)
                }
                zoomAreaRoot.updateNormalRect(zoomAreaRoot);
            }
        }

        MouseArea {
            id: mouseArea
            visible: zoomAreaRoot.editable
            anchors.fill : zoomAreaRect
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton

            property real mx: 0
            property real my: 0
            property bool isResizing: false
            property bool enable_x0: false
            property bool enable_x1: false
            property bool enable_y0: false
            property bool enable_y1: false

            onWheel: {
                if(wheel.angleDelta.y === 0)
                    return;
                var isRegularWheel = (wheel.pixelDelta.y === 0 || wheel.angleDelta.y === 120 || wheel.angleDelta.y === -120);
                var deltaY = (wheel.angleDelta.y / 120) * -0.1;
                var scaleDelta = (1.0 + deltaY);

                //wheel.accepted = false;
                var tx = zoomAreaRect.x
                var ty = zoomAreaRect.y
                var tw = zoomAreaRect.width
                var th = zoomAreaRect.height
                tw = zoomAreaRect.width * scaleDelta;
                th = zoomAreaRect.height * scaleDelta;
                tx = zoomAreaRect.x +((zoomAreaRect.width-tw) / 2);
                ty = zoomAreaRect.y +((zoomAreaRect.height-th) / 2);

                if (tw < 16 || th < 16) {
                    return;
                }
                if (tx <= 0 || (tx+tw) > contentRectW ||
                    ty <= 0 || (ty+th) > contentRectH)  {
                    return;
                }
                zoomAreaRect.updatePosition(tx, ty, tw, th);
            }

            onExited: {
                if (!pressed) {
                    reset();
                }
            }

            onPressed: {
                mx = mouseX
                my = mouseY
                isResizing = checkMouseResizable(mouseX, mouseY);
            }

            onReleased: {
                reset();
            }

            onPositionChanged: {
                var dx = mouseX - mx
                var dy = mouseY - my

                if (!pressed) {
                    checkMouseResizable(mouseX, mouseY);
                    return;
                }

                var tx = zoomAreaRect.x
                var ty = zoomAreaRect.y
                var tw = zoomAreaRect.width
                var th = zoomAreaRect.height

                // resize
                if (isResizing) {
                    if (enable_x0) {
                        tx += dx
                        tw -= dx;
                    }
                    if (enable_y0) {
                        ty += dy;
                        th -= dy;
                    }
                    if (enable_x1) {
                        tw += dx;
                    }
                    if (enable_y1) {
                        th += dy;
                    }
                    if (tw < 16 || th < 16) {
                        mx = mouseX; my = mouseY;
                        return;
                    }

                    if (tx <= 0 || (tx+tw) > contentRectW ||
                        ty <= 0 || (ty+th) > contentRectH)  {
                        mx = mouseX; my = mouseY;
                        return;
                    }

                    zoomAreaRect.updatePosition(tx, ty, tw, th);
                    mx = mouseX; my = mouseY;
                    return;
                }

                // move position
                tx += dx
                ty += dy
                if (tx <= 0 || (tx+tw) > contentRectW ||
                    ty <= 0 || (ty+th) > contentRectH)  {
                    mx = mouseX; my = mouseY;
                    return;
                }
                zoomAreaRect.updatePosition(tx, ty, tw, th);
                mx = mouseX; my = mouseY;
            }

            function reset()
            {
                mx = 0;
                my = 0;
                enable_x0 = false;
                enable_x1 = false;
                enable_y0 = false;
                enable_y1 = false;
                isResizing = false;
                if (cursorShape !== Qt.ArrowCursor)
                    cursorShape = Qt.ArrowCursor;
            }

            /////////////////////////////////////////////////////////////////////////////
            // 마우스가 아이템 크기 조정 영역에 있는지 확인
            /////////////////////////////////////////////////////////////////////////////
            function checkMouseResizable(msx, msy)
            {
                var offset = width >= 640 ? 8 : (width <= 100 ? 4 : 6);

                // 안쪽 사각 체크
                if (msx <= (offset) || msx >= (width-offset) ||
                    msy <= (offset) || msy >= (height-offset)) {

                    // 3등분한 9구역에 대해서 계산
                    var base = width >= height ? height : width;
                    var div = base / 6; // 모서리 기준 값

                    var x_1 = div;
                    var x_2 = width - div;
                    var y_1 = div;
                    var y_2 = height - div;

                    // left top
                    if (msx <= x_1 && msy <= y_1) {
                        enable_x0 = true;
                        enable_y0 = true;
                        cursorShape = Qt.SizeFDiagCursor;
                        return true;
                    }
                    // right bottom
                    if (msx >= x_2 && msy >= y_2) {
                        enable_x1 = true;
                        enable_y1 = true;
                        cursorShape = Qt.SizeFDiagCursor;
                        return true;
                    }

                    // right top
                    if (msx >= x_2 && msy <= y_1) {
                        enable_x1 = true;
                        enable_y0 = true;
                        cursorShape = Qt.SizeBDiagCursor;
                        return true;
                    }

                    // left bottom
                    if (msx <= x_1 && msy >= y_2) {
                        enable_x0 = true;
                        enable_y1 = true;
                        cursorShape = Qt.SizeBDiagCursor;
                        return true;
                    }

                    // top
                    if (msy <= y_1) {
                        enable_y0 = true;
                        cursorShape = Qt.SizeVerCursor;
                        return true;
                    }

                    // bottom
                    if (msy >= y_2) {
                        enable_y1 = true;
                        cursorShape = Qt.SizeVerCursor;
                        return true;
                    }

                    // left
                    if (msx <= x_1) {
                        enable_x0 = true;
                        cursorShape = Qt.SizeHorCursor;
                        return true;
                    }

                    // right
                    enable_x1 = true;
                    cursorShape = Qt.SizeHorCursor;
                    return true;
                }

                if (cursorShape !== Qt.ArrowCursor)
                    cursorShape = Qt.ArrowCursor;
                return false;
            }
        }
    }


    Component.onDestruction: {
        console.log("VideoItemZoomArea onDestruction!!");
    }
}
