import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu{
    id: multiChannelMenu
    property string itemUuid
    property string displayName
    property var treeItem
    property bool cameraSettingVisible : false    
    property bool isResourceTree : true
    property bool exportVideoVisible : false

    signal openItems()
    signal openItemsNewWindow()
    signal deleteItems(var remove)
    signal moveGroup(var groupId)
    signal newGroupWithSelection()
    signal exportVideoWithSelection()

    signal openResourceTreeCameraSettings()
    signal openLayoutTreeCameraSettings()

    signal changeConnectionStatus(var connections)

    signal expandAll(var isResourceTree)
    signal collapseAll(var isResourceTree)

    function setMoveGroupVisible(visible){
        moveToMenu.setMenuVisible(visible)
        moveToMenu.enabled = resourceViewModel.contextMenuGroupListModel.count > 0
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons
        settingsVisible: cameraSettingVisible
        editVisible: false
        deleteVisible: userGroupModel && userGroupModel.isAdmin ? true : (isResourceTree ? false : true)
        useOpenToolTip: true

        onOpenInNewTabClicked: multiChannelMenu.openItems()
        onOpenInNewWindowClicked: multiChannelMenu.openItemsNewWindow()
        onDeleteClicked: multiChannelMenu.deleteItems(true)

        onSettingsClicked: {
            if(isResourceTree)
                multiChannelMenu.openResourceTreeCameraSettings()
            else
                multiChannelMenu.openLayoutTreeCameraSettings()
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
                    multiChannelMenu.moveGroup(model.groupId)
                }
            }
        }
    }

    WisenetMediaMenuItem{
        text : WisenetLinguist.ungroup
        visible: isResourceTree && userGroupModel && userGroupModel.isAdmin

        onTriggered: multiChannelMenu.deleteItems(false)
    }

    WisenetMediaMenuItem{
        text : WisenetLinguist.newGroupWithSelection
        visible: isResourceTree && userGroupModel && userGroupModel.isAdmin

        onTriggered: multiChannelMenu.newGroupWithSelection()
    }

    WisenetMediaMenuItem{
        text : WisenetLinguist.exportVideo
        visible: exportVideoVisible

        onTriggered: multiChannelMenu.exportVideoWithSelection()
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem{
        id: connectMenuItem
        text : WisenetLinguist.connect
        visible: isResourceTree

        onTriggered: {
            multiChannelMenu.changeConnectionStatus(true)
        }
    }
    WisenetMediaMenuItem{
        id: disconnectMenuItem
        text : WisenetLinguist.disconnect
        visible: isResourceTree

        onTriggered: {
            multiChannelMenu.changeConnectionStatus(false)
        }
    }

    WisenetMediaMenuSeparator {}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: multiChannelMenu.expandAll(isResourceTree)
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: multiChannelMenu.collapseAll(isResourceTree)
    }
}
