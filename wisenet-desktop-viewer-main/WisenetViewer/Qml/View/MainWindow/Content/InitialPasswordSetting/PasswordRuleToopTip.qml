import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

ToolTip{
    id: passwordRuleTooltip

    property var isS1Support: versionManager.s1Support ? true : false

    width: isS1Support === true
            ? 600
            : 300
    height: isS1Support === true
            ? rule1.height + rule2.height + rule3.height + rule4.height + rule5.height + rule6.height + rule7.height + rule8.height + 30 + 25 + 15
            : rule1.height + rule2.height + rule3.height + rule4.height + rule5.height + rule6.height + 30 + 25 + 15

    background: Rectangle {
        color: WisenetGui.contrast_08_dark_grey
        border.color: WisenetGui.contrast_07_grey
        border.width: 1
    }

    Rectangle{
        color: "transparent"
        anchors.fill: parent
        anchors.topMargin: 15
        anchors.bottomMargin: 15
        anchors.leftMargin: 15
        anchors.rightMargin: 15

        Text{
            id: rule1

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            width: 300
            text: isS1Support === true ? WisenetLinguist.s1PasswordTooltip1 : "- " + WisenetLinguist.passwordRuleUnderTen
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12

        }

        Text{
            id: rule2

            anchors.top: rule1.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.right: parent.right
            width: 300
            text: isS1Support === true ? "- " + WisenetLinguist.s1PasswordTooltip2 : "- " + WisenetLinguist.passwordRuleOverTen
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12
        }

        Text{
            id: rule3

            anchors.top: rule2.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.right: parent.right
            width: 300
            text: isS1Support === true ? "- " + WisenetLinguist.s1PasswordTooltip3 : "- " + WisenetLinguist.passwordRuleId
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12
        }

        Text{
            id: rule4

            anchors.top: rule3.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.right: parent.right
            width: 300
            text: isS1Support === true ? "- " + WisenetLinguist.s1PasswordTooltip4 : "- " + WisenetLinguist.passwordRuleSpecialCharacters
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12
        }

        Text{
            id: rule5

            anchors.top: rule4.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.right: parent.right

            width: 300
            text: isS1Support === true ? "- " + WisenetLinguist.s1PasswordTooltip5 : "- " + WisenetLinguist.passwordRuleConsecutive
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12
        }

        Text{
            id: rule6

            anchors.top: rule5.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.right: parent.right
            //anchors.bottom: parent.bottom

            width: 300
            text: isS1Support === true ? "- " + WisenetLinguist.s1PasswordTooltip6 : "- " + WisenetLinguist.passwordRuleRepeated
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12
        }

        Text{
            id: rule7

            anchors.top: rule6.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.right: parent.right
            //anchors.bottom: parent.bottom

            width: 300
            text: "- " + WisenetLinguist.s1PasswordTooltip7
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12
            visible: isS1Support
        }

        Text{
            id: rule8

            anchors.top: rule7.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.right: parent.right
            //anchors.bottom: parent.bottom

            width: 300
            text: "- " + WisenetLinguist.s1PasswordTooltip8
            wrapMode: Text.WrapAnywhere
            color: WisenetGui.contrast_03_light_grey
            font.pixelSize: 12
            visible: isS1Support
        }
    }
}
