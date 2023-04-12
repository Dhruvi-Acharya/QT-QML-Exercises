import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

Item {
    id : fisheyeControl
    anchors.fill: parent
    property bool ptzEnable: false

    // 0:original, 1:quad, 2:single, 3:perimeter
    property int fisheyeViewMode

    // 0:ceiling, 1:wall, 2:ground
    property int fisheyeLocation

    property bool fisheyeError: false

    property real view1ZoomValue: 0
    property real view2ZoomValue: 0
    property real view3ZoomValue: 0
    property real view4ZoomValue: 0

    signal ptzControlClosed();

    signal arrowPtzTriggered(int viewIndex, real dx, real dy);
    signal directionPtzTriggered(int viewIndex, int direction);
    signal stopPtzTriggered(int viewIndex);
    signal zoomTriggered(int viewIndex, real dz)
    signal zoom1xTriggered(int viewIndex)
    signal pointMovePtzTriggered(int viewIndex, real px, real py);
    signal areaMovePtzTriggered(int viewIndex, real px, real py, real pw, real ph);
    signal absZoomTriggered(int viewIndex, real zoomValue);

    component ViewRegion: Item {
        property alias viewIndex: ptzNavigator.viewIndex
        property alias zoomValue: ptzNavigator.zoomValue
        PtzNavigator {
            id: ptzNavigator
            property int viewIndex : 1
            property bool supportFullPtz : fisheyeControl.fisheyeViewMode === MediaLayoutItemViewModel.V_Quad || fisheyeControl.fisheyeViewMode === MediaLayoutItemViewModel.V_Single
            supportPointMove: supportFullPtz
            supportAreaZoom: supportFullPtz
            support1xZoom: supportFullPtz
            supportOnlyPan: fisheyeControl.fisheyeViewMode === MediaLayoutItemViewModel.V_Panorama
            onArrowPtzTriggered: fisheyeControl.arrowPtzTriggered(viewIndex, dx, dy);
            onDirectionPtzTriggered: fisheyeControl.directionPtzTriggered(viewIndex, direction);
            onStopPtzTriggered: fisheyeControl.stopPtzTriggered(viewIndex);
            onZoomTriggered: fisheyeControl.zoomTriggered(viewIndex, dz);
            onZoom1xTriggered: fisheyeControl.zoom1xTriggered(viewIndex);
            onPointMovePtzTriggered: {
                var cp = fisheyeControl.mapFromItem(ptzNavigator, px, py);
                fisheyeControl.pointMovePtzTriggered(viewIndex, cp.x, cp.y);
            }
            onAreaMovePtzTriggered: {
                var cp = fisheyeControl.mapFromItem(ptzNavigator, px, py);
                fisheyeControl.areaMovePtzTriggered(viewIndex, cp.x, cp.y, pw, ph);
            }
            onAbsZoomTriggered: fisheyeControl.absZoomTriggered(viewIndex, zoomValue);
        }
    }

    // 뷰모드 그리드
    Grid {
        id: viewModeGrid
        anchors.fill: parent
        spacing: 0
        columns: (fisheyeViewMode === MediaLayoutItemViewModel.V_Quad) ? 2 : 1
        visible: fisheyeControl.ptzEnable

        ViewRegion {
            id: view1
            viewIndex: 1
            zoomValue: fisheyeControl.view1ZoomValue
            visible: (fisheyeViewMode !== MediaLayoutItemViewModel.V_Original) &&
                     !(fisheyeViewMode === MediaLayoutItemViewModel.V_Panorama && fisheyeLocation === MediaLayoutItemViewModel.L_Wall)
            width : (fisheyeViewMode === MediaLayoutItemViewModel.V_Quad) ? parent.width/2 : parent.width
            height : (fisheyeViewMode === MediaLayoutItemViewModel.V_Quad) ? parent.height/2 : parent.height
        }
        ViewRegion {
            id: view2
            viewIndex: 2
            zoomValue: fisheyeControl.view2ZoomValue
            visible: (fisheyeViewMode === MediaLayoutItemViewModel.V_Quad)
            width: parent.width/2
            height: parent.height/2
        }
        ViewRegion {
            id: view3
            viewIndex: 3
            zoomValue: fisheyeControl.view3ZoomValue
            visible: (fisheyeViewMode === MediaLayoutItemViewModel.V_Quad)
            width : parent.width/2
            height: parent.height/2
        }
        ViewRegion {
            id: view4
            viewIndex: 4
            zoomValue: fisheyeControl.view4ZoomValue
            visible: (fisheyeViewMode === MediaLayoutItemViewModel.V_Quad)
            width : parent.width/2
            height: parent.height/2
        }
    }

    Text {
        id: fisheyeErrorText
        width: parent.width-12
        visible: fisheyeControl.fisheyeError && width >= (paintedWidth+8)
        text: WisenetLinguist.fisheyeDewarpingError
        property real maxHeight: 36
        property real minHeight: 9
        property real calcHeight: parent.height/15

        anchors.centerIn: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        color: WisenetGui.contrast_00_white
        font.bold: true

        height: Math.min(calcHeight, maxHeight)
        fontSizeMode: Text.VerticalFit
        minimumPixelSize: 10
        font.pixelSize: 18
    }

    WisenetMediaButtonItem {
        visible: ptzEnable

        anchors {
            //horizontalCenter: parent.horizontalCenter
            bottom : parent.bottom
            bottomMargin: height * 0.3
        }
        // quad view에서 zoom in/out 버튼이 닫기버튼에 가려지기 때문에 1/4지점에 닫기 버튼 위치
        x: (fisheyeControl.fisheyeViewMode === MediaLayoutItemViewModel.V_Quad)
           ? (parent.width/4)-(width/2) : (parent.width/2)-(width/2)
        property real maxHeight: 32
        property real minHeight: 12
        property real baseHeight: parent.height > parent.width ? parent.width : parent.height
        property real zoomFactor: 0.12
        property real calcHeight: baseHeight*zoomFactor
        property real iconSize: height

        height: Math.min(calcHeight, maxHeight)
        width: height*2.25

        imgHeight: height*0.625
        imgWidth: imgHeight
        imgSourceSize : "32x32"
        normalOpacity: 0.7
        hoverOpacity: 0.8
        pressOpacity: 1.0
        normalColor: WisenetGui.color_live_control_bg
        normalSource: "images/return_Default.svg"
        pressSource: "images/return_Press.svg"
        hoverSource: "images/return_Hover.svg"
        WisenetMediaToolTip {
            visible: parent.hover
            text: WisenetLinguist.close
        }
        onMouseClicked: fisheyeControl.ptzControlClosed();
    }
}
