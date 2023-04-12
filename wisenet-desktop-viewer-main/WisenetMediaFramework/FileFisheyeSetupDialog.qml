import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import Qt.labs.qmlmodels 1.0

/////////////////////////////////////////////////////////////
// 로컬 파일 Fisheye Dewarping Setup 팝업
/////////////////////////////////////////////////////////////
WisenetMediaDialog {
    id: dialog
    title: WisenetLinguist.fisheyeSetup
    width: 450
    height: 220+headerHeight
    modal:true
    applyAndClose: false

    property var fileItem
    function showDialog(item) {
        fileItem = item;
        fisheyeUseCheckBox.checked = fileItem.itemModel.fileFisheyeEnable;
        if (fileItem.itemModel.fileFisheyeEnable) {
            lensTypeCombo.currentIndex = lensTypeCombo.indexOfValue(fileItem.itemModel.fileLensType);
            mountCombo.currentIndex = mountCombo.indexOfValue(fileItem.itemModel.fileLensLocation);
        }
        else {
            lensTypeCombo.currentIndex = 0;
            mountCombo.currentIndex = 0;
        }
        open();
    }
    onClosed: {
        fileItem = null;
    }
    onAccepted: {
        if (fileItem) {
            if (fileItem.itemModel.fileFisheyeEnable !== fisheyeUseCheckBox.checked ||
                fileItem.itemModel.fileLensType !== lensTypeCombo.currentValue ||
                fileItem.itemModel.fileLensLocation !== mountCombo.currentValue) {
                console.log("fisheye enable::",
                            fisheyeUseCheckBox.checked,
                            lensTypeCombo.currentValue, mountCombo.currentValue);
                fileItem.itemModel.fileFisheyeEnable = fisheyeUseCheckBox.checked;
                fileItem.itemModel.fileLensType = lensTypeCombo.currentValue;
                fileItem.itemModel.fileLensLocation = mountCombo.currentValue;
                fileItem.updateLocalFisheyeSettings();
            }
        }
        close();
    }

    contentItem: Item{
        Row {
            id: fisheyeEnableRow
            anchors {
                top: parent.top
                topMargin: 28
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 15
            WisenetMediaTextLabel {
                id: enableLabel
                text: WisenetLinguist.fisheyeDewarping
                width: 115
                height: 20
                textElide: Text.ElideRight
            }

            Item {
                width: 228
                height: fisheyeUseCheckBox.height
                anchors.verticalCenter: parent.verticalCenter
                WisenetMediaCheckBox {
                    id: fisheyeUseCheckBox
                    text: WisenetLinguist.enable
                    WisenetMediaToolTip {
                        visible: parent.hovered && enableLabel.textTruncated
                        text: WisenetLinguist.fisheyeDewarping
                    }
                }
            }
        }

        Row {
            id: lensTypeRow
            anchors {
                top: fisheyeEnableRow.bottom
                topMargin: 28
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 15
            WisenetMediaTextLabel {
                id: lensTypeLabel
                text: WisenetLinguist.type /* todo */
                width: 115
                height: 20
            }

            WisenetMediaComboBox {
                id: lensTypeCombo
                width: 228
                model: ["SNF-8010", "PNF-9010", "XNF-8010", "HCF-8010", "QNF-8010", "QNF-9010", "XNF-9010", "TNF-9010", "XNF-9013"]
                anchors.verticalCenter: parent.verticalCenter
                onCurrentIndexChanged: {
                    //console.log("currentIndex=", currentIndex)
                }
            }
        }
        Row {
            id: mountLocationRow
            anchors {
                top: lensTypeRow.bottom
                topMargin: 28
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 15
            WisenetMediaTextLabel {
                id: mountLabel
                text: WisenetLinguist.mount /* todo */
                width: 115
                height: 20
            }

            WisenetMediaComboBox {
                id: mountCombo
                width: 228
                textRole: "text"
                valueRole: "value"
                anchors.verticalCenter: parent.verticalCenter
                model: [
                    { value: MediaLayoutItemViewModel.L_Ceiling, text: WisenetLinguist.ceiling },
                    { value: MediaLayoutItemViewModel.L_Wall, text: WisenetLinguist.wall },
                    { value: MediaLayoutItemViewModel.L_Ground, text: WisenetLinguist.ground }
                ]
                onCurrentIndexChanged: {
                    //console.log("currentIndex=", currentIndex)
                }
            }
        }
    }
}
