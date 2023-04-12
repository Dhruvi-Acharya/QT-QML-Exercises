import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0

WisenetMessageDialog{
    id : root
    applyClose: false
    height: 394
    width: 452
    applyButtonText: WisenetLinguist.register
    property int _topLabelMargin : 30
    property int _topMargin : 27
    property int _leftMargin: 40
    property int _rightMargin: 67
    onVisibleChanged: {
        if(visible){
            savePassword.checked = localSettingViewModel.autoRegister
            if(savePassword.checked){
                inputDeviceID.text = localSettingViewModel.autoRegisterId
                inputDevicePassword.text = localSettingViewModel.autoRegisterPassword
            }
            else
            {
                inputDeviceID.text = ""
                inputDevicePassword.text = ""
            }

            macAddressTextBox.clearText()
            serialTextBox.text = ""
        }
    }

    onApplyButtonClicked: {
        if(inputDevicePassword.text.length < 1 || inputDeviceID.text.length < 1){
            errorDialog.message = WisenetLinguist.pleaseCheckIdAndPassword
            errorDialog.show()
        }
        else if(macAddressRadioButton.checked && macAddressTextBox.macAddress.length < 12){
            errorDialog.message = WisenetLinguist.pleaseCheckProductId
            errorDialog.show()
        }
        else if(!macAddressRadioButton.checked && serialTextBox.text.length < 1)
        {
            errorDialog.message = WisenetLinguist.pleaseCheckProductId
            errorDialog.show()
        }
        else{
            var s1Id
            if(macAddressRadioButton.checked)
                s1Id = macAddressTextBox.macAddress
            else
                s1Id = serialTextBox.text

            localSettingViewModel.saveDeviceCredential(savePassword.checked, inputDeviceID.text, inputDevicePassword.text)
            manualDiscoveryViewModel.startS1Register(s1Id, inputDeviceID.text, inputDevicePassword.text, s1SipRadioButton.checked)
            root.close()
        }
    }
    WisenetMessageDialog{
        id  : errorDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    Rectangle{
        id : rect
        anchors.fill: parent
        anchors.leftMargin: 50
        anchors.rightMargin: 50
        color: "transparent"

        WisenetText{
            id: ipTypeText
            text: WisenetLinguist.ipType
            width: 120
            height: 14
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.left: parent.left
        }

        Item{
            id: typeRadioButtons
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.left: ipTypeText.right
            anchors.leftMargin: 2
            anchors.right: parent.right
            height: 14

            WisenetRadioButton{
                id: s1DdnsRadioButton
                checked: true

                anchors.top: parent.top
                anchors.left: parent.left
                width: 97

                text: "S1 DDNS"
            }

            WisenetRadioButton{
                id: s1SipRadioButton
                checked: false
                anchors.top: parent.top
                anchors.left: s1DdnsRadioButton.right
                width: 97
                text: "S1 SIP"
            }
        }

        WisenetText{
            id: idText
            text: WisenetLinguist.id_
            width: ipTypeText.width
            height: 14
            anchors.top: ipTypeText.bottom
            anchors.topMargin: _topLabelMargin
            anchors.left: parent.left
        }

        WisenetTextBox{
            id : inputDeviceID
            anchors.top: ipTypeText.bottom
            anchors.topMargin: 29
            anchors.left: ipTypeText.right
            anchors.leftMargin: 2
            anchors.right: parent.right
            placeholderText: WisenetLinguist.id_
        }

        WisenetText{
            id: passwordText
            text: WisenetLinguist.password
            width: ipTypeText.width
            height: 14
            anchors.top: idText.bottom
            anchors.topMargin: _topLabelMargin
            anchors.left: parent.left
        }

        WisenetPasswordBox{
            id : inputDevicePassword
            anchors.top: inputDeviceID.bottom
            anchors.topMargin: _topMargin
            anchors.left: ipTypeText.right
            anchors.leftMargin: 2
            anchors.right: parent.right
            placeholderText: WisenetLinguist.password
            showPasswordButtonVisible: false
        }
        WisenetCheckBox2{
            id: savePassword
            checked: localSettingViewModel.autoRegister
            text: WisenetLinguist.saveIdPassword
            anchors.top: inputDevicePassword.bottom
            anchors.topMargin: _topMargin
            anchors.left: ipTypeText.right
            anchors.leftMargin: 2
            anchors.rightMargin: 10
            width: 170
            textWrapMode: Text.Wrap
        }
        WisenetSaveCheckQuestionButton{
            anchors.top: inputDevicePassword.bottom
            anchors.topMargin: _topMargin
            anchors.left: savePassword.right
        }

        Item{
            id: registerTypeItems

            anchors.top: savePassword.bottom
            anchors.topMargin: _topMargin
            anchors.left: savePassword.left
            anchors.right: parent.right
            height: 14

            WisenetRadioButton{
                id: macAddressRadioButton
                checked: true
                width: 100
                text: WisenetLinguist.macAddress
            }
            WisenetRadioButton{
                id: serialNumberRadioButton
                checked: false
                anchors.left: macAddressRadioButton.right
                anchors.leftMargin: 15
                width: 100
                text: WisenetLinguist.serialNumber
            }
        }

        WisenetText{
            id: macAddressText
            text: macAddressRadioButton.checked === true ? WisenetLinguist.macAddress : WisenetLinguist.serialNumber
            width: ipTypeText.width
            height: 14
            anchors.top: registerTypeItems.bottom
            anchors.topMargin: _topLabelMargin
            anchors.left: parent.left
        }

        WisenetMacAddressBox{
            id : macAddressTextBox
            anchors.top: registerTypeItems.bottom
            anchors.topMargin: 32
            anchors.left: macAddressText.right
            anchors.leftMargin: 2
            anchors.right: parent.right
            visible: macAddressRadioButton.checked === true
        }

        WisenetTextBox{
            id : serialTextBox

            anchors.left: macAddressText.right
            anchors.leftMargin: 2
            anchors.bottom: macAddressTextBox.bottom
            anchors.right: parent.right

            visible: macAddressRadioButton.checked !== true
        }
    }
}
