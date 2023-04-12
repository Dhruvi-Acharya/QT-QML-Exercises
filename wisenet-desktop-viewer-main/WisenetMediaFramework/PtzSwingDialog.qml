import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import "qrc:/WisenetStyle/"

WisenetMediaDialog {
    id: dialog
    title: WisenetLinguist.swing
    width: 298
    height: 321
    modal:false
    onlyCloseType: true
    property var swingItem

    property int swingTypeIdx: 0
    property int startPresetIdx: 0
    property int endPresetIdx: 0
    property string startPresetName: ""
    property string endPresetName: ""

    readonly property int stopSwingActionIndex: 3

    function showDialog(item) {
        console.log("PtzSwingDialog::showDialog()")
        swingItem = item
        swingItem.player.devicePtzPresetGet()
        swingItem.player.devicePtzSwingGet()

        swingTypeIdx = 0
        startPresetIdx = swingItem.player.swingPanStartPreset
        endPresetIdx = swingItem.player.swingPanEndPreset

        open();
    }

    function updateSwingPresets() {
        refreshSwingPresets()
        updateSwingName()
    }

    function refreshSwingPresets() {

        if(swingItem === undefined || swingItem.player === undefined || swingItem === null || swingItem.player === null) {
            return
        }

        if(swingTypeIdx === 0) { // Pan
            startPresetIdx = swingItem.player.swingPanStartPreset
            endPresetIdx = swingItem.player.swingPanEndPreset
        }
        else if(swingTypeIdx === 1) { // Tilt
            startPresetIdx = swingItem.player.swingTiltStartPreset
            endPresetIdx = swingItem.player.swingTiltEndPreset

        }
        else if(swingTypeIdx === 2) { // PanTilt
            startPresetIdx = swingItem.player.swingPanTiltStartPreset
            endPresetIdx = swingItem.player.swingPanTiltEndPreset
        }
    }

    function updateSwingName() {
        if(swingItem === undefined || swingItem.player === undefined || swingItem === null || swingItem.player === null) {
            return
        }

        var presetList = swingItem.player.ptzPresets
//        console.log("updateSwingName - presetList.length",presetList.length)
        if(presetList.length > 0) {
            startPresetName = getPresetName(startPresetIdx, presetList)
            endPresetName = getPresetName(endPresetIdx, presetList)
        }
    }

    function getPresetName(presetNo, presetList) {
        if (presetList) {
            for (var i  = 0 ; i < presetList.length ; i++) {
                if (presetList[i].presetNo === presetNo)
                    return presetList[i].presetName;
            }
        }
        return "";
    }

    onClosed: {
        console.log("PtzSwingDialog::onClosed");
        swingItem = null
    }
    contentItem: Item {
        WisenetComboBox{
            id: swingTypeComboBox
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 30
            currentIndex: swingTypeIdx

            model: ["Pan","Tilt","PanTilt"]

            onCurrentIndexChanged: {
//                console.log("swingTypeComboBox - currentIndex=",currentIndex)
                swingTypeIdx = currentIndex

                refreshSwingPresets()
                updateSwingName()
            }
        }

        Item {
            id: startStopRow
            anchors {
                top: swingTypeComboBox.bottom
                left: swingTypeComboBox.left
                right: swingTypeComboBox.right
                topMargin: 16
                leftMargin: 10
            }

            WisenetImageAndTextButton {
                id: startButton
                width: 85
                height: 16
                imageWidth: 14
                imageHeight: 13

                anchors {
                    left: parent.left
                    top: parent.top
                }

                txt: WisenetLinguist.start

                normalImage: WisenetImage.ptz_start_default
                hoverImage: WisenetImage.ptz_start_hover
                pressImage: WisenetImage.ptz_start_selected

                normalTextColor: WisenetGui.contrast_02_light_grey
                hoverTextColor: WisenetGui.contrast_00_white
                pressTextColor: WisenetGui.color_primary

                onButtonClicked: {
                    if (swingItem) {
                        swingItem.player.devicePtzSwingMoveStop(swingTypeIdx)
                    }
                }
            }

            WisenetImageAndTextButton {
                id: stopButton
                width: 85
                height: 16
                imageWidth: 12
                imageHeight: 12

                anchors {
                    top: parent.top
                    left: startButton.right
                    leftMargin: 30
                }

                txt: WisenetLinguist.stop

                normalImage: WisenetImage.ptz_stop_default
                hoverImage: WisenetImage.ptz_stop_hover
                pressImage: WisenetImage.ptz_stop_selected

                normalTextColor: WisenetGui.contrast_02_light_grey
                hoverTextColor: WisenetGui.contrast_00_white
                pressTextColor: WisenetGui.color_primary

                onButtonClicked: {
                    if (swingItem) {
                        swingItem.player.devicePtzSwingMoveStop(dialog.stopSwingActionIndex);
                    }
                }
            }

        }

        Text {
            id: presetListText
            text: WisenetLinguist.presetList
            color: WisenetGui.contrast_04_light_grey
            fontSizeMode: Text.VerticalFit
            minimumPixelSize: 4
            font.bold: true
            font.pixelSize: 12
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors {
                top: startStopRow.bottom
                left: startStopRow.left
                topMargin: 35
            }
        }

        WisenetMediaTextLabel {
            id: startText
            text: WisenetLinguist.start
            height: presetListText.height
            anchors {
                top: presetListText.bottom
                left: presetListText.left
                topMargin: 10
            }
        }

        Text {
            id: startSwingPresetText
            text: startPresetName
            color: WisenetGui.contrast_04_light_grey
            fontSizeMode: Text.VerticalFit
            minimumPixelSize: 4
            font.pixelSize: 12
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            height: presetListText.height
            anchors {
                top: startText.top
                right: parent.right
            }

        }

        WisenetMediaTextLabel {
            id: endText
            text: WisenetLinguist.end
            height: presetListText.height
            anchors {
                top: startText.bottom
                left: startText.left
                topMargin: 5
            }
        }

        Text {
            id: endSwingPresetText
            text: endPresetName
            color: WisenetGui.contrast_04_light_grey
            fontSizeMode: Text.VerticalFit
            minimumPixelSize: 4
            font.pixelSize: 12
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            height: presetListText.height
            anchors {
                top: endText.top
                right: parent.right
            }
        }
    }
}
