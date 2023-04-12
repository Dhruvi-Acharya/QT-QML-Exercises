import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu{
    id: cameraMenu
    property string itemUuid
    property string displayName
    property string parentId
    property string channelNumber
    property bool isResourceTree : true

    signal addNewVideoChannel(var deviceId, var channelId)
    signal moveGroup(var groupId)
    signal resourceTreeDeleteItems(var remove)
    signal layoutTreeDeleteItems()

    signal openRenameDialog(var uuid, var name)

    signal expandAll(var isResourceTree)
    signal collapseAll(var isResourceTree)

    signal changeConnectionStatus(var connections)

    function setMenuVisible(isResource){
        var isAdmin = userGroupModel.isAdmin
        var playback = userGroupModel.playback

        moveToMenu.setMenuVisible(isResource && isAdmin)
        moveToMenu.enabled = resourceViewModel.contextMenuGroupListModel.count > 0
        unGroupMenuItem.visible = (isResource && isAdmin)
        eventSearchMenuItem.visible = playback
        nvrSettingsMenuItem.visible = isAdmin
        eventLogMenuItem.visible = isAdmin
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons

        deleteVisible: isResourceTree ? (userGroupModel && userGroupModel.isAdmin ? true : false) : true
        editVisible: userGroupModel && userGroupModel.isAdmin
        settingsVisible : userGroupModel && userGroupModel.isAdmin

        useOpenToolTip: true
        onOpenInNewTabClicked: {
            var ids = itemUuid.split('_')
            cameraMenu.addNewVideoChannel(ids[0], channelNumber)
        }
        onOpenInNewWindowClicked: {
            var ids = itemUuid.split('_')
            windowHandler.initializeNewWindowWithMediaOpen(ItemType.Camera, ids[0], channelNumber)
        }
        onDeleteClicked: {
            if(isResourceTree)
                cameraMenu.resourceTreeDeleteItems(true)
            else
                cameraMenu.layoutTreeDeleteItems()
        }
        onEditClicked: cameraMenu.openRenameDialog(itemUuid, displayName)
        onSettingsClicked: {
            var deviceGuid = [];
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceList)
            deviceGuid.push(itemUuid)
            windowService.setupMainView_SelectDeviceListChannel(deviceGuid)
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
                    cameraMenu.moveGroup(model.groupId)
                }
            }
        }
    }

    WisenetMediaMenuItem{
        id: unGroupMenuItem
        text : WisenetLinguist.ungroup
        visible: isResourceTree

        onTriggered: cameraMenu.resourceTreeDeleteItems(false)
    }

    WisenetMediaMenuItem{
        id: eventSearchMenuItem
        text : WisenetLinguist.eventSearch
        onTriggered: {

            windowService.eventSearchView_SetVisible(true)

            windowService.eventSearchView_ResetFilter();

            var channelGuids = [];
            channelGuids.push(itemUuid)
            windowService.eventSearchView_SetChannelFilter(channelGuids)

            //var from = new Date(2021,11,27,10,11,59)
            //var to = new Date(2021,11,28,22,10,30)
            //eventSearchView.setDateFilter(from, to)
            windowService.eventSearchView_SearchByFilter();
        }
    }

    WisenetMediaMenuItem{
        id: nvrSettingsMenuItem
        text : WisenetLinguist.nvrSettings

        onTriggered: {
            var deviceGuid = [];
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceList)

            var ids = itemUuid.split('_')

            console.log("Open NVR Setting id : ", ids[0])
            deviceGuid.push(ids[0])
            windowService.setupMainView_SelectDeviceList(deviceGuid)
        }
    }

    WisenetMediaMenuItem{
        id: eventLogMenuItem
        text: WisenetLinguist.eventLog

        onTriggered: {
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.EventLog)
            windowService.setupMainView_SearchEventLog()
        }
    }

    WisenetMediaMenuItem{
        id: exportVideoMenuItem
        text: WisenetLinguist.exportVideo
        visible: isResourceTree && userGroupModel && userGroupModel.exportVideo

        onTriggered: {
            // 현재시간 기준 1시간 범위, 우클릭 한 카메라, -1 track으로 내보내기 요청
            var now = new Date().getTime()
            mediaControllerView.showExportVideoPopup(now - 3600000, now, [itemUuid + "_-1"])
        }
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem{
        id: connectMenuItem
        text : WisenetLinguist.connect
        visible: isResourceTree

        onTriggered: {
            cameraMenu.changeConnectionStatus(true)
        }
    }
    WisenetMediaMenuItem{
        id: disconnectMenuItem
        text : WisenetLinguist.disconnect
        visible: isResourceTree

        onTriggered: {
            cameraMenu.changeConnectionStatus(false)
        }
    }

    WisenetMediaMenuSeparator {}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: cameraMenu.expandAll(isResourceTree)
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: cameraMenu.collapseAll(isResourceTree)
    }
}
