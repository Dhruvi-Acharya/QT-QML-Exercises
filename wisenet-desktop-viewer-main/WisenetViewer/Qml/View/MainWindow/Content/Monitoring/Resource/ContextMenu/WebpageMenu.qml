import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu{
    id: webpageMenu
    property string itemUuid
    property string displayName

    property bool isResourceTree

    signal addNewWebpage(var webpageId)
    signal moveGroup(var groupId)
    signal resourceTreeDeleteItems(var remove)
    signal layoutTreeDeleteItems()

    signal openRenameDialog(var uuid, var name)

    signal expandAll(var isResourceTree)
    signal collapseAll(var isResourceTree)

    function setMenuVisible(isResource){
        var isAdmin = userGroupModel.isAdmin

        moveToMenu.setMenuVisible(isResource && isAdmin)
        moveToMenu.enabled = resourceViewModel.contextMenuGroupListModel.count > 0

        quickButtons.deleteVisible = isAdmin
        quickButtons.editVisible = isAdmin
        quickButtons.settingsVisible = isAdmin
        unGroupMenuItem.visible = (isResource && isAdmin)
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons

        useOpenToolTip: true

        onOpenInNewTabClicked: {
            webpageMenu.addNewWebpage(itemUuid)
        }
        onOpenInNewWindowClicked: {
            windowHandler.initializeNewWindowWithMediaOpen(ItemType.WebPage, itemUuid)
        }
        onDeleteClicked: {
            if(isResourceTree)
                webpageMenu.resourceTreeDeleteItems(true)
            else
                webpageMenu.layoutTreeDeleteItems()
        }
        onEditClicked: webpageMenu.openRenameDialog(itemUuid, displayName)
        onSettingsClicked: {
            webpageAddViewModel.setWebpage(itemUuid)
            windowService.webPageAddView_Open(WisenetLinguist.modifyWebpage)
        }
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenu{
        id: moveToMenu
        title: WisenetLinguist.moveTo

        Repeater{
            model: (resourceViewModel && resourceViewModel.contextMenuGroupListSortModel) ? resourceViewModel.contextMenuGroupListSortModel : null

            WisenetMediaMenuItem{
                text: model.groupName

                onTriggered: {
                    console.log("Move to " + model.groupId)
                    webpageMenu.moveGroup(model.groupId)
                }
            }
        }
    }

    WisenetMediaMenuItem{
        id: unGroupMenuItem
        text : WisenetLinguist.ungroup
        visible: isResourceTree

        onTriggered: webpageMenu.resourceTreeDeleteItems(false)
    }

    WisenetMediaMenuSeparator {}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: webpageMenu.expandAll(isResourceTree)
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: webpageMenu.collapseAll(isResourceTree)
    }
}
