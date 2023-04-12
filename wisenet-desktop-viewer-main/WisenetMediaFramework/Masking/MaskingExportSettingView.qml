import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0

Item {
    property int textColumnWidth: Math.max(digitalSignatureText.contentWidth + 35, 122)
    property var focusedViewingGridItem: null
    property var progressModel: focusedViewingGridItem && focusedViewingGridItem.hasPlayer ? focusedViewingGridItem.player.maskingExportProgressModel : null
    property bool isInProgress: xButton.visible

    component BaseText: Text{
        color: WisenetGui.contrast_04_light_grey
        height: 14
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        color: WisenetGui.transparent
        anchors.fill: parent
        anchors.margins: 20
        enabled: !isInProgress

        BaseText {
            id: dateTimeText
            text: WisenetLinguist.dateAndTime
            width: textColumnWidth
        }

        BaseText {
            id: fromText
            text: WisenetLinguist.from
            width: 50
            anchors.left: dateTimeText.right
        }

        BaseText {
            id: toText
            text: WisenetLinguist.to
            width: 50
            anchors.top: fromText.bottom
            anchors.topMargin: 32
            anchors.left: fromText.left
        }

        Rectangle{
            id: dayTimeDivider
            width:parent.width; height:1
            color: WisenetGui.setupPageLine
            anchors.top: toText.bottom
            anchors.topMargin: 32
        }

        BaseText {
            id: fileNamingText
            text: WisenetLinguist.fileNaming
            width: textColumnWidth
            anchors.top: dayTimeDivider.bottom
            anchors.topMargin: 32
        }

        BaseText {
            id: fileFormatText
            text: WisenetLinguist.fileFormat
            width: textColumnWidth
            anchors.top: fileNamingText.bottom
            anchors.topMargin: 32
        }

        BaseText {
            id: digitalSignatureText
            text: WisenetLinguist.digitalSignature
            enabled: wnmRadioButton.checked
            width: textColumnWidth
            anchors.top: fileFormatText.bottom
            anchors.topMargin: 32
        }

        BaseText {
            id: usePasswordText
            text: WisenetLinguist.usePassword
            width: textColumnWidth
            anchors.top: digitalSignatureText.bottom
            anchors.topMargin: 32
        }

        BaseText {
            id: exportToText
            text: WisenetLinguist.exportTo
            width: textColumnWidth
            anchors.top: usePasswordText.bottom
            anchors.topMargin: 107
        }

        WisenetDateEditor {
            id: fromDateEditor
            width: 80
            anchors.top: fromText.top
            anchors.left: fromText.right

            onControlFocusChanged: {
                if(!controlFocus) {
                    updateFromDateTime()
                }
            }
        }

        WisenetTimeEditor {
            id: fromTimeEditor
            width: 80
            buttonVisible: true
            anchors.top: fromText.top
            anchors.right: parent.right

            onControlFocusChanged: {
                if(!controlFocus) {
                    updateFromDateTime()
                }
            }
        }

        WisenetDateEditor {
            id: toDateEditor
            width: 80
            anchors.top: toText.top
            anchors.left: toText.right
            onControlFocusChanged: {
                if(!controlFocus) {
                    updateToDateTime()
                }
            }
        }

        WisenetTimeEditor {
            id: toTimeEditor
            width: 80
            buttonVisible: true
            anchors.top: toText.top
            anchors.right: parent.right

            onControlFocusChanged: {
                if(!controlFocus) {
                    updateToDateTime()
                }
            }
        }

        WisenetTextField {
            id: fileNamingTextField
            anchors.top: fileNamingText.top
            anchors.topMargin: -1
            anchors.left: fileNamingText.right
            textField.validator: RegExpValidator{
                // '.' 으로 시작할 수 없음
                // \ , / , : , * , ? , “ , < , > , | 을 포함할 수 없음
                regExp: /[^\.\\/:\*\?"<>\|][^\\/:\*\?"<>\|]*/
            }
        }

        Rectangle{
            id: fileFormatRadioButton
            color:"transparent"
            anchors.top: fileFormatText.top
            anchors.topMargin: -1
            anchors.left: fileFormatText.right
            property string fileExtension: mkvRadioButton.checked? "mkv" : "wnm"

            WisenetRadioButton{
                id: mkvRadioButton
                checked: true
                text: "MKV"
                font.pixelSize: 12
            }

            WisenetRadioButton{
                id:  wnmRadioButton
                text: "WNM"
                font.pixelSize: 12
                anchors.left: mkvRadioButton.right
                anchors.leftMargin: 85
            }
        }

        WisenetCheckBox2 {
            id: digitalSignatureCheckBox
            text: WisenetLinguist.enable
            enabled: wnmRadioButton.checked
            anchors.top: digitalSignatureText.top
            anchors.topMargin: -1
            anchors.left: digitalSignatureText.right
        }

        WisenetCheckBox2 {
            id: usePasswordCheckBox
            text: WisenetLinguist.enable
            enabled: wnmRadioButton.checked
            anchors.top: usePasswordText.top
            anchors.topMargin: -1
            anchors.left: usePasswordText.right
        }

        WisenetPasswordBox {
            id: passwordBox
            height: 17
            anchors.top: usePasswordText.bottom
            anchors.topMargin: 22
            anchors.left: usePasswordText.right
            anchors.right: parent.right
            enabled: usePasswordCheckBox.checked
            KeyNavigation.tab: confirmPasswordBox.textField
            placeholderText: WisenetLinguist.password
            showPassword: confirmPasswordBox.showPassword
            showPasswordButtonVisible: false
        }

        WisenetPasswordBox {
            id: confirmPasswordBox
            height: 17
            anchors.top: passwordBox.bottom
            anchors.topMargin: 20
            anchors.left: usePasswordText.right
            anchors.right: parent.right
            enabled: usePasswordCheckBox.checked
            placeholderText: WisenetLinguist.confirmPassword
        }

        WisenetComboBox{
            id: exportCombo
            anchors.top: exportToText.top
            anchors.topMargin: -1
            anchors.left: exportToText.right
            model:[WisenetLinguist.mediaFileFolder, WisenetLinguist.chooseFolder]
        }

        WisenetOrangeButton {
            id: exportButton
            anchors.top: exportCombo.bottom
            anchors.topMargin: 32
            anchors.horizontalCenter: parent.horizontalCenter
            text: WisenetLinguist.exportButton

            onClicked: {
                // check password
                if(usePasswordCheckBox.checked) {
                    if(passwordBox.text != confirmPasswordBox.text) {
                        messageDialog.message = WisenetLinguist.confirmPwNotMached
                        messageDialog.show()
                        return
                    }
                    else if(!passwordStrengthChecker.exportPasswordTest(passwordBox.text)) {
                        messageDialog.message = WisenetLinguist.exportPasswordMust
                        messageDialog.show()
                        return
                    }
                }

                if(exportCombo.currentIndex == 0) { // 디폴트 폴더가 선택되었을 경우
                    var startTime = new Date(fromDateEditor.getDateText() + " " + fromTimeEditor.getTimeText()).getTime()
                    var endTime = new Date(toDateEditor.getDateText() + " " + toTimeEditor.getTimeText()).getTime()
                    var fimeName = fileNamingTextField.textField.text + "." + fileFormatRadioButton.fileExtension
                    var filePath = localSettingViewModel.localRecordingPath
                    startMaskingExport(startTime, endTime, filePath, fimeName,
                                       usePasswordCheckBox.checked, passwordBox.text, digitalSignatureCheckBox.checked)
                }
                else { // 사용자가 choose folder later를 선택했을 경우
                    exportFolderDialog.open()
                }
            }
        }
    }

    Rectangle {
        width: parent.width - x - 20
        height: 24
        x: exportButton.x + 20
        y: exportButton.y + exportButton.height + 32
        color: WisenetGui.transparent
        visible: progressModel

        ProgressBar {
            id: progressBar
            enabled: progressModel && !progressModel.failed && !progressModel.canceled
            height: 8
            anchors.left: parent.left
            anchors.right: xButton.visible ? xButton.left : statusText.left
            anchors.rightMargin: 19
            anchors.verticalCenter: parent.verticalCenter
            from: 0
            to: 100
            value: progressModel ? progressModel.progress : 0

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
            anchors.right: parent.right
            hoverEnabled: true
            visible: progressModel && progressModel.progress !== 100 && !progressModel.failed && !progressModel.canceled
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
                if(progressModel.progress !== 0) {
                    stopConfirmDialog.show()
                }
                else {
                    stopMaskingExport()
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
            text: !progressModel ? "" :
                  progressModel.failed ? WisenetLinguist.failed :
                  progressModel.canceled ? WisenetLinguist.canceled : WisenetLinguist.success
            color: !progressModel ? WisenetGui.transparent :
                   progressModel.failed ? WisenetGui.color_accent : WisenetGui.contrast_04_light_grey
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
            visible: !xButton.visible && progressModel && progressModel.progress !== 0
            background: Rectangle {
                color: WisenetGui.transparent
            }
            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: parent.hovered ? WisenetImage.mediafile_root_hover : WisenetImage.mediafile_root_normal
            }
            onClicked: {
                progressModel.openFilePath()
            }
        }
    }

    WisenetMessageDialog {
        id: messageDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        message: WisenetLinguist.confirmPwNotMached
    }

    WisenetMessageDialog {
        id: stopConfirmDialog
        message: WisenetLinguist.exportCancelWarn
        applyButtonText: WisenetLinguist.ok

        onApplyButtonClicked: {
            stopMaskingExport()
        }
    }

    FileDialog {
        id: exportFolderDialog
        title: WisenetLinguist.exportVideo
        selectFolder: true
        folder: shortcuts.documents
        visible: false

        onAccepted: {
            var startTime = new Date(fromDateEditor.getDateText() + " " + fromTimeEditor.getTimeText()).getTime()
            var endTime = new Date(toDateEditor.getDateText() + " " + toTimeEditor.getTimeText()).getTime()
            var fimeName = fileNamingTextField.textField.text + "." + fileFormatRadioButton.fileExtension
            var filePath = filePathConveter.getPathByUrl(fileUrl)
            startMaskingExport(startTime, endTime, filePath, fimeName,
                               usePasswordCheckBox.checked, passwordBox.text, digitalSignatureCheckBox.checked)

        }
    }

    // 3가지 상태 문구 중 가장 긴 문구의 width
    property real statusTextWidth: Math.max(dummyFailedText.contentWidth, dummySuccessText.contentWidth, dummyCanceledText.contentWidth)
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

    Connections {
        target: mediaControllerView
        function onSelectionStartTimeChanged() {
            setExportTime(mediaControllerView.selectionStartTime, mediaControllerView.selectionEndTime)
        }
        function onSelectionEndTimeChanged() {
            setExportTime(mediaControllerView.selectionStartTime, mediaControllerView.selectionEndTime)
        }
    }

    function setExportTime(startTime, endTime) {
        if(startTime === 0 && endTime === 0)
            return

        if(startTime === 0)
            startTime = endTime - 1000
        if(endTime === 0)
            endTime = startTime + 1000

        setExportStartTime(startTime)
        setExportEndTime(endTime)
    }

    function setExportStartTime(startTime) {
        var startDateTime = new Date(startTime)
        fromDateEditor.setDate(startDateTime)
        fromTimeEditor.setTimeText(startDateTime.getHours(), startDateTime.getMinutes(), startDateTime.getSeconds())
    }

    function setExportEndTime(endTime) {
        var endDateTime = new Date(endTime)
        toDateEditor.setDate(endDateTime)
        toTimeEditor.setTimeText(endDateTime.getHours(), endDateTime.getMinutes(), endDateTime.getSeconds())
    }

    function updateFromDateTime() {
        var fromDateTime = new Date(fromDateEditor.getDateText() + " " + fromTimeEditor.getTimeText())
        var toDateTime = new Date(toDateEditor.getDateText() + " " + toTimeEditor.getTimeText())
        var startTime = fromDateTime.getTime()
        var endTime = toDateTime.getTime()

        // check time range
        if(startTime >= endTime)
            startTime = endTime - 1000
        if(startTime < mediaControllerView.timelineStartTime)
            startTime = mediaControllerView.timelineStartTime
        if(startTime != fromDateTime.getTime())
            setExportStartTime(startTime)

        // update timeline selection
        mediaControllerView.selectionStartTime = startTime
    }

    function updateToDateTime() {
        var fromDateTime = new Date(fromDateEditor.getDateText() + " " + fromTimeEditor.getTimeText())
        var toDateTime = new Date(toDateEditor.getDateText() + " " + toTimeEditor.getTimeText())
        var startTime = fromDateTime.getTime()
        var endTime = toDateTime.getTime()

        // check time range
        if(startTime >= endTime)
            endTime = startTime + 1000
        if(endTime > mediaControllerView.timelineEndTime)
            endTime = mediaControllerView.timelineEndTime
        if(endTime != toDateTime.getTime())
            setExportEndTime(endTime)

        // update timeline selection
        mediaControllerView.selectionEndTime = endTime
    }
}
