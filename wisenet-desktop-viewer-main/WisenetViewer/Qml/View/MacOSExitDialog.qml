import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Wisenet.Process 1.0

WisenetMessageDialog{
    id: exitDialog
    visible: true
    buttonWidthSize: 140; buttonHeightSize: 40
    applyButtonVisible: false
    closeButtonVisible: false
    cancelButtonText: WisenetLinguist.exitProgram
    message: WisenetLinguist.notSupportedMacOs

    onCancelButtonClicked: {
        console.log("This macOS version is lower than 11, force to exit the program")
    }
}
