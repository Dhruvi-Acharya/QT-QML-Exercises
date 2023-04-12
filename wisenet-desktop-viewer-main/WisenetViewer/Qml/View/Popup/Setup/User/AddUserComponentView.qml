import QtQuick 2.15
import WisenetStyle 1.0
import Wisenet.Setup 1.0
import QtQml.Models 2.15
import WisenetLanguage 1.0

Column{
    id : root

    x: 30
    topPadding: 20
    leftPadding: 0

    width: parent.width
    height: 90
    //leftPadding: 35
    //columns: 2
    // rows : 2
    //columnSpacing: 20
    //rowSpacing: 50
    spacing: 20
    visible: false

    property alias userId : loginId.text
    property alias password: passwordId.text
    property alias passwordConfirm: passwordConfirmId.text
    property alias userName: nameId.text
    property alias description : descriptionId.text
    property alias email: email.text
    property bool isLdap: false

    property int localRowSpacing: 20

    AddUserGroupViewModel{
        id:addUserGroupViewModel
    }

    function reset(){
        console.log("[AddUserComponentView] reset")
        passwordId.text = passwordConfirmId.text = ""
        passwordRow.visible = true
        loginId.enabled = true
        email.enabled = true
        ldapDNRow.visible = false
    }

    function setLdapUserView() {
        console.log("[AddUserComponentView] setLdapUserView")
        passwordRow.visible = false
        loginId.enabled = false
        email.enabled = false
        ldapDNRow.visible = true
    }

    onVisibleChanged: {
        if(visible) {
            if(isLdap == true){
                console.log("AddUserComponentView onVisibleChanged dn : ", addUserViewModel.dn)
                setLdapUserView()
            }
            else
                reset()
        }
    }

    Row{
        width: parent.width
        spacing: 20
        WisenetLabel{
            text :  WisenetLinguist.id_
        }

        Row{
            width: parent.width - 200
            spacing: 50
            WisenetTextBox{
                id : loginId
                width: 228
                text: addUserViewModel.loginId
                enabled: !addUserViewModel.isOwner
                onTextChanged: {
                    duplicateId.visible = addUserViewModel.isDuplicateLoginId(loginId.text)
                }
            }
            Row{
                id: passwordRow
                spacing: root.localRowSpacing
                WisenetText{
                    id: passwordText
                    text : WisenetLinguist.password
                    width: 100

                }
                WisenetPasswordBox{
                    id : passwordId
                    width: loginId.width
                    placeholderText: WisenetLinguist.password
                }
                WisenetPasswordBox{
                    id : passwordConfirmId
                    width: passwordId.width
                    placeholderText: WisenetLinguist.confirmPassword
                }
                WisenetPasswordRuleButton{
                    id: passwordRuleButton
                    visible: true
                }
            }
            Row{
                id: ldapDNRow
                visible: false
                spacing: root.localRowSpacing

                WisenetText{
                    text: "DN"
                    width: 100
                }
                WisenetTextBox {
                    id: ldapUserDN
                    text: addUserViewModel.dn
                    width: 600
                    enabled: false
                }
            }
        }
    }

    Row{

        width: parent.width
        spacing: 20
        WisenetLabel{
            text : WisenetLinguist.name
        }
        Row{
            width: parent.width - 200
            spacing: 50
            WisenetTextBox{
                id : nameId
                text: addUserViewModel.name
                width: loginId.width
            }
            Row{
                spacing: root.localRowSpacing
                WisenetText{
                    text : WisenetLinguist.emailPageSubject
                    width: 100
                }
                WisenetTextBox{
                    id : email
                    text: addUserViewModel.email
                    width: passwordId.width
                }
            }
            Row{
                spacing: root.localRowSpacing
                WisenetText{
                    text : WisenetLinguist.description
                    width: 100
                }
                WisenetTextBox{
                    id : descriptionId
                    text: addUserViewModel.description
                    width: passwordId.width
                }
            }
        }
    }

}

