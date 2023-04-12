import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu {
    id: menu
    property string filePath
    property var wnmList: []

    signal addNewLocalFile()
    signal expandAll()
    signal collapseAll()

    function popupMenu(parent, x, y)
    {
        if(parent.itemType === ItemType.LocalFile) {
            filePath = parent.uuid
            quickButtons.visible = true
            openFolderMenu.visible = true
        }
        else {
            filePath = ""
            quickButtons.visible = false
            openFolderMenu.visible = false
        }

        signatureVerifyMenu.visible = wnmList.length > 0

        menu.popup(parent, x, y)
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons

        openInNewTabVisibile: true
        openInNewWindowVisible: false
        editVisible: false
        deleteVisible: false
        settingsVisible: false
        useOpenToolTip: true

        onOpenInNewTabClicked: menu.addNewLocalFile()
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem {
        id: openFolderMenu
        text : WisenetLinguist.openFolder
        onTriggered: {
            resourceViewModel.treeModel.openLocalDirPath(filePath)
        }
    }

    WisenetMediaMenuItem {
        id: signatureVerifyMenu
        text : WisenetLinguist.verifySignature
        onTriggered: {
            resourceViewModel.treeModel.verifySignature(wnmList)
        }
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem {
        text : WisenetLinguist.expandAll
        onTriggered: menu.expandAll()
    }

    WisenetMediaMenuItem {
        text : WisenetLinguist.collapseAll
        onTriggered: menu.collapseAll()
    }
}
