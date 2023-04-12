import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

WisenetMessageDialog{
    id : root
    applyClose: false
    applyButtonText: WisenetLinguist.ok
    width: 450
    height: 330
    property bool needConfirm : false
    property int margin : 10
    property int mainTitleHeight: 30
    property int _topMargin : 27
    property int _leftMargin: 25
    property int _rightMargin: 25
    property alias password : inputPassword.text

    signal passwordEntered()

    property bool isValid : (inputPassword.text.length > 0 && (!needConfirm || (needConfirm && inputPassword.text == inputPasswordConfirm.text)))

    function showDialog()
    {
        inputPassword.text = ""
        inputPasswordConfirm.text = ""

        inputPassword.showPassword = false
        inputPasswordConfirm.showPassword = false
        inputPassword.setTextFocus()
        showNormal();
    }

    WisenetMessageDialog{
        id : msg
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
        message: WisenetLinguist.pleaseCheckPassword
        onClosing: root.show()
    }

    function apply()
    {
        if(!isValid || (passwordStrengthChecker.test(inputPassword.text) !== 0)){
            msg.message = WisenetLinguist.pleaseCheckPassword
            msg.show()
        }       
        else{
            root.passwordEntered()
            root.close()
        }
    }

    onApplyButtonClicked: {
        apply()
    }

    Rectangle{
        id : rect
        anchors.fill: parent
        color: WisenetGui.transparent

        WisenetText{
            id: passwordText
            text: WisenetLinguist.password
            width: 120
            height: 14
            x : 30
            y : needConfirm? 30 : 80
        }

        WisenetPasswordBox{
            id : inputPassword
            placeholderText: WisenetLinguist.password
            showPasswordButtonVisible: false

            x : 150
            y : passwordText.y
        }

        WisenetPasswordRuleButton{
            id: passwordRuleButton
            visible: true
            x : 420
            y : passwordText.y + 5
        }

        WisenetText{
            id: passwordConfirmText
            visible: needConfirm
            text: WisenetLinguist.confirmPassword
            width: 120
            height: 14
            x : passwordText.x
            y : 80
        }

        WisenetPasswordBox{
            id : inputPasswordConfirm
            visible: needConfirm
            x : inputPassword.x
            y : passwordConfirmText.y
            placeholderText: WisenetLinguist.confirmPassword
            showPasswordButtonVisible: false
        }
        WisenetText{
            text: WisenetLinguist.deviceConfigBackupMessage
            visible: needConfirm
            width: parent.width
            height: 14
            x : passwordText.x
            y : 130
        }

    }
}
