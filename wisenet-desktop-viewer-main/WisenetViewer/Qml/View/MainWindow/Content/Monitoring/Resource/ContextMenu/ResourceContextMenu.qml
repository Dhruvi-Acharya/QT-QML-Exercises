import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import Wisenet.Define 1.0

Item {
    id: resourceContextMenu

    // To screen
    signal addNewGroup(var groupId)
    signal addNewVideoChannel(var deviceId, var channelId)
    signal addNewWebpage(var webpageId)
    signal addNewLocalFile(var localFileUrl)

    // Layout select or add
    signal selectLayout(var type, var name, var uuid, var layoutViewModel)
    signal selectSequence(var type, var name, var uuid)

    // To Resource tree view
    signal makeGroup();
    signal moveGroup(var groupId)
    signal resourceTreeDeleteItems(var remove)
    signal openMediaFileDialog()

    // To layout tree view
    signal layoutTreeDeleteItems()

    // Rename
    signal openRenameDialog(var itemType, var uuid, var name)

    // Multi items - resource tree
    signal openResourceTreeItems()
    signal openResourceTreeItemsNewWindow()
    signal newGroupWithSelection()
    signal exportVideoWithSelection()

    // Multi items - layout tree
    signal openLayoutTreeItems()
    signal openLayoutTreeItemsNewWindow()
    signal verifyWnmItems()

    // Camera settings
    signal openResourceTreeCameraSettings()
    signal openLayoutTreeCameraSettings()

    // 초기 비밀번호 설정
    signal openInitializeDeviceCredential()
    // P2P register
    signal openP2pRegister()

    // Tree
    signal expandResourceTree()
    signal collapseResourceTree()
    signal expandLayoutTree()
    signal collapseLayoutTree()

    signal changeConnectionStatus(var connections)

    function popupMenu(parent, x, y, isResourceTree, layoutViewModel)
    {
        switch(parent.itemType){
        case ItemType.ResourceRoot:
            if(userGroupModel.isAdmin === false)
                return;

            rootMenu.popup(parent, x, y)
            break;
        case ItemType.Group:
            groupMenu.itemUuid = parent.uuid
            groupMenu.displayName = parent.displayName
            console.log("group UUID", parent.uuid);
            groupMenu.setMenuVisible()
            groupMenu.popup(parent, x, y)
            break;
        case ItemType.Camera:
            cameraMenu.itemUuid = parent.uuid
            cameraMenu.displayName = parent.displayName
            cameraMenu.parentId = parent.deviceUuid
            cameraMenu.channelNumber = parent.channelNumber
            cameraMenu.setMenuVisible(isResourceTree)
            cameraMenu.popup(parent, x, y)
            cameraMenu.isResourceTree = isResourceTree
            break;
        case ItemType.WebPage:
            webpageMenu.itemUuid = parent.uuid
            webpageMenu.displayName = parent.displayName
            webpageMenu.setMenuVisible(isResourceTree)
            webpageMenu.popup(parent, x, y)
            webpageMenu.isResourceTree = isResourceTree
            break;
        case ItemType.LocalFileRoot:
            localFileRootMenu.popup(parent, x, y)
            break;
        case ItemType.LocalFile:
            localFileMenu.itemUuid = parent.uuid
            localFileMenu.useBackgroundVisible = resourceContextMenu.isImageFile(parent.uuid)
            localFileMenu.verifyMenuVisible = resourceContextMenu.isWnmFile(parent.uuid)
            localFileMenu.popup(parent, x, y)
            localFileMenu.isResourceTree = isResourceTree
            break;
        case ItemType.LayoutRoot:
            layoutRootMenu.popup(parent, x, y)
            break;
        case ItemType.Layout:
            layoutMenu.itemUuid = parent.uuid
            layoutMenu.displayName = parent.displayName
            layoutMenu.saveStatus = parent.saveStatus
            layoutMenu.treeItem = parent
            layoutMenu.layoutModel = layoutViewModel
            layoutMenu.setMenuVisible()
            layoutMenu.popup(parent, x, y)
            break;
        case ItemType.SequenceRoot:
            sequenceRootMenu.popup(parent, x, y)
            break;
        case ItemType.Sequence:
            sequenceMenu.itemUuid = parent.uuid
            sequenceMenu.displayName = parent.displayName
            sequenceMenu.treeItem = parent
            sequenceMenu.popup(parent, x, y)
            break;
        default:
            sortMenu.popup(parent, x, y)
            break;
        }

        console.log("parent : " + parent + " x,y : "+ x + "," + y + " type : " + parent.resourceType + " Uuid : " + parent.uuid)
    }

    function popupMultiItemMenu(parent, selectionModel, x, y, isResourceTree, deleteVisible = true){
        multiItemMenu.isResourceTree = isResourceTree
        multiItemMenu.deleteVisible = deleteVisible
        multiItemMenu.verifyMenuVisible = false
        for(let index of selectionModel.selectedIndexes)
        {
            console.log("selectedItemIndex ::", index);
            var item = resourceViewModel.treeModel.data(resourceViewModel.treeProxyModel.mapToSource(index), 0x0100+1);
            if(resourceContextMenu.isWnmFile(item.uuid)){
                multiItemMenu.verifyMenuVisible= true
                break
            }
        }
        multiItemMenu.popup(parent, x, y)
    }

    function popupMultiChannelMenu(parent, x, y, cameraSettingVisible, isResourceTree){
        multiChannelMenu.isResourceTree = isResourceTree
        multiChannelMenu.setMoveGroupVisible(isResourceTree)
        multiChannelMenu.cameraSettingVisible = cameraSettingVisible
        multiChannelMenu.exportVideoVisible = cameraSettingVisible && isResourceTree && userGroupModel.exportVideo

        if(userGroupModel.isAdmin === false)
        {
            multiChannelMenu.setMoveGroupVisible(false)
            multiChannelMenu.cameraSettingVisible = false
        }

        multiChannelMenu.popup(parent, x, y)
    }

    function popupExpandMenu(parent, x, y, isResourceTree){
        sortMenu.isResourceTree = isResourceTree
        sortMenu.popup(parent, x, y)
    }

    function isImageFile(fileName){
        var fileSplit = fileName.split('.');
        var ext = fileSplit[fileSplit.length-1];

        //console.log("isImageFile ", fileName, ext)

        if(ext === "png" || ext === "jpeg" || ext === "jpg" || ext === "bmp")
            return true

        return false
    }

    function isWnmFile(itemUuid){
        var fileSplit = itemUuid.split('.');
        var ext = fileSplit[fileSplit.length-1];
        console.log("isWnmFile ", itemUuid, ext)

        if(ext === "wnm")
            return true
        else
            return false
    }

    RootMenu{
        id: rootMenu

        onMakeGroup: resourceContextMenu.makeGroup()
        onOpenInitializeDeviceCredential: resourceContextMenu.openInitializeDeviceCredential()
        onOpenP2pRegister: resourceContextMenu.openP2pRegister()

        onExpandAll: resourceContextMenu.expandResourceTree()
        onCollapseAll: resourceContextMenu.collapseResourceTree()
    }

    GroupMenu{
        id: groupMenu

        onAddNewGroup: resourceContextMenu.addNewGroup(groupId)
        onResourceTreeDeleteItems: resourceContextMenu.resourceTreeDeleteItems(remove)
        onMakeGroup: resourceContextMenu.makeGroup()
        onOpenItemsNewWindow: resourceContextMenu.openResourceTreeItemsNewWindow()
        onOpenRenameDialog: resourceContextMenu.openRenameDialog(ItemType.Group, uuid, name)

        onExpandAll: resourceContextMenu.expandResourceTree()
        onCollapseAll: resourceContextMenu.collapseResourceTree()
    }

    CameraMenu{
        id: cameraMenu

        onAddNewVideoChannel : resourceContextMenu.addNewVideoChannel(deviceId, channelId)
        onMoveGroup : resourceContextMenu.moveGroup(groupId)
        onResourceTreeDeleteItems : resourceContextMenu.resourceTreeDeleteItems(remove)
        onLayoutTreeDeleteItems : resourceContextMenu.layoutTreeDeleteItems()

        onOpenRenameDialog: resourceContextMenu.openRenameDialog(ItemType.Camera, uuid, name)

        onChangeConnectionStatus: {
            console.log("onConnnectDevices1")
            resourceContextMenu.changeConnectionStatus(connections)
        }


        onExpandAll: {
            if(isResourceTree)
                resourceContextMenu.expandResourceTree()
            else
                resourceContextMenu.expandLayoutTree()
        }

        onCollapseAll: {
            if(isResourceTree)
                resourceContextMenu.collapseResourceTree()
            else
                resourceContextMenu.collapseLayoutTree()
        }
    }

    WebpageMenu{
        id: webpageMenu

        onAddNewWebpage: resourceContextMenu.addNewWebpage(webpageId)
        onMoveGroup : resourceContextMenu.moveGroup(groupId)
        onResourceTreeDeleteItems : resourceContextMenu.resourceTreeDeleteItems(remove)
        onLayoutTreeDeleteItems : resourceContextMenu.layoutTreeDeleteItems()

        onOpenRenameDialog: resourceContextMenu.openRenameDialog(ItemType.WebPage, uuid, name)

        onExpandAll: {
            if(isResourceTree)
                resourceContextMenu.expandResourceTree()
            else
                resourceContextMenu.expandLayoutTree()
        }

        onCollapseAll: {
            if(isResourceTree)
                resourceContextMenu.collapseResourceTree()
            else
                resourceContextMenu.collapseLayoutTree()
        }
    }

    LocalFileRootMenu{
        id: localFileRootMenu

        //onOpenMediaFileDialog : resourceContextMenu.openMediaFileDialog()
        onExpandAll: resourceContextMenu.expandResourceTree()
        onCollapseAll: resourceContextMenu.collapseResourceTree()
    }

    LocalFileMenu{
        id: localFileMenu

        onAddNewLocalFile : resourceContextMenu.addNewLocalFile(localFileUrl)
        onResourceTreeDeleteItems : resourceContextMenu.resourceTreeDeleteItems(remove)
        onLayoutTreeDeleteItems : resourceContextMenu.layoutTreeDeleteItems()

        onExpandAll: {
            if(isResourceTree)
                resourceContextMenu.expandResourceTree()
            else
                resourceContextMenu.expandLayoutTree()
        }

        onCollapseAll: {
            if(isResourceTree)
                resourceContextMenu.collapseResourceTree()
            else
                resourceContextMenu.collapseLayoutTree()
        }
    }

    LayoutRootMenu{
        id: layoutRootMenu

        onExpandAll: resourceContextMenu.expandLayoutTree()
        onCollapseAll: resourceContextMenu.collapseLayoutTree()
    }

    LayoutMenu{
        id: layoutMenu

        onSelectLayout : resourceContextMenu.selectLayout(type, name, uuid, layoutViewModel)
        onLayoutTreeDeleteItems : resourceContextMenu.layoutTreeDeleteItems()

        onOpenRenameDialog: resourceContextMenu.openRenameDialog(ItemType.Layout, uuid, name)

        onExpandAll: resourceContextMenu.expandLayoutTree()
        onCollapseAll: resourceContextMenu.collapseLayoutTree()
    }

    SequenceRootMenu{
        id: sequenceRootMenu

        onExpandAll: resourceContextMenu.expandLayoutTree()
        onCollapseAll: resourceContextMenu.collapseLayoutTree()
    }

    SequenceMenu{
        id: sequenceMenu

        onSelectSequence : resourceContextMenu.selectSequence(type, name, uuid)
        onExpandAll: resourceContextMenu.expandLayoutTree()
        onCollapseAll: resourceContextMenu.collapseLayoutTree()
    }

    SortMenu{
        id: sortMenu

        onExpandAll: {
            if(isResourceTree)
                resourceContextMenu.expandResourceTree()
            else
                resourceContextMenu.expandLayoutTree()
        }

        onCollapseAll: {
            if(isResourceTree)
                resourceContextMenu.collapseResourceTree()
            else
                resourceContextMenu.collapseLayoutTree()
        }
    }

    MultiItemMenu{
        id: multiItemMenu

        onOpenItems : {
            if(isResourceTree)
                resourceContextMenu.openResourceTreeItems()
            else
                resourceContextMenu.openLayoutTreeItems()
        }
        onOpenItemsNewWindow : {
            if(isResourceTree)
                resourceContextMenu.openResourceTreeItemsNewWindow()
            else
                resourceContextMenu.openLayoutTreeItemsNewWindow()
        }
        onDeleteItems : {
            if(isResourceTree)
                resourceContextMenu.resourceTreeDeleteItems(remove)
            else
                resourceContextMenu.layoutTreeDeleteItems()
        }

        onExpandAll: {
            if(isResourceTree)
                resourceContextMenu.expandResourceTree()
            else
                resourceContextMenu.expandLayoutTree()
        }

        onCollapseAll: {
            if(isResourceTree)
                resourceContextMenu.collapseResourceTree()
            else
                resourceContextMenu.collapseLayoutTree()
        }

        onVerifyAll: {
            if(isResourceTree) {
                resourceContextMenu.verifyWnmItems()
            }
        }
    }

    MultiChannelMenu{
        id: multiChannelMenu

        onOpenItems : {
            if(isResourceTree)
                resourceContextMenu.openResourceTreeItems()
            else
                resourceContextMenu.openLayoutTreeItems()
        }
        onOpenItemsNewWindow : {
            if(isResourceTree)
                resourceContextMenu.openResourceTreeItemsNewWindow()
            else
                resourceContextMenu.openLayoutTreeItemsNewWindow()
        }
        onDeleteItems : {
            if(isResourceTree)
                resourceContextMenu.resourceTreeDeleteItems(remove)
            else
                resourceContextMenu.layoutTreeDeleteItems()
        }
        onMoveGroup: resourceContextMenu.moveGroup(groupId)
        onNewGroupWithSelection: resourceContextMenu.newGroupWithSelection()
        onExportVideoWithSelection: resourceContextMenu.exportVideoWithSelection()

        onOpenResourceTreeCameraSettings: resourceContextMenu.openResourceTreeCameraSettings()
        onOpenLayoutTreeCameraSettings: resourceContextMenu.openLayoutTreeCameraSettings()

        onExpandAll: {
            if(isResourceTree)
                resourceContextMenu.expandResourceTree()
            else
                resourceContextMenu.expandLayoutTree()
        }

        onCollapseAll: {
            if(isResourceTree)
                resourceContextMenu.collapseResourceTree()
            else
                resourceContextMenu.collapseLayoutTree()
        }
        onChangeConnectionStatus: {
            resourceContextMenu.changeConnectionStatus(connections)
        }
    }
}
