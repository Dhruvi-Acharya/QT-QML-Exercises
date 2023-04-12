import QtQuick 2.15
import QtQuick.Controls 2.15
import Wisenet.MediaController 1.0
import Wisenet.Define 1.0
import "qrc:/WisenetStyle/"

Popup {
    id: control
    width: Math.max(playFilteredSectionText.contentWidth + 135, 336)
    height: 494
    padding: 0
    modal: true
    focus: true

    onVisibleChanged: {
        if(visible) {
            recordTypeFilterViewModel.reloadRecordTypeFilter(focusedViewingGridItem.mediaParam.deviceId, mediaControlModel.recordingTypeFilter)
        }
    }

    RecordTypeFilterViewModel {
        id: recordTypeFilterViewModel
    }

    Rectangle {
        anchors.fill: parent
        color: WisenetGui.contrast_09_dark
        border.width: 1
        border.color: WisenetGui.contrast_06_grey

        Text {
            id: titleText
            x: 35
            y: 21
            height: 28
            text: WisenetLinguist.filter
            font.pixelSize: 24
            color: WisenetGui.contrast_00_white
            verticalAlignment: Text.AlignVCenter
        }

        Button {
            width: 24
            height: 24
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 11
            hoverEnabled: true
            background: Rectangle {
                color: WisenetGui.transparent
            }
            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: {
                    if(parent.pressed)
                        return WisenetImage.mediaController_close_press
                    else if(parent.hovered)
                        return WisenetImage.mediaController_close_hover
                    else
                        return WisenetImage.mediaController_close_default
                }
            }
            onClicked: {
                control.close()
            }
        }

        Rectangle {
            id: separator
            height: 1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 70
            anchors.leftMargin: 34
            anchors.rightMargin: 37
            color: WisenetGui.contrast_08_dark_grey
        }

        Rectangle {
            id: listRect
            anchors.fill: parent
            anchors.topMargin: 93
            anchors.leftMargin: 34
            anchors.rightMargin: 37
            anchors.bottomMargin: 57
            color: WisenetGui.contrast_08_dark_grey
            border.width: 1
            border.color: WisenetGui.contrast_07_grey

            ButtonGroup {
                id: childGroup
                exclusive: false
                checkState: allBox.checkState
            }

            WisenetCheckBox2 {
                id: allBox
                x: 12
                y: 10
                text: WisenetLinguist.all
                checkState: childGroup.checkState
                textLeftPadding: 2
            }

            Rectangle {
                width: parent.width
                height: 1
                y: 34
                color: WisenetGui.contrast_07_grey
            }

            ListView {
                id: recordTypeList
                anchors.fill: parent
                anchors.topMargin: 44
                anchors.bottomMargin: 14
                spacing: 0
                model: recordTypeFilterViewModel
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: WisenetScrollBar {}
                delegate: Rectangle {
                    x: 1
                    width: recordTypeList.width - 2
                    height: 26
                    color: WisenetGui.transparent

                    Rectangle {
                        anchors.fill: parent
                        color: checkbox.checked ? WisenetGui.color_primary : WisenetGui.transparent
                        opacity: 0.6
                    }

                    WisenetCheckBox2 {
                        id: checkbox
                        x: 12
                        anchors.verticalCenter: parent.verticalCenter
                        ButtonGroup.group: childGroup
                        text: getName(model.recordType)
                        checked: model.checked
                        showImage: true
                        imageSource: getImageSource(model.recordType)
                        onCheckedChanged: {
                            updateRecordTypeFilter(index, checked)
                        }
                    }
                }
            }
        }

        Text {
            id: playFilteredSectionText
            height: 14
            anchors.left: parent.left
            anchors.leftMargin: 35
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            text: WisenetLinguist.playFilteredSection
            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 12
            verticalAlignment: Text.AlignVCenter
        }

        Button {
            flat: true
            width: 46
            height: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 27
            anchors.right: parent.right
            anchors.rightMargin: 36
            checkable: true
            checked: false
            background: Rectangle {
                color: WisenetGui.transparent
            }

            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: {
                    if(parent.checked)
                        return WisenetImage.mediaController_switch_on
                    else
                        return WisenetImage.mediaController_switch_off
                }
            }

            onCheckedChanged: {
                controlButtonAreaView.changeAutoskipMode(checked)
            }
        }
    }

    function updateRecordTypeFilter(index, checked) {
        if(!control.visible)
            return

        var newTypeFilter = recordTypeFilterViewModel.saveRecordTypeFilter(index, checked)
        if(mediaControlModel.recordingTypeFilter !== newTypeFilter) {
            mediaControlModel.recordingTypeFilter = newTypeFilter
            timelineControlView.redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
            controlButtonAreaView.updateAutoskipTime(focusedMediaPlayPosition)
        }
    }

    function getName(recordType) {
        if(recordType === RecordFilterType.EmergencyTrigger)
            return "EmergencyTrigger"
        if(recordType === RecordFilterType.InternalHDDWarmup)
            return "InternalHDDWarmup"
        if(recordType === RecordFilterType.GSensorEvent)
            return "GSensorEvent"
        if(recordType === RecordFilterType.ShockDetection)
            return "ShockDetection"
        if(recordType === RecordFilterType.TemperatureChangeDetection)
            return "TemperatureChangeDetection"
        if(recordType === RecordFilterType.BoxTemperatureDetection)
            return "BoxTemperatureDetection"
        if(recordType === RecordFilterType.ObjectDetectionVehicle)
            return WisenetLinguist.objectDetectionVehicle
        if(recordType === RecordFilterType.ObjectDetectionFace)
            return WisenetLinguist.objectDetectionFace
        if(recordType === RecordFilterType.ObjectDetectionPerson)
            return WisenetLinguist.objectDetectionPerson
        if(recordType === RecordFilterType.FaceRecognition)
            return WisenetLinguist.faceRecognition
        if(recordType === RecordFilterType.ObjectDetectionLicensePlate)
            return WisenetLinguist.objectDetectionLicensePlate


        if(recordType === RecordFilterType.Normal)
            return WisenetLinguist.normalRecording
        if(recordType === RecordFilterType.Event)
            return WisenetLinguist.event
        if(recordType === RecordFilterType.AlarmInput)
            return WisenetLinguist.alarmInput
        if(recordType === RecordFilterType.VideoAnalysis)
            return WisenetLinguist.iva
        if(recordType === RecordFilterType.MotionDetection)
            return WisenetLinguist.motionDetection
        if(recordType === RecordFilterType.NetworkDisconnect)
            return "NetworkDisconnect"
        if(recordType === RecordFilterType.FaceDetection)
            return WisenetLinguist.faceDetection
        if(recordType === RecordFilterType.TamperingDetection)
            return WisenetLinguist.tamperingDetection
        if(recordType === RecordFilterType.AudioDetection)
            return WisenetLinguist.audioDetection
        if(recordType === RecordFilterType.Tracking)
            return WisenetLinguist.autoTracking
        if(recordType === RecordFilterType.Manual)
            return WisenetLinguist.manualRecording
        if(recordType === RecordFilterType.UserInput)
            return "UserInput"
        if(recordType === RecordFilterType.DefocusDetection)
            return WisenetLinguist.defocusDetection
        if(recordType === RecordFilterType.FogDetection)
            return WisenetLinguist.fogDetection
        if(recordType === RecordFilterType.AudioAnalysis)
            return WisenetLinguist.soundClassification
        if(recordType === RecordFilterType.QueueEvent)
            return "QueueEvent"
        if(recordType === RecordFilterType.videoloss)
            return WisenetLinguist.videoLoss


        return "Unknown"
    }

    function getImageSource(recordType) {
        if(recordType ===  RecordFilterType.EmergencyTrigger)
            return WisenetImage.recordType_dummy
        if(recordType ===  RecordFilterType.InternalHDDWarmup)
            return WisenetImage.recordType_dummy
        if(recordType ===  RecordFilterType.GSensorEvent)
            return WisenetImage.recordType_dummy
        if(recordType ===  RecordFilterType.ShockDetection)
            return WisenetImage.recordType_shock
        if(recordType ===  RecordFilterType.TemperatureChangeDetection)
            return WisenetImage.recordType_temperature
        if(recordType ===  RecordFilterType.BoxTemperatureDetection)
            return WisenetImage.recordType_boxTemperature
        if(recordType ===  RecordFilterType.ObjectDetectionVehicle)
            return WisenetImage.recordType_objectVehicle
        if(recordType ===  RecordFilterType.ObjectDetectionFace)
            return WisenetImage.recordType_objectFace
        if(recordType ===  RecordFilterType.ObjectDetectionPerson)
            return WisenetImage.recordType_objectPerson
        if(recordType ===  RecordFilterType.FaceRecognition)
            return WisenetImage.recordType_faceRecognition
        if(recordType ===  RecordFilterType.ObjectDetectionLicensePlate)
            return WisenetImage.recordType_objectLPR


        if(recordType === RecordFilterType.Normal)
            return WisenetImage.recordType_continuous
        if(recordType ===  RecordFilterType.Event)
            return WisenetImage.recordType_dummy
        if(recordType ===  RecordFilterType.AlarmInput)
            return WisenetImage.recordType_alarmIn
        if(recordType ===  RecordFilterType.VideoAnalysis)
            return WisenetImage.recordType_iva
        if(recordType ===  RecordFilterType.MotionDetection)
            return WisenetImage.recordType_motion
        if(recordType ===  RecordFilterType.NetworkDisconnect)
            return WisenetImage.recordType_dummy
        if(recordType ===  RecordFilterType.FaceDetection)
            return WisenetImage.recordType_face
        if(recordType ===  RecordFilterType.TamperingDetection)
            return WisenetImage.recordType_tampering
        if(recordType ===  RecordFilterType.AudioDetection)
            return WisenetImage.recordType_audio
        if(recordType ===  RecordFilterType.Tracking)
            return WisenetImage.recordType_tracking
        if(recordType ===  RecordFilterType.Manual)
            return WisenetImage.recordType_manual
        if(recordType ===  RecordFilterType.UserInput)
            return WisenetImage.recordType_dummy
        if(recordType ===  RecordFilterType.DefocusDetection)
            return WisenetImage.recordType_defocus
        if(recordType ===  RecordFilterType.FogDetection)
            return WisenetImage.recordType_fog
        if(recordType ===  RecordFilterType.AudioAnalysis)
            return WisenetImage.recordType_soundClassification
        if(recordType ===  RecordFilterType.QueueEvent)
            return WisenetImage.recordType_dummy
        if(recordType ===  RecordFilterType.videoloss)
            return WisenetImage.recordType_videoLoss


        return WisenetImage.recordType_dummy
    }
}
