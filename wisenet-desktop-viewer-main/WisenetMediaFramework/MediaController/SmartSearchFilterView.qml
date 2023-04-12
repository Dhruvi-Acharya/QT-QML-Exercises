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

            Column {
                anchors.fill: parent
                anchors.topMargin: 44
                anchors.bottomMargin: 14
                spacing: 0
                clip: true

                FilterItemRect {
                    id: passingFilter
                    text: WisenetLinguist.smartSearchPassing
                    checked: true
                }

                FilterItemRect {
                    id: motionFilter
                    text: WisenetLinguist.motionDetection
                }

                FilterItemRect {
                    id: enteringFilter
                    text: WisenetLinguist.smartSearchEntering
                }

                FilterItemRect {
                    id: exitingFilter
                    text: WisenetLinguist.smartSearchExiting
                }

                component FilterItemRect: Rectangle {
                    x: 1
                    width: parent.width - 2
                    height: 26
                    color: WisenetGui.transparent

                    property alias text: checkbox.text
                    property alias checked: checkbox.checked

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
                        checked: true
                        showImage: true
                        imageSource: getImageSource(text)
                        onCheckedChanged: updateTypeFilter()
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

    function updateTypeFilter() {
        if(!control.visible)
            return

        var newFilterList = []
        if(passingFilter.checked)
            newFilterList.push(SmartSearchResultType.Pass)
        if(motionFilter.checked)
            newFilterList.push(SmartSearchResultType.Motion)
        if(enteringFilter.checked)
            newFilterList.push(SmartSearchResultType.Enter)
        if(exitingFilter.checked)
            newFilterList.push(SmartSearchResultType.Exit)

        if(smartSearchGraphModel.smartSearchTypeFilter !== newFilterList) {
            smartSearchGraphModel.smartSearchTypeFilter = newFilterList
            timelineControlView.redraw(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime)
            controlButtonAreaView.updateAutoskipTime(focusedMediaPlayPosition)
        }
    }

    function getImageSource(typeName) {
        if(typeName === WisenetLinguist.smartSearchPassing)
            return WisenetImage.videoAnalyticsPassing
        else if(typeName === WisenetLinguist.motionDetection)
            return WisenetImage.motionDetection
        else if(typeName === WisenetLinguist.smartSearchEntering)
            return WisenetImage.videoAnalyticsEntering
        else if(typeName === WisenetLinguist.smartSearchExiting)
            return WisenetImage.videoAnalyticsExiting

        return WisenetImage.recordType_dummy
    }
}
