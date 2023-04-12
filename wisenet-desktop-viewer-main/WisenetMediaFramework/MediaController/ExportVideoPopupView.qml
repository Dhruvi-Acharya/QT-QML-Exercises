import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Wisenet.MediaController 1.0
import WisenetMediaFramework 1.0
import QtMultimedia 5.15
import QtQuick.Dialogs 1.3
import "qrc:/"
import "qrc:/WisenetStyle/"
import WisenetStyle 1.0

WisenetPopupWindowBase {
    id: exportPopupWindow
    title: WisenetLinguist.exportVideo
    width: 792
    height: 695
    minimumWidth: 792
    minimumHeight: 695

    minmaxButtonVisible: false
    resizeEnabled: true
    applyButtonVisible: true
    applyButtonText: WisenetLinguist.ok
    cancelButtonText: WisenetLinguist.cancel

    property ExportVideoModel exportVideoModel: exportVideoModel
    property bool singleMode: true
    property bool multiModeOlny: false
    property int textColumnWidth: Math.max(resolutionText.contentWidth + 35, 122)
    property var multichannelCheckList: []
    property string fileDefaultName: focusedItemIsCamera ? focusedViewingGridItem.mediaName : ""
    property string folderDefaultName: "MultiChannelExport"

    onVisibleChanged: {        
        if(visible) {
            // 패스워드 설정 초기화
            exportVideoModel.usePassword = false;
            exportVideoModel.password = "";
            passwordBox.text = "";
            confirmPasswordBox.text = "";

            // 파일 이름 초기화
            if(singleMode)
                fileNamingTextField.textField.text = exportVideoModel.getAvailableFileName(fileDefaultName)
            else
                fileNamingTextField.textField.text = folderDefaultName

            // 타임라인 범위 선택 동기화
            timelineControlView.selectionStartTime = exportVideoModel.fromDateTime.getTime()
            timelineControlView.selectionEndTime = exportVideoModel.toDateTime.getTime()

            // 폴더 선택 초기화
            exportCombo.currentIndex = 0
        }

        if(multiModeOlny) {
            // multiModeOlny 인 경우 singleMode 비활성화
            singleMode = false
            singleText.enabled = false
        }
        else {
            singleMode = true
            singleText.enabled = true
        }

        // singleMode VideoSource Setup
        if(visible && !multiModeOlny && focusedViewingGridItem.hasPlayer) {
            mediaPlayer.setRefSourceType(focusedViewingGridItem.mediaSource);
        }
        else {
            mediaPlayer.setRefSourceType(null)
        }

        // multiMode playerList Setup
        multichannelCheckList = []
        if(visible) {
            var count = exportVideoModel.getMultiChannelCount()
            for(var i=0 ; i<count ; i++) {
                multichannelCheckList.push(true)
            }
            multiListRepeater.setModel(count)
        }
        else {
            exportVideoModel.setMultiChannelList([])
        }
    }

    onApplyButtonClicked: {
        if(fileNamingTextField.textField.text.length === 0) {
            if(singleMode)
                messageDialog.message = WisenetLinguist.pleaseEnterFileName
            else
                messageDialog.message = WisenetLinguist.pleaseEnterFolderName
            messageDialog.show()
            return
        }
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
            exportVideoModel.usePassword = true
            exportVideoModel.password = passwordBox.text
        }
        else {
            exportVideoModel.usePassword = false
            exportVideoModel.password = ""
        }

        var checkedAny
        var defaultFileName = ""

        // 파일 이름 설정
        if(singleMode) {
            checkedAny = true
            if (fileNamingTextField.textField.text == fileDefaultName) // 디폴트 이름을 사용할 경우
                exportVideoModel.fileName = focusedViewingGridItem.mediaName + "." + fileFormatRadioButton.fileExtension
            else // 다른 이름을 사용할 경우
                exportVideoModel.fileName = fileNamingTextField.textField.text + "." + fileFormatRadioButton.fileExtension
        }
        else {
            checkedAny = false
            for(var i=0 ; i<multichannelCheckList.length ; i++) {
                if(multichannelCheckList[i]) {
                    checkedAny = true
                    exportVideoModel.fileName = exportVideoModel.getChannelName(i) + "." + fileFormatRadioButton.fileExtension
                    break;
                }
            }
            if(fileNamingTextField.textField.text === folderDefaultName) // 디폴트 폴더 이름을 사용할 경우
                exportVideoModel.folderName = "MultiChannelExport"
            else
                exportVideoModel.folderName = fileNamingTextField.textField.text
        }

        if(!checkedAny) {
            exportPopupWindow.close()   // 멀티채널 백업에 체크 아이템이 없는 경우
        }
        else {
            if(exportCombo.currentIndex == 0) { // 디폴트 폴더가 선택되었을 경우
                exportVideoModel.filePath = localSettingViewModel.localRecordingPath
                exportVideo()
            }
            else { // 사용자가 choose folder later를 선택했을 경우
                exportFolderDialog.folder = exportVideoModel.getExportPathUrl(localSettingViewModel.localRecordingPath)
                exportFolderDialog.open()
            }
        }
    }

    onSingleModeChanged: {
        // 파일 이름 초기화
        if(singleMode)
            fileNamingTextField.textField.text = exportVideoModel.getAvailableFileName(fileDefaultName)
        else
            fileNamingTextField.textField.text = folderDefaultName
    }

    ExportVideoModel {
        id: exportVideoModel

        onFromDateTimeChanged: {
            fromDateEditor.setDate(fromDateTime)
            fromTimeEditor.setTimeText(fromDateTime.getHours(), fromDateTime.getMinutes(), fromDateTime.getSeconds())
        }

        onToDateTimeChanged: {
            toDateEditor.setDate(toDateTime)
            toTimeEditor.setTimeText(toDateTime.getHours(), toDateTime.getMinutes(), toDateTime.getSeconds())
        }
    }

    FileDialog {
        id: exportFolderDialog
        title: WisenetLinguist.exportVideo
        //acceptLabel: WisenetLinguist.exportButton
        //rejectLabel: WisenetLinguist.cancel
        selectFolder: true
        folder: shortcuts.documents// StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
        visible: false

        onAccepted: {
            exportVideoModel.filePath = filePathConveter.getPathByUrl(fileUrl) // 폴더 경로를 filePath로 넘김
            exportVideo()
        }
    }

    // Content Area
    Rectangle {
        anchors.fill: parent
        color: WisenetGui.transparent
        Rectangle {
            id: contentArea
            anchors.top: parent.top
            anchors.topMargin: 17
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 35
            anchors.right: parent.right
            anchors.rightMargin: 35
            color: WisenetGui.transparent

            component BaseText: Text{
                color: WisenetGui.contrast_04_light_grey
                height: 14
                font.pixelSize: 12
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            BaseText {
                id: singleText
                text: WisenetLinguist.single
                height: 16
                font.pixelSize: 12
                color: singleMode ? WisenetGui.contrast_00_white : WisenetGui.contrast_06_grey

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        singleMode = true
                    }
                }
            }

            Rectangle {
                id: modeSeparator
                width: 1
                height: 12
                y: 2
                anchors.left: singleText.right
                anchors.leftMargin: 20
                color: WisenetGui.contrast_08_dark_grey
            }

            BaseText {
                id: multiText
                text: WisenetLinguist.multi
                height: 16
                anchors.left: modeSeparator.right
                anchors.leftMargin: 20
                font.pixelSize: 12
                color: singleMode ? WisenetGui.contrast_06_grey : WisenetGui.contrast_00_white

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        singleMode = false
                    }
                }
            }

            Rectangle {
                id: leftArea
                color: WisenetGui.transparent
                anchors.top: singleText.bottom
                anchors.topMargin: 12
                anchors.left: parent.left
                anchors.right: rightArea.left
                anchors.rightMargin: 37
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 38

                Rectangle {
                    id: singleThumbnail
                    property double ratio: 9/16
                    width: parent.width; height: width * ratio

                    anchors.top: parent.top
                    visible: singleMode
                    color: WisenetGui.transparent
                    border.width: 1
                    border.color: WisenetGui.contrast_07_grey

                    WisenetMediaPlayer {
                        id: mediaPlayer
                    }

                    VideoOutput {
                        id: videoOutput
                        source: mediaPlayer
                        fillMode : VideoOutput.Stretch
                        width: parent.width
                        height: parent.height
                        orientation: 0;
                    }
                }

                Rectangle {
                    id: multiList
                    anchors.fill: parent
                    visible: !singleMode
                    color: WisenetGui.contrast_08_dark_grey

                    ListView {
                        id: multiListRepeater
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.topMargin: 12
                        anchors.rightMargin: 14
                        anchors.bottomMargin: 12
                        spacing: 15
                        model: 0
                        clip: true
                        boundsBehavior: Flickable.StopAtBounds
                        ScrollBar.vertical: WisenetScrollBar {}
                        delegate: WisenetCheckBox2 {
                            //anchors.verticalCenter: parent.verticalCenter
                            text: exportVideoModel.getChannelName(index)
                            checked: multichannelCheckList[index] !== undefined ? multichannelCheckList[index] : false
                            onCheckedChanged: {
                                multichannelCheckList[index] = checked
                            }
                        }

                        function setModel(count) {
                            model = 0
                            model = count
                        }
                    }
                }
            }

            Rectangle {
                id: rightArea
                color: WisenetGui.transparent
                anchors.top: singleText.bottom
                anchors.topMargin: 13
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: textColumnWidth + 228

                BaseText {
                    id: dateTimeText
                    text: WisenetLinguist.dateAndTime
                    width: textColumnWidth
                }

                BaseText {
                    id: fromText
                    text: WisenetLinguist.from
                    width: 50
                    anchors.topMargin: 32
                    anchors.rightMargin: 10
                    anchors.left: dateTimeText.right
                }

                BaseText {
                    id: toText
                    text: WisenetLinguist.to
                    width: 50
                    anchors.top: fromText.bottom
                    anchors.topMargin: 32
                    anchors.rightMargin: 10
                    anchors.left: fromText.left
                }

                BaseText {
                    id: resolutionText
                    text: WisenetLinguist.highResolution
                    width: textColumnWidth
                    anchors.top: dayTimeDivider.bottom
                    anchors.topMargin: 23
                }

                BaseText {
                    id: fileNamingText
                    text: singleMode? WisenetLinguist.fileNaming : WisenetLinguist.folderNaming
                    width: textColumnWidth
                    anchors.top: resolutionDevider.bottom
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

                Rectangle{
                    id: dayTimeDivider
                    width:350; height:1
                    color: WisenetGui.setupPageLine
                    anchors.top: toText.bottom
                    anchors.topMargin: 32
                }

                WisenetCheckBox2 {
                    id: resolutionCheckBox
                    text: WisenetLinguist.enable
                    anchors.top: resolutionText.top
                    anchors.topMargin: -1
                    anchors.left: resolutionText.right
                    checked: exportVideoModel.enableHighResolution
                    Binding { target: exportVideoModel; property: "enableHighResolution"; value: resolutionCheckBox.checked }
                }

                Rectangle{
                    id: resolutionDevider
                    width:350; height:1
                    color: WisenetGui.setupPageLine
                    anchors.top: resolutionText.bottom
                    anchors.topMargin: 23
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
                    checked: mkvRadioButton.checked? false : exportVideoModel.enableDigitalSignature
                    Binding { target: exportVideoModel; property: "enableDigitalSignature"; value: digitalSignatureCheckBox.checked }
                }

                WisenetCheckBox2 {
                    id: usePasswordCheckBox
                    text: WisenetLinguist.enable
                    enabled: wnmRadioButton.checked
                    anchors.top: usePasswordText.top
                    anchors.topMargin: -1
                    anchors.left: usePasswordText.right
                    checked: mkvRadioButton.checked? false : exportVideoModel.usePassword
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
                    model:[ WisenetLinguist.mediaFileFolder, WisenetLinguist.chooseFolder]
                }
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
        id: confirmDialog
        applyButtonVisible: true
        applyButtonText: WisenetLinguist.ok
        message: WisenetLinguist.exportFreespaceCheck
        onApplyButtonClicked: {
            if(singleMode) {
                if(focusedViewingGridItem.hasPlayer)
                    var overlappedIdList = graphAreaModel.getOverlappedIdList(timelineControlView.selectionStartTime, timelineControlView.selectionEndTime)
                    exportVideoModel.startSingleChannelExport(focusedViewingGridItem.player, overlappedIdList)
            }
            else {
                exportVideoModel.startMultichannelExport(multichannelCheckList)
            }

            exportPopupWindow.close()
            openExportProgressPopup()
        }
    }

    function updateFromDateTime() {
        exportVideoModel.fromDateTime = new Date(fromDateEditor.getDateText() + " " + fromTimeEditor.getTimeText())
        timelineControlView.selectionStartTime = exportVideoModel.fromDateTime.getTime()

        // 시간 역전이 발생 한 경우
        if(timelineControlView.selectionStartTime >timelineControlView.selectionEndTime) {
            timelineControlView.selectionEndTime = timelineControlView.selectionStartTime + 1000
            exportVideoModel.toDateTime = new Date(timelineControlView.selectionEndTime)
        }

        // 24시간 이상 범위를 설정 한 경우
        if(timelineControlView.selectionEndTime > timelineControlView.selectionStartTime + 24*3600000) {
            timelineControlView.selectionEndTime = timelineControlView.selectionStartTime + 24*3600000
            exportVideoModel.toDateTime = new Date(timelineControlView.selectionEndTime)
        }

        requestSeek(timelineControlView.selectionStartTime)
    }

    function updateToDateTime() {
        exportVideoModel.toDateTime = new Date(toDateEditor.getDateText() + " " + toTimeEditor.getTimeText())
        timelineControlView.selectionEndTime = exportVideoModel.toDateTime.getTime()

        // 시간 역전이 발생 한 경우
        if(timelineControlView.selectionStartTime >timelineControlView.selectionEndTime) {
            timelineControlView.selectionStartTime = timelineControlView.selectionEndTime - 1000
            exportVideoModel.fromDateTime = new Date(timelineControlView.selectionStartTime)
        }

        // 24시간 이상 범위를 설정 한 경우
        if(timelineControlView.selectionEndTime > timelineControlView.selectionStartTime + 24*3600000) {
            timelineControlView.selectionStartTime = timelineControlView.selectionEndTime - 24*3600000
            exportVideoModel.fromDateTime = new Date(timelineControlView.selectionStartTime)
        }

        requestSeek(timelineControlView.selectionEndTime)
    }

    function exportVideo() {
        var freeMbytes = exportVideoModel.getMbytesAvailable()
        console.log("exportFolderDialog.freeMbytes : ", freeMbytes)

        if(freeMbytes === -1) {
            messageDialog.message = WisenetLinguist.exportDiskError
            messageDialog.show()
            return
        }
        else if(freeMbytes < 50) {
            messageDialog.message = WisenetLinguist.exportDiskFull
            messageDialog.show()
            return
        }
        else if(freeMbytes < 2048) {
            confirmDialog.show()
            return
        }

        if(singleMode) {
            if(focusedViewingGridItem.hasPlayer) {
                var overlappedIdList = graphAreaModel.getOverlappedIdList(timelineControlView.selectionStartTime, timelineControlView.selectionEndTime)
                exportVideoModel.startSingleChannelExport(focusedViewingGridItem.player, overlappedIdList)
            }
        }
        else {
            exportVideoModel.startMultichannelExport(multichannelCheckList)
        }

        exportPopupWindow.close()
        openExportProgressPopup()
    }
}
