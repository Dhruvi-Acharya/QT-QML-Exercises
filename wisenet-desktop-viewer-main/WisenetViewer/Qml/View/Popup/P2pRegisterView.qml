import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0

WisenetMessageDialog{
    id : root
    applyClose: false
    height: 350
    width: 570
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
        }
    }

    onApplyButtonClicked: {
        if(inputDevicePassword.text.length < 1 || inputDeviceID.text.length < 1){
            errorDialog.message = WisenetLinguist.pleaseCheckIdAndPassword
            errorDialog.show()
        }
        else if(ddnsId.text.length < 1){
            errorDialog.message = WisenetLinguist.pleaseCheckProductId
            errorDialog.show()
        }
        else{

            console.debug("P2P Register checked:" + savePassword.checked + " ddnsid:" + ddnsId.text + " id:", inputDeviceID.text)
            localSettingViewModel.saveDeviceCredential(savePassword.checked, inputDeviceID.text, inputDevicePassword.text)
            manualDiscoveryViewModel.startP2pRegister(ddnsId.text, inputDeviceID.text, inputDevicePassword.text, dtlsOn.checked)
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
        color: "transparent"

        WisenetText{
            id: ddnsText
            text: WisenetLinguist.productId
            width: 70
            height: 14
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: _leftMargin
        }
        WisenetText{
            id: idText
            text: WisenetLinguist.id_
            width: ddnsText.width
            height: 14
            anchors.top: ddnsText.bottom
            anchors.topMargin: _topLabelMargin
            anchors.left: parent.left
            anchors.leftMargin: _leftMargin
        }
        WisenetText{
            id: passwordText
            text: WisenetLinguist.password
            width: ddnsText.width
            height: 14
            anchors.top: idText.bottom
            anchors.topMargin: _topLabelMargin
            anchors.left: parent.left
            anchors.leftMargin: _leftMargin
        }

        WisenetTextBox{
            id : ddnsId
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.left: ddnsText.right
            anchors.leftMargin: 45
            anchors.right: parent.right
            anchors.rightMargin: _rightMargin
            text: ""
            placeholderText: WisenetLinguist.productId
        }
        WisenetTextBox{
            id : inputDeviceID
            anchors.top: ddnsId.bottom
            anchors.topMargin: _topMargin
            anchors.left: ddnsText.right
            anchors.leftMargin: 45
            anchors.right: parent.right
            anchors.rightMargin: _rightMargin
            placeholderText: WisenetLinguist.id_
        }

        WisenetPasswordBox{
            id : inputDevicePassword
            anchors.top: inputDeviceID.bottom
            anchors.topMargin: _topMargin
            anchors.left: ddnsText.right
            anchors.leftMargin: 45
            anchors.right: parent.right
            anchors.rightMargin: _rightMargin
            placeholderText: WisenetLinguist.password
            showPasswordButtonVisible: false
        }
        WisenetCheckBox2{
            id: savePassword
            checked: localSettingViewModel.autoRegister
            text: WisenetLinguist.saveIdPassword
            anchors.top: inputDevicePassword.bottom
            anchors.topMargin: _topMargin
            anchors.left: ddnsText.right
            anchors.leftMargin: 45
            anchors.rightMargin: 10
            width: 170
            textWrapMode: Text.Wrap
        }
        WisenetSaveCheckQuestionButton{
            anchors.top: inputDevicePassword.bottom
            anchors.topMargin: _topMargin
            anchors.left: savePassword.right
        }
        WisenetText{
            id: dtlsText
            text: WisenetLinguist.dtls
            height: 14
            width: ddnsText.width
            anchors.top: savePassword.bottom
            anchors.topMargin: _topLabelMargin
            anchors.left: parent.left
            anchors.leftMargin: _leftMargin
        }

        WisenetRadioButton{
            id: dtlsOn
            checked: true
            anchors.top: savePassword.bottom
            anchors.topMargin: _topMargin
            anchors.left: dtlsText.right
            anchors.leftMargin: 45
            width: 150

            text: WisenetLinguist.on + "(" + WisenetLinguist.securityFirst +")"
        }
        WisenetRadioButton{
            id: dtlsOff
            checked: false
            anchors.top: savePassword.bottom
            anchors.topMargin: _topMargin
            anchors.right: parent.right
            anchors.rightMargin: _rightMargin
            width: 160
            text: WisenetLinguist.off + "(" + WisenetLinguist.performanceFirst +")"
        }
    }
}
