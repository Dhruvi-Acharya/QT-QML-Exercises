import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Qt.labs.platform 1.1
import QtQuick.Dialogs 1.3
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Setup 1.0

Page{
    id : root
    property int xMargin : 24
    property int yMargin: 20

    // result1
    property bool saveResult: false;
    // result2
    property int sendResult: 0
    // test email - subject
    property string testEmailSubject: "Test E-mail"
    // test email - part
    property string testEmailPart: "This is a test E-mail from Wisenet Viewer."

    onVisibleChanged: {
        console.log("[eventEmailViewModel] - onVisibleChanged (1)")
        eventEmailViewModel.readEmail()

        serverAddrTextBox.text = eventEmailViewModel.serverAddress;
        authCheckBox.checked = eventEmailViewModel.authentication ? true : false
        idTextBox.text = eventEmailViewModel.id
        pwdTextBox.text = eventEmailViewModel.password
        portTextBox.text = eventEmailViewModel.port.toString()
        senderTextBox.text = eventEmailViewModel.sender
        sendingTestTextBox.text = eventEmailViewModel.receiver;

        console.log("[eventEmailViewModel] - onVisibleChanged (2)")
    }

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    EventEmailViewModel{
        id : eventEmailViewModel
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
            label: WisenetLinguist.emailPageSubject
        }

        // Server Address
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: serverAddrLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                visible: true
                text: WisenetLinguist.emailServerAddress
                wrapMode: Text.Wrap
            }
            WisenetTextBox {
                id: serverAddrTextBox
                width: 351
                height: 23
                Layout.alignment: Qt.AlignVCenter
            }
        }

        // Authentication
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: authLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                visible: true
                text: WisenetLinguist.emailAuthentication
                wrapMode: Text.Wrap
            }
            WisenetCheckBox {
                id: authCheckBox
                width: mainLayout.defaultWidth
                height: 15
                text: WisenetLinguist.enable
                Layout.alignment: Qt.AlignVCenter
                onCheckedChanged: {
                    if (checked == false){
                        idTextBox.text = ""
                        pwdTextBox.text = ""
                        idTextBox.enabled = false
                        pwdTextBox.enabled = false
                    }
                    else {
                        idTextBox.text = eventEmailViewModel.id
                        pwdTextBox.text = eventEmailViewModel.password
                        idTextBox.enabled = true
                        pwdTextBox.enabled = true
                    }
                }
            }
        }

        // ID
        Row{
            x: 205
            width: 500
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: idLabel
                width: mainLayout.defaultWidth - 50
                height: mainLayout.defaultHeight
                visible: true
                text: WisenetLinguist.id_
                wrapMode: Text.Wrap
            }
            WisenetTextBox {
                id: idTextBox
                width: 228
                height: 23
                Layout.alignment: Qt.AlignVCenter
            }
        }

        // Password
        Row{
            x: 205
            width: 500
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: pwdLabel
                width: mainLayout.defaultWidth - 50
                height: mainLayout.defaultHeight
                visible: true
                text: WisenetLinguist.password
                wrapMode: Text.Wrap
            }
            WisenetPasswordBox {
                id: pwdTextBox
                width: 228
                height: 23
                Layout.alignment: Qt.AlignLeft
            }
        }

        // Port
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargi
            WisenetLabel {
                id: portLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                visible: true
                text: WisenetLinguist.port
                wrapMode: Text.Wrap
            }
            WisenetTextBox {
                id: portTextBox
                width: 351
                height: 23
                Layout.alignment: Qt.AlignVCenter
            }
        }

        // Sender
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin
            WisenetLabel {
                id: senderLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                visible: true
                text: WisenetLinguist.emailSender
                wrapMode: Text.Wrap
            }
            WisenetTextBox {
                id: senderTextBox
                width: 351
                height: 23
                Layout.alignment: Qt.AlignVCenter
                placeholderText: "user@example.com"
            }
        }

        // Sending test
        Row{
            spacing: mainLayout.defaultSpacing
            Layout.leftMargin: mainLayout.defaultMargin

            WisenetLabel {
                id: sendingTestLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                visible: true
                text: WisenetLinguist.emailSendingTest
                wrapMode: Text.Wrap
            }

            WisenetTextBox {
                id: sendingTestTextBox
                width: 351
                height: 23
                Layout.alignment: Qt.AlignVCenter
                placeholderText: "user@example.com"
            }

            WisenetButton2 {
                id: sendingTestButton
                width: 66
                height: 28
                Layout.alignment: Qt.AlignVCenter
                text: WisenetLinguist.emailTest
                onClicked: {
                    console.log("[eventEmailViewModel] - sendEmail (1)")

                    // send
                    sendingTestResultMessageLabel.visible = false
                    sendResult = eventEmailViewModel.sendTestEmail(serverAddrTextBox.text, portTextBox.text, idTextBox.text, pwdTextBox.text,  senderTextBox.text, sendingTestTextBox.text, testEmailSubject, testEmailPart, authCheckBox.checked);
                    sendingTestResultMessageLabel.visible = true

                    // result
                    switch (sendResult)
                    {
                    case -1: // host connect fail
                        sendingTestResultMessageLabel.text = WisenetLinguist.emailTestFail1
                        break;
                    case -2: // host login fail
                        sendingTestResultMessageLabel.text = WisenetLinguist.emailTestFail2
                        break;
                    case -3: // e-mail send fail
                        sendingTestResultMessageLabel.text = WisenetLinguist.emailTestFail3
                        break;
                    case -4: // Database insert fail
                        sendingTestResultMessageLabel.text = WisenetLinguist.emailTestFail4
                        break;
                    case 1:  // e-mail send success
                    default:
                        sendingTestResultMessageLabel.text = WisenetLinguist.emailTestSuccess
                        break;
                    }

                    console.log("[eventEmailViewModel] - sendEmail (2)")
                }
            }

            WisenetLabel {
                id: sendingTestResultMessageLabel
                width: mainLayout.defaultWidth
                height: mainLayout.defaultHeight
                visible: false
                text: ""
                wrapMode: Text.Wrap
            }
        }
    }

    footer:
        WisenetDialogButtonBox {
        onAccepted: {
            // exception
            if(serverAddrTextBox.text === ""){
                errorDialog.message = WisenetLinguist.pleaseCheckAddress
                errorDialog.visible = true
                return;
            }

            if (authCheckBox.checked) {
                if(idTextBox.text === "" || pwdTextBox.text === ""){
                    errorDialog.message = WisenetLinguist.pleaseCheckIdAndPassword
                    errorDialog.visible = true
                    return;
                }
            }

            if(portTextBox.text === ""){
                errorDialog.message = WisenetLinguist.pleaseCheckPort
                errorDialog.visible = true
                return;
            }

            if(senderTextBox.text === ""){
                errorDialog.message = WisenetLinguist.pleaseCheckSender
                errorDialog.visible = true
                return;
            }

            // save
            console.log("[eventEmailViewModel] - saveEmail (1)")
            eventEmailViewModel.serverAddress = serverAddrTextBox.text
            eventEmailViewModel.authentication = authCheckBox.checked ? "1" : "0"
            eventEmailViewModel.id = idTextBox.text
            eventEmailViewModel.password = pwdTextBox.text
            eventEmailViewModel.port = portTextBox.text
            eventEmailViewModel.sender = senderTextBox.text
            eventEmailViewModel.receiver = sendingTestTextBox.text
            console.log("[eventEmailViewModel] - ", eventEmailViewModel.serverAddress)
            console.log("[eventEmailViewModel] - ", eventEmailViewModel.authentication)
            console.log("[eventEmailViewModel] - ", eventEmailViewModel.id)
            console.log("[eventEmailViewModel] - ", eventEmailViewModel.password)
            console.log("[eventEmailViewModel] - ", eventEmailViewModel.port)
            console.log("[eventEmailViewModel] - ", eventEmailViewModel.sender)
            console.log("[eventEmailViewModel] - ", eventEmailViewModel.receiver)
            saveResult = eventEmailViewModel.saveEmail();
            console.log("[eventEmailViewModel] - saveEmail (2)")
        }
    }

    WisenetMessageDialog {
        id: errorDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        message: WisenetLinguist.nameCannotBlank
    }
}
