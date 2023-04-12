import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu{
    id: sequenceMenu
    property string itemUuid
    property string displayName
    property var treeItem

    signal selectSequence(var type, var name, var uuid)

    signal openRenameDialog(var uuid, var name)
    signal expandAll()
    signal collapseAll()

    WisenetMediaMenuQuickButtons {
        id: quickButtons
        isSeqeunceType: true

        onOpenInNewTabClicked: {
            console.log("sequenceMenu onOpenInNewTabClicked : itemUuid",itemUuid," displayName",displayName," treeItem",treeItem)
            sequenceMenu.selectSequence(ItemType.Sequence, sequenceMenu.displayName, sequenceMenu.itemUuid)
        }
        onOpenInNewWindowClicked: {
            console.log("sequenceMenu onOpenInNewWindowClicked : itemUuid",itemUuid," displayName",displayName," treeItem",treeItem)
            windowHandler.initializeNewWindowWithSequence(sequenceMenu.itemUuid, displayName)
        }
        onDeleteClicked: layoutTree.deleteItems()
        onEditClicked: layoutTree.openRenameDialog(ItemType.Sequence, sequenceMenu.itemUuid, sequenceMenu.displayName)
        onSettingsClicked: {
            console.log("sequenceMenu onSettingsClicked : itemUuid",itemUuid," displayName",displayName," treeItem",treeItem)
            windowService.editSequence(sequenceMenu.displayName, sequenceMenu.itemUuid)
        }
    }

//    WisenetMediaMenuSeparator {}

//    Action{
//        text : WisenetLinguist.deleteText
//        shortcut: StandardKey.Delete
//        onTriggered: layoutTree.deleteItems()
//    }

    WisenetMediaMenuSeparator{}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: layoutRootMenu.expandAll()
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: layoutRootMenu.collapseAll()
    }
}
