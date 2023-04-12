import QtQuick 2.15
import WisenetStyle 1.0
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0
import "qrc:/"

Item {
    id: maskOsdControl

    property bool controlVisible: false
    property bool isDrawing: fixedDrawControl.enabled || manualDrawControl.enabled || autoDrawControl.enabled
    property int newMaskId: 0

    property bool autoOn : false
    property bool manualOn : false
    property bool fixedOn : false

    property var selectedMaskRectangle : null    // selected MaskRectangle.qml object

    function stopMaskDrawMode() {
        maskOsdControl.autoOn = false
        maskOsdControl.manualOn = false
        maskOsdControl.fixedOn = false
    }

    function clearMaskRects() {
        fixedDrawControl.clearAll()
        manualDrawControl.clearAll()
        autoDrawControl.clearAll()
    }

    function getMaskDataList() {
        var maskDataList = []

        fixedDrawControl.rectList.forEach(function(rect) {
            // maskData[0]:maskId, maskData[1]:maskType, maskData[2]:rectCoordinates([x,y,width,height])
            var maskData = []
            maskData.push(rect.maskId)
            maskData.push(rect.maskType)
            maskData.push(rect.getRectCoordinates())

            maskDataList.push(maskData)
        });

        manualDrawControl.rectList.forEach(function(rect) {
            // maskData[0]:maskId, maskData[1]:maskType, maskData[2]:rectCoordinates([x,y,width,height])
            var maskData = []
            maskData.push(rect.maskId)
            maskData.push(rect.maskType)
            maskData.push(rect.getRectCoordinates())

            maskDataList.push(maskData)
        });

        autoDrawControl.rectList.forEach(function(rect) {
            // maskData[0]:maskId, maskData[1]:maskType, maskData[2]:rectCoordinates([x,y,width,height])
            var maskData = []
            maskData.push(rect.maskId)
            maskData.push(rect.maskType)
            maskData.push(rect.getRectCoordinates())

            maskDataList.push(maskData)
        });

        return maskDataList
    }

    function updateMaskRectPosition(maskRectMap) {
        // update all mask rect position in maskRectMap
        autoDrawControl.updateMaskRectPosition(maskRectMap)
        manualDrawControl.updateMaskRectPosition(maskRectMap)
        fixedDrawControl.updateMaskRectPosition(maskRectMap)
    }

    function startAutoMasking(isOn){
        maskOsdControl.autoOn = isOn
        maskOsdControl.fixedOn = false
        maskOsdControl.manualOn = false
    }

    function startFixedMasking(isOn){
        maskOsdControl.autoOn = false
        maskOsdControl.fixedOn = isOn
        maskOsdControl.manualOn = false
    }

    function startManualMasking(isOn){
        maskOsdControl.autoOn = false
        maskOsdControl.fixedOn = false
        maskOsdControl.manualOn = isOn
    }

    function selectMaskItem(maskId){
        var fixed = fixedDrawControl.selectMaskItem(maskId)
        var manual = manualDrawControl.selectMaskItem(maskId)
        var auto = autoDrawControl.selectMaskItem(maskId)

        if(!fixed && !manual && !auto) {
            selectedMaskRectangle = null
        }
        else {
            if(fixed)
                selectedMaskRectangle = fixed
            else if(manual)
                selectedMaskRectangle = manual
            else
                selectedMaskRectangle = auto
        }
    }

    function changeMaskItemVisible(maskId, isVisible){
        fixedDrawControl.changeMaskItemVisible(maskId, isVisible)
        manualDrawControl.changeMaskItemVisible(maskId, isVisible)
        autoDrawControl.changeMaskItemVisible(maskId, isVisible)
    }

    function changeMaskItemName(maskId, maskName){
        fixedDrawControl.changeMaskItemName(maskId, maskName)
        manualDrawControl.changeMaskItemName(maskId, maskName)
        autoDrawControl.changeMaskItemName(maskId, maskName)
    }

    function removeMaskItem(maskId){
        fixedDrawControl.removeMaskItemById(maskId)
        manualDrawControl.removeMaskItemById(maskId)
        autoDrawControl.removeMaskItemById(maskId)
    }

    function getMaskingRectId(){ // 현재 화면에 그려져 있는 모든 Rect들의 Id를 리턴
        var rectIdList = []
        autoDrawControl.rectList.forEach(function(rect) {
            rectIdList.push(rect.maskId)
        });
        manualDrawControl.rectList.forEach(function(rect) {
            rectIdList.push(rect.maskId)
        });
        fixedDrawControl.rectList.forEach(function(rect) {
            rectIdList.push(rect.maskId)
        });
        return rectIdList
    }

    function trackingButtonOff() {
        console.log("MaskDrawOsdControl trackingButtonOff")
        trackButton.toggleOn = false
    }

    signal startTracking(var speed)
    signal stopTracking()
    signal openMessageDialog(var message)
    signal drawFinished(var maskId, var maskType, var rectCoordinates)
    signal maskRemoved(var maskId, var maskType)
    signal updateFinished(var maskId, var maskType, var rectCoordinates)
    signal manualBoxUpdated(var maskId, var maskType, var rectCoordinates)
    signal selectedMaskItemChanged(var maskId)

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
            when: !maskOsdControl.controlVisible
            PropertyChanges {
                target: maskOsdControl
                opacity: 0.0
            }
        },
        State {
            when: maskOsdControl.controlVisible
            PropertyChanges {
                target: maskOsdControl
                opacity: 1.0
            }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation{
                target: maskOsdControl;
                properties:"opacity";
                duration:300
            }
        }
    ]

    // Fixed mask draw control
    RectDrawControl {
        id: fixedDrawControl
        anchors.fill: parent
        enabled: maskOsdControl.fixedOn
        z: enabled ? 1 : 0
        opacity: enabled ? 1 : 0.5
        maskType: MaskType.Fixed

        onDrawFinished: maskOsdControl.drawFinished(maskId, maskType, rectCoordinates)
        onMaskRemoved: maskOsdControl.maskRemoved(maskId, maskType)
        onUpdateFinished: maskOsdControl.updateFinished(maskId, maskType, rectCoordinates)
        /*
        onMaxCountOverflowed: {
            openMessageDialog(WisenetLinguist.maxLineWarn)
        }
        */

        onSelectedMaskItemChanged: {
            selectedMaskRectangle = maskItem
            maskOsdControl.selectedMaskItemChanged(maskItem.maskId)
        }
    }

    // Manual mask draw control
    RectDrawControl {
        id: manualDrawControl
        anchors.fill: parent
        enabled: maskOsdControl.manualOn
        z: enabled ? 1 : 0
        opacity: enabled ? 1 : 0.5
        maskType: MaskType.Manual

        onDrawFinished: maskOsdControl.drawFinished(maskId, maskType, rectCoordinates)
        onMaskRemoved: maskOsdControl.maskRemoved(maskId, maskType)
        onManualBoxUpdated: maskOsdControl.manualBoxUpdated(maskId, maskType, rectCoordinates)
        onUpdateFinished: maskOsdControl.updateFinished(maskId, maskType, rectCoordinates)

        /*
        onMaxCountOverflowed: {
            openMessageDialog(WisenetLinguist.maxRoiAreaWarn)
        }
        */

        onSelectedMaskItemChanged: {
            selectedMaskRectangle = maskItem
            maskOsdControl.selectedMaskItemChanged(maskItem.maskId)
        }
    }

    // Auto mask draw control
    RectDrawControl {
        id: autoDrawControl
        anchors.fill: parent
        enabled: maskOsdControl.autoOn
        z: enabled ? 1 : 0
        opacity: enabled ? 1 : 0.5
        maskType: MaskType.Auto

        onDrawFinished: maskOsdControl.drawFinished(maskId, maskType, rectCoordinates)
        onMaskRemoved: maskOsdControl.maskRemoved(maskId, maskType)
        onUpdateFinished: maskOsdControl.updateFinished(maskId, maskType, rectCoordinates)
        /*
        onMaxCountOverflowed: {
            openMessageDialog(WisenetLinguist.maxExcldeAreaWarn)
        }
        */

        onSelectedMaskItemChanged: {
            selectedMaskRectangle = maskItem
            maskOsdControl.selectedMaskItemChanged(maskItem.maskId)
        }
    }

    // 하단 버튼 영역
    Item {
        id: buttonControl
        z: 2
        x: !selectedMaskRectangle ? 0 :
           selectedMaskRectangle.maskRectangle.x + width < maskOsdControl.width ? selectedMaskRectangle.maskRectangle.x
                                                                                : maskOsdControl.width - width
        y: !selectedMaskRectangle ? 0 :
           selectedMaskRectangle.maskRectangle.y + selectedMaskRectangle.maskRectangle.height + height < maskOsdControl.height ? selectedMaskRectangle.maskRectangle.y + selectedMaskRectangle.maskRectangle.height
                                                                                                                               : maskOsdControl.height - height
        width: bottomBg.width
        height: 32
        visible: selectedMaskRectangle && selectedMaskRectangle.selected && selectedMaskRectangle.maskRectangle.visible

        signal mouseMoved();

        component BottomButtonItem: WisenetMediaButtonItem {
            width: buttonControl.height
            height: buttonControl.height
            imgHeight: 20
            imgWidth: 20
            imgSourceSize : "24x24"
            normalOpacity: 0.0
            hoverOpacity: 0.5
            pressOpacity: 0.8
            onHoverChanged: {
                if (hover) {
                    buttonControl.mouseMoved();
                }
            }
        }
        component BottomToggleButtonItem: WisenetMediaToggleImageButtonItem{
            visible: selectedMaskRectangle && selectedMaskRectangle.maskType !== MaskType.Fixed
            width: visible ? buttonControl.height : 0
            height: buttonControl.height
            imgHeight: 20
            imgWidth: 20
            imgSourceSize : "20x20"
            normalOpacity: 0.0
            normalOnOpacity: 0.8
            hoverOpacity: 0.5
            pressOpacity: 0.8
            onHoverChanged: {
                if (hover) {
                    buttonControl.mouseMoved();
                }
            }
        }

        Rectangle{
            id: bottomBg
            color: WisenetGui.color_live_control_bg
            opacity: 0.7
            anchors.centerIn: parent
            width: bottomButtons.calcWidth
            height: parent.height
            radius: 4
        }

        Row {
            id: bottomButtons
            anchors.centerIn: parent
            topPadding: 0
            height: parent.height
            spacing: 1
            property real calcWidth: width + 26

            BottomToggleButtonItem {
                id: trackButton
                normalSource: "qrc:/images/Masking/tracking_normal.svg"
                normalOnSource: "qrc:/images/Masking/tracking_selected.svg"
                pressSource: "qrc:/images/Masking/tracking_hover.svg"
                pressOnSource: "qrc:/images/Masking/tracking_selected.svg"
                hoverSource: "qrc:/images/Masking/tracking_hover.svg"
                hoverOnSource: "qrc:/images/Masking/tracking_selected.svg"
                onMouseClicked: {
                    toggleOn = !toggleOn
                    if(toggleOn)
                        startTracking(speedButton.toggleOn ? 0.5 : 1)
                    else
                        stopTracking()
                }
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: trackButton.toggleOn ? WisenetLinguist.trackingStop : WisenetLinguist.trackingStart
                }
            }
            BottomToggleButtonItem {
                id: speedButton
                enabled: !trackButton.toggleOn
                onMouseClicked: toggleOn = !toggleOn
                Text {
                    y: 6
                    height: 12
                    font.pixelSize: 12
                    color: WisenetGui.white
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: speedButton.toggleOn ? "x1/2" : "x1"
                }
            }            
            BottomButtonItem {
                id: deleteButton
                enabled: !trackButton.toggleOn
                normalSource: "qrc:/images/Masking/delete_normal.svg"
                pressSource: "qrc:/images/Masking/delete_hover.svg"
                hoverSource: "qrc:/images/Masking/delete_hover.svg"
                onMouseClicked: selectedMaskRectangle.deleteButtonClicked()
                WisenetMediaToolTip {
                    visible: parent.hover
                    text: WisenetLinguist.deleteText
                }
            }
        }
    }
}
