import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"

WisenetPopupWindowBase {
    title: WisenetLinguist.bookmark + " - " + bookmarkModel.cameraName
    width: 449
    height: 462
    resizeEnabled: false
    minmaxButtonVisible: false
    applyButtonText: WisenetLinguist.ok

    property bool editBookmark: false

    onVisibleChanged: {
        if(visible && !editBookmark)
            getThumbnailImage(bookmarkModel.startTime)
    }

    onApplyButtonClicked: {
        if(bookmarkModel.name.trim() === "") {
            checkNameDialog.show()
            return
        }

        bookmarkModel.saveBookmark()
        close()
    }

    // for thumbnail
    function getThumbnailImage(startTime) {
        thumbnailTimer.stop()
        thumbnailTimer.repeatCount = 0
        thumbnailTimer.lastMediaPlayPosition = focusedMediaPlayPosition
        requestSeektoRecordExist(startTime)
        thumbnailTimer.restart()
    }

    function setThumbanilImage() {
        var errorStatus = focusedViewingGridItem.mediaStatus !== WisenetMediaPlayer.Loading &&
                          focusedViewingGridItem.mediaStatus !== WisenetMediaPlayer.Loaded &&
                          focusedViewingGridItem.mediaStatus !== WisenetMediaPlayer.ReadyToLoaded

        focusedViewingGridItem.getThumbnailImage(function(result){
            bookmarkModel.setThumbnailImage(result.image)
            /*
            console.log("BookmarkPopupView::setThumbanilImage() : thumbnail updated")
            result.saveToFile("thumbnail.jpeg");
            */
        }, 192, errorStatus)
    }

    WisenetMessageDialog {
        id: checkNameDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        message: WisenetLinguist.nameCannotBlank
    }

    Timer {
        id: thumbnailTimer
        interval: 100; running: false; repeat: true

        property int repeatCount: 0
        property double lastMediaPlayPosition: 0

        onTriggered: {
            repeatCount++
            var isReady = false

            if((focusedViewingGridItem.mediaStatus !== WisenetMediaPlayer.Loading &&
                focusedViewingGridItem.mediaStatus !== WisenetMediaPlayer.Loaded &&
                focusedViewingGridItem.mediaStatus !== WisenetMediaPlayer.ReadyToLoaded) || repeatCount == 50) {
                // Loading,Loaded,ReadyToLoaded 상태가 아닌 실패 상태이거나, 50회 확인(5초)동안 재생시간의 변화가 없으면 No Image thumbnail 저장 (ToDo)
                console.log("BookmarkPopupView::thumbnailTimer Image Load failed", focusedViewingGridItem.mediaStatus, repeatCount)
                isReady = true
            }

            if(lastMediaPlayPosition != focusedMediaPlayPosition) {
                // 재생시간이 업데이트 된 경우 이미지 수신으로 판단
                console.log("BookmarkPopupView::thumbnailTimer Image capture ready")
                isReady = true
            }

            if(isReady) {
                setThumbanilImage()
                stop()
            }
        }
    }

    // Content Area
    Rectangle {
        id: contentArea
        anchors.fill: parent
        anchors.topMargin: 27
        anchors.bottomMargin: 25
        anchors.leftMargin: 50
        anchors.rightMargin: 49
        color: WisenetGui.transparent

        property int textColumnWidth: 122

        component BaseText: Text{
            color: WisenetGui.contrast_04_light_grey
            width: contentArea.textColumnWidth
            height: 14
            font.pixelSize: 12
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        BaseText {
            id: nameText
            text: WisenetLinguist.name
        }

        BaseText {
            id: fromText
            text: WisenetLinguist.from
            anchors.top: nameText.bottom
            anchors.topMargin: 32
        }

        BaseText {
            id: toText
            text: WisenetLinguist.to
            anchors.top: fromText.bottom
            anchors.topMargin: 32
        }

        BaseText {
            id: descriptionText
            text: WisenetLinguist.description
            anchors.top: toText.bottom
            anchors.topMargin: 32
        }

        WisenetTextField {
            id: nameTextField
            height: 17
            anchors.top: nameText.top
            anchors.left: nameText.right
            anchors.right: parent.right
            textField.maximumLength: 64
            textField.text: bookmarkModel.name
            textField.onTextChanged: {
                bookmarkModel.name = textField.text;
            }
        }

        WisenetDateEditor {
            id: fromDateEditor
            anchors.top: fromText.top
            anchors.left: fromText.right
            minimumYear: new Date(mediaControlModel.timelineStartTime).getFullYear()
            maximumYear: new Date(mediaControlModel.timelineEndTime).getFullYear()
            enabled: !editBookmark

            Connections {
                target: bookmarkModel
                function onStartTimeChanged() {
                    fromDateEditor.setDate(new Date(bookmarkModel.startTime))
                }
            }

            onControlFocusChanged: {
                if(!controlFocus && !editBookmark) {
                    var datetime = new Date(fromDateEditor.getDateText() + " " + fromTimeEditor.getTimeText()).getTime()
                    bookmarkModel.startTime = datetime
                    getThumbnailImage(datetime)
                    timelineControlView.selectionStartTime = datetime
                }
            }
        }

        WisenetTimeEditor {
            id: fromTimeEditor
            buttonVisible: true
            anchors.top: fromText.top
            anchors.right: parent.right
            enabled: !editBookmark

            Connections {
                target: bookmarkModel
                function onStartTimeChanged() {
                    var datetime = new Date(bookmarkModel.startTime)
                    fromTimeEditor.setTimeText(datetime.getHours(), datetime.getMinutes(), datetime.getSeconds())
                }
            }

            onControlFocusChanged: {
                if(!controlFocus && !editBookmark) {
                    var datetime = new Date(fromDateEditor.getDateText() + " " + fromTimeEditor.getTimeText()).getTime()
                    bookmarkModel.startTime = datetime
                    getThumbnailImage(datetime)
                    timelineControlView.selectionStartTime = datetime
                }
            }
        }

        WisenetDateEditor {
            id: toDateEditor
            anchors.top: toText.top
            anchors.left: toText.right
            minimumYear: new Date(mediaControlModel.timelineStartTime).getFullYear()
            maximumYear: new Date(mediaControlModel.timelineEndTime).getFullYear()
            enabled: !editBookmark

            Connections {
                target: bookmarkModel
                function onEndTimeChanged() {
                    toDateEditor.setDate(new Date(bookmarkModel.endTime))
                }
            }

            onControlFocusChanged: {
                if(!controlFocus) {
                    var datetime = new Date(toDateEditor.getDateText() + " " + toTimeEditor.getTimeText()).getTime()
                    bookmarkModel.endTime = datetime
                    timelineControlView.selectionEndTime = datetime
                }
            }
        }

        WisenetTimeEditor {
            id: toTimeEditor
            buttonVisible: true
            anchors.top: toText.top
            anchors.right: parent.right
            enabled: !editBookmark

            Connections {
                target: bookmarkModel
                function onEndTimeChanged() {
                    var datetime = new Date(bookmarkModel.endTime)
                    toTimeEditor.setTimeText(datetime.getHours(), datetime.getMinutes(), datetime.getSeconds())
                }
            }

            onControlFocusChanged: {
                if(!controlFocus) {
                    var datetime = new Date(toDateEditor.getDateText() + " " + toTimeEditor.getTimeText()).getTime()
                    bookmarkModel.endTime = datetime
                    timelineControlView.selectionEndTime = datetime
                }
            }
        }

        TextArea {
            id: descriptionTextBox
            //height: 135
            anchors.top: descriptionText.top
            anchors.topMargin: -6
            anchors.left: nameText.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            background: Rectangle {
                color: WisenetGui.transparent
                border.color: WisenetGui.contrast_06_grey
                border.width: 1
            }
            padding: 6
            leftPadding: 7
            rightPadding: 7
            textMargin: 1
            color: WisenetGui.contrast_01_light_grey
            selectionColor: WisenetGui.color_primary_deep_dark
            selectedTextColor: WisenetGui.contrast_00_white
            font.pixelSize: 12
            selectByMouse: true
            wrapMode: TextEdit.Wrap

            text: bookmarkModel.description
            onTextChanged: {
                var pos = positionAt(leftPadding + 1, height - bottomPadding + 1);
                if(length >= pos) {
                    remove(pos, length);
                }
                bookmarkModel.description = text
            }

            onCursorPositionChanged: {
                var pos = positionAt(leftPadding + 1, height - bottomPadding + 1);
                if(cursorPosition >= pos)
                    cursorPosition = length
            }
        }
    }
}
