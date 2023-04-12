import QtQuick 2.15
import QtQuick.Controls 2.15 as Control2
import QtQml.Models 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0
import WisenetLanguage 1.0

Item {
    id: eventFilterTreeItem
    anchors.fill: parent

    //public variable
    property var treeModel : null
    property alias allEvent: allEventButton.checked
    property var selectedEventValue: null
    property var treeSlectionMode : SelectionMode.MultiSelection
    property var visibleImage : true

    //public function
    function reset(){
        selectedEventValue = null;
        allEventButton.checked = true        
        searchBox.reset()
    }
    function unCheckItem(name){
        var sourceIndex = eventFilterTreeView.model.sourceModel.getIndexByValue(name)
        var filterIndex = eventFilterTreeView.model.mapFromSource(sourceIndex)
        treeItemSelectionModel.select(filterIndex, ItemSelectionModel.Deselect)
    }

    //public signal
    signal selectedEventChanged()

    function getResourceIcon(name){
        if(name === "MotionDetection"){
            return WisenetImage.motionDetection
        }else if(name === "FaceDetection"){
            return WisenetImage.faceDetection
        }else if(name === "Tampering"){
            return WisenetImage.tamperingDetection
        }else if(name === "AudioDetection"){
            return WisenetImage.audioDetection
        }else if(name === "VideoAnalytics"){
            return WisenetImage.iva
        }else if(name === "Tracking"){
            return WisenetImage.autoTracking
        }else if(name === "Tracking.Start"){
            return WisenetImage.autoTracking
        }else if(name === "Tracking.End"){
            return WisenetImage.autoTracking
        }else if(name === "DefocusDetection"){
            return WisenetImage.defocusDetection
        }else if(name === "FogDetection"){
            return WisenetImage.fogDetection
        }else if(name === "AudioAnalytics"){
            return WisenetImage.soundClassification
        }else if(name === "AlarmInput"){
            return WisenetImage.alarmInput
        }else if(name === "Videoloss"){
            return WisenetImage.videoloss
        }else if(name === "Videoloss.Start"){
            return WisenetImage.videoloss
        }else if(name === "Videoloss.End"){
            return WisenetImage.videoloss
        }else if(name === "VideoAnalytics.Passing" || name === "Passing"){
            return WisenetImage.videoAnalyticsPassing
        }else if(name === "VideoAnalytics.Intrusion"){
            return WisenetImage.videoAnalyticsIntrusion
        }else if(name === "VideoAnalytics.Entering" || name === "Entering"){
            return WisenetImage.videoAnalyticsEntering
        }else if(name === "VideoAnalytics.Exiting" || name === "Exiting"){
            return WisenetImage.videoAnalyticsExiting
        }else if(name === "VideoAnalytics.Appearing"){
            return WisenetImage.videoAnalyticsAppearing
        }else if(name === "VideoAnalytics.Disappearing"){
            return WisenetImage.videoAnalyticsDisappearing
        }else if(name === "VideoAnalytics.Loitering"){
            return WisenetImage.videoAnalyticsLoitering
        }else if(name === "AudioAnalytics.Scream"){
            return WisenetImage.audioAnalyticsScream
        }else if(name === "AudioAnalytics.Gunshot"){
            return WisenetImage.audioAnalyticsGunshot
        }else if(name === "AudioAnalytics.Explosion"){
            return WisenetImage.audioAnalyticsExplosion
        }else if(name === "AudioAnalytics.GlassBreak"){
            return WisenetImage.audioAnalyticsGlassBreak
        }else if(name === "NetworkAlarmInput"){
            return WisenetImage.networkAlarmInput
        }

        return WisenetImage.dynamicEvent
    }

    function getDisplayName(name){
        if(name === "MotionDetection"){
            return WisenetLinguist.motionDetection;
        }else if(name === "FaceDetection"){
            return WisenetLinguist.faceDetection;
        }else if(name === "Tampering"){
            return WisenetLinguist.tamperingDetection;
        }else if(name === "AudioDetection"){
            return WisenetLinguist.audioDetection;
        }else if(name === "VideoAnalytics"){
            return WisenetLinguist.iva;
        }else if(name === "Tracking"){
            return WisenetLinguist.autoTracking;
        }else if(name === "Tracking.Start"){
            return WisenetLinguist.autoTrackingStart;
        }else if(name === "Tracking.End"){
            return WisenetLinguist.autoTrackingEnd;
        }else if(name === "DefocusDetection"){
            return WisenetLinguist.defocusDetection;
        }else if(name === "FogDetection"){
            return WisenetLinguist.fogDetection;
        }else if(name === "AudioAnalytics"){
            return WisenetLinguist.soundClassification;
        }else if(name === "AlarmInput"){
            return WisenetLinguist.alarmInput;
        }else if(name === "Videoloss"){
            return WisenetLinguist.videoloss;
        }else if(name === "Videoloss.Start"){
            return WisenetLinguist.videolossStart;
        }else if(name === "Videoloss.End"){
            return WisenetLinguist.videolossEnd;
        }else if(name === "VideoAnalytics.Passing"){
            return WisenetLinguist.videoAnalyticsPassing;
        }else if(name === "VideoAnalytics.Intrusion"){
            return WisenetLinguist.videoAnalyticsIntrusion;
        }else if(name === "VideoAnalytics.Entering"){
            return WisenetLinguist.videoAnalyticsEntering;
        }else if(name === "VideoAnalytics.Exiting"){
            return WisenetLinguist.videoAnalyticsExiting;
        }else if(name === "VideoAnalytics.Appearing"){
            return WisenetLinguist.videoAnalyticsAppearing;
        }else if(name === "VideoAnalytics.Disappearing"){
            return WisenetLinguist.videoAnalyticsDisappearing;
        }else if(name === "VideoAnalytics.Loitering"){
            return WisenetLinguist.videoAnalyticsLoitering;
        }else if(name === "AudioAnalytics.Scream"){
            return WisenetLinguist.audioAnalyticsScream;
        }else if(name === "AudioAnalytics.Gunshot"){
            return WisenetLinguist.audioAnalyticsGunshot;
        }else if(name === "AudioAnalytics.Explosion"){
            return WisenetLinguist.audioAnalyticsExplosion;
        }else if(name === "AudioAnalytics.GlassBreak"){
            return WisenetLinguist.audioAnalyticsGlassBreak;
        }else if(name === "NetworkAlarmInput"){
            return WisenetLinguist.networkAlarmInput;
        }
        else if(name === "SystemEvent.DualSMPSFail")
            return WisenetLinguist.dualSmpsFail;
        else if(name === "SystemEvent.FanError")
            return WisenetLinguist.fanError;
        else if(name === "SystemEvent.CPUFanError")
            return WisenetLinguist.cpuFanError;
        else if(name === "SystemEvent.FrameFanError")
            return WisenetLinguist.frameFanError;
        else if(name === "SystemEvent.LeftFanError")
            return WisenetLinguist.leftFanError;
        else if(name === "SystemEvent.RightFanError")
            return WisenetLinguist.rightFanError;

        else if(name === "SystemEvent.HDDFull")
            return WisenetLinguist.hddFull;
        else if(name === "SystemEvent.SDFull")
            return WisenetLinguist.sdFull;
        else if(name === "SDFull")
            return WisenetLinguist.channelSdFull;
        else if(name === "SystemEvent.NASFull")
            return WisenetLinguist.nasFull;
        else if(name === "SystemEvent.HDDNone")
            return WisenetLinguist.hddNone;
        else if(name === "SystemEvent.HDDFail")
            return WisenetLinguist.hddFail;
        else if(name === "SystemEvent.SDFail")
            return WisenetLinguist.sdFail;
        else if(name === "SystemEvent.NASFail")
            return WisenetLinguist.nasFail;
        else if(name === "SDFail")
            return WisenetLinguist.sdFail;
        else if(name === "SystemEvent.HDDError")
            return WisenetLinguist.hddError;
        else if(name === "SystemEvent.RAIDDegrade")
            return WisenetLinguist.raidDegrade;
        else if(name === "SystemEvent.RAIDRebuildStart")
            return WisenetLinguist.raidRebuildStart;
        else if(name === "SystemEvent.RAIDFail")
            return WisenetLinguist.raidFail;
        else if(name === "SystemEvent.iSCSIDisconnect")
            return WisenetLinguist.iScsiDisconnect;
        else if(name === "SystemEvent.NASDisconnect")
            return WisenetLinguist.nasDisconnect;

        else if(name === "SystemEvent.RecordFiltering")
            return WisenetLinguist.recordFiltering;
        else if(name === "SystemEvent.RecordingError")
            return WisenetLinguist.recordingError;

        else if(name === "SystemEvent.CpuOverload")
            return WisenetLinguist.cpuOverload;
        else if(name === "SystemEvent.NetTxTrafficOverflow")
            return WisenetLinguist.netTxTrafficOverflow;
        else if(name === "SystemEvent.NetCamTrafficOverFlow")
            return WisenetLinguist.netCamTrafficOverFlow;
        else if(name === "SystemEvent.VPUError")
            return WisenetLinguist.vpuError;
        else if(name === "SystemEvent.MemoryError")
            return WisenetLinguist.memoryError;
        else if(name === "SystemEvent.LowFps")
            return WisenetLinguist.lowFps;
        else if(name === "LowFps")
            return WisenetLinguist.channelLowFps;
        else if(name === "SystemEvent.NewFWAvailable")
            return WisenetLinguist.newFwAvailable;

        else if(name === "SystemEvent.CoreService.DeviceDisconnectByService")
            return WisenetLinguist.systemEventDeviceDisconnectByService;
        else if(name === "SystemEvent.CoreService.FirmwareUpgrade")
            return WisenetLinguist.systemEventFirmwareUpgrade;

        else if(name === "User Login")
            return WisenetLinguist.userLoginAuditType
        else if(name === "Backup Settings")
            return WisenetLinguist.backupSettingsAuditType
        else if(name === "Restore Settings")
            return WisenetLinguist.restoreSettingsAuditType
        else if(name === "Initialize Settings")
            return WisenetLinguist.initializeSettingsAuditType
        else if(name === "Log Settings")
            return WisenetLinguist.logSettingsAuditType
        else if(name === "User Settings")
            return WisenetLinguist.userSettingsAuditType
        else if(name === "User Group Settings")
            return WisenetLinguist.userGroupSettingsAuditType
        else if(name === "Event Rule Settings")
            return WisenetLinguist.eventRuleSettingsAuditType
        else if(name === "Schedule Settings")
            return WisenetLinguist.scheduleSettingsAuditType
        else if(name === "Device Settings")
            return WisenetLinguist.deviceSettingsAuditType
        else if(name === "Channel Settings")
            return WisenetLinguist.channelSettingsAuditType
        else if(name === "Device Status")
            return WisenetLinguist.deviceStatusAuditType
        else if(name === "PTZ Control")
            return WisenetLinguist.ptzControlAuditType
        else if(name === "Watching Live")
            return WisenetLinguist.watchingLiveAuditType
        else if(name === "Watching Playback")
            return WisenetLinguist.watchingPlaybackAuditType
        else if(name === "Exporting Video")
            return WisenetLinguist.exportingVideoAuditType
        else if(name === "Software upgrade")
            return WisenetLinguist.updateSoftwareAuditType


        return name;
    }

    function expandAll() {
        //console.log("deviceSelectionTreeItem~~expandAll")

        var rootRowCount = eventFilterTreeView.model.rowCount(eventFilterTreeView.rootIndex)
        //console.log("deviceSelectionTreeItem~~rootRowCount " + rootRowCount)

        for(var i=0; i < rootRowCount; i++) {

            var idx = eventFilterTreeView.model.index(i, 0, eventFilterTreeView.rootIndex)
            eventFilterTreeView.expand(idx)

            var childRowCount = eventFilterTreeView.model.rowCount(idx)
            for(var childRow =0; childRow < childRowCount; childRow++ ){
                expandChild(childRow, idx);
            }
        }

        eventFilterTreeView.resizeColumnsToContents()
    }

    function expandChild(row, parent){
        var idx = eventFilterTreeView.model.index(row, 0, parent);
        var item = eventFilterTreeView.model.data(idx, 0x100 +1);

        //if(item)
        //    console.log("deviceSelectionTreeItem~~expandChild " + row + " " + parent + " parent " + item.displayName)
        //else
        //    console.log("deviceSelectionTreeItem~~expandChild " + row + " " + parent + " va " + item)

        eventFilterTreeView.expand(idx)

        var rowCount = eventFilterTreeView.model.rowCount(idx);

        //console.log("deviceSelectionTreeItem~~RowCount : " + rowCount)

        for(var i=0; i< rowCount; i++){
            expandChild(i, idx);
        }
    }

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            console.debug("onClicked in Popup")
        }
    }


    Rectangle{
        id: eventFilterView
        anchors.fill: parent
        color: WisenetGui.contrast_07_grey



        WisenetSearchBox{
            id: searchBox
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            height: 31
            anchors.margins: 1
            focus: true

            onSearchTextChanged: {
                console.debug("Search Text: " + text)
                eventFilterTreeView.model.filterText = text;
                expandAll();
            }

        }

        Rectangle{
            id: allDeviceArea
            anchors.left: parent.left
            anchors.top: searchBox.bottom
            anchors.right: parent.right
            height: 33
            anchors.margins: 1
            color: WisenetGui.contrast_08_dark_grey

            WisenetAllDeviceCheckBox{
                id: allEventButton
                anchors.fill: parent

                checked : true
                selectedIcon : WisenetImage.all_event_select
                hoveredIcon : WisenetImage.all_event_hover
                normalIcon : WisenetImage.all_event_normal

                Component.onCompleted: {
                    changeCheckBoxText(WisenetLinguist.allEvents)
                }

                onCheckStateChanged: {
                    console.debug("onCheckStateChanged :" + state)
                    if(state){
                        treeItemSelectionModel.clear()
                    }

                    //allEvent = state;
                    //selectedEventChanged()
                }
            }
        }


        Rectangle{
            id: deviceTreeArea
            anchors.left: parent.left
            anchors.top: allDeviceArea.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 1
            color: WisenetGui.contrast_08_dark_grey

            TreeView{
                id: eventFilterTreeView
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.topMargin: 10
                anchors.rightMargin: 5
                anchors.bottomMargin: 10
                headerVisible: false
                alternatingRowColors: false
                frameVisible: false
                model:treeModel

                TableViewColumn {
                    id: tvColumn
                    role: "display"
                    title: "DeviceTree"
                    width: 300
                    resizable: true
                }

                selection: ItemSelectionModel{
                    id: treeItemSelectionModel
                    model: treeModel
                    onSelectedIndexesChanged: {

                        if(!hasSelection || (selectedIndexes.length === 0)){
                            allEventButton.checked = true
                            selectedEventValue = null
                        }else{
                            allEventButton.checked = false
                            selectedEventValue = [];
                            selectedIndexes.forEach(function(item) {
                                selectedEventValue.push(eventFilterTreeView.model.data(item, 0x100 +1));
                            });
                        }
                        selectedEventChanged()
                        console.log("onSelectedIndexesChanged - selectedRows: " + selectedIndexes )
                    }
                }

                selectionMode: treeSlectionMode

                style: WisenetTreeViewStyle {
                    id: treeItemStyle
                    textColor: "white"
                    backgroundColor: WisenetGui.contrast_08_dark_grey

                    itemDelegate: Item{
                        id: itemArea

                        Rectangle {
                            id: resourceItem
                            anchors.leftMargin: styleData.depth >= 1 ? 8 : 4
                            anchors.fill: parent
                            height: 24
                            color : "transparent"

                            Image{
                                id: checkImage
                                anchors.left: parent.left

                                width: 16
                                height: 16
                                visible: ((eventFilterTreeView.model.flags(styleData.index) & Qt.ItemIsSelectable) && (treeSlectionMode === SelectionMode.MultiSelection))
                                anchors.verticalCenter: parent.verticalCenter
                                sourceSize: Qt.size(16, 16)
                                source: styleData.selected ? WisenetImage.check_on : WisenetImage.check_off
                            }

                            Image {
                                id: iconImage

                                anchors.left: checkImage.visible ? checkImage.right : parent.left
                                anchors.leftMargin: checkImage.visible ? 6 : 0
                                anchors.verticalCenter: parent.verticalCenter

                                width: 16
                                height: 16
                                visible: eventFilterTreeItem.visibleImage
                                sourceSize: Qt.size(16, 16)
                                source: !styleData.value || getResourceIcon(styleData.value.uuid)===undefined ? "" : getResourceIcon(styleData.value.uuid)
                            }

                            Text{
                                id: resourceText
                                anchors.left: iconImage.right
                                anchors.leftMargin: 4
                                anchors.rightMargin: 5
                                text: !styleData.value ? "" : styleData.value.displayName + "                        "
                                color: WisenetGui.contrast_02_light_grey
                                visible: true
                                anchors.verticalCenter: parent.verticalCenter
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft

                                font.pixelSize: 12
                                elide: Qt.ElideRight
                            }
                        }
                    }

                    rowDelegate: Rectangle {
                        id: rowD
                        height: 24
                        color: styleData.selected === true ? WisenetGui.contrast_07_grey : "transparent"
                    }

                    branchDelegate: Item {
                        width: 20
                        height: 24

                        Image {
                            id: branchImage
                            visible: styleData.column === 0 && styleData.hasChildren
                            anchors.centerIn: parent
                            sourceSize: Qt.size(14, 14)
                            source: styleData.isExpanded ? WisenetImage.tree_branch_expanded : WisenetImage.tree_branch_collapsed
                        }
                    }

                    //scrollToClickedPosition: true
                    transientScrollBars: true
                }

                onDoubleClicked: {

                    if(eventFilterTreeView.isExpanded(index)){
                        eventFilterTreeView.collapse(index)
                    }
                    else{
                        eventFilterTreeView.expand(index)
                        console.log(index);
                    }
                }
            }
        }
    }
}
