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
            savePassword.checked = false
            inputDeviceID.text = ""
            inputDevicePassword.text = ""

        }
    }

    onApplyButtonClicked: {
        if(inputDevicePassword.text.length < 1 || inputDeviceID.text.length < 1){
            msgDialog.message = WisenetLinguist.pleaseCheckIdAndPassword
            msgDialog.show()
        }
        else{
            localSettingViewModel.saveDeviceCredential(savePassword.checked, inputDeviceID.text, inputDevicePassword.text)
            autoDiscoveryViewModel.startDiscoveryAndRegister("Nvr", inputDeviceID.text, inputDevicePassword.text)
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
            WisenetTextBox{
                id : inputDeviceID
                width: 300
                height: 30
                Layout.alignment: Qt.AlignHCenter
                placeholderText: WisenetLinguist.id_
            }

            WisenetPasswordBox{
                id : inputDevicePassword
                width: 300
                height: 30
                Layout.alignment: Qt.AlignHCenter
                placeholderText: WisenetLinguist.password
                showPasswordButtonVisible: true
            }
            Row{
                spacing: 1
                height: 30
                WisenetCheckBox2{
                    id: savePassword
                    checked: localSettingViewModel.autoRegister
                    text: WisenetLinguist.saveIdPassword
                    anchors.verticalCenter: parent.verticalCenter
                    width: 170
                    textWrapMode: Text.Wrap
                }
                WisenetSaveCheckQuestionButton{
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

        }

    }
}
