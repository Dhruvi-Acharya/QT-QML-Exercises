import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import "qrc:/WisenetLanguage"
import "qrc:/WisenetStyle"
import WisenetMediaFramework 1.0

WisenetMediaMenu{
    id: rootMenu

    signal makeGroup()
    signal openInitializeDeviceCredential()
    signal openP2pRegister()
    signal expandAll()
    signal collapseAll()

    Connections{
        target: autoDiscoveryViewModel
        onConnectFailDeviceFound: {
            msg.message = WisenetLinguist.checkExistUnregisterDeviceMessage
            msg.visible = true
        }
    }
    Connections{
        target: manualDiscoveryViewModel
        function onConnectFailDevice(message){
            msg.message = manualDiscoveryViewModel.translate(message)
            msg.visible = true
        }
    }
    Connections{
        target: manualDiscoveryViewModel
        function onConnectFailDeviceRelayMode(){
            console.log("onConnectFailDeviceRelayMode")
            msg.message = WisenetLinguist.systemEventDeviceDisconnectedWithErrorMessage
            msg.visible = true
        }
    }


    Action{
        text : WisenetLinguist.addGroup
        onTriggered: rootMenu.makeGroup()
    }

    Action{
        text : WisenetLinguist.addDevice
        onTriggered: {
            windowService.setupMainView_GotoSetup(WisenetSetupTree.SubPageIndex.DeviceList)
            windowService.setupMainView_SelectAddDevice()
        }
    }

    Action{
        text : WisenetLinguist.addWebpage
        onTriggered: {
            webpageAddViewModel.resetModel()
            windowService.webPageAddView_Open(WisenetLinguist.newWebpage)
        }
    }

    WisenetMediaMenuSeparator{}

    Action {
        text : WisenetLinguist.autoRegister
        onTriggered: {
            if(!localSettingViewModel.autoRegister)
                rootMenu.openInitializeDeviceCredential()
            else
                autoDiscoveryViewModel.startDiscoveryAndRegister("Nvr", localSettingViewModel.autoRegisterId, localSettingViewModel.autoRegisterPassword)
        }
    }
    Action {
        text : WisenetLinguist.ddnsAndP2pRegister
        onTriggered: rootMenu.openP2pRegister()
    }

    WisenetMediaMenuSeparator{}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: rootMenu.expandAll()
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: rootMenu.collapseAll()
    }

    WisenetMessageDialog{
        id : msg
        width: 400
        height: 200
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

}


