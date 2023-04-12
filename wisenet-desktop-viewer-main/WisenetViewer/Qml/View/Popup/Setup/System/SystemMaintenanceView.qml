import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Qt.labs.platform 1.1
import QtQuick.Dialogs 1.3
import "qrc:/"

Page{
    id : root

    property var isS1Support: versionManager.s1Support ? true : false

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    function checkUpdate(){
        console.log("systemMaintenanceViewModel::checkUpdate")
        systemMaintenanceViewModel.checkUpdate();
    }


    onVisibleChanged: {
        if(visible) checkUpdate();
    }

        Connections{
        target: systemMaintenanceViewModel
        function onShowMessageBox(title, text, quit, fileName){
            if(quit){
                console.log("[SystemMaintenanceView] quit true")
                quitMsgDialog.title = title;
                quitMsgDialog.message = title + " " + text;
                quitMsgDialog.visible = true;
            }
            else{
                console.log("[SystemMaintenanceView] quit false")
                normalMsgDialog.title = title;
                normalMsgDialog.message = title + " " + text + "\n" + fileName;
                normalMsgDialog.visible = true;
            }
        }

        function onUpdateresultChanged(updateresult) {
            if(!updateresult)
            {
                msgDialog.visible = true
            }
        }
   }


    WisenetMessageDialog{
        id: quitMsgDialog
        message: "Cannot confirm"
        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        onClosing: {
            console.log("quit message dialog.")
            systemMaintenanceViewModel.restartApplication()
        }
        onVisibleChanged: {
            if(visible){
                console.log("[SystemMaintenanceView] quitMsgDialog visible")
            }
        }
    }
    WisenetMessageDialog{
        id: normalMsgDialog
        message: "Cannot confirm"
        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        onApplyButtonClicked: {
            console.log("quit message dialog.")
        }
    }

    WisenetMessageDialog{
        id: msgDialog
        message: WisenetLinguist.downloadErrorMessage
        cancelButtonText: WisenetLinguist.close
        applyButtonVisible: false
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
        property int updateHeight : 2

        WisenetDivider{
            label: WisenetLinguist.maintenance
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: currentVersionLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.currentVersion
                wrapMode: Text.Wrap
            }
            WisenetLabel {
                id: currentVersionValue
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: systemMaintenanceViewModel ? Qt.application.version + ", " + (systemMaintenanceViewModel.buildDate ? systemMaintenanceViewModel.buildDate : "") : ""
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            visible: Qt.platform.os !== "linux"
            WisenetLabel {
                id: latestVersionLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.latestVersion
                wrapMode: Text.Wrap
            }
            WisenetLabel {
                id: latestVersionValue
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: systemMaintenanceViewModel ? systemMaintenanceViewModel.latestversion : ""
            }
            WisenetColorLabel {
                id: updateButton
                text: WisenetLinguist.update
                width: 49
                height: mainLayout.defaultHeight
                color: {
                    if (systemMaintenanceViewModel && systemMaintenanceViewModel.needUpdate) {
                        updatemouseArea.containsMouse ? (updatemouseArea.pressed ? WisenetGui.color_secondary_dark : WisenetGui.color_secondary_bright)
                                                                   : WisenetGui.color_secondary
                    }
                    else
                    {
                        WisenetGui.color_secondary_dark
                    }
                }
                imageSource: {
                    if (systemMaintenanceViewModel && systemMaintenanceViewModel.needUpdate) {
                        updatemouseArea.containsMouse ? (updatemouseArea.pressed ? WisenetImage.textLink_press : WisenetImage.textLink_hover)
                                                                    : WisenetImage.textLink
                    }
                    else
                    {
                        WisenetImage.textLink_press
                    }
                }
                MouseArea{
                    id : updatemouseArea
                    acceptedButtons: Qt.LeftButton
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (systemMaintenanceViewModel.needUpdate) {
                            console.log("needUpdate")
//                            s1DotTimer.restart()
                            systemMaintenanceViewModel.startUpdate()
                        }
                        else
                        {
                            console.log("Don't needUpdate")
                        }
                    }
                }
            }
            UpdateProgress{
                visible: !isS1Support
                value: systemMaintenanceViewModel ? systemMaintenanceViewModel.updateprogress : 0
            }

            WisenetLabel {
                id: downloadingLabel
                width: 35
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                visible: isS1Support
                text: systemMaintenanceViewModel.updateprogress > 0 ? WisenetLinguist.downloading : ""
            }

            Item {
                id : downloadIndicator
                width: 30
                height: 30
                visible: isS1Support

                // 둥근 로딩 이미지
                WisenetMediaBusyIndicator {
                    id: chaningIndicator
                    anchors.centerIn: parent
                    width: 30
                    height: 30
                    running:systemMaintenanceViewModel.updateprogress > 0
                }
            }

            // 점 연속 찍힘
//            WisenetLabel {
//                id: dotLabel
//                width: 54
//                height: mainLayout.defaultHeight
//                verticalAlignment: Text.AlignVCenter
//                visible: isS1Support
//                text: systemMaintenanceViewModel.updateprogress > 0 ? "." : ""
//            }
        }
        Row{
            visible: Qt.platform.os !== "linux"
            Label {
                id: restartLabel
                width: 500
                height: mainLayout.updateHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.restartMessage
                color: WisenetGui.contrast_04_light_grey
                font.pixelSize: 12
                leftPadding: 205
                visible: {
                    if (systemMaintenanceViewModel && systemMaintenanceViewModel.needUpdate) {
                        true
                    }
                    else
                    {
                        false
                    }
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: debugLogLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.log
            }
            WisenetColorLabel {
                id: logOpenButton
                text: WisenetLinguist.open
                width: 49
                height: mainLayout.defaultHeight
                onClicked: {
                    console.log("Click logOpen")
                    systemMaintenanceViewModel.openLogDir()
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                id: backupLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text:WisenetLinguist.backup
            }
            WisenetColorLabel {
                id: backupButton
                width: 97
                height: mainLayout.defaultHeight
                text: WisenetLinguist.backup
                onClicked: {
                    console.log("Click backupButton")
                    backupFolderDialog.visible = true
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: restoreLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.restore
            }
            WisenetColorLabel {
                id: restoreButton
                text: WisenetLinguist.restore
                width: 132
                height: mainLayout.defaultHeight
                onClicked: {
                    console.log("Click restoreButton")
                    restoreFileDialog.visible = true
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: resetLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                text: WisenetLinguist.reset
            }
            WisenetColorLabel {
                id: resetButton
                width: 130
                height: mainLayout.defaultHeight
                text: WisenetLinguist.reset
                onClicked: {
                    console.log("Click resetButton")
                    if (!deleteDialog.dontAskAgain) {
                        deleteDialog.open();
                    }
                    else {
                        systemMaintenanceViewModel.startReset()
                    }
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: opensourceLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.opensourceLicense
                wrapMode: Text.Wrap
            }
            WisenetColorLabel {
                id: opensourceButton
                text: WisenetLinguist.view
                width: 49
                height: mainLayout.defaultHeight
                onClicked: {
                    console.log("Click opensource")
                    systemMaintenanceViewModel.openSourceLicense()
                }
            }
        }
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            visible: Qt.platform.os === "windows"

            WisenetLabel {
                id: licenseLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.license
                wrapMode: Text.Wrap
            }

            WisenetColorLabel {
                id: licenseApplyButton
                text: WisenetLinguist.activate
                width: 49
                height: mainLayout.defaultHeight

                visible: !systemMaintenanceViewModel.licenseActivated

                onClicked: {
                    licenseDialog.open()
                }
            }

            WisenetLabel {
                id: licenseActivatedLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                verticalAlignment: Text.AlignVCenter
                text: WisenetLinguist.activated
                wrapMode: Text.Wrap

                visible: systemMaintenanceViewModel.licenseActivated
            }
        }

        WisenetSetupAskDialog {
            id: deleteDialog
            msg : WisenetLinguist.resetConfirmMessage
            dontAskVisible : false
            onAccepted: {
                 systemMaintenanceViewModel.startReset()
            }
            onRejected: {
                dontAskAgain = false
            }
        }

        FileDialog {
            id: backupFolderDialog
            visible: false
            //flags: FolderDialog.ShowDirsOnly
            folder: shortcuts.documents
            selectFolder: true
            onAccepted: {
                console.log("system backup " + fileUrl)
                systemMaintenanceViewModel.startBackup(filePathConveter.getPathByUrl(fileUrl))
            }

        }
        FileDialog {
            id: restoreFileDialog
            title: "Please choose a file"
            nameFilters: [ "Upload files (*.wdb)"]
            folder: shortcuts.home
            onAccepted: {
                console.log("You choose: " + restoreFileDialog.fileUrl + " " + Qt.platform.os.toString())

                systemMaintenanceViewModel.startRestore(filePathConveter.getPathByUrl(restoreFileDialog.fileUrl))
            }
            onRejected: {
                console.log("Canceled")
            }
        }

        LicenseDialog{
            id: licenseDialog

            applyEnabled: systemMaintenanceViewModel.isValidLicense

            onAccepted: {
                systemMaintenanceViewModel.applyLicense()
            }
            onRejected: {
                deleteDialog.close()
            }
        }
    }

    // 500ms 마다 로딩 점 연속 찍힘(. , .. , ... 전환)
//    Timer {
//        id: s1DotTimer
//        interval: 500
//        repeat: true
//        onTriggered: {
//            if(dotLabel.text === ".") {
//                dotLabel.text = ".."
//            }
//            else if(dotLabel.text === "..") {
//                dotLabel.text = "..."
//            }
//            else if(dotLabel.text === "...") {
//                dotLabel.text = "."
//            }
//        }
//    }
}

