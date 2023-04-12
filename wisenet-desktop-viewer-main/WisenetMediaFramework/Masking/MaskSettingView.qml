import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import Wisenet.Define 1.0
import WisenetMediaFramework 1.0
import "qrc:/"

Item {
    id: maskSettingView

    property alias previewOn: maskPreviewButton.toggleOn
    property alias fixedOn: fixedButton.toggleOn
    property alias manualOn: manualButton.toggleOn
    property alias autoOn: autoButton.toggleOn

    property alias maskPaintType: maskPaintTypeComboBox.currentValue
    property alias mosaicCellSize: cellSizeSlider.value
    property alias solidColor: colorComboBox.currentValue
    property alias areaType: includeRadioButton.checked

    signal openDialog()
    signal autoButtonClicked(var isOn)
    signal fixedButtonClicked(var isOn)
    signal manualButtonClicked(var isOn)
    signal previewButtonClicked(var isOn)

    function resetButtons(){
        fixedButton.toggleOn = false
        manualButton.toggleOn = false
        autoButton.toggleOn = false
    }

    function selectMaskType(maskType){
        if(maskType === MaskType.Fixed){
            fixedButton.toggleOn = true
            manualButton.toggleOn = false
            autoButton.toggleOn = false
            fixedButtonClicked(true)
        }
        else if(maskType === MaskType.Manual){
            fixedButton.toggleOn = false
            manualButton.toggleOn = true
            autoButton.toggleOn = false
            manualButtonClicked(true)
        }
        else if(maskType === MaskType.Auto){
            fixedButton.toggleOn = false
            manualButton.toggleOn = false
            autoButton.toggleOn = true
            autoButtonClicked(true)
        }
    }

    Rectangle{
        id: mainRect

        anchors.fill: parent
        anchors.topMargin: 16
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        color: "transparent"

        WisenetMediaButtonItem{
            id: openFileButton

            width: 24
            height: 24
            imgWidth: 24
            imgHeight: 24
            imgSourceSize: "24x24"

            anchors.top: parent.top
            anchors.left: parent.left

            normalSource: "qrc:/images/Masking/file_open_normal.svg"
            pressSource: "qrc:/images/Masking/file_open_normal.svg"
            hoverSource: "qrc:/images/Masking/file_open_hover.svg"

            onMouseClicked: maskSettingView.openDialog()

            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.openMediaFiles
            }
        }

        Rectangle {
            id: spliter
            width: 1
            height: 16

            anchors.top: parent.top
            anchors.left: openFileButton.right
            anchors.topMargin: 4
            anchors.leftMargin: 8
            color: WisenetGui.contrast_08_dark_grey
        }

        WisenetMediaToggleImageButtonItem{
            id: maskPreviewButton

            width: 24
            height: 24
            imgWidth: 24
            imgHeight: 24
            imgSourceSize: "24x24"
            anchors.top: parent.top
            anchors.left: spliter.right
            anchors.leftMargin: 8

            normalSource: "qrc:/images/Masking/preview_mask_normal.svg"
            normalOnSource: "qrc:/images/Masking/preview_mask_selected.svg"
            pressSource: "qrc:/images/Masking/preview_mask_normal.svg"
            pressOnSource: "qrc:/images/Masking/preview_mask_selected.svg"
            hoverSource: "qrc:/images/Masking/preview_mask_hover.svg"
            hoverOnSource: "qrc:/images/Masking/preview_mask_hover.svg"

            onMouseClicked: {
                toggleOn = !toggleOn
                previewButtonClicked(toggleOn)
            }

            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.maskPreview
            }
        }

        Rectangle {
            id: spliter2
            width: 1
            height: 16

            anchors.top: parent.top
            anchors.left: maskPreviewButton.right
            anchors.topMargin: 4
            anchors.leftMargin: 8
            color: WisenetGui.contrast_08_dark_grey
        }

        WisenetMediaToggleButtonItem{
            id: fixedButton

            width: 24
            height: 24
            imgWidth: 24
            imgHeight: 24
            imgSourceSize: "24x24"
            anchors.top: parent.top
            anchors.left: spliter2.right
            anchors.leftMargin: 8

            normalSource: "qrc:/images/Masking/fixed_mask_normal.svg"
            normalOnSource: "qrc:/images/Masking/fixed_mask_selected.svg"
            pressSource: "qrc:/images/Masking/fixed_mask_selected.svg"
            hoverSource: "qrc:/images/Masking/fixed_mask_hover.svg"

            onMouseClicked: {
                fixedButton.toggleOn = !fixedButton.toggleOn
                manualButton.toggleOn = false
                autoButton.toggleOn = false
                maskSettingView.fixedButtonClicked(fixedButton.toggleOn)
            }

            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.fixedMask
            }
        }

        WisenetMediaToggleButtonItem{
            id: manualButton

            width: 24
            height: 24
            imgWidth: 24
            imgHeight: 24
            imgSourceSize: "24x24"
            anchors.top: parent.top
            anchors.left: fixedButton.right
            anchors.leftMargin: 10

            normalSource: "qrc:/images/Masking/manual_mask_normal.svg"
            normalOnSource: "qrc:/images/Masking/manual_mask_selected.svg"
            pressSource: "qrc:/images/Masking/manual_mask_selected.svg"
            hoverSource: "qrc:/images/Masking/manual_mask_hover.svg"

            onMouseClicked: {
                fixedButton.toggleOn = false
                manualButton.toggleOn = !manualButton.toggleOn
                autoButton.toggleOn = false
                maskSettingView.manualButtonClicked(manualButton.toggleOn)
            }

            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.manualMask
            }
        }

        WisenetMediaToggleButtonItem{
            id: autoButton

            width: 24
            height: 24
            imgWidth: 24
            imgHeight: 24
            imgSourceSize: "24x24"
            anchors.top: parent.top
            anchors.left: manualButton.right
            anchors.leftMargin: 10

            normalSource: "qrc:/images/Masking/auto_mask_normal.svg"
            normalOnSource: "qrc:/images/Masking/auto_mask_selected.svg"
            pressSource: "qrc:/images/Masking/auto_mask_selected.svg"
            hoverSource: "qrc:/images/Masking/auto_mask_hover.svg"

            onMouseClicked: {
                fixedButton.toggleOn = false
                manualButton.toggleOn = false
                autoButton.toggleOn = !autoButton.toggleOn
                maskSettingView.autoButtonClicked(autoButton.toggleOn)
            }

            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.autoMask
            }
        }

        Rectangle{
            id: maskPaintTypeRect

            height: 17
            anchors.top: openFileButton.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 19

            color: "transparent"

            WisenetMediaTextLabel{
                id: maskingTypeText

                width: 80
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                text: WisenetLinguist.maskingType
            }

            WisenetMediaComboBox{
                id: maskPaintTypeComboBox

                width: 108
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                textRole: "text"
                valueRole: "value"
                model: [
                    { value: MaskPaintType.Mosic, text: WisenetLinguist.mosaic },
                    { value: MaskPaintType.Solid, text: WisenetLinguist.solid }
                ]
            }
        }

        Rectangle{
            id: cellSizeRect

            height: 17
            anchors.top: maskPaintTypeRect.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 14

            color: "transparent"

            visible: maskPaintTypeComboBox.currentIndex == 0

            WisenetMediaTextLabel{
                id: cellSizeText

                width: 80
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: WisenetLinguist.cellSize

            }

            Slider {
                id: cellSizeSlider
                hoverEnabled: true
                width: 108
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                leftPadding: 0
                rightPadding: 0

                from: 5
                to: 50
                snapMode: Slider.SnapOnRelease
                value: 20
                stepSize: 3

                // Custom background
                background: Rectangle {
                    x: cellSizeSlider.leftPadding
                    y: cellSizeSlider.topPadding + cellSizeSlider.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 1
                    width: cellSizeSlider.availableWidth
                    height: implicitHeight
                    color: WisenetGui.contrast_06_grey

                    Rectangle {
                        width: cellSizeSlider.visualPosition * parent.width
                        height: parent.height
                        color: WisenetGui.contrast_06_grey
                    }
                }

                // Custom handle
                handle: Rectangle {
                    x: cellSizeSlider.leftPadding + cellSizeSlider.visualPosition * (cellSizeSlider.availableWidth - width)
                    y: cellSizeSlider.topPadding + cellSizeSlider.availableHeight / 2 - height / 2
                    implicitWidth: 12
                    implicitHeight: 12
                    radius: 6
                    color: WisenetGui.contrast_06_grey
                }
            }
        }

        Rectangle{
            id: colorRect

            height: 17
            anchors.top: maskPaintTypeRect.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 14

            color: "transparent"

            visible: maskPaintTypeComboBox.currentIndex == 1

            WisenetMediaTextLabel{
                id: colorText

                width: 80
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: WisenetLinguist.color
            }

            WisenetMediaComboBox{
                id: colorComboBox

                width: 108
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                textRole: "text"
                valueRole: "rgb"
                model: [
                    { rgb: "#FFFFFF", text: WisenetLinguist.white },
                    { rgb: "#000000", text: WisenetLinguist.black },
                    { rgb: "#CC601A", text: WisenetLinguist.orange },
                    { rgb: "#496621", text: WisenetLinguist.green },
                    { rgb: "#8A0000", text: WisenetLinguist.red },
                    { rgb: "#3FC6AE", text: WisenetLinguist.emerald },
                    { rgb: "#00D1FF", text: WisenetLinguist.skyblue }
                ]
            }
        }

        Rectangle{
            id: areaRect

            height: 27

            anchors.top: cellSizeRect.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 9

            color: "transparent"

            WisenetMediaTextLabel{
                id: areaText

                width: 80
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: WisenetLinguist.area
            }

            Rectangle{
                id: radioButtonRect

                height: 24
                width: 108

                anchors.right: parent.right
                anchors.bottom: parent.bottom

                color: "transparent"

                WisenetRadioButton{
                    id: includeRadioButton

                    width: 23
                    height: 24
                    anchors.top: parent.top
                    anchors.right: includeImage.left
                    anchors.topMargin: 5

                    text: ""
                    checked: true
                }

                Image{
                    id: includeImage

                    width:24
                    height:24
                    sourceSize.width: 24
                    sourceSize.height: 24

                    anchors.top: parent.top
                    anchors.right: excludeRadioButton.left
                    anchors.rightMargin: 14

                    source: "qrc:/images/Masking/include_normal.svg"

                    MouseArea{
                        anchors.fill: parent

                        onClicked: includeRadioButton.checked = true
                    }
                }

                WisenetRadioButton{
                    id: excludeRadioButton

                    width: 23
                    height: 24
                    anchors.top: parent.top
                    anchors.right: excludeImage.left
                    anchors.topMargin: 5

                    text: ""
                }

                Image{
                    id: excludeImage

                    width:24
                    height:24
                    sourceSize.width: 24
                    sourceSize.height: 24

                    anchors.top: parent.top
                    anchors.right: parent.right

                    source: "qrc:/images/Masking/exclude_normal.svg"

                    MouseArea{
                        anchors.fill: parent

                        onClicked: excludeRadioButton.checked = true
                    }
                }
            }
        }
    }
}
