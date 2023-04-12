import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "Popup"
import "Popup/Setup/Device"

Item {
    id: setupParent

    Connections{
        target: setupService

        onSetupMainView_SetSetupVisible: setupMainView.setVisible(visible)
        onSetupMainView_GotoHome: setupMainView.gotoHome()
        onSetupMainView_GotoSetup: setupMainView.gotoSetup(menu)
        onSetupMainView_SelectDeviceList: setupMainView.selectDeviceList(channels)
        onSetupMainView_SelectDeviceListChannel: setupMainView.selectDeviceListChannel(channels)
        onSetupMainView_SearchEventLog: setupMainView.searchEventLog()
        onSetupMainView_SelectAddDevice: setupMainView.selectAddDevice()

        onCloseAll: setupMainView.close()

        onInitializeTree: setupMainView.initializeTree()
    }

    SetupMainView{
        id: setupMainView

        function setVisible(visibility){

            if(visible && visibility){
                if(setupMainView.visibility === Window.Minimized)
                {
                    setupMainView.showNormal()
                }

                setupMainView.raise()
                return
            }

            setupMainView.visible = visibility
        }

        visible: false

        onOpenDdnsP2pSetup: {
            setupService.setupMainView_openP2p()
        }
    }
}
