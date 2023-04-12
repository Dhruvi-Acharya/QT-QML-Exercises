import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "DeviceRegister.js" as DeviceRegister

WisenetMessageDialog{
    id : root
    property int margin : 10
    property int mainTitleHeight: 33

    applyClose: false
    applyButtonText: WisenetLinguist.ok

    onApplyButtonClicked: {
        console.log("IpConfiguratation accept")
        if(root.checkInputParam()){
            console.log("IpConfiguratation accept check success")
            myAutoDiscoveryViewModel.startIpSetting(checkDhcp.checked, inputDeviceID.text, inputDevicePassword.text,
                                                  port.text, startIp.text,
                                                  subnetMask.text, gateway.text);
            root.close()
        }
    }
    onVisibleChanged: {
        if(visible){
            inputDeviceID.text = localSettingViewModel.autoRegisterId
            inputDevicePassword.text = localSettingViewModel.autoRegisterPassword
            startIp.text = myAutoDiscoveryViewModel.getFirstItemIp()
            savePassword.checked = localSettingViewModel.autoRegister
        }
    }

    function isInvalidIp(ip){
        return false;
    }

    function checkInputParam(){
        if(inputDeviceID.text =="" || inputDeviceID.text.length < 1)
            return false;
        if(inputDevicePassword.text =="" || inputDevicePassword.text.length < 1)
            return false;
        else if(port.text =="")
            return false;
        else if(!checkDhcp.checked){
            if(isInvalidIp(startIp.text))
                return false;
            else if(isInvalidIp(subnetMask.text))
                return false;
            else if(isInvalidIp(gateway.text))
                return false;
        }
        return true;
    }

    Column{
        id : inputLayout
        anchors.fill: parent
        spacing: 40

        property int defaultPadding: 34
        topPadding: 20
        leftPadding: defaultPadding
        rightPadding: defaultPadding

        property int cellDefaultWidth: 120
        property int cellDefaultHeight: 40
        property int cellDefaultSpacing: 10
        Row{
            spacing: inputLayout.cellDefaultSpacing
            WisenetLabel{
                id: txtID
                text: WisenetLinguist.id_
            }

            WisenetTextBox{
                id : inputDeviceID
                text:localSettingViewModel.autoRegisterId
                placeholderText:WisenetLinguist.inputId
                width: 228
                Layout.alignment: Qt.AlignVCenter
            }
        }
        Row{
            spacing: inputLayout.cellDefaultSpacing
            WisenetLabel {
                id: txtPassword
                text: WisenetLinguist.password
            }
            WisenetPasswordBox{
                id : inputDevicePassword
                text : localSettingViewModel.autoRegisterPassword
                width: 228
                showPasswordButtonVisible: false
            }
        }

        Row{
            spacing: inputLayout.cellDefaultSpacing
            WisenetLabel{
                id: txtDhcp
                text: "DHCP"
            }

            WisenetCheckBox2{
                id : checkDhcp
                text :WisenetLinguist.enable
                checked: false
            }
        }
        Row{
            spacing: inputLayout.cellDefaultSpacing
            WisenetLabel{
                id : startIpText
                text: "IP"
                visible: !checkDhcp.checked
            }
            IpControl{
                id : startIp
                visible: !checkDhcp.checked
                text : "192.168.255.50"
            }

            WisenetLabel{
                text : "~"
                width: 20
                visible: !checkDhcp.checked
            }
            IpControl{
                id : endIp
                visible: !checkDhcp.checked
                text : startIp.text

            }
        }
        Row{
            spacing: inputLayout.cellDefaultSpacing
            WisenetLabel{
                id : subnetMaskText
                visible: !checkDhcp.checked
                text: WisenetLinguist.subnetMask
            }

            IpControl{
                id : subnetMask
                visible: !checkDhcp.checked
                text : "255.255.255.0"
            }
        }

        Row{
            spacing: inputLayout.cellDefaultSpacing
            WisenetLabel{
                id : portText
                visible: true
                text: WisenetLinguist.port
            }
            PortControl{
                id : port
            }
        }
        Row{
            id : gatewayRow
            spacing: inputLayout.cellDefaultSpacing
            WisenetLabel{
                id : gatewayText
                visible: !checkDhcp.checked
                text: WisenetLinguist.gateway
            }

            IpControl{
                id : gateway
                visible: !checkDhcp.checked
                text : DeviceRegister.calculateGateway(startIp.text)
            }
        }
    }
}
