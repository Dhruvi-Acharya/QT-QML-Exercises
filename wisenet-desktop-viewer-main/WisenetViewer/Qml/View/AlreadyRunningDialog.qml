import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Wisenet.Process 1.0

WisenetMessageDialog {
    id: window
    visible:true
    applyButtonVisible : (Qt.platform.os === "windows") ?true:false
    cancelButtonText: WisenetLinguist.close
    applyButtonText: WisenetLinguist.ok
    message: (Qt.platform.os === "windows") ? (WisenetLinguist.alreadyRunningMessage + " " + WisenetLinguist.shutDownRestartMessage) :
                                              WisenetLinguist.alreadyRunningMessage


    ProcessModel{
        id : processModel
    }

    onApplyButtonClicked: {
        console.log("Already running message")
        processModel.runRestartBatch()

    }
}
