import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0

WisenetMessageDialog{
    id : root
    property int margin : 10
    property int mainTitleHeight: 30
    applyClose: false
    property alias id: inputDeviceID.text
    property alias password : inputDevicePassword.text
    applyButtonText: WisenetLinguist.ok

    signal credentialChanged()

    property bool isValid : (inputDeviceID.text.length > 0 && inputDevicePassword.text.length > 0)
    onVisibleChanged: {
        if(!visible){
        inputDeviceID.text = ""
        inputDevicePassword.text = ""
        }
    }

    function showDialog()
    {
        inputDeviceID.setTextFocus()
        showNormal();
    }

    WisenetMessageDialog{
        id : msg
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        message: WisenetLinguist.pleaseCheckIdAndPassword
        onClosing: root.show()
    }

    function apply()
    {
        if(!isValid){
            msg.show()
        }
        else{
            if (versionManager.s1Support) {
                if (PasswordStrength.Success !== passwordStrengthChecker.test(inputDevicePassword.text)) {
                    msg.show()
                    return;
                }
            }

            deviceListViewModel.changeCredential(inputDeviceID.text, inputDevicePassword.text)
            root.credentialChanged()
            root.close()
        }
    }

    onApplyButtonClicked: {
        apply()
    }

    Rectangle{
        id : rect
        x : margin
        width: parent.width - (margin *2)
        height: parent.height -(margin * 2)
        color: WisenetGui.transparent
        Layout.fillWidth: true

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
                onAccepted: {
                    if (root.isValid) {
                        root.apply();
                    }
                }
            }

            WisenetPasswordBox{
                id : inputDevicePassword
                width: 300
                height: 30
                Layout.alignment: Qt.AlignHCenter
                placeholderText: WisenetLinguist.password
                showPasswordButtonVisible: versionManager.s1Support ? true : false
                onAccepted: {
                    if (root.isValid) {
                        root.apply();
                    }
                }
            }

        }
    }
}
