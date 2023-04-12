import QtQuick 2.0
import "qrc:/"
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0

WisenetMediaDialog {
    id: deleteDialog
    width: 450
    height:  186+headerHeight
    dim:true
    property alias dontAskAgain: dontAskAgainCheckBox.checked
    property alias msg : messageText.text
    property alias dontAskVisible : dontAskAgainCheckBox.visible
    applyText : WisenetLinguist.ok
    contentItem: Item {
        Rectangle {
            anchors.fill: parent
            color: WisenetGui.transparent
            Text {
                id: messageText
                anchors.centerIn: parent
                width: parent.width
                text: WisenetLinguist.deviceDeleteConfirmMessage
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 12
                color: WisenetGui.contrast_00_white
                wrapMode: Text.Wrap
            }
            WisenetCheckBox{
                id: dontAskAgainCheckBox

                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter

                height:14
                text: WisenetLinguist.doNotAskAgain
                checked: false
            }
        }
    }
}
