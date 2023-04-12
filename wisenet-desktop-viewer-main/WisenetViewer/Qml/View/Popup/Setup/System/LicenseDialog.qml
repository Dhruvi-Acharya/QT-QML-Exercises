import QtQuick 2.15
import "qrc:/"
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0
import WisenetStyle 1.0

WisenetMediaDialog {
    id: deleteDialog
    width: 450
    height:  186+headerHeight
    dim:true
    applyText : WisenetLinguist.apply
    contentItem: Item {
        Rectangle {
            anchors.fill: parent
            color: WisenetGui.transparent

            WisenetTextBox {
                id: licenseKeyTextBox

                width: 280
                height: 30
                anchors.centerIn: parent

                placeholderText: WisenetLinguist.enterLicenseKey

                onTextChanged: systemMaintenanceViewModel.licenseKey = licenseKeyTextBox.text
            }
            Image{
                id: licenseValidation

                width: 20
                height: 20
                sourceSize.width: 20
                sourceSize.height: 20

                anchors.left: licenseKeyTextBox.right
                anchors.leftMargin: 6
                anchors.verticalCenter: licenseKeyTextBox.verticalCenter

                source: systemMaintenanceViewModel.isValidLicense ? WisenetImage.setup_license_right_key : WisenetImage.setup_license_wrong_key
            }
        }
    }

    onOpened: {
        licenseKeyTextBox.text = ""
    }
}
