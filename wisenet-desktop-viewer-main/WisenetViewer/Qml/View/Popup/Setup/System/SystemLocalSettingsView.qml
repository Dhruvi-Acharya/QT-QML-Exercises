import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import QtQuick.Dialogs 1.3
import "qrc:/"

Page{
    id : root
    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    Component.onCompleted: {
        autoStartRow.visible = localSettingViewModel.isWindows()
    }

    onVisibleChanged: {
        if(visible){
            resetLocalSetting()
        }
    }

    function resetLocalSetting(){
        languageCombo.currentIndex = languageManager.getLanguageEnum(localSettingViewModel.language)

        liveBufferLengthCombo.currentIndex = liveBufferLengthCombo.indexOfValue(localSettingViewModel.liveBufferLength)

        localRecordingPathText.text = localSettingViewModel.localRecordingPath
        console.log("LocalSetting:", folderDialog.folder, localRecordingPathText.text , localSettingViewModel.localRecordingPath);

        localRecordingTimeCombo.currentIndex = localRecordingTimeCombo.indexOfValue(localSettingViewModel.localRecordingTime)
        localRecordingFormatCombo.currentIndex = localSettingViewModel.localRecordingFormat
        autoStartCheck.checked = localSettingViewModel.autoStart
        useHwDecodingCheckBox.checked = localSettingViewModel.useHwDecoding
        useControllerCheckBox.checked = localSettingViewModel.useController
    }

    Connections{
        target: localSettingViewModel
        function onShowMessageBox( text, quit){
            console.log("LocalSettingViewModel::onShowMessageBox() quit:", title, text, quit);
            quitMsgDialog.message = text;
            quitMsgDialog.isQuit = quit
            quitMsgDialog.visible = true;
        }
    }

    Connections {
        target: gControllerManager

        function onControllerConnectStatusChanged() {
            console.log("SystemLocalSetting onControllerConnectStatusChanged - controllerConnectNotification.showMessage()")
            controllerConnectNotification.showMessage(WisenetLinguist.controllerConnected)
        }
    }

    NotificationMessage {
        id: controllerConnectNotification

        anchors {
            top: mainLayout.top
            right: mainLayout.right
            margins: 8
        }
    }

    WisenetMessageDialog{
        id: quitMsgDialog
        message: "Cannot confirm"
        property bool isQuit: false
        visible: false
        applyButtonVisible : false
        cancelButtonText: WisenetLinguist.close
        onClosing: {
            console.log("quit message onclosing." + isQuit)
            if(isQuit  == true){
                console.log("quit message dialog.")
                systemMaintenanceViewModel.restartApplication()
            }
        }
    }

    Column{
        id: mainLayout
        anchors.fill: parent
        spacing: 30
        leftPadding: 24
        rightPadding: 24
        topPadding: 20

        property int defaultWidth : 175
        property int defaultHeight: 30
        property int defaultSpacing : 30
        property int defaultMargin : 20


        WisenetDivider{
            label: WisenetLinguist.localSettings
        }

        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.language
                wrapMode: Text.Wrap
            }


            WisenetComboBox{
                id : languageCombo
                anchors.margins: 20
                anchors.verticalCenter: parent.verticalCenter
                //currentIndex: languageManager.language
                model: [
                    WisenetLinguist.korean,
                    WisenetLinguist.english + "-USA",
                    WisenetLinguist.english + "-UK",
                    WisenetLinguist.chinese,
                    WisenetLinguist.croatian,
                    WisenetLinguist.czech,
                    WisenetLinguist.danish,
                    WisenetLinguist.dutch,
                    WisenetLinguist.finnish,
                    WisenetLinguist.french,
                    WisenetLinguist.german,
                    WisenetLinguist.greek,
                    WisenetLinguist.hungarian,
                    WisenetLinguist.italian,
                    WisenetLinguist.japanese,
                    WisenetLinguist.norwegian,
                    WisenetLinguist.polish,
                    WisenetLinguist.portuguese,
                    WisenetLinguist.romanian,
                    WisenetLinguist.russian,
                    WisenetLinguist.serbian,
                    WisenetLinguist.spanish,
                    WisenetLinguist.taiwanese,
                    WisenetLinguist.turkish]
            }
        }

        Row{
            id: autoStartRow
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.autoRun
            }
            WisenetCheckBox{
                id: autoStartCheck
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.maximumLiveBufferLength
                wrapMode: Text.Wrap
            }
            Item {
                width: liveBufferLengthCombo.width + 20
                height: liveBufferLengthCombo.height
                WisenetComboBox{
                    id: liveBufferLengthCombo
                    anchors.margins: 20
                    anchors.verticalCenter: parent.verticalCenter
                    model: ["0","100","200", "300", "400", "500"]
                }
                WisenetLabel{
                    text : "ms"
                    anchors.left: liveBufferLengthCombo.right
                    anchors.leftMargin: 10
                }
            }
        }


        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                id: localRecordingLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.mediaFileFolder
                wrapMode: Text.Wrap
            }

            Item {
                width: localRecordingPathText.width + 20
                height: localRecordingPathText.height
                WisenetTextBox{
                    id : localRecordingPathText
                    border.width: 1
                    border.color: WisenetGui.contrast_06_grey
                    width: 403
                    height: mainLayout.defaultHeight
                    readOnly: true
                }
                WisenetColorLabel {
                    id : localRecordingButton
                    text: WisenetLinguist.change
                    height: mainLayout.defaultHeight
                    anchors.left: localRecordingPathText.right
                    anchors.leftMargin: 10
                    onClicked: {
                        folderDialog.folder = filePathConveter.getUrlNameByFileName(localSettingViewModel.localRecordingPath)
                        console.log("LocalSetting change click:", folderDialog.folder)
                        folderDialog.visible = true
                    }
                }

                FileDialog {
                    id: folderDialog
                    visible: false
                    //flags: FolderDialog.ShowDirsOnly
                    selectFolder: true
                    folder: localSettingViewModel.localRecordingPath
                    onAccepted: {
                        localRecordingPathText.text = filePathConveter.getPathByUrl(fileUrl)
                    }
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.maximumRecordingTime
            }
            Item {
                width: localRecordingTimeCombo.width + 20
                height: localRecordingTimeCombo.height
                WisenetComboBox{
                    id: localRecordingTimeCombo
                    anchors.margins: 20
                    anchors.verticalCenter: parent.verticalCenter
                    model: [5,10,20,30,40,50,60,70,80,90,100,110,120]
                    displayText: currentValue
                }
                WisenetLabel{
                    text : WisenetLinguist.minute
                    anchors.left: localRecordingTimeCombo.right
                    anchors.leftMargin: 10
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.recordingFileType
            }
            WisenetComboBox{
                id: localRecordingFormatCombo
                anchors.margins: 20
                anchors.verticalCenter: parent.verticalCenter
                model: [ "Matroska (*.mkv)", "Wisenet Media (*.wnm)" ]
            }
        }

        Row{
            id: hwDecodingRow
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            visible: decoderManager.supportsHardwareDecoding()

            WisenetLabel {
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.useHardwareDecoding
            }
            WisenetCheckBox{
                id: useHwDecodingCheckBox
                anchors.verticalCenter: parent.verticalCenter

                onCheckedChanged: {
                    console.log("supportsHardwareDecoding", decoderManager.supportsHardwareDecoding())
                }
            }
        }

        Row{
            id: controllerRow
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            visible: Qt.platform.os !== "osx"

            WisenetLabel {
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.enableController + " (SPC-2000/2001)"
            }
            WisenetCheckBox{
                id: useControllerCheckBox
                anchors.verticalCenter: parent.verticalCenter

                onCheckedChanged: {
                    console.log("useControllerCheckBox.checked=",useControllerCheckBox.checked)

//                    console.log("supportsHardwareDecoding", decoderManager.supportsHardwareDecoding())
                }
            }
        }
    }

    function saveLocalSetting(){
        localSettingViewModel.language = languageManager.getLanguageString(languageCombo.currentIndex)
        localSettingViewModel.localRecordingPath = localRecordingPathText.text
        localSettingViewModel.localRecordingTime = localRecordingTimeCombo.currentValue
        localSettingViewModel.localRecordingFormat = localRecordingFormatCombo.currentIndex

        localSettingViewModel.liveBufferLength = liveBufferLengthCombo.currentValue
        localSettingViewModel.autoStart = autoStartCheck.checked
        localSettingViewModel.useHwDecoding = useHwDecodingCheckBox.checked
        localSettingViewModel.useController = useControllerCheckBox.checked
        localSettingViewModel.saveSetting()
    }

    WisenetSetupAskDialog {
        id: deleteDialog
        msg : WisenetLinguist.languageChangeConfirmMessage
        dontAskVisible : false
        onAccepted: {
            saveLocalSetting()
        }
        onRejected: {
            dontAskAgain = false
        }
    }

    footer:
        WisenetDialogButtonBox {
        onAccepted: {
            console.log("[SystemLocalSettingView] select language : " + languageCombo.currentText + " / " + localSettingViewModel.language)
            if(languageCombo.currentIndex !== languageManager.getLanguageEnum(localSettingViewModel.language))
                deleteDialog.open();
            else
                saveLocalSetting();
        }
    }
}

