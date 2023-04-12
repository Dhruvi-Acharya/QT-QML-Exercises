import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "Popup"
import "Popup/Sequence"
import "Popup/Webpage"
import "qrc:/MediaController"

Item {
    id: popupParent

    Connections{
        target: popupService

        onAddNewSequence: sequenceAddView.addNewSequence()
        onEditSequence: sequenceAddView.editSequence(displayName, itemUuid)
        onOpenSequenceAddCancelDialog: sequenceAddCancelDialog.show()

        onOpenInitializeDeviceCredentialView: initializeDeviceCredentialView.show()
        onOpenP2pView: {
            if(versionManager.s1Support)
                s1DdnsRegisterView.show()
            else
                p2pView.show()
        }

        onWebPageAddView_Open: {
            webpageAddView.setTitle(title)
            webpageAddView.visible = true
        }

        onExportProgressView_Open: {
            exportProgressView.show()
            exportProgressView.raise()
        }
        onExportProgressView_ForceClose: {
            exportVideoProgressModel.stopAllExportItem()
            exportProgressView.needCloseConfirm = false
            exportProgressView.close()
        }

        onCloseAll: {
            sequenceAddView.close()
            sequenceAddCancelDialog.close()
            webpageAddView.close()
            initializeDeviceCredentialView.close()
            p2pView.close()

            exportVideoProgressModel.stopAllExportItem()
            exportProgressView.needCloseConfirm = false
            exportProgressView.close()
        }
    }

    SequenceAddView{
        id : sequenceAddView
        visible: false
    }

    WisenetMessageDialog{
        id  : sequenceAddCancelDialog
        message: WisenetLinguist.sequenceAddCancelMessage
        applyButtonVisible: false
        buttonWidthSize : 140
        buttonHeightSize: 40

        onApplyButtonClicked: {
            sequenceAddCancelDialog.close()
        }
    }

    WebpageAddView{
        id: webpageAddView
        visible: false
    }


    InitializeDeviceCredentialView{
        id : initializeDeviceCredentialView
        visible: false
    }

    P2pRegisterView{
        id : p2pView
        visible: false
    }

    S1DdnsRegisterView{
        id : s1DdnsRegisterView
        visible: false
    }

    ExportVideoProgressView {
        id: exportProgressView
    }
}
