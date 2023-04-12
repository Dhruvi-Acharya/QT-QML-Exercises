import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0

/////////////////////////////////////////////////////////////
// 레이아웃 Save As 팝업
/////////////////////////////////////////////////////////////
WisenetMediaDialog {
    id: saveAsDialog
    width: 450
    height:186+headerHeight
    modal:true
    property alias text: inputField.text
    title: WisenetLinguist.saveLayoutAs

    function showDialog() {
        inputField.selectAll();
        open()
        forceActiveFocus();
    }

    contentItem: Item {
        Row {
            id: inputGroup
            anchors.centerIn: parent
            spacing: 45
            WisenetMediaTextLabel {
                id: inputLabel
                text: WisenetLinguist.name
                width: 80
                height: 20
            }

            WisenetMediaTextField {
                id: inputField
                focus:true
                maximumLength: 32
                width: 228
                height: 20

                onEntered: saveAsDialog.accept()
                onCanceled: saveAsDialog.reject()
            }
        }
    }
}
