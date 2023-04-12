import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

WisenetMediaMenu{
    id: groupMenu
    property string itemUuid
    property string displayName
    property var treeItem

    signal addNewGroup(var groupId)

    signal resourceTreeDeleteItems(var remove)
    signal makeGroup()

    signal openRenameDialog(var uuid, var name)
    signal openItemsNewWindow()

    signal expandAll()
    signal collapseAll()

    function setMenuVisible(){
        var isAdmin = userGroupModel.isAdmin

        quickButtons.deleteVisible = isAdmin
        quickButtons.editVisible = isAdmin
        addGroupMenuItem.visible = isAdmin
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons
        settingsVisible: false
        useOpenToolTip: true

        onOpenInNewTabClicked: groupMenu.addNewGroup(itemUuid)
        onOpenInNewWindowClicked: groupMenu.openItemsNewWindow()
        onDeleteClicked: groupMenu.resourceTreeDeleteItems(false)
        onEditClicked: groupMenu.openRenameDialog(itemUuid, displayName)
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem{
        id: addGroupMenuItem
        text : WisenetLinguist.addGroup
        onTriggered: groupMenu.makeGroup()
    }

    WisenetMediaMenuSeparator{}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: groupMenu.expandAll()
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: groupMenu.collapseAll()
    }
}
