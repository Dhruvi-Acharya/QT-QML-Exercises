import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import QtWebEngine 1.10
import WisenetLanguage 1.0
import Wisenet.Setup 1.0
import Wisenet.Define 1.0

Rectangle{
    id : deviceListDeviceViewId
    property int margin: 5
    property bool singleDeviceVisible : true
    property bool emptyDeviceDisable : false
    property bool ipv6Support : ((Qt.platform.os === "windows") && !versionManager.s1Support)

    color: WisenetGui.transparent
    border.color: WisenetGui.color_setup_line
    border.width: 1
    anchors.fill: parent
    visible: true
    onVisibleChanged: {
        if(visible)
            readDevice()
        else
            deviceListViewModel.reset()
    }


    Connections{
        target: deviceListViewModel
        function onDeviceNameChanged(){ deviceNameId.text = deviceListViewModel.deviceName }
    }

    function convertDeviceIpType(type){
        switch(type){
        case DeviceListViewModel.DeviceStaticIp:    return "IPv4"
        case DeviceListViewModel.DeviceStaticIpSSL: return "IPv4+SSL"
        case DeviceListViewModel.DeviceStaticIpv6:    return "IPv6"
        case DeviceListViewModel.DeviceStaticIpv6SSL:   return "IPv6+SSL"
        case DeviceListViewModel.DeviceUrl:             return "URL"
        case DeviceListViewModel.DeviceUrlSSL:          return "URL+SSL"
        case DeviceListViewModel.DeviceS1Ddns:          return "S1 DDNS"
        case DeviceListViewModel.DeviceS1Sip:           return "S1 SIP"
        default : return "DDNS/P2P"
        }
    }

    function readDevice()
    {
        console.log("DeviceListDeviceView readDevice ", deviceListViewModel.deviceName, deviceListViewModel.deviceGuid, deviceListViewModel.ip)
        singleDeviceVisible = (deviceListViewModel.deviceCount > 1)?false:true
        emptyDeviceDisable = (deviceListViewModel.deviceCount == 0)?false:true
        deviceNameId.text = deviceListViewModel.deviceName
        guidId.content = deviceListViewModel.deviceGuid
        modelId.content = deviceListViewModel.model
        versionId.content = deviceListViewModel.version
        macId.content = deviceListViewModel.macAddress.toUpperCase()
        deviceCertificateId.content = deviceListViewModel.translate(deviceListViewModel.deviceCertResult)
        connectedTypeId.content = deviceListViewModel.connectedType
        streamingProtocolId.currentIndex = deviceListViewModel.streamingProtocol

        addressCombo.currentIndex = addressCombo.indexOfValue(convertDeviceIpType(deviceListViewModel.ipType))

        switch(deviceListViewModel.ipType){
        case DeviceListViewModel.DeviceStaticIp:
        case DeviceListViewModel.DeviceStaticIpSSL:
            ipId.text = deviceListViewModel.host
            ipv6Id.text =""
            addressEdit.text = ""
            ddnsEdit.text = ""
            break;
        case DeviceListViewModel.DeviceStaticIpv6:
        case DeviceListViewModel.DeviceStaticIpv6SSL:
            ipId.text = ""
            ipv6Id.text = deviceListViewModel.host
            addressEdit.text = ""
            ddnsEdit.text = ""
            break;
        case DeviceListViewModel.DeviceUrl:
        case DeviceListViewModel.DeviceUrlSSL:
            ipId.text = ""
            ipv6Id.text = ""
            addressEdit.text = deviceListViewModel.host
            ddnsEdit.text = ""
            break;
        case DeviceListViewModel.DeviceS1Ddns:
        case DeviceListViewModel.DeviceS1Sip:
            ipId.text = ""
            ipv6Id.text = ""
            addressEdit.text = ""
            ddnsEdit.text = ""
            if(deviceListViewModel.host.length == 12)
            {
                // MAC
                macRadioButton.checked = true
                s1MacEdit.setText(deviceListViewModel.host)
            }
            else
            {
                // Serial
                macRadioButton.checked = false
                serialRadioButton.checked = true
                serialNumberTextBox.text = deviceListViewModel.host
            }
            break;
        default:
            ipId.text = ""
            ipv6Id.text = ""
            addressEdit.text = ""
            ddnsEdit.text = deviceListViewModel.host
            break;
        }

        portId.text = deviceListViewModel.port
        connectionId.checked = deviceListViewModel.connection
        dtlsOn.checked = deviceListViewModel.dtls

        console.log("DeviceListDeviceView readDevice singleDeviceVisible=", singleDeviceVisible)
    }

    Column{
        id : mainLayout
        enabled: deviceListDeviceViewId.emptyDeviceDisable

        Layout.fillWidth: true
        spacing: 15
        Layout.leftMargin: 15
        Layout.topMargin: 20
        leftPadding: 10
        topPadding: 20
        rightPadding: 10

        state : "IPv4"

        states:[
            State{
                name : "IPv4"
                PropertyChanges {target: ipId; visible:true}
                PropertyChanges {target: ipv6Id; visible:false}
                PropertyChanges {target: addressEdit; visible:false}
                PropertyChanges {target: ddnsEdit; visible:false}
                PropertyChanges {target: s1MacAndSerial; visible:false}
                PropertyChanges {target: portRow; visible:singleDeviceVisible}
                PropertyChanges {target: dtlsRow; visible:false}
            },
            State{
                name : "IPv6"
                PropertyChanges {target: ipId; visible:false}
                PropertyChanges {target: ipv6Id; visible:true}
                PropertyChanges {target: addressEdit; visible:false}
                PropertyChanges {target: ddnsEdit; visible:false}
                PropertyChanges {target: s1MacAndSerial; visible:false}
                PropertyChanges {target: portRow; visible:singleDeviceVisible}
                PropertyChanges {target: dtlsRow; visible:false}
            },
            State{
                name : "URL"
                PropertyChanges {target: ipId; visible: false}
                PropertyChanges {target: ipv6Id; visible:false}
                PropertyChanges {target: addressEdit; visible:true}
                PropertyChanges {target: ddnsEdit; visible:false}
                PropertyChanges {target: s1MacAndSerial; visible:false}
                PropertyChanges {target: portRow; visible:singleDeviceVisible}
                PropertyChanges {target: dtlsRow; visible:false}
            },
            State{
                name : "DDNS/P2P"
                PropertyChanges {target: ipId; visible: false}
                PropertyChanges {target: ipv6Id; visible:false}
                PropertyChanges {target: addressEdit; visible:false}
                PropertyChanges {target: ddnsEdit; visible:true}
                PropertyChanges {target: s1MacAndSerial; visible:false}
                PropertyChanges {target: portRow; visible:false}
                PropertyChanges {target: dtlsRow; visible:true}
            },
            State{
                name : "S1 DDNS"
                PropertyChanges {target: ipId; visible: false}
                PropertyChanges {target: ipv6Id; visible:false}
                PropertyChanges {target: addressEdit; visible:false}
                PropertyChanges {target: ddnsEdit; visible:false}
                PropertyChanges {target: s1MacAndSerial; visible:true}
                PropertyChanges {target: portRow; visible:false}
                PropertyChanges {target: dtlsRow; visible:false}
            },
            State{
                name : "S1 SIP"
                PropertyChanges {target: ipId; visible: false}
                PropertyChanges {target: ipv6Id; visible:false}
                PropertyChanges {target: addressEdit; visible:false}
                PropertyChanges {target: ddnsEdit; visible:false}
                PropertyChanges {target: s1MacAndSerial; visible:true}
                PropertyChanges {target: portRow; visible:false}
                PropertyChanges {target: dtlsRow; visible:false}
            }
        ]

        DeviceListRowControl{
            id : deviceNameRow
            width: 600
            WisenetLabel {
                text: WisenetLinguist.deviceName
                width: 120
                wrapMode: Text.Wrap
            }
            WisenetTextBox{
                id : deviceNameId
                text: WisenetLinguist.deviceName
                width: 228
                height: 23
                fontSize: 18
                Layout.alignment: Qt.AlignVCenter
                enabled: singleDeviceVisible
            }
            WisenetSetupImageButton {
                id : updateDeviceName
                text: WisenetLinguist.resetDeviceName
                //height: 23
                //Layout.alignment: Qt.AlignVCenter
                onClicked: {
                    if (!updateNameDialog.dontAskAgain) {
                        updateNameDialog.open();
                    }
                    else {
                        deviceListViewModel.resetDeviceName()
                    }
                }
            }

            WisenetSetupAskDialog {
                id: updateNameDialog
                msg : WisenetLinguist.resetDeviceNameMessage
                onAccepted: {
                    deviceListViewModel.resetDeviceName()
                }
                onRejected: {
                    dontAskAgain = false
                }
            }
        }

        WisenetCell{
            id : guidId
            title : WisenetLinguist.guid
            visible: singleDeviceVisible
        }

        WisenetCell{
            id : modelId
            title : WisenetLinguist.model
            visible: singleDeviceVisible
        }

        WisenetCell{
            id : versionId
            title : WisenetLinguist.version
            visible: singleDeviceVisible
        }

        WisenetCell{
            id : macId
            title : WisenetLinguist.macAddress
            visible: singleDeviceVisible
        }

        WisenetCell{
            id : deviceCertificateId
            title : WisenetLinguist.deviceCertificate
            visible: singleDeviceVisible
        }

        WisenetCell{
            id : connectedTypeId
            title : WisenetLinguist.connectedType
            visible: singleDeviceVisible
        }

        DeviceListRowControl{
            visible: singleDeviceVisible
            WisenetLabel {
                text: WisenetLinguist.network
                width: 120
                wrapMode: Text.Wrap
            }
            Row{
                id : networkSubRowId
                Layout.preferredWidth: 520
                Layout.fillWidth: true
                spacing: 10
                WisenetComboBox{
                    id : addressCombo
                    width: 98
                    model: versionManager.s1Support ? ["IPv4", "IPv4+SSL", "URL", "URL+SSL", "S1 DDNS", "S1 SIP"] : (
                                                          deviceListDeviceViewId.ipv6Support ? ["IPv4", "IPv4+SSL", "IPv6", "IPv6+SSL","URL", "URL+SSL", "DDNS/P2P"] : ["IPv4", "IPv4+SSL", "URL", "URL+SSL", "DDNS/P2P"])

                    currentIndex: 0
                    onCurrentValueChanged: {
                        if(displayText.match("IPv4"))   mainLayout.state = "IPv4"
                        else if(displayText.match("IPv6"))   mainLayout.state = "IPv6"
                        else if(displayText.match("URL"))   mainLayout.state = "URL"
                        else if(displayText.match("S1 DDNS"))   mainLayout.state = "S1 DDNS"
                        else if(displayText.match("S1 SIP"))   mainLayout.state = "S1 SIP"
                        else
                            mainLayout.state = "DDNS/P2P"
                    }
                }

                IpControl{
                    id : ipId
                    height: 20
                }
                Ipv6Control{
                    id : ipv6Id
                    visible: false
                    height: 20
                    text: ""
                }
                WisenetTextBox{
                    id : addressEdit
                    text: ""
                    height: 20
                    visible: false
                }
                WisenetTextBox{
                    id : ddnsEdit
                    text: ""
                    height: 20
                    visible: false
                }

                Rectangle{
                    id: s1MacAndSerial

                    height: 20
                    width: macRadioButton.width + serialRadioButton.width + 25 + 159 + 28
                    color: "transparent"

                    WisenetRadioButton{
                        id: macRadioButton

                        checked: true
                        text: WisenetLinguist.macAddress
                        font.pixelSize: 12
                    }

                    WisenetRadioButton{
                        id:  serialRadioButton
                        text: WisenetLinguist.serialNumber
                        font.pixelSize: 12
                        anchors.left: macRadioButton.right
                        anchors.leftMargin: 25
                    }

                    WisenetMacAddressBox{
                        id : s1MacEdit

                        height: 17
                        width: 159

                        anchors.left: serialRadioButton.right
                        anchors.leftMargin: 28
                        anchors.bottom: serialRadioButton.bottom
                        anchors.bottomMargin: 9

                        visible: macRadioButton.checked
                    }

                    WisenetTextBox{
                        id : serialNumberTextBox
                        width: 159

                        anchors.left: serialRadioButton.right
                        anchors.leftMargin: 28
                        anchors.bottom: serialRadioButton.bottom
                        anchors.bottomMargin: 9

                        text: ""
                        visible: serialRadioButton.checked

                        capitalization: Font.AllUppercase
                        validator: RegExpValidator { regExp: /[0-9A-Za-z]+/ }
                    }
                }
            }
        }
        DeviceListRowControl{
            visible: singleDeviceVisible
            id : portRow
            WisenetLabel {
                text: WisenetLinguist.port
                width: 120
                wrapMode: Text.Wrap
            }
            PortControl{
                id : portId
            }
        }
        DeviceListRowControl{
            WisenetLabel {
                text: WisenetLinguist.streamingProtocol
                width: 120
                wrapMode: Text.WordWrap
            }
            WisenetComboBox{
                id : streamingProtocolId
                model: ["TCP", "UDP", "HTTP", "MULTICAST"]
            }
        }
        DeviceListRowControl{
            WisenetLabel {
                text: WisenetLinguist.connection
                width: 120
                wrapMode: Text.Wrap
            }
            WisenetCheckBox2{
                id : connectionId
                width: 288
                text: WisenetLinguist.enable
            }
        }
        DeviceListRowControl{
            id: dtlsRow
            WisenetLabel {
                text: WisenetLinguist.dtls
                width: 120
                wrapMode: Text.Wrap
            }
            Row{
                spacing: 20
                WisenetRadioButton{
                    id: dtlsOn
                    checked: true

                    text: WisenetLinguist.on + "(" + WisenetLinguist.securityFirst +")"
                }
                WisenetRadioButton{
                    checked: !dtlsOn.checked
                    text: WisenetLinguist.off + "(" + WisenetLinguist.performanceFirst +")"
                }
            }
        }

        DeviceListRowControl{
            visible: singleDeviceVisible && emptyDeviceDisable
            WisenetLabel {
                text: WisenetLinguist.authorization
                width: 120
                wrapMode: Text.Wrap
            }
            WisenetColorLabel {
                id : changeId
                text : WisenetLinguist.change
                onClicked: {
                    deviceListCredential.id = deviceListViewModel.id
                    deviceListCredential.password = deviceListViewModel.password
                    deviceListCredential.showDialog()
                }
            }
        }
        DeviceListRowControl{
            visible: singleDeviceVisible && emptyDeviceDisable
            WisenetLabel {
                text: WisenetLinguist.webViewer
                width: 120
                wrapMode: Text.Wrap
            }
            WisenetColorLabel {
                text : WisenetLinguist.open
                onClicked: {
                    /*if(Qt.platform.os === "linux")
                        Qt.openUrlExternally(deviceListViewModel.url);
                    else*/
                    if(versionManager.s1Support)
                        deviceListViewModel.openBrowser()
                    else{
                        webPage.reload(deviceListViewModel.url,
                                       deviceListViewModel.id,
                                       deviceListViewModel.password,
                                       deviceListViewModel.supportWebSocket);
                        webPage.showNormal();
                    }    
                }
            }
        }
    }

    DeviceListCredential{
        id : deviceListCredential

        onCredentialChanged:{
            webPage.clearCredential()
        }
    }

    DeviceListWebPage{
        id : webPage
        onNotFound: {
            webPage.reload(deviceListViewModel.url2,
                           deviceListViewModel.id,
                           deviceListViewModel.password,
                           false);

            webPage.showNormal();

            // test 필요.
            //webPage.visible = false
            //webPage.close();
            //webPage.url = deviceListViewModel.url2
            //webPage.supportDigest = false
            //webPage.visible = true
        }
    }

    RowLayout{
        width: deviceListDeviceViewId.width
        height: dialogButtonBox.height
        y : parent.height - 50

        WisenetDialogButtonBox{
            id: dialogButtonBox

            onAccepted: {
                var paramItem;
                if(addressCombo.currentText.match("IPv4"))   paramItem = ipId.text;
                else if(addressCombo.currentText.match("IPv6"))   paramItem = ipv6Id.text;
                else if(addressCombo.currentText.match("URL"))  paramItem = addressEdit.text
                else if(addressCombo.currentText.match("S1 DDNS") || addressCombo.currentText.match("S1 SIP"))
                {
                    if(macRadioButton.checked)
                        paramItem = s1MacEdit.macAddress
                    else
                        paramItem = serialNumberTextBox.text
                }
                else  paramItem = ddnsEdit.text

                deviceListViewModel.changeConnectInfo(connectionId.checked, addressCombo.currentIndex, streamingProtocolId.currentIndex, portId.text, paramItem, deviceNameId.text, dtlsOn.checked)
            }
        }
    }
}

