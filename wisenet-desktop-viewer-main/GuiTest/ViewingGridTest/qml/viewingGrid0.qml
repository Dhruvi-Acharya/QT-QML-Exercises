import QtQuick 2.15
import QtQuick.Controls 2.15
import "viewingGrid0.js" as GridSystem


Rectangle {
    id: gridMainView
    width: 1280; height: 800
    color: '#101010'
    property real fitAnimDuration: 400
    property real zoomAnimDuration: 500
    property real gridVisibleAnimDuration: 1000

    // gridView property binding
    property real gridPixelX: gridView.gridPositionX
    property real gridPixelY: gridView.gridPositionY
    property real gridPixelW: gridView.gridPixelWidth
    property real gridPixelH: gridView.gridPixelHeight

    Rectangle {
        id: itemViewRegion
        anchors {fill: parent ; leftMargin: 100 ; rightMargin: 100 ; topMargin: 30 ; bottomMargin: 30}
        color: "transparent"
        border.color: "green"
    }

    // 백그라운드 그리드 뷰
    ShaderEffect{
        id: gridView
        anchors.fill: parent

        property real   gridPositionX : 0  // 그리드의 0,0 기준 X좌표
        property real   gridPositionY : 0  // 그리드의 0,0 기준 Y좌표
        property real   gridZoomScale : 1.0 // 그리드의 줌 스케일

        property real   gridCellSizeW : 160
        property real   gridCellSizeH : 90
        property real   gridPixelWidth : gridCellSizeW*gridZoomScale
        property real   gridPixelHeight : gridCellSizeH*gridZoomScale

        property real   gridZoomAnimationX: 0               // 줌 타겟 X
        property real   gridZoomAnimationY: 0               // 줌 타겟 Y
        property real   gridZoomAnimationScale: 0           // 줌 타겟 스케일
        property real   gridAlpha: 1.0

        state: "hide"
        states: [
            State {
                name: "hide"
                PropertyChanges {
                    target: gridView
                    //visible: false
                    gridAlpha: 0.0
                }
            },
            State {
                name: "show"
                PropertyChanges {
                    target: gridView
                    //visible: true
                    gridAlpha: 1.0
                }
            }
        ]
        transitions: [
            Transition {
                NumberAnimation{target: gridView; properties:"gridAlpha"; duration:gridMainView.gridVisibleAnimDuration}
            }
        ]

        fragmentShader: "
uniform highp float gridPixelWidth;
uniform highp float gridPixelHeight;

uniform highp float gridPositionX;
uniform highp float gridPositionY;

varying highp vec2 qt_TexCoord0;
uniform highp float width;
uniform highp float height;
uniform highp float gridAlpha;

void main()
{
    if (gridAlpha == 0.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }
    highp vec2 xy = vec2(qt_TexCoord0.x*width, qt_TexCoord0.y*height);
    highp vec2 offset = xy - vec2(gridPositionX, gridPositionY);

    if (int(mod(offset.x, gridPixelWidth)) == 0 ||
        int(mod(offset.y, gridPixelHeight)) == 0)
    {
        gl_FragColor = vec4(0.419, 0.207, 0.086, 1.0)*gridAlpha;
    }
    else
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
"
        Component.onCompleted : {
            init();
        }

        // 초기화
        function init()
        {
            GridSystem.init(gridView);
            var viewRatio = itemViewRegion.width / itemViewRegion.height;
            var cellRatio = gridCellSizeW / gridCellSizeH;
            var calculatedCellWidth = 0;
            var calculatedCellHeight = 0;
            var dx = 0, dy = 0;

            // 하나의 그리드 셀이 화면 가운데에 위치하도록 조정한다.
            if (viewRatio <= cellRatio) {
                calculatedCellWidth = itemViewRegion.width;
                calculatedCellHeight = itemViewRegion.width / gridCellSizeW * gridCellSizeH;
                dy = (itemViewRegion.height - calculatedCellHeight) / 2;
            }
            else {
                calculatedCellWidth = itemViewRegion.height / gridCellSizeH * gridCellSizeW;
                calculatedCellHeight = itemViewRegion.height;
                dx = (itemViewRegion.width - calculatedCellWidth) / 2;
            }

            var scale = calculatedCellWidth / gridCellSizeW;
            gridZoomScale = scale;
            gridPositionX = itemViewRegion.x + dx;
            gridPositionY = itemViewRegion.y + dy;
        }

        // Fit in View
        function fitInView()
        {
            scaleAnimation.stop();
            GridSystem.calculateFitInView(itemViewRegion.x, itemViewRegion.y,
                                          itemViewRegion.width, itemViewRegion.height);
            scaleAnimation.easingType = Easing.OutQuad;
            scaleAnimation.duration = gridMainView.fitAnimDuration;
            scaleAnimation.restart();
        }

        // 줌 인/아웃
        function zoomAnimation(originX, originY, scaleDelta)
        {
            var zoomScale = gridZoomScale * scaleDelta;
            var cellPixelWidth = zoomScale*gridCellSizeW;

            // 셀의 폭이 min/max를 초과한 경우에는 예외처리
            var overflowed = false;
            if (cellPixelWidth  < 16 || cellPixelWidth > 40960) {
                overflowed = true;
            }

            /* 애니메이션 적용시 (TEST) */
            gridZoomAnimationScale = zoomScale;
            gridZoomAnimationX = gridPositionX  + (originX * (1 - scaleDelta));
            gridZoomAnimationY = gridPositionY  + (originY * (1 - scaleDelta));
            scaleAnimation.easingType = overflowed ? Easing.SineCurve : Easing.OutQuad;
            scaleAnimation.duration = overflowed ? 500 : gridMainView.zoomAnimDuration;
            scaleAnimation.restart();

            /* 애니메이션 적용 안하고 바로 값 변경 시 공식*/
            /*
            if (!overflowed) {
                gridZoomScale *= scaleDelta;
                gridPositionX += (originX * (1 - scaleDelta));
                gridPositionY += (originY * (1 - scaleDelta));
            }
            */
        }

        // Pan/Tilt
        function panTilt(deltaX, deltaY)
        {
            scaleAnimation.stop();

            gridPositionX -= deltaX;
            gridPositionY -= deltaY;
        }

        // 아이템 추가 (TEST)
        function addNewIem()
        {
            GridSystem.addNewItem(gridMainView);
        }

        // 아이템 전체 삭제 (TEST)
        function clear()
        {
            GridSystem.cleanUp();
        }

    }

    /// mouse event control
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        cursorShape: Qt.ArrowCursor
        focus:true          // 키보드 입력 이벤트를 위해서 true
        hoverEnabled: true  // 마우스 오버 이벤트 처리를 위해서 true

        property bool mousePanning : false
        property real mousePanningX : 0
        property real mousePanningY : 0
        property bool shiftKeyPressed : false
        property bool controlKeyPressed : false
        property real zoomSpeedOffset: 0.25 // 30%

        property bool itemMovingReady: false
        property bool itemResizingEnable: false
        property bool itemResizingReady: false
        property bool itemResizingXl: false
        property bool itemResizingXr: false
        property bool itemResizingYt: false
        property bool itemResizingYb: false
        // 마우스 좌클릭+드래그시 그려주는 선택 사각형
        Rectangle {
            id: mouseSelectRect
            visible: false
            width: 0;height: 0; x: 0; y: 0; z: 10;
            color: "#80FF6C00"
            border {color: "#FF6C00"; width: 1}
            property real mouseX: 0
            property real mouseY: 0

            function start(mx, my)
            {
                visible = true
                mouseX = mx
                mouseY = my
                x = mx
                y = my
                width = 0
                height = 0
            }
            function update(mx, my)
            {
                if (visible) {
                    width = (Math.abs (mx - mouseX));
                    height = (Math.abs (my - mouseY));
                    x = (mx < mouseX) ? mx : mouseX;
                    y = (my < mouseY) ? my : mouseY;
                }
            }
            function stop()
            {
                if (visible)
                    visible = false;
            }
        }

        // 아이템 이동시 타겟 사각형 영역 (TEST)
        Rectangle {
            id: itemTargetRect
            visible: false
            color: "#803A911E"
            property int gx: 0
            property int gy: 0
            property int gw: 1
            property int gh: 1
            width: gridView.gridPixelWidth*gw
            height: gridView.gridPixelHeight*gh
            x: (gx*gridView.gridPixelWidth)+gridView.gridPositionX
            y: (gy*gridView.gridPixelHeight)+gridView.gridPositionY
        }

        // 마우스 팬틸트 시작
        function startPanTilt(mx, my)
        {
            mousePanning  = true
            mousePanningX = mx
            mousePanningY = my
        }

        // 마우스 팬틸트 업데이트 ==> 실제 그리드뷰의 좌표 변환 및 커서 변경
        function updatePanTilt(mx, my)
        {
            if (mousePanning) {
                if (cursorShape != Qt.ClosedHandCursor)
                    cursorShape = Qt.ClosedHandCursor;

                var dx = (mousePanningX - mx);
                var dy = (mousePanningY - my);
                gridView.panTilt(dx, dy);
                mousePanningX = mx
                mousePanningY = my
            }
        }
        // 마우스 팬틸트 종료 및 커서 복귀
        function stopPanTilt()
        {
            if (mousePanning)
                mousePanning = false;
            if (cursorShape != Qt.ArrowCursor)
                cursorShape = Qt.ArrowCursor;
        }

        onPressed: {
            // 마우스 좌다운 ==> 선택영역 표시 준비
            if (mouse.button == Qt.LeftButton) {
                console.log("left press");

                GridSystem.clearSelectedItems();

                if (itemResizingEnable) {
                    itemResizingReady = true;
                    GridSystem.readyToResizeItem(mouse.x, mouse.y, itemTargetRect);
                    return;
                }

                if (GridSystem.checkFocusItem(mouse.x, mouse.y)) {
                    itemMovingReady = true;
                    GridSystem.readyToMoveFocusedItem(mouse.x, mouse.y, itemTargetRect)
                    console.log("object moving ready")
                }
                else {
                    mouseSelectRect.start(mouse.x, mouse.y);
                    itemMovingReady = false;
                }
            }
            // 마우스 우다운 ==> 그리드 팬/틸트 준비
            else if (mouse.button == Qt.RightButton) {
                console.log("right press");
                startPanTilt(mouse.x, mouse.y);
            }
        }

        onPressAndHold: {
            // 마우스 좌다운 길게 ==> 선택아이템이 있는 경우 이동 준비
            if (mouse.button == Qt.LeftButton) {
                console.log("left press and hold");
            }
        }

        onPositionChanged: {
            // 마우스 좌다운 + 드래그(1) ==> 아이템 이동
            if (itemMovingReady == true) {
                gridView.state="show";
                GridSystem.startToMoveFocusedItem(mouse.x, mouse.y, itemTargetRect);
                return;
            }
            if (itemResizingReady == true) {
                gridView.state="show";
                GridSystem.startToResizeItem(mouse.x, mouse.y, itemTargetRect,
                                             itemResizingXl, itemResizingXr, itemResizingYt, itemResizingYb);
                return;
            }

            gridView.state="hide";
            // 마우스 좌다운 + 드래그(2) ==> 선택영역 표시
            if (mouseSelectRect.visible) {
                mouseSelectRect.update(mouse.x, mouse.y);
                GridSystem.updateSelectedItems(mouseSelectRect);
                return;
            }
            // 마우스 우다운 + 드래그 ==> 그리드 팬/틸트
            if (mousePanning) {
                updatePanTilt(mouse.x, mouse.y);
                return;
            }

            // 마우스 오버시 크기 조절
            if (mouse.buttons == Qt.NoButton) {
                var limit = 4;
                var item = GridSystem.getResizingTargetItem(mouse.x, mouse.y, limit);
                if (item == null) {
                    itemResizingEnable = false;
                    itemResizingReady = false;
                    if (cursorShape != Qt.ArrowCursor)
                        cursorShape = Qt.ArrowCursor;
                    return;
                }

                itemResizingEnable = true;
                itemResizingXl = (mouse.x >= item.x-limit && mouse.x <= item.x+limit);
                itemResizingXr = (mouse.x >= (item.x+item.width-limit) && mouse.x <= item.x+item.width+limit);
                itemResizingYt = (mouse.y >= item.y-limit && mouse.y <= item.y+limit);
                itemResizingYb = (mouse.y >= (item.y+item.height-limit) && mouse.y <= item.y+item.height+limit);

                //console.log("xy=", itemResizingXl, itemResizingXr, itemResizingYt, itemResizingYb);
                if (itemResizingXl || itemResizingXr || itemResizingYt || itemResizingYb) {

                    if (itemResizingXl && itemResizingYt || itemResizingXr && itemResizingYb)
                        cursorShape = Qt.SizeFDiagCursor;
                    else if (itemResizingXl && itemResizingYb || itemResizingXr && itemResizingYt)
                        cursorShape = Qt.SizeBDiagCursor;
                    else if (itemResizingXr || itemResizingXl)
                        cursorShape = Qt.SizeHorCursor;
                    else if (itemResizingYt || itemResizingYb)
                        cursorShape = Qt.SizeVerCursor;

                }
            }
        }
        onReleased: {
            // 마우스 좌다운 릴리즈 ==> 객체 이동 최종 처리 및 비활성화
            gridView.state="hide";
            if (itemMovingReady) {
                itemMovingReady = false;
                if (GridSystem.stopToMoveFocusedItem(mouse.x, mouse.y, itemTargetRect))
                    gridView.fitInView();
            }
            // 마우스 좌다운 릴리즈 ==> 선택마우스 영역 제거
            if (mouseSelectRect.visible) {
                mouseSelectRect.stop();
            }
            // 마우스 우다운 릴리즈 ==> 그리드 팬/틸트 종료
            if (mousePanning) {
                stopPanTilt();
            }
            if (itemResizingReady) {
                itemResizingReady = false;
                if (GridSystem.stopToResizeItem(mouse.x, mouse.y, itemTargetRect))
                    gridView.fitInView();
            }

        }
        onClicked: {
            // 마우스 좌클릭
            if (mouse.button == Qt.LeftButton) {
                console.log("left clicked");
            }
            // 마우스 우클릭 ==> 컨텍스트 메뉴
            else if (mouse.button == Qt.RightButton) {
                console.log("right clicked")
            }
        }

        onWheel: {
            // 마우스 휠 ==> 줌 인/아웃
            var scaleDelta = wheel.angleDelta.y > 0 ? 1.0+zoomSpeedOffset : 1.0-zoomSpeedOffset;
            var originX = wheel.x - gridView.gridPositionX;
            var originY = wheel.y - gridView.gridPositionY;
            gridView.zoomAnimation(originX, originY, scaleDelta);
        }

        Keys.onPressed: {
            switch (event.key) {
            case Qt.Key_Control:
                console.log("control key pressed");
                controlKeyPressed = true
                break;

            case Qt.Key_Shift:
                console.log("shift key pressed");
                shiftKeyPressed = true
                break;

            case Qt.Key_Delete:
                console.log("delete key pressed");
            }
        }
        Keys.onReleased: {
            switch (event.key) {
            case Qt.Key_Control:
                console.log("control key released");
                controlKeyPressed = false
                break;

            case Qt.Key_Shift:
                console.log("shift key released");
                shiftKeyPressed = false
                break;
            }
        }
    }

    // 줌 인/아웃 애니메이션
    ParallelAnimation {
        id: scaleAnimation
        property int easingType: Easing.OutExpo//Easing.OutQuad
        property int duration: 1000
        NumberAnimation {
            target: gridView
            properties: "gridPositionX"
            to: gridView.gridZoomAnimationX
            duration: scaleAnimation.duration
            easing.type: scaleAnimation.easingType
        }
        NumberAnimation {
            target: gridView
            properties: "gridPositionY"
            to: gridView.gridZoomAnimationY
            duration: scaleAnimation.duration
            easing.type: scaleAnimation.easingType
        }
        NumberAnimation {
            target: gridView
            properties: "gridZoomScale"
            to: gridView.gridZoomAnimationScale
            duration: scaleAnimation.duration
            easing.type: scaleAnimation.easingType
        }
        onStarted: {
            var startDate = new Date()
            console.log("started animation at", startDate.toISOString(), scaleAnimation.duration)
        }

        onFinished: {
            var startDate = new Date()
            console.log("ended animation at", startDate.toISOString())
        }

    }

    //--------  여기서부터 테스트용 버튼 들 -------------//
    Button {
        id: newVideoButton
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        text: "+1"
        z:50
        onClicked : {
            console.debug("newVideoButton clicked");
            gridView.addNewIem();
            gridView.fitInView();
        }
    }
    Button {
        id: newVideo10Button
        anchors.top: parent.top
        anchors.left: newVideoButton.right
        anchors.margins: 10
        text: "+10"
        z:50
        onClicked : {
            console.debug("newVideo10Button clicked");
            for (var i = 0 ; i < 10 ; i++) {
                gridView.addNewIem();
            }
            gridView.fitInView();
        }
    }
    Button {
        id: newVideo100Button
        anchors.top: parent.top
        anchors.left: newVideo10Button.right
        anchors.margins: 10
        text: "+100"
        z:50
        onClicked : {
            console.debug("newVideo100Button clicked");
            for (var i = 0 ; i < 100 ; i++) {
                gridView.addNewIem();
            }
            gridView.fitInView();
        }
    }
    Button {
        id: newVideo1000Button
        anchors.top: parent.top
        anchors.left: newVideo100Button.right
        anchors.margins: 10
        text: "+1000"
        z:50
        onClicked : {
            console.debug("newVideo1000Button clicked");
            for (var i = 0 ; i < 1000 ; i++) {
                gridView.addNewIem();
            }
            gridView.fitInView();
        }
    }
    Button {
        id: clearButton
        anchors.top: parent.top
        anchors.left: newVideo1000Button.right
        anchors.margins: 10
        text: "Clear"
        z:50
        onClicked : {
            console.debug("clearButton clicked");
            gridView.clear();
            gridView.fitInView();
        }

    }
    Button {
        id: fitButton
        anchors.top: parent.top
        anchors.left: clearButton.right
        anchors.margins: 10
        text: "Fit in view"
        z:50
        onClicked : {
            console.debug("fitInView clicked");
            gridView.fitInView();

        }
    }

}
