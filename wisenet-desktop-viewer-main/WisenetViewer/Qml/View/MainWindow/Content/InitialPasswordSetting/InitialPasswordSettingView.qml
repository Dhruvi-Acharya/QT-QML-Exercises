import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import WisenetMediaFramework 1.0

Item{
    id: setAdminPasswordRect
    width:604
    height:472
    //anchors.fill: parent
    //color: WisenetGui.contrast_11_bg

    property int comboboxHeight: 21
    property int comboboxWidth: 114
    property int comboboxTopMargin: 13
    property int comboboxLeftMargin: 7
    property int languageIconSize: 20
    property var textBoxWidth : 280
    property bool changedLanguage:false;

    Component.onCompleted: {
        password.setTextFocus();
    }

    function setOkButtonEnable(){
        if(password.isEnabled && (password.text == confirmPassword.text))
            okButton.enabled = true;
        else
            okButton.enabled = false;
    }

    function getPasswordVaildText(text){
        if(text === "admin")
            return WisenetLinguist.passwordWarningSameWithId

        var passwordStrength = passwordStrengthChecker.test(text)

        if (versionManager.s1Support)
        {
            switch(passwordStrength){
            case PasswordStrength.Success:
                return WisenetLinguist.passwordHighSecurity
            default:
                return WisenetLinguist.s1PasswordGuide
            }
        }
        else
        {
            switch(passwordStrength){
            case PasswordStrength.Success:
                return WisenetLinguist.passwordHighSecurity
            case PasswordStrength.Blank:
                return WisenetLinguist.passwordWarningEmpty
            case PasswordStrength.NoNumbers:
                return WisenetLinguist.passwordWarningOverTen
            case PasswordStrength.LengthUnderEight:
            case PasswordStrength.LengthUnderTenAndNoSymbols:
                return WisenetLinguist.passwordWarningUnderTen
            case PasswordStrength.RepeatedValue:
                return WisenetLinguist.passwordWarningRepeated
            case PasswordStrength.ConsecutiveValue:
                return WisenetLinguist.passwordWarningConsecutive
            default:
                return WisenetLinguist.passwordHighSecurity
            }
        }
    }

    Rectangle{
        //anchors.fill: parent
        x:0
        y:0
        width:parent.width
        height:parent.height-(comboboxHeight+comboboxTopMargin)

        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1

        Rectangle{
            id: titleRect
            color: "transparent"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 145

            Text{
                id: title
                text: WisenetLinguist.setupAdminPassword
                color: "white"
                width: 300
                height: 23
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent
            }
        }

        Rectangle{

            color: "transparent"
            anchors.top: titleRect.bottom
            anchors.left: parent.left
            anchors.leftMargin: 161
            anchors.right: parent.right
            anchors.rightMargin: 161
            anchors.bottom: buttonRect.top

            Text {
                id: idTextBox
                height: 30
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 6

                font.pixelSize:  14
                font.bold: true
                color: WisenetGui.contrast_01_light_grey
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                text: "ID : admin"
            }

            WisenetPasswordBox{
                id: password
                height: 30
                width: textBoxWidth
                anchors.top: idTextBox.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.right: parent.right

                placeholderText: "<i>"+WisenetLinguist.password+"</i>"

                isEnabled: passwordStrengthChecker.test(text) ===PasswordStrength.Success
                onIsEnabledChanged: setOkButtonEnable()
            }

            Text{
                id: passwordCheckText

                height: 20
                width: textBoxWidth
                anchors.top: password.bottom
                anchors.topMargin: 12
                anchors.left: parent.left
                anchors.right: parent.right

                fontSizeMode: Text.VerticalFit
                font.pixelSize: 12
                minimumPixelSize: 9
                color: password.isEnabled ? "#00FF00" : "#FF0000"

                wrapMode: Text.WrapAnywhere
                text: getPasswordVaildText(password.text)
            }

            WisenetTextBox{
                id: confirmPassword

                height: 30
                width: textBoxWidth
                anchors.top: passwordCheckText.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.right: parent.right

                validator: RegExpValidator{
                    regExp: /[0-9a-zA-Z`!@#$%^&*()-_=+\{\}\[\]\\|:;\'\"<>,\.\/?~]+/
                }
                placeholderText: "<i>"+WisenetLinguist.confirmPassword+"</i>"

                maximumLength: 64

                onTextChanged: setOkButtonEnable()
                echomode: password.showPassword ? TextInput.Normal : TextInput.Password

            }

            /*WisenetCheckBox{
                id: savePasswordAdmin

                height: 15
                width: textBoxWidth
                anchors.top: confirmPassword.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.right: parent.right

                text: WisenetLinguist.saveIdPassword

                checked: initialPasswordSettingViewModel.savePasswordChecked
                Binding { target: initialPasswordSettingViewModel; property: "savePasswordChecked"; value: savePasswordAdmin.checked }
            }*/


            WisenetPasswordRuleButton{
                anchors {
                    left: password.right
                    leftMargin: 4
                    verticalCenter: password.verticalCenter
                }
            }
        }

        Rectangle{
            id: buttonRect
            color: "transparent"
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 145

            WisenetButton2{
                id: okButton

                anchors.centerIn: parent
                width: 282
                height: 36

                text: WisenetLinguist.ok
                enabled: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        initialPasswordSettingViewModel.initializePassword(password.text);
                        if( changedLanguage === true )
                        {
                            systemMaintenanceViewModel.restartApplication();
                        }
                        localSettingViewModel.language = languageManager.getLanguageString(languageCombo.currentIndex);
                        localSettingViewModel.saveSetting( true );
                        languageManager.language = languageCombo.currentIndex;
                    }
                }
            }
        }
    }
    Image {
        x:languageCombo.x-(languageIconSize+comboboxLeftMargin)
        //y:parent.height-(comboboxHeight)
        y:parent.height-(languageIconSize)
        id: langugeIcon
        sourceSize: Qt.size(languageIconSize,languageIconSize)
        source: WisenetImage.setup_language
    }
    WisenetComboBox{
        id : languageCombo
        x:parent.width-(comboboxWidth+50)
        y:parent.height-(comboboxHeight)
        width:comboboxWidth+50
        height:comboboxHeight
        currentIndex : languageManager.language;
        onActivated:{
            localSettingViewModel.language = languageManager.getLanguageString(languageCombo.currentIndex);
            localSettingViewModel.saveSetting( true );
            languageManager.language = languageCombo.currentIndex;
            changedLanguage = true;
        }

        model: [
            WisenetLinguist.korean,
            WisenetLinguist.english +"-USA",
            WisenetLinguist.english +"-UK",
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
