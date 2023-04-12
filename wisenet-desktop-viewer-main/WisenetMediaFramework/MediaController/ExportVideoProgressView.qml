import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import WisenetLanguage 1.0
import Wisenet.MediaController 1.0
import "qrc:/WisenetStyle/"

WisenetPopupWindowBase {
    id: rootWindow
    width: 792
    height: 422
    minimumWidth: 792
    minimumHeight: 422
    modality: Qt.NonModal
    title: WisenetLinguist.exportVideo
    applyButtonVisible: false
    cancelButtonText: exportVideoProgressModel.exportFinished ? WisenetLinguist.close : WisenetLinguist.cancel

    property bool needCloseConfirm: false

    onVisibleChanged: {
        if(visible) {
            exportVideoProgressModel.clearFinishedItem()
            needCloseConfirm = true
        }
    }

    onClosing: {
        if(needCloseConfirm && !exportVideoProgressModel.exportFinished) {
            stopAllConfirmDialog.show()
            close.accepted = false
        }
    }

    Text {
        id: dummyFailedText
        font.pixelSize: 12
        text: WisenetLinguist.failed
        visible: false
    }

    Text {
        id: dummySuccessText
        font.pixelSize: 12
        text: WisenetLinguist.success
        visible: false
    }

    Text {
        id: dummyCanceledText
        font.pixelSize: 12
        text: WisenetLinguist.canceled
        visible: false
    }

    // 3가지 상태 문구 중 가장 긴 문구의 width
    property real statusTextWidth: Math.max(dummyFailedText.contentWidth, dummySuccessText.contentWidth, dummyCanceledText.contentWidth)

    ListView {
        id: progressList
        anchors.fill: parent
        anchors.topMargin: 40
        anchors.leftMargin: 34
        anchors.rightMargin: 34
        anchors.bottomMargin: 19
        spacing: 24
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: WisenetScrollBar {}

        /*
        property int textColumnWidth: 0
        readonly property int maximumTextColumnWidth: 300
        */

        model: exportVideoProgressModel
        delegate: Rectangle {
            width: parent ? parent.width : 0
            height: 24
            color: WisenetGui.transparent

            Text {
                id: nameText
                anchors.verticalCenter: parent.verticalCenter
                width: 154 //progressList.textColumnWidth
                elide: Text.ElideRight
                text: model.name
                color: WisenetGui.contrast_04_light_grey
                height: 14
                font.pixelSize: 12
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                clip: true

                /*
                onContentWidthChanged: {
                    if(contentWidth > progressList.maximumTextColumnWidth) {
                        progressList.textColumnWidth = progressList.maximumTextColumnWidth
                    }
                    else if(contentWidth > progressList.textColumnWidth) {
                        progressList.textColumnWidth = contentWidth
                    }
                }
                */
            }

            ProgressBar {
                id: progressBar
                enabled: !model.isFailed && !model.isCanceled
                height: 8
                anchors.left: nameText.right
                anchors.leftMargin: 19
                anchors.right: statusText.left
                anchors.rightMargin: 19
                anchors.verticalCenter: parent.verticalCenter
                from: 0
                to: 100
                value: model.progress

                background: Rectangle {
                    anchors.fill: parent
                    color: enabled ? WisenetGui.contrast_07_grey : WisenetGui.contrast_08_dark_grey
                }

                contentItem: Item {
                    anchors.fill: parent
                    Rectangle {
                        width: progressBar.visualPosition * parent.width
                        height: parent.height
                        color: enabled ? WisenetGui.contrast_01_light_grey : WisenetGui.contrast_07_grey
                    }
                }
            }

            Button {
                id: xButton
                width: 24
                height: 24
                anchors.left: statusText.left
                hoverEnabled: true
                visible: model.progress !== 100 && !model.isFailed && !model.isCanceled
                background: Rectangle {
                    color: WisenetGui.transparent
                }
                Image {
                    anchors.fill: parent
                    sourceSize: Qt.size(width, height)
                    source: {
                        if(parent.pressed)
                        return WisenetImage.mediaController_delete_press
                        else if(parent.hovered)
                        return WisenetImage.mediaController_delete_hover
                        else
                        return WisenetImage.mediaController_delete_default
                    }
                }
                onClicked: {
                    if(model.progress !== 0) {
                        stopConfirmDialog.itemIndex = index
                        stopConfirmDialog.show()
                    }
                    else {
                        exportVideoProgressModel.stopExportItem(index)
                    }
                }
            }

            Text {
                id: statusText
                visible: !xButton.visible
                width: statusTextWidth
                height: 14
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: openFilePathButton.left
                anchors.rightMargin: 9
                elide: Text.ElideRight
                text: model.isFailed ? WisenetLinguist.failed :
                      model.isCanceled ? WisenetLinguist.canceled : WisenetLinguist.success
                color: model.isFailed ? WisenetGui.color_accent : WisenetGui.contrast_04_light_grey
                font.pixelSize: 12
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                clip: true
            }

            Button {
                id: openFilePathButton
                width: 24
                height: 24
                anchors.right: parent.right
                hoverEnabled: true
                visible: !xButton.visible && model.progress !== 0
                background: Rectangle {
                    color: WisenetGui.transparent
                }
                Image {
                    anchors.fill: parent
                    sourceSize: Qt.size(width, height)
                    source: parent.hovered ? WisenetImage.mediafile_root_hover : WisenetImage.mediafile_root_normal
                }
                onClicked: {
                    exportVideoProgressModel.openFilePath(index)
                }
            }
        }
    }

    WisenetMessageDialog {
        id: stopConfirmDialog
        message: WisenetLinguist.exportCancelWarn
        applyButtonText: WisenetLinguist.ok

        property int itemIndex: -1

        onApplyButtonClicked: {
            if(itemIndex >= 0) {
                exportVideoProgressModel.stopExportItem(itemIndex)
            }
        }
    }

    WisenetMessageDialog {
        id: stopAllConfirmDialog
        message: WisenetLinguist.exportCancelWarn
        applyButtonText: WisenetLinguist.ok

        onApplyButtonClicked: {
            exportVideoProgressModel.stopAllExportItem()
            rootWindow.needCloseConfirm = false
            rootWindow.close()
        }
    }
}
