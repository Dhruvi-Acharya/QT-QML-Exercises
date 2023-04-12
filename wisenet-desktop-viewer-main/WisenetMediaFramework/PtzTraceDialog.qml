import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import "qrc:/WisenetStyle/"

WisenetMediaDialog {
    id: dialog
    title: WisenetLinguist.trace
    width: 298
    height: 239
    modal:false
    onlyCloseType: true
    property var traceItem

    function showDialog(item) {
        traceItem = item
        tourModel.clear()
        traceItem.player.devicePtzTraceGet()
        traceTypeComboBox.currentIndex = 0
        open();
    }

    function updateTracePresets() {
        tourModel.clear()

        var newItems = traceItem.player.tracePresets;
        console.log("PtzTraceDialog::updateTracePresets()");
        for (var i = 0 ; i < newItems.length ; i++) {
            tourModel.append({"text" : "Trace " + newItems[i]})
        }
    }
    onClosed: {
        console.log("PtzTraceDialog::onClosed");
        traceItem = null
        tourModel.clear();
    }
    ListModel {
        id: tourModel
    }
    contentItem: Item {

        WisenetComboBox{
            id: traceTypeComboBox
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: 30
            }

            model: tourModel
        }

        Item {
            id: startStopRow
            anchors {
                top: traceTypeComboBox.bottom
                left: traceTypeComboBox.left
                right: traceTypeComboBox.right
                topMargin: 18
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
                    if (traceItem) {
                        traceItem.player.devicePtzTraceMove(traceTypeComboBox.currentIndex + 1)
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
                    if (traceItem) {
                        traceItem.player.devicePtzTraceStop(traceTypeComboBox.currentIndex + 1)
                    }
                }
            }
        }
    }
}
