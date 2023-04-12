import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu{
    id: localfileMenu
    property string itemUuid
    property bool isResourceTree
    property bool useBackgroundVisible
    property alias verifyMenuVisible: verifyMenu.visible

    signal addNewLocalFile(var localFileUrl)
    signal resourceTreeDeleteItems(var remove)
    signal layoutTreeDeleteItems()

    signal expandAll(var isResourceTree)
    signal collapseAll(var isResourceTree)

    WisenetMediaMenuQuickButtons {
        id: quickButtons

        editVisible: false
        settingsVisible: false
        useOpenToolTip: true
        deleteVisible: !isResourceTree

        onOpenInNewTabClicked: localfileMenu.addNewLocalFile(itemUuid)

        onOpenInNewWindowClicked: {
            windowHandler.initializeNewWindowWithMediaOpen(ItemType.LocalFile, itemUuid)
        }

        onDeleteClicked: {
            if(isResourceTree)
                localfileMenu.resourceTreeDeleteItems(true)
            else
                localfileMenu.layoutTreeDeleteItems()
        }
    }

    WisenetMediaMenuSeparator {}

    Action{
        text : WisenetLinguist.openFolder
        onTriggered: {
            resourceViewModel.treeModel.openLocalDirPath(itemUuid)
        }
    }

    WisenetMediaMenuSeparator {}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: localfileMenu.expandAll(isResourceTree)
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: localfileMenu.collapseAll(isResourceTree)
    }

    WisenetMediaMenuItem {
        id: verifyMenu
        text : WisenetLinguist.verifySignature
        onTriggered: resourceViewModel.treeModel.verifySignature(itemUuid)
    }
}
