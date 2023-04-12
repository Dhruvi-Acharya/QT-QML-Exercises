import QtQuick 2.15
import WisenetStyle 1.0
import Wisenet.Tree 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.UserLayoutTreeSourceModel 1.0
import Wisenet.Setup 1.0
import QtQml.Models 2.15
import WisenetLanguage 1.0

Grid{
    id : root
    property var resourceDeviceValue: null
    property alias userGroupView: addUserGroupViewModel
    property string groupGuid : ""
    signal resultMessage(bool success, string result)

    property bool isGroupView: true
    property bool isOwner : false


    x: 30
    topPadding: 20
    leftPadding: isGroupView? 30 : 0

    width: parent.width
    height: parent.height - 100
    //leftPadding: 35
    columns: 2
    rows : (isGroupView? 3 : 4)
    spacing: 20
    columnSpacing: 20
    visible: false
    enabled : true

    onVisibleChanged: {
        if(visible) {
            addUserGroupViewModel.readUserGroup(groupGuid, root.isGroupView);
            var prevGroupName = addUserGroupViewModel.groupName
            groupNameCombo.model = addUserGroupViewModel.groupNameList
            var prevIndex = groupNameCombo.currentIndex;
            if(prevGroupName.length > 0){
                groupNameCombo.currentIndex = groupNameCombo.indexOfValue(prevGroupName)

            }
            else {
                groupNameCombo.currentIndex = 0
            }
            if(prevIndex == groupNameCombo.currentIndex)
                reset()
            else if(isGroupView)
                reset()
        }
        else{
            groupNameCombo.model = {}
        }
    }

    AddUserGroupViewModel{
        id:addUserGroupViewModel
    }

   function selectDeviceTree(guid){
        var modelIndex = addUserGroupDeviceTreeSourceModel.getModelIndex(guid, deviceSeletectionTree.treeItemSelectionModelVal)
        var proxyIndex = addUserGroupDeviceTreeProxyModel.mapFromSource(modelIndex)
        deviceSeletectionTree.treeItemSelectionModelVal.select(proxyIndex,  ItemSelectionModel.Select)
    }
    function selectLayoutTree(guid){
        var modelIndex = addUserGroupLayoutTreeSourceModel.getModelIndex(guid, layoutSeletectionTree.treeItemSelectionModelVal)
        var proxyIndex = addUserGroupLayoutTreeProxyModel.mapFromSource(modelIndex)
        layoutSeletectionTree.treeItemSelectionModelVal.select(proxyIndex,  ItemSelectionModel.Select)
    }

    function checkIsEditable(){
        if(addUserGroupViewModel.isAdminGroup){
            return false
        }
        if(root.isOwner){
            return false
        }
        if(root.groupGuid=="")
            return true

        if(groupNameCombo.visible && groupNameCombo.currentValue !== "New user group"){
            return false
        }
        else{
            return true
        }
    }

    function checkIsCombo(){
        if(root.isOwner){
            return false
        }
        else
            return true
    }

    function checkIsEditableLayout(){
        if(addUserGroupViewModel.isAdminGroup){
            return true
        }
        if(root.isOwner){
            return false
        }
        if(root.groupGuid=="")
            return true
        if(groupNameCombo.visible && groupNameCombo.currentText !== "New user group")
            return false

        else
            return true
    }

    function acceptAddGroup(){
        var str = groupName.text
        if(str.replace(/\s/g, '').length === 0){
            resultMessage(false, WisenetLinguist.pleaseInputUserGroupName)
        }
        else{
            addUserGroupViewModel.groupName = groupName.text

            var itemGuidList = []

            if(deviceSeletectionTree.selectedDeviceValue){
                deviceSeletectionTree.selectedDeviceValue.forEach(function(item) {
                    itemGuidList.push(item.uuid);
                });
            }

            var itemLayoutGuidList = []

            if(layoutSeletectionTree.selectedDeviceValue){
                layoutSeletectionTree.selectedDeviceValue.forEach(function(item) {
                    itemLayoutGuidList.push(item.uuid);
                });
            }

            addUserGroupViewModel.playback = playbackId.checked
            addUserGroupViewModel.exportVideo = exportVideoId.checked
            addUserGroupViewModel.localRecording = localRecordingId.checked
            addUserGroupViewModel.ptzControl = ptzControlId.checked
            addUserGroupViewModel.audio = audioId.checked
            addUserGroupViewModel.mic = micId.checked
            addUserGroupViewModel.saveUserGroup(deviceSeletectionTree.allDevice, itemGuidList, itemLayoutGuidList)//, resourceDeviceValue);
        }
    }

    function isGroupSelect(){
        if(groupName.text.length == 0)
            return false
        else
            return true
    }

    function reset(){
            deviceSeletectionTree.reset()
            layoutSeletectionTree.reset()

            addUserGroupViewModel.readUserGroup(groupGuid, root.isGroupView);


            playbackId.checked = addUserGroupViewModel.playback;
            exportVideoId.checked = addUserGroupViewModel.exportVideo;
            localRecordingId.checked = addUserGroupViewModel.localRecording;
            ptzControlId.checked = addUserGroupViewModel.ptzControl;
            audioId.checked = addUserGroupViewModel.audio;
            micId.checked = addUserGroupViewModel.mic;

            deviceSeletectionTree.allDevice = addUserGroupViewModel.isAllResource

            deviceSeletectionTree.expandAll()
            var channelGuidList = addUserGroupViewModel.channelGuidList
            channelGuidList.forEach(function(guid) {
                selectDeviceTree(guid)
            });

            layoutSeletectionTree.expandAll()
            var layoutGuidList = addUserGroupViewModel.layoutGuidList
            layoutGuidList.forEach(function(guid) {
                selectLayoutTree(guid)
            });

            var editItem = checkIsEditable()
            var editLayout = checkIsEditableLayout()

            groupNameCombo.enabled = checkIsCombo()
            groupName.enabled = editItem
            permisssionGrid.enabled = editItem
            resoureRect.enabled = editItem
            layoutRect.enabled = editLayout

            setupId.checked = (groupName.text == "administrator")? true:false
        }

    WisenetLabel{
        id : groupNameComboLabel
        text : WisenetLinguist.group
        visible: !root.isGroupView
    }

    WisenetComboBox{
        id : groupNameCombo
        visible: groupNameComboLabel.visible
        enabled: !isOwner
        onCurrentValueChanged: {
            if(groupNameCombo.currentText !== "New user group"){
                groupGuid = addUserGroupViewModel.findUserGroupId(groupNameCombo.currentText)
            }
            else
                groupGuid = ""
            reset()
        }
    }

    WisenetLabel{
        text : WisenetLinguist.groupName
    }
    WisenetTextBox{
        id : groupName
        text: addUserGroupViewModel.groupName
    }

    WisenetLabel{
        id: permission
        text : WisenetLinguist.permission
    }

    Grid{
        id : permisssionGrid
        width: parent.width - 100
        columns: 7
        rows : 1
        spacing: 30
        columnSpacing: 70

        WisenetCheckBox{
            id : playbackId
            text : WisenetLinguist.playback
            onCheckedChanged: {
                if(checked)
                    exportVideoId.enabled = true;
                else{
                    exportVideoId.enabled = false;
                    exportVideoId.checked = false;
                }
            }
        }
        WisenetCheckBox{
            id : exportVideoId
            enabled: false
            text : WisenetLinguist.exportVideo
        }
        WisenetCheckBox{
            id : localRecordingId
            text : WisenetLinguist.localRecording
        }
        WisenetCheckBox{
            id : ptzControlId
            text : WisenetLinguist.ptzControl
        }
        WisenetCheckBox{
            id : audioId
            text : WisenetLinguist.audio
        }
        WisenetCheckBox{
            id : micId
            text : WisenetLinguist.mic
        }
        WisenetCheckBox{
            id : setupId
            text : WisenetLinguist.setup
            enabled: false
        }
    }

    WisenetLabel{
        id: resource
        text : WisenetLinguist.resource
    }
    Grid{
        rows : 1
        columns: 3
        columnSpacing: 70
        id : grid

        property color borderColor : WisenetGui.contrast_06_grey

        Rectangle{
            id : resoureRect
            width: WisenetGui.popupDeviceTreeDefaultWidth
            height: root.height -120
            border.width: 1
            border.color: parent.borderColor

            ResourceTreeProxyModel{
                id: addUserGroupDeviceTreeProxyModel
                sourceModel: DevicePhysicalTreeSourceModel{
                    id: addUserGroupDeviceTreeSourceModel
                }
                Component.onCompleted: {
                    doSort(Qt.AscendingOrder)
                }
            }

            WisenetDeviceSelectionTree{
                id: deviceSeletectionTree
                anchors.fill: parent
                treeModel: addUserGroupDeviceTreeProxyModel
                borderColor: grid.borderColor
            }
        }
        WisenetText{
            text : WisenetLinguist.sharedLayout
        }
        Rectangle{
            id : layoutRect
            width: WisenetGui.popupDeviceTreeDefaultWidth
            height: root.height -120
            border.width: 1
            border.color: parent.borderColor

            ResourceTreeProxyModel{
                id: addUserGroupLayoutTreeProxyModel
                sourceModel: UserLayoutTreeSourceModel{
                    id: addUserGroupLayoutTreeSourceModel
                }
            }

            WisenetDeviceSelectionTree{
                id: layoutSeletectionTree
                anchors.fill: parent
                borderColor: grid.borderColor
                treeModel: addUserGroupLayoutTreeProxyModel
                state: "Layout"
            }
        }
    }
}
