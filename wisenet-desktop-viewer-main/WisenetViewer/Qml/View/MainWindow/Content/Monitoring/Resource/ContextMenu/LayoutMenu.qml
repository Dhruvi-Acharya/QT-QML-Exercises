import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu{
    id: layoutMenu
    property string itemUuid
    property string displayName
    property bool saveStatus : true
    property var treeItem
    property var layoutModel

    // 공유레이아웃 여부
    readonly property bool isShared: (layoutModel && layoutModel.ownerId && layoutModel.loginId && (layoutModel.loginId !== layoutModel.ownerId)) ? true : false

    signal selectLayout(var type, var name, var uuid, var layoutViewModel)
    signal layoutTreeDeleteItems()

    signal openRenameDialog(var uuid, var name)

    signal expandAll()
    signal collapseAll()

    function setMenuVisible(){
        var isAdmin = userGroupModel.isAdmin
        console.log("layoutMenu.setMenuVisible userGroupModel:" + userGroupModel.userId + ",layoutModel:" + layoutModel.ownerId)
        var myLayout = (userGroupModel.userId === layoutModel.ownerId)

        if(!myLayout)
        {
            saveLayoutMenuItem.visible = false
            quickButtons.deleteVisible = false
            quickButtons.editVisible = false
            unlockMenuItem.visible = false
        }
        else
        {
            saveLayoutMenuItem.visible = true
            quickButtons.deleteVisible = true
            quickButtons.editVisible = true
            unlockMenuItem.visible = true
        }
    }


    WisenetMediaMenuQuickButtons {
        id: quickButtons
        settingsVisible: false
        useOpenToolTip: true

        onOpenInNewTabClicked: layoutMenu.selectLayout(ItemType.Layout, layoutModel.name, layoutModel.layoutId, layoutModel)
        onOpenInNewWindowClicked: {
            var layoutList = []
            layoutList.push(itemUuid)
            windowHandler.initializeNewWindowWithLayouts(layoutList)
        }
        onDeleteClicked: layoutMenu.layoutTreeDeleteItems()
        onEditClicked: layoutMenu.openRenameDialog(itemUuid, displayName)
    }

    WisenetMediaMenuSeparator{
    }

    WisenetMediaMenuItem{
        id: saveLayoutMenuItem
        text : WisenetLinguist.saveLayout
        enabled: !saveStatus && !layoutModel.locked && !isShared    // 잠김, 공유 레이아웃 저장 비활성화
        onTriggered: treeItem.saveLayout()
    }

    Action{
        text : WisenetLinguist.saveLayoutAs
        onTriggered: {
            layoutSaveAsDialog.text = layoutModel.name + "#";
            layoutSaveAsDialog.saveAs = true
            layoutSaveAsDialog.layout = layoutModel
            layoutSaveAsDialog.showDialog();
        }
    }

    WisenetMediaMenuItem{
        id: unlockMenuItem
        text : (layoutModel && layoutModel.locked === true) ? WisenetLinguist.unlock : WisenetLinguist.lock
        onTriggered: {
            layoutModel.locked = !layoutModel.locked
            treeItem.saveLayout()
            layoutMenu.close()
        }
    }

    WisenetMediaMenuSeparator{}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: layoutMenu.expandAll()
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: layoutMenu.collapseAll()
    }
}
