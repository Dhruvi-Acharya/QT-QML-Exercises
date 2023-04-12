import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

WisenetPopupWindowBase{
    id: webpageAddView
    width: 550
    height: 422
    minimumWidth: 550
    minimumHeight: 422
    title: WisenetLinguist.newWebpage
    applyButtonText: WisenetLinguist.ok
    resizeEnabled: false
    minmaxButtonVisible: false

    onApplyButtonClicked: {

        if(webpageNameEditBox.text === ""){
            errorDialog.message = WisenetLinguist.enterNameMessage
            errorDialog.visible = true
            return;
        }

        if(urlEditBox.text === ""){
            errorDialog.message = WisenetLinguist.pleaseCheckAddress
            errorDialog.visible = true
            return;
        }

        if(digestEnableCheckBox.checked=== true){
            if(idTextBox.text === ""){
                errorDialog.message = WisenetLinguist.inputId
                errorDialog.visible = true
                return;
            }

            if(passwordBox.text === ""){
                errorDialog.message = WisenetLinguist.passwordWarningEmpty
                errorDialog.visible = true
                return;
            }
        }

        webpageAddViewModel.webpageName = webpageNameEditBox.text
        webpageAddViewModel.url = urlEditBox.text
        webpageAddViewModel.useAuth = digestEnableCheckBox.checked
        webpageAddViewModel.userId = idTextBox.text
        webpageAddViewModel.userPassword = passwordBox.text
        webpageAddViewModel.saveWebpage()
        close()
    }

    WisenetMessageDialog{
        id: errorDialog

        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    Rectangle{
        anchors.fill : parent
        color: "transparent"

        WisenetText{
            id: nameText
            text: WisenetLinguist.name

            height: 14
            width: digestText.width

            anchors.top: parent.top
            anchors.topMargin: 27
            anchors.left: parent.left
            anchors.leftMargin: 36
        }

        WisenetText{
            id: urlText

            text: "URL"

            height: 14
            width: digestText.width

            anchors.top: nameText.bottom
            anchors.topMargin: 29
            anchors.left: parent.left
            anchors.leftMargin: 36
        }


        WisenetText{
            id: digestText

            text: WisenetLinguist.digestAuthorization

            height: 14

            anchors.top: urlText.bottom
            anchors.topMargin: 29
            anchors.left: parent.left
            anchors.leftMargin: 36
        }


        WisenetTextBox{
            id: webpageNameEditBox

            anchors.top: parent.top
            anchors.topMargin: 27
            anchors.left: nameText.right
            anchors.leftMargin: 45
            anchors.right: parent.right
            anchors.rightMargin: 67

            placeholderText: WisenetLinguist.enterNameMessage
            text: webpageAddViewModel.webpageName
        }

        WisenetTextBox{
            id: urlEditBox

            anchors.top: webpageNameEditBox.bottom
            anchors.topMargin: 26
            anchors.left: urlText.right
            anchors.leftMargin: 45
            anchors.right: parent.right
            anchors.rightMargin: 67

            placeholderText: "http://"
            text: webpageAddViewModel.url
        }

        WisenetCheckBox{
            id: digestEnableCheckBox

            anchors.top: urlEditBox.bottom
            anchors.topMargin: 27
            anchors.left: digestText.right
            anchors.leftMargin: 45
            anchors.right: parent.right
            anchors.rightMargin: 67
            text: WisenetLinguist.enable

            checked: webpageAddViewModel.useAuth

            onCheckedChanged: {
                if(checked == false){
                    idTextBox.text = ""
                    passwordBox.text = ""
                }
            }
        }

        WisenetText{
            id: idText

            width: 80
            height: 14

            anchors.top: digestEnableCheckBox.bottom
            anchors.topMargin: 29
            anchors.left: digestText.right
            anchors.leftMargin: 45

            text: "ID"
        }


        WisenetText{
            id: passwordText

            width: 80
            height: 14

            anchors.top: idText.bottom
            anchors.topMargin: 26
            anchors.left: digestText.right
            anchors.leftMargin: 45

            text: WisenetLinguist.password
        }

        WisenetTextBox{
            id: idTextBox

            anchors.top: digestEnableCheckBox.bottom
            anchors.topMargin: 31
            anchors.left: idText.right
            anchors.leftMargin: 23
            anchors.right: parent.right
            anchors.rightMargin: 67

            enabled: digestEnableCheckBox.checked

            text: webpageAddViewModel.userId
        }

        WisenetPasswordBox{
            id: passwordBox

            anchors.top: idTextBox.bottom
            anchors.topMargin: 23
            anchors.left: passwordText.right
            anchors.leftMargin: 23
            anchors.right: parent.right
            anchors.rightMargin: 67

            enabled: digestEnableCheckBox.checked
            showPasswordButtonVisible: false

            text: webpageAddViewModel.userPassword
        }
    }

    onVisibleChanged: {
        if(visible === true)
        {
            webpageNameEditBox.text = webpageAddViewModel.webpageName
            urlEditBox.text = webpageAddViewModel.url
            digestEnableCheckBox.checked = webpageAddViewModel.useAuth
            idTextBox.text = webpageAddViewModel.userId
            passwordBox.text = ""
        }
    }
}
