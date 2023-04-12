import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0

WisenetMessageDialog{
    id : root

    applyClose: false
    applyButtonText: WisenetLinguist.ok

    onVisibleChanged: {
        if(visible){
            inputDevicePassword.text = localSettingViewModel.autoRegisterPassword

        }
    }

    onApplyButtonClicked: {
        if(inputDevicePassword.text != inputDevicePasswordConfirm.text){
            msgDialog.message = WisenetLinguist.pleaseCheckPassword
            msgDialog.show()
        }
        else{
            myAutoDiscoveryViewModel.startInitPasswordSetting(inputDevicePassword.text);
            root.close()
        }
    }
    WisenetMessageDialog{
        id  : msgDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }


    Rectangle{
        id : rect
        anchors.fill: parent
        color: "transparent"

        Column{
            id : inputLayout
            anchors.centerIn: parent
            spacing: 10

            WisenetPasswordBox{
                id : inputDevicePassword
                width: 300
                height: 30
                Layout.alignment: Qt.AlignHCenter
                placeholderText: WisenetLinguist.password
                text : localSettingViewModel.autoRegisterPassword
                showPasswordButtonVisible: false
            }
            WisenetPasswordBox{
                id : inputDevicePasswordConfirm
                width: 300
                height: 30
                Layout.alignment: Qt.AlignHCenter
                placeholderText: WisenetLinguist.confirmPassword
                showPasswordButtonVisible: false
            }

        }

    }
}
