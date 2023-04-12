import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Qt.labs.qmlmodels 1.0
import QtQuick.Dialogs 1.3
import Wisenet.Setup 1.0
import QtQml.Models 2.15
import Wisenet.DeviceListTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

Page{
    id : deviceListViewPage

    property int xMargin : 15
    property int yMargin: 20
    visible: true

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    function reset(){
        console.log("deviceListView reset")

        deviceListSeletectionTree.reset()
        deviceListDeviceView.visible = true
        deviceListChannelView.visible = false
    }
    DeviceListViewModel{
        id : deviceListViewModel
    }
    ChannelListViewModel{
        id : channelListViewModel
    }

    onVisibleChanged: {
        console.log("DeviceListView visible:", visible)
        if(visible == true) reset()
    }

    Component.onCompleted: {
        if(visible == true) reset()
    }

    function selectDeviceTree(guid){
        var modelIndex = deviceListTreeSourceModel.getModelIndex(guid, deviceListSeletectionTree.treeItemSelectionModelVal)
        var proxyIndex = deviceTreeProxyModel.mapFromSource(modelIndex)
        deviceListSeletectionTree.treeItemSelectionModelVal.select(proxyIndex,  ItemSelectionModel.Select)
    }

    function selectDevices(devices){
        deviceListSeletectionTree.reset()

        devices.forEach(function(guid){
            selectDeviceTree(guid)
        });
        //deviceListDeviceView.visible = false
        //deviceListChannelView.visible = false


        //deviceListViewModel.selectDevices(devices)
        //deviceListDeviceView.visible = true
    }

    function selectChannels(channels){
        deviceListSeletectionTree.reset()
        channels.forEach(function(guid){
            selectDeviceTree(guid)
        });
        //deviceListDeviceView.visible = false
        //deviceListChannelView.visible = false
        //channelListViewModel.selectChannels(channels)
        //deviceListChannelView.visible = true
    }

    function selectAddDevice(){
        deviceRegister.visible = true
    }

    function initializeTree(){
        deviceListTreeSourceModel.readDevices()
    }

    Connections{
        target: deviceListTreeSourceModel
        function onExpand(){
            console.log("deviceTreeSourceModel onExpand:")
            deviceListSeletectionTree.expandAll()
        }
    }

    Connections{
        target: deviceListViewModel
        function onResultMessage(success, msg){
            applyResultMessage.showMessage(success? WisenetLinguist.success : msg)
        }
        function onRemovedDevice(){
            console.log("[DeviceListView] onRemovedDevice")
            deviceListViewModel.reset()
            deviceListDeviceView.readDevice()
        }
    }
    Connections{
        target: channelListViewModel
        function onApplyResult(success){
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }
    }

    Connections{
        target: deviceListSeletectionTree
        function onSelectedDeviceChanged(){
            var guidList = []
            if(deviceListSeletectionTree.selectedDeviceValue){
                deviceListSeletectionTree.selectedDeviceValue.forEach(function(item) {
                    guidList.push(item.uuid);
                    console.log("DeviceListView onSelectedDeviceChanged:", item.uuid)
                });
                deviceListViewModel.selectDevices(guidList)
                channelListViewModel.selectChannels(guidList)

                if(deviceListViewModel.deviceCount > 0 || channelListViewModel.channelCount===0){
                    deviceListDeviceView.visible = true
                    deviceListChannelView.visible = false
                    deviceListDeviceView.readDevice()
                }
                else{
                    deviceListChannelView.visible = true
                    deviceListDeviceView.visible = false
                    deviceListChannelView.readChannel()
                }
            }
        }
    }

    DeviceRegisterView{
        id : deviceRegister
        width: WisenetGui.popupSecondWindowDefaultWidth
        height: WisenetGui.popupSecondWindowDefaultHeight
        visible: false
    }

    WisenetDivider{
        id : divider
        x : xMargin
        y : 20
        Layout.fillWidth: true
        label: WisenetLinguist.deviceList
    }

    // Apply 적용 결과
    WisenetSetupApplyResultMessage{
        id: applyResultMessage
        anchors {
            right : divider.right
            rightMargin: 30
            verticalCenter: divider.bottom
        }
    }

    Row {
        id: buttonLayout
        Layout.fillWidth: true
        anchors.left: divider.left
        anchors.top: divider.bottom
        anchors.topMargin: 20

        WisenetSetupAddButton{
            text : WisenetLinguist.add
            onClicked: {
                deviceRegister.visible = true
            }
        }
        WisenetSetupDeleteButton{
            id : deleteButton
            enabled: (deviceListViewModel.deviceCount > 0 )
            text : WisenetLinguist.deleteText
            onClicked: {
                if (!deleteDialog.dontAskAgain) {
                    deleteDialog.open();
                }
                else {
                    deviceListViewModel.deleteDevices()
                }
            }
        }
    }

    WisenetSetupAskDialog {
        id: deleteDialog
        msg : WisenetLinguist.deviceDeleteConfirmMessage
        onAccepted: {
            deviceListViewModel.deleteDevices()
        }
        onRejected: {
            dontAskAgain = false
        }
    }

    DeviceListTreeSourceModel{
        id: deviceListTreeSourceModel
    }

    Rectangle{
        id : resoureRect
        width: 280
        height: deviceListViewPage.height - 120

        anchors.left: buttonLayout.left
        anchors.top: buttonLayout.bottom

        ResourceTreeProxyModel{
            id: deviceTreeProxyModel
            sourceModel: deviceListTreeSourceModel

            Component.onCompleted: {
                doSort(Qt.AscendingOrder)
            }
        }

        WisenetDeviceSelectionTree{
            id: deviceListSeletectionTree
            anchors.fill: parent
            treeModel: deviceTreeProxyModel
            state: "DeviceList"
            bgColor: WisenetGui.contrast_10_dark
        }

    }
    Rectangle{
        anchors.left: resoureRect.right
        anchors.leftMargin: 10
        anchors.top: resoureRect.top
        width: parent.width - resoureRect.width -(xMargin *2) - 10
        height: resoureRect.height
        color: WisenetGui.contrast_10_dark

        DeviceListDeviceView{
            id : deviceListDeviceView
            anchors.fill: parent
            visible: false
        }
        DeviceListChannelView{
            id : deviceListChannelView
            anchors.fill: parent
            visible: false
        }
    }

}
