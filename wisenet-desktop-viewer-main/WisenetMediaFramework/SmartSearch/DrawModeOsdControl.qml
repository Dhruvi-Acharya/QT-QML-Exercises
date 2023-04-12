import QtQuick 2.15
import WisenetStyle 1.0
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0
import "qrc:/"

Item {
    id: osdControl
    property bool controlVisible: false
    property bool searchFilterVisible : false
    property bool isDrawing: lineDrawControl.enabled || roiDrawControl.enabled || excludeDrawControl.enabled
    property real blockAeraWidth: blockAreaBase * 0.075
    property real blockAreaBase: height > width ? width : height

    function stopSmartSearchDrawMode() {
        lineButton.toggleOn = false
        roiButton.toggleOn = false
        excludeButton.toggleOn = false
    }

    function clearSmartSearchAreas() {
        lineDrawControl.clearAll()
        roiDrawControl.clearAll()
        excludeDrawControl.clearAll()
    }

    function getVirtualLineList() {
        var lineDataList = []

        lineDrawControl.lineList.forEach(function(line) {
            // lineData[0]:방향, lineData[1]:coordinates(array)
            var lineData = []
            lineData.push(line.direction)
            lineData.push([line.normalX1, line.normalY1, line.normalX2, line.normalY2])

            lineDataList.push(lineData)
        });

        return lineDataList
    }

    function getVirtualAreaList() {
        var areaDataList = []

        roiDrawControl.areaList.forEach(function(area) {
            // areaData[0]:areaType, areaData[1]:coordinates(array), areaData[2]:eventTypes(array), areaData[3]:aiTypes(array)
            var areaData = []
            areaData.push(area.areaType)
            areaData.push(area.getPoints())
            areaData.push(area.getEventTypes())
            areaData.push(area.getAITypes())

            areaDataList.push(areaData)
        });

        excludeDrawControl.areaList.forEach(function(area) {
            // areaData[0]:areaType, areaData[1]:coordinates(array), areaData[2]:eventTypes(array), areaData[3]:aiTypes(array)
            var areaData = []
            areaData.push(area.areaType)
            areaData.push(area.getPoints())
            areaData.push([])
            areaData.push([])

            areaDataList.push(areaData)
        });

        return areaDataList
    }

    signal smartSearchRequest()
    signal openMessageDialog(var message)

    // CHECK POINT
    // 아이템간 바인딩이 많은 경우 성능 부하 발생하여 visible 상태가 아닌경우에는 binding을 풀어줌.
    onVisibleChanged: {
        if (visible) {
            width = Qt.binding(function() { return videoItem.width})
            height = Qt.binding(function() { return videoItem.height})
        }
        else {
            width = 1;
            height = 1;
        }
    }

    states: [
        State {
            when: !osdControl.controlVisible
            PropertyChanges {
                target: osdControl
                opacity: 0.0
            }
        },
        State {
            when: osdControl.controlVisible
            PropertyChanges {
                target: osdControl
                opacity: 1.0
            }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation{
                target: osdControl;
                properties:"opacity";
                duration:300
            }
        }
    ]

    // 라인 그리기 영역
    LineDrawControl {
        id: lineDrawControl
        anchors.fill: parent        
        enabled: lineButton.toggleOn
        z: enabled ? 1 : 0
        opacity: enabled ? 1 : 0.5

        onMaxCountOverflowed: {
            openMessageDialog(WisenetLinguist.maxLineWarn)
        }
    }

    // 관심 영역 그리기 영역
    AreaDrawControl {
        id: roiDrawControl
        anchors.fill: parent
        enabled: roiButton.toggleOn
        z: enabled ? 1 : 0
        opacity: enabled ? 1 : 0.5
        areaType: SmartSearchAreaType.Inside
        aiFilterVisible: osdControl.searchFilterVisible

        onMaxCountOverflowed: {
            openMessageDialog(WisenetLinguist.maxRoiAreaWarn)
        }
    }

    // 제외 영역 그리기 영역
    AreaDrawControl {
        id: excludeDrawControl
        anchors.fill: parent
        enabled: excludeButton.toggleOn
        z: enabled ? 1 : 0
        opacity: enabled ? 1 : 0.5
        areaType: SmartSearchAreaType.Outside

        onMaxCountOverflowed: {
            openMessageDialog(WisenetLinguist.maxExcldeAreaWarn)
        }
    }

    // 하단 버튼 영역
    Item {
        id: bottomControl
        z: 2

        property bool isPlayback: false
        property int  profileMode:0

        signal lineButtonClicked();
        signal roiAreaButtonClicked();
        signal excludeAreaButtonClicked();
        signal clearButtonClicked();
        signal searchButtonClicked();
        signal mouseMoved();

        anchors {
            left : parent.left
            right: parent.right
            bottom : parent.bottom
            bottomMargin: height * 0.3
        }

        property real maxHeight: 32
        property real minHeight: 10
        property real baseHeight: parent.height > parent.width ? parent.width : parent.height
        property real zoomFactor: 0.12
        property real calcHeight: baseHeight*zoomFactor

        property real iconSize: height

        height: Math.min(calcHeight, maxHeight)
        visible: height >= minHeight ? true : false;

        component BottomButtonItem: WisenetMediaButtonItem {
            height: bottomControl.iconSize
            width: bottomControl.iconSize
            imgHeight: bottomControl.iconSize*0.625
            imgWidth: imgHeight
            imgSourceSize : "32x32"
            normalOpacity: 0.0
            hoverOpacity: 0.5
            pressOpacity: 0.8
            onHoverChanged: {
                if (hover) {
                    bottomControl.mouseMoved();
                }
            }
        }
        component BottomToggleButtonItem: WisenetMediaToggleButtonItem {
            height: bottomControl.iconSize
            width: bottomControl.iconSize
            imgHeight: bottomControl.iconSize*0.625
            imgWidth: imgHeight
            imgSourceSize : "32x32"
            normalOpacity: 0.0
            normalOnOpacity: 0.8
            hoverOpacity: 0.5
            pressOpacity: 0.8
            onHoverChanged: {
                if (hover) {
                    bottomControl.mouseMoved();
                }
            }
        }
        component BottomSeperator: Item {
            height: bottomControl.iconSize
            width: height*0.5
            Rectangle {
                anchors.centerIn: parent
                height: parent.width
                width:1
                color: WisenetGui.contrast_04_light_grey
            }
        }

        Rectangle{
            id: bottomBg
            color: WisenetGui.color_live_control_bg
            opacity: 0.7
            anchors.centerIn: parent
            width: bottomButtons.calcWidth
            height: parent.height
            radius: parent.height * 0.2
        }

        Row {
            id: bottomButtons
            anchors.centerIn: parent
            topPadding: 0
            height: parent.height
            spacing: 1
            property real calcWidth: width + (parent.iconSize*0.5)

            BottomToggleButtonItem {
                id: lineButton
                normalSource: "qrc:/images/SmartSearch/line_default.svg"
                normalOnSource: "qrc:/images/SmartSearch/line_select.svg"
                pressSource: "qrc:/images/SmartSearch/line_select.svg"
                hoverSource: "qrc:/images/SmartSearch/line_hover.svg"
                onMouseClicked: bottomControl.lineButtonClicked();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.virtualLine
                }
            }
            BottomToggleButtonItem {
                id: roiButton
                normalSource: "qrc:/images/SmartSearch/roi_default.svg"
                normalOnSource: "qrc:/images/SmartSearch/roi_select.svg"
                pressSource: "qrc:/images/SmartSearch/roi_select.svg"
                hoverSource: "qrc:/images/SmartSearch/roi_hover.svg"
                onMouseClicked: bottomControl.roiAreaButtonClicked();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.roiArea
                }
            }
            BottomToggleButtonItem {
                id: excludeButton
                normalSource: "qrc:/images/SmartSearch/exclude_default.svg"
                normalOnSource: "qrc:/images/SmartSearch/exclude_select.svg"
                pressSource: "qrc:/images/SmartSearch/exclude_select.svg"
                hoverSource: "qrc:/images/SmartSearch/exclude_hover.svg"
                onMouseClicked: bottomControl.excludeAreaButtonClicked();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.excludeArea
                }
            }
            BottomButtonItem {
                id: clearButton
                normalSource: "qrc:/images/SmartSearch/clear_default.svg"
                pressSource: "qrc:/images/SmartSearch/clear_select.svg"
                hoverSource: "qrc:/images/SmartSearch/clear_hover.svg"
                onMouseClicked: bottomControl.clearButtonClicked();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.clearAll
                }
            }
            BottomSeperator {

            }
            BottomButtonItem {
                id: searchButton
                normalSource: "qrc:/images/SmartSearch/search_default.svg"
                pressSource: "qrc:/images/SmartSearch/search_select.svg"
                hoverSource: "qrc:/images/SmartSearch/search_hover.svg"
                onMouseClicked: bottomControl.searchButtonClicked();
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.search
                }
            }
        }

        onLineButtonClicked: {
            lineButton.toggleOn = !lineButton.toggleOn;
            roiButton.toggleOn = false;
            excludeButton.toggleOn = false;
        }

        onRoiAreaButtonClicked: {
            lineButton.toggleOn = false;
            roiButton.toggleOn = !roiButton.toggleOn;
            excludeButton.toggleOn = false;
        }

        onExcludeAreaButtonClicked: {
            lineButton.toggleOn = false;
            roiButton.toggleOn = false;
            excludeButton.toggleOn = !excludeButton.toggleOn;
        }

        onClearButtonClicked: {
            clearSmartSearchAreas()
        }

        onSearchButtonClicked: {
            // 검색 수행
            smartSearchRequest()

            /*
            var lineList = []
            lineDrawControl.lineList.forEach(function(line) {
                var linePoints = [line.normalX1, line.normalY1, line.normalX2, line.normalY2]
                console.log("onSearchButtonClicked, line:", linePoints)
            });

            var roiList = []
            roiDrawControl.areaList.forEach(function(area) {
                var roiPoints = area.getPoints()
                console.log("onSearchButtonClicked, roi:", roiPoints)
            });
            */
        }
    }

    component BlockRect: Rectangle {
        color: "black"
        z: -1
        opacity: 0.3
        visible: isDrawing
    }

    BlockRect {
        id: topBlockRect
        x: blockAeraWidth
        width: parent.width - (height*2)
        height: blockAeraWidth
    }

    BlockRect {
        id: leftBlockRect
        width: blockAeraWidth
        height: parent.height
    }

    BlockRect {
        id: rightBlockRect
        anchors.right: parent.right
        width: blockAeraWidth
        height: parent.height
    }

    BlockRect {
        id: bottomBlockRect
        anchors.bottom: parent.bottom
        x: blockAeraWidth
        width: parent.width - (height*2)
        height: blockAeraWidth
    }
}
