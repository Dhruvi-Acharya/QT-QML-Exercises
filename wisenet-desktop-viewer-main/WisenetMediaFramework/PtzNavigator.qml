import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Item {
    id: ptzNavigator
    anchors.fill: parent
    property real baseSize: parent.width > parent.height ? parent.height : parent.width
    property real arrowCenterLength: baseSize * 0.17
    property real arrowScaleW: baseSize * 0.09
    property real arrowRatio: 1.917
    property real calcArrowWidth: arrowScaleW
    property real calcArrowHeight: arrowScaleW * arrowRatio

    property int ptzMode : PtzNavigator.PtzMode.Ready
    property bool sizeOk : calcArrowWidth >= 8

    property real zoomSpeedOffset: 10 //-100~100

    // point move 또는 area zoom 지원
    property bool supportPointMove: false
    property bool supportAreaZoom: false
    // 1x zoom 지원
    property bool support1xZoom: false
    // pan만 지원 (fisheye perimeter viewMode)
    property bool supportOnlyPan: false


    property alias zoomValue: zoomSlider.value

    enum PtzMode {
        Ready,
        ArrowButtonMode, // 8 방향 화살표 표시
        ArrowLineMode    // 가운데 중심 방향선 표시
    }
    enum PtzDirection {
        Up,
        Right,
        Down,
        Left,
        UpRight,
        DownRight,
        UpLeft,
        DownLeft
    }

    signal arrowPtzTriggered(real dx, real dy);
    signal directionPtzTriggered(int direction);
    signal stopPtzTriggered();
    signal zoomTriggered(real dz)
    signal zoom1xTriggered();
    signal pointMovePtzTriggered(real px, real py);
    signal areaMovePtzTriggered(real px, real py, real pw, real ph);
    signal absZoomTriggered(real zoomValue);

    component ArrowButtonItem: WisenetMediaButtonItem {
        visible: (support && ptzNavigator.ptzMode === PtzNavigator.PtzMode.ArrowButtonMode) ? ptzNavigator.sizeOk : false
        width:ptzNavigator.calcArrowWidth
        height:ptzNavigator.calcArrowHeight
        imgWidth: ptzNavigator.calcArrowWidth
        imgHeight: ptzNavigator.calcArrowHeight
        normalSource: "images/ptz_left_arrow_Default.svg"
        hoverSource: "images/ptz_left_arrow_Hover.svg"
        pressSource: "images/ptz_left_arrow_Press.svg"
        imgSourceSize: "36x69"
        normalColor: "transparent"
        hoverColor: "transparent"
        pressColor: "transparent"
        property bool support: true
        property int ptzDirection:0

        property real px: width*0.5
        property real py: height*0.5
        property real radian: (Math.PI/180)*rotation
        x: centerCross.centerX - px - ptzNavigator.arrowCenterLength*Math.cos(radian)
        y: centerCross.centerY - py - ptzNavigator.arrowCenterLength*Math.sin(radian)

        onMousePressed: controller.directionPtz(ptzDirection);
        onMouseReleased: controller.stopPtz();
    }


    Timer {
        id: controller
        repeat: false
        running: false

        property real newDx: 0
        property real newDy: 0
        property real newDz: 0
        property bool isUpdatePt: false
        property bool isUpdateZ: false
        property bool isUpdateStopZ: false
        property int ptInterval: 150
        property int zoomInterval: 150
        property int zoomStopInterval: 500


        function reset()
        {
            if(running)
                stop();
            newDx = 0;
            newDy = 0;
            newDz = 0;
            isUpdatePt = false;
            isUpdateZ = false;
            isUpdateStopZ = false;
        }

        function stopPtz()
        {
            reset();
            ptzNavigator.stopPtzTriggered();
        }

        function directionPtz(direction)
        {
            reset();
            ptzNavigator.directionPtzTriggered(direction);
        }

        function zoomPtz(wheelY)
        {
            if (wheelY !== 0) {
                isUpdateZ = true;
                isUpdateStopZ = false;

                var delta = (wheelY / 120) * ptzNavigator.zoomSpeedOffset;
                //console.log("zoomPtz", delta);
                newDz += delta;

                if (newDz > 100)
                    newDz = 100;
                if (newDz < -100)
                    newDz = -100;
                if (!running) {
                    startTimer(zoomInterval);
                }
            }
        }

        function startTimer(timeout)
        {
            interval = timeout;
            start();
        }

        function arrowPtz(dx, dy)
        {
            isUpdatePt = true
            newDx = dx;
            newDy = dy;

            if (!running) {
                ptzNavigator.arrowPtzTriggered(newDx, newDy);
                isUpdatePt = false;
                startTimer(ptInterval);
            }
        }

        onTriggered: {
            //console.log("ptzInterval timer triggered, isUpdate=", isUpdate, Date.now(), newDx, newDy)
            if (isUpdatePt) {
                ptzNavigator.arrowPtzTriggered(newDx, newDy);
                isUpdatePt = false;
                startTimer(ptInterval)
            }
            else if (isUpdateZ) {
                ptzNavigator.zoomTriggered(newDz);
                isUpdateZ = false;
                isUpdateStopZ = true;
                startTimer(zoomStopInterval);
            }
            else if (isUpdateStopZ) {
                stopPtz();
            }
        }
    }

    // 전역 마우스 영역
    // point move 또는 area zoom 동작은 여기에서
    MouseArea {
        id: globalArea
        anchors.fill: parent
        visible: ptzNavigator.ptzMode !== PtzNavigator.PtzMode.ArrowLineMode ? ptzNavigator.sizeOk : false
        onVisibleChanged: {
            //console.log("globalArea visible", visible, ptzNavigator.ptzMode)
        }

        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        property real startX:0
        property real startY:0

        onWheel: {
            controller.zoomPtz(wheel.angleDelta.y);
        }

        onEntered: {
            ptzNavigator.ptzMode = PtzNavigator.PtzMode.Ready;
        }

        onPressed: {
            if (supportAreaZoom) {
                startX = mouseX
                startY = mouseY
            }
        }
        onPositionChanged: {
            if (supportAreaZoom && pressed) {
                var rectWidth = (Math.abs (mouseX - startX));
                var rectHeight = (Math.abs (mouseY - startY));
                var rectX = (mouse.x < startX) ? mouseX : startX;
                var rectY = (mouse.y < startY) ? mouseY : startY;
                areaSelectRect.showAreaRect(rectX, rectY, rectWidth, rectHeight);
            }
        }
        onReleased: {
            if (supportAreaZoom || supportPointMove){
                startX = 0;
                startY = 0;

                // areazoom action
                if (areaSelectRect.width > 8 && areaSelectRect.height > 8) {
                    areaSelectRect.hideAreaRect();
                    ptzNavigator.areaMovePtzTriggered(areaSelectRect.x, areaSelectRect.y,
                                                      areaSelectRect.width, areaSelectRect.height);
                }
                // pointmove action
                else if (!pointShowAnimation.running) {
                    areaSelectRect.showPoint(mouseX, mouseY);
                    ptzNavigator.pointMovePtzTriggered(mouseX, mouseY);
                }
            }
        }

    }

    // 가운데 십자버튼과 화살버튼 영역을 포함한 마우스 영역
    MouseArea {
        id: centerMouseArea
        anchors.centerIn: parent
        property real calcSize : ((ptzNavigator.calcArrowWidth*0.5) + ptzNavigator.arrowCenterLength) * 2
        width: calcSize
        height: calcSize
        hoverEnabled: true
        visible: ptzNavigator.ptzMode === PtzNavigator.PtzMode.ArrowButtonMode ? ptzNavigator.sizeOk : false

        onWheel: {
            controller.zoomPtz(wheel.angleDelta.y);
        }
    }

    // 가운데 십자버튼
    Item {
        id: centerCross
        anchors.centerIn: parent
        property real crossSize: parent.baseSize * 0.1
        width:crossSize
        height:crossSize
        visible: ptzNavigator.sizeOk
        property real centerX: x + (width/2)
        property real centerY: y + (height/2)

        property alias bgOpacity: centerBg.opacity
        Rectangle {
            id: centerBg
            anchors.fill: parent
            color: WisenetGui.contrast_08_dark_grey
            radius: width*0.5
            opacity: 0.0
            visible: !movingCross.running
        }

        Image {
            anchors.fill: parent
            sourceSize: "40x40"
            source: "images/ptz_cross.svg"
            visible: !movingCross.running
        }
    }
    // 움직이는 십자버튼
    Item {
        id: movingCross
        width:centerCross.width
        height:width
        visible: running
        property bool running: false
        property real centerOffset: width*0.5

        Rectangle {
            anchors.fill: parent
            color: WisenetGui.contrast_08_dark_grey
            radius: width*0.5
            opacity: centerBg.opacity
        }
        Image {
            anchors.fill: parent
            sourceSize: "40x40"
            source: "images/ptz_cross.svg"
        }
        function start()
        {
            running = true;
            x = centerCross.x
            y = centerCross.y
            arrowLineCanvas.visible = true
        }
        function update()
        {
            if (running) {
                x = centerCrossMouseArea.mouseX-(centerOffset)
                y = centerCrossMouseArea.mouseY-(centerOffset)
                arrowLineCanvas.requestPaint();
            }
        }
        function stop()
        {
            arrowLineCanvas.visible = false;
            stopAnim.restart()
        }

        ParallelAnimation {
            id: stopAnim
            NumberAnimation {
                target: movingCross
                properties: "x"
                to: centerCross.x
                duration: 150
            }
            NumberAnimation {
                target: movingCross
                properties: "y"
                to: centerCross.y
                duration: 150
            }
            onStopped: {
                movingCross.running = false;
            }
        }
    }

    // 가운데 십자버튼 마우스 영역 (mouse 다운시 확장)
    MouseArea {
        id: centerCrossMouseArea
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        anchors.fill: ptzNavigator.ptzMode === PtzNavigator.PtzMode.ArrowLineMode ? parent : centerCross
        visible: ptzNavigator.sizeOk
        property real margin: movingCross.centerOffset

        onPositionChanged: {
            if (ptzNavigator.ptzMode === PtzNavigator.PtzMode.ArrowLineMode && pressed) {
                //check margins
                if (mouseX < margin || mouseX > (width-margin) ||
                    mouseY < margin || mouseY > (height-margin)) {
                    return;
                }

                if (!movingCross.running) {
                    movingCross.start();
                }

                var dx = mouseX - centerCross.centerX;
                var dy = mouseY - centerCross.centerY;
                movingCross.update();
                controller.arrowPtz(dx, dy);
            }
        }

        onEntered: {
            if (ptzNavigator.ptzMode !== PtzNavigator.PtzMode.ArrowLineMode || !pressed) {
                centerCross.bgOpacity = 0.3
                ptzNavigator.ptzMode = PtzNavigator.PtzMode.ArrowButtonMode
            }
        }

        onExited: {
            if (ptzNavigator.ptzMode !== PtzNavigator.PtzMode.ArrowLineMode || !pressed) {
                centerCross.bgOpacity = 0.0
            }
        }

        onPressed: {
            centerCross.bgOpacity = 0.6
            ptzNavigator.ptzMode = PtzNavigator.PtzMode.ArrowLineMode
            console.log("centerCrossMouseArea onPressed--")
        }

        onReleased: {
            centerCross.bgOpacity = containsMouse ? 0.3 : 0.0
            ptzNavigator.ptzMode = PtzNavigator.PtzMode.Ready
            movingCross.stop();
            controller.stopPtz();
            console.log("released", pressed);
        }
        onWheel: {
            controller.zoomPtz(wheel.angleDelta.y);
        }

    }


    // 8방향 화살표 버튼
    ArrowButtonItem{
        id: leftArrow
        rotation: 0
        ptzDirection: PtzNavigator.PtzDirection.Left
    }
    ArrowButtonItem{
        id: leftUpArrow
        rotation: 45
        ptzDirection: PtzNavigator.PtzDirection.UpLeft
        support: !supportOnlyPan
    }
    ArrowButtonItem{
        id: upArrow
        rotation: 90
        ptzDirection: PtzNavigator.PtzDirection.Up
        support: !supportOnlyPan
    }
    ArrowButtonItem{
        id: rightUpArrow
        rotation: 135
        ptzDirection: PtzNavigator.PtzDirection.UpRight
        support: !supportOnlyPan
    }
    ArrowButtonItem{
        id: rightArrow
        rotation: 180
        ptzDirection: PtzNavigator.PtzDirection.Right
    }
    ArrowButtonItem{
        id: rightDownArrow
        rotation: 225
        ptzDirection: PtzNavigator.PtzDirection.DownRight
        support: !supportOnlyPan
    }
    ArrowButtonItem{
        id: downArrow
        rotation: 270
        ptzDirection: PtzNavigator.PtzDirection.Down
        support: !supportOnlyPan
    }
    ArrowButtonItem{
        id: leftDownArrow
        rotation: 315
        ptzDirection: PtzNavigator.PtzDirection.DownLeft
        support: !supportOnlyPan
    }

    // https://stackoverflow.com/questions/65803655/paint-arrow-with-qml

    Canvas {
        id: arrowLineCanvas
        anchors.fill: parent
        visible: false
        property real headAngle: 6
        property bool running: false
        property real cx: centerCross.centerX
        property real cy: centerCross.centerY
        property real tx: 0
        property real ty: 0

        function start()
        {
            visible = true;
            running = true;
        }
        function stop()
        {
            visible = false;
            running = false;
        }

        // Code to draw a simple arrow on TypeScript canvas got from https://stackoverflow.com/a/64756256/867349
        function updateArrow(context) {
            const dx = tx - cx;
            const dy = ty - cy;
            const distance = Math.sqrt(dx * dx + dy * dy);
            if (distance < 12)
                return;
            const headlen = 10;  //Math.sqrt(dx * dx + dy * dy) * 0.3; // length of head in pixels
            const angle = Math.atan2(dy, dx);
            const downAngle = (angle - Math.PI / headAngle);
            const upAngle = (angle + Math.PI / headAngle);
            context.beginPath();
            context.moveTo(cx, cy);
            context.lineTo(tx, ty);
            context.stroke();
            context.beginPath();
            context.moveTo(tx - headlen * Math.cos(downAngle), ty - headlen * Math.sin(downAngle));
            context.lineTo(tx, ty );
            context.lineTo(tx - headlen * Math.cos(upAngle), ty - headlen * Math.sin(upAngle));
            context.stroke();
            const degrees = angle*57.296;//(180/Math.pi); // radian to degree
        }

        onPaint: {
            // Get the canvas context
            var ctx = getContext("2d");
            // Fill a solid color rectangle
            ctx.strokeStyle = WisenetGui.color_primary
            ctx.clearRect(0, 0, width, height);

            // Draw an arrow on given context starting at position (0, 0) -- top left corner up to position (mouseX, mouseY)
            //   determined by mouse coordinates position
            //console.log("onPaint, ", centerCross.centerX, centerCross.centerY, centerCrossMouseArea.mouseX, centerCrossMouseArea.mouseY)
            tx = centerCrossMouseArea.mouseX;
            ty = centerCrossMouseArea.mouseY;
            updateArrow(ctx)
        }
    }

    component ZoomBarButtonItem: WisenetMediaButtonItem {
        width: zoomBar.btnSize
        height: width
        imgWidth:width
        imgHeight:height
        imgSourceSize: "28x28"
        radius: width*0.5
        anchors.horizontalCenter: parent.horizontalCenter
        normalColor: WisenetGui.contrast_08_dark_grey
        normalOpacity: 0.3
        hoverOpacity: 0.6
        pressOpacity: 0.8
    }

    Column {
        id: zoomBar
        visible: !ptzNavigator.supportOnlyPan
        width: centerCross.width

        spacing: width*0.1
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: zoomBar.spacing
        }
        property real btnSize : width*0.5

        ZoomBarButtonItem {
            normalSource: "images/zoomIn_Default.svg"
            hoverSource: "images/zoomIn_Hover.svg"
            pressSource: "images/zoomIn_Press.svg"
            onMousePressed: {
                ptzNavigator.zoomTriggered(15);
            }
            onMouseReleased: {
                ptzNavigator.stopPtzTriggered();
            }
        }

        Slider {
            id: zoomSlider
            visible: ptzNavigator.support1xZoom
            from: 180
            to:20
            value: 0.0
            width: zoomBar.width
            height: ptzNavigator.height*0.5
            orientation: Qt.Vertical
            anchors.horizontalCenter: parent.horizontalCenter
            live:false
            handle: Rectangle {
                x: zoomSlider.leftPadding + (zoomSlider.availableWidth - width) / 2
                y: zoomSlider.topPadding + zoomSlider.visualPosition * (zoomSlider.availableHeight - height)
                implicitWidth: zoomBar.width*0.3
                implicitHeight: implicitWidth
                radius: width*0.5
                color: {
                    if(zoomSlider.pressed)
                        return WisenetGui.contrast_06_grey
                    else if(zoomSlider.hovered)
                        return WisenetGui.contrast_00_white
                    else
                        return WisenetGui.contrast_02_light_grey
                }
            }
            background: Rectangle {
                x: zoomSlider.leftPadding + (zoomSlider.availableWidth - width) / 2
                y: zoomSlider.topPadding
                implicitWidth: 1
                //implicitHeight: 200
                width: implicitWidth
                height: zoomSlider.availableHeight
                color: WisenetGui.contrast_05_grey

                Rectangle {
                    y: zoomSlider.visualPosition * parent.height
                    width: parent.width
                    height: zoomSlider.position * parent.height
                    color: WisenetGui.color_primary
                }
            }
            onPositionChanged: {
                if (pressed) {
                    var valueAtPositon = valueAt(position);
                    ptzNavigator.absZoomTriggered(valueAtPositon);
                }
            }
        }

        ZoomBarButtonItem {
            normalSource: "images/zoomOut_Default.svg"
            hoverSource: "images/zoomOut_Hover.svg"
            pressSource: "images/zoomOut_Press.svg"
            onMousePressed: {
                ptzNavigator.zoomTriggered(-15);
            }
            onMouseReleased: {
                ptzNavigator.stopPtzTriggered();
            }
        }

        Item {
            visible: ptzNavigator.support1xZoom
            width: zoomBar.spacing
            height: width
        }

        ZoomBarButtonItem {
            visible: ptzNavigator.support1xZoom
            normalSource: "images/zoom1x_Default.svg"
            hoverSource: "images/zoom1x_Hover.svg"
            pressSource: "images/zoom1x_Press.svg"
            onMouseClicked: {
                ptzNavigator.zoom1xTriggered();
            }
            normalColor: "transparent"
            pressColor: "transparent"
            hoverColor: "transparent"
        }
    }

    Rectangle {
        id: areaSelectRect
        visible: false
        color: "transparent"
        border.width : 1
        border.color: WisenetGui.color_primary
        radius: isAreaRect? 0 : width*0.5

        property real centerX: 0
        property real centerY: 0
        property real animDuraiton: 250
        property real targetSize: parent.baseSize*0.06
        property bool isAreaRect: false


        function showAreaRect(startX, startY, rWidth, rHeight) {
            visible = true;
            opacity = 1.0;
            x = startX;
            y = startY;
            width = rWidth;
            height = rHeight;
            isAreaRect = true;
        }
        function hideAreaRect()
        {
            if (visible && width > 0 && !areaHideAnimation.running) {
                areaHideAnimation.start();
            }
        }

        function cleanUp() {
            isAreaRect = false;
            visible = false;
            x = 0; y = 0; width = 0; height = 0;
        }

        function showPoint(startX, startY) {
            isAreaRect = false;
            pointShowAnimation.stop()
            centerX = startX;
            centerY = startY;
            visible = true;
            pointShowAnimation.restart();
        }

        NumberAnimation {
            id: areaHideAnimation
            target: areaSelectRect
            duration: areaSelectRect.animDuraiton
            property: "opacity"
            from: 1.0
            to: 0.0
            onStopped: {
                areaSelectRect.cleanUp();
            }
        }

        ParallelAnimation {
            id: pointShowAnimation
            NumberAnimation {
                target: areaSelectRect
                property: "x"
                from: areaSelectRect.centerX
                to: areaSelectRect.centerX-(areaSelectRect.targetSize/2);
                duration: areaSelectRect.animDuraiton
            }
            NumberAnimation {
                target: areaSelectRect
                property: "y"
                from: areaSelectRect.centerY
                to: areaSelectRect.centerY-(areaSelectRect.targetSize/2);
                duration: areaSelectRect.animDuraiton
            }
            NumberAnimation {
                target: areaSelectRect
                property: "width"
                from: 0
                to: areaSelectRect.targetSize
                duration: areaSelectRect.animDuraiton
            }
            NumberAnimation {
                target: areaSelectRect
                property: "height"
                from: 0
                to: areaSelectRect.targetSize
                duration: areaSelectRect.animDuraiton
            }
            NumberAnimation {
                target: areaSelectRect
                property: "opacity"
                from: 1.0
                to: 0.0
                duration: areaSelectRect.animDuraiton
            }
            onStopped: {
                areaSelectRect.cleanUp();
            }
        }
    }
}
