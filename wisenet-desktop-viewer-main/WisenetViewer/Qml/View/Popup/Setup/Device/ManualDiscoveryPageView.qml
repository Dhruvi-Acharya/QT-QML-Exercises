import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import WisenetStyle 1.0
import Wisenet.Setup 1.0
import WisenetLanguage 1.0
import "DeviceRegister.js" as DeviceRegister

Rectangle{
    id: manualDiscoveryView

    property int defaultCellHeight: 36
    property int defaultLabelWidth: 92
    property int defaultControlWidth: 159
    property bool ipv6Support : ((Qt.platform.os === "windows") && !versionManager.s1Support)

    onVisibleChanged: {
        if(visible){
            inputID.text = localSettingViewModel.autoRegisterId
            inputPassword.text = localSettingViewModel.autoRegisterPassword
            savePassword.checked = localSettingViewModel.autoRegister
            startIp.text = ""
            ipv6.text = ""
            urlEdit.text =""
            ddnsEdit.text = ""
            s1DdnsEdit.clearText()
            myManualDiscoveryViewModel.reset()
        }
    }

    function checkInput(){
        var ret = false
        if(inputID.text.length < 1 || inputPassword.text.length < 1){
            msgDialog.message = WisenetLinguist.pleaseCheckIdAndPassword
        }
        else if(manualDiscoveryView.state == "DDNS/P2P" && ddnsEdit.text.length < 1){
            msgDialog.message = WisenetLinguist.pleaseCheckProductId
        }
        else if(port.text.length < 1 || port.text == "0")
            msgDialog.message = WisenetLinguist.pleaseCheckPort
        else if((manualDiscoveryView.state == "IPv4" || manualDiscoveryView.state == "IPv4+SSL") && (startIp.text.length < 1 || endIp.text < 1)){
            msgDialog.message = WisenetLinguist.pleaseCheckIpAddress
        }
        else if((manualDiscoveryView.state == "IPv6" || manualDiscoveryView.state == "IPv6+SSL")  && (ipv6.text.length < 1 )){
            msgDialog.message = WisenetLinguist.pleaseCheckIpAddress
        }
        else if((manualDiscoveryView.state == "URL" || manualDiscoveryView.state == "URL+SSL") && urlEdit.text.length < 1){
            msgDialog.message = WisenetLinguist.pleaseCheckAddress
        }
        else
            ret = true
        return ret
    }

    Connections{
        target: myManualDiscoveryViewModel
        function onConnectFailDeviceRelayModeManual(){
            console.log("onConnectFailDeviceRelayModeManual")
            msg.message = WisenetLinguist.systemEventDeviceDisconnectedWithErrorMessage
            msg.visible = true
        }
    }

    ListModel {
        id: deviceListColumns

        Component.onCompleted: {
            append({ "columnWidth": 380, "title": WisenetLinguist.ip, "sort": 0, "splitter": 1})
            append({ "columnWidth": 380, "title": WisenetLinguist.port, "sort": 0, "splitter": 1})
            append({ "columnWidth": 390, "title": WisenetLinguist.status, "sort": 0, "splitter": 1})
        }
    }

    DelegateChooser {
        id: deviceListChooser

        DelegateChoice {
            column: 2
            delegate: Rectangle {
                id: statusRec

                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

                WisenetTableCellText{
                    color: WisenetGui.contrast_04_light_grey;
                    text: myManualDiscoveryViewModel.translate(display)//getStatusString(display)
                }
                WisenetTableCellLine{}

                MouseArea {
                    anchors.fill : parent
                    hoverEnabled: true
                    /*onContainsMouseChanged: {
                        myManualDiscoveryViewModel.setHoveredRow(row, containsMouse);
                    }*/
                    onPressed: {
                        myManualDiscoveryViewModel.setHighlightRow(row, mouse.modifiers);
                    }
                }
            }
        }

        DelegateChoice {
            delegate: Rectangle {
                id: column0Rec

                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

                WisenetTableCellText{
                    text: display
                }
                WisenetTableCellLine{}

                MouseArea {
                    anchors.fill : parent
                    hoverEnabled: true
                    onPressed: {
                        myManualDiscoveryViewModel.setHighlightRow(row, mouse.modifiers);
                    }
                }
            }
        }
    }

    color: "transparent"

    state : "IP"
    states:[
        State{
            name : "IPv4"
            PropertyChanges {target: ipV4Items; visible:true}
            PropertyChanges {target: ipV6Items; visible:false}
            PropertyChanges {target: urlItems; visible:false}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: s1DdnsItems; visible:false}
            PropertyChanges {target: portItems; visible:true}
            PropertyChanges {target: portItems; anchors.left: passwordItems.left}
            PropertyChanges {target: portItems; anchors.leftMargin: 0}
            PropertyChanges {target: port; text : "80"}
            PropertyChanges {target: registerButton; anchors.left: saveCheckItems.left}
            PropertyChanges {target: registerButton; anchors.leftMargin: 0}
        },
        State{
            name : "IPv4+SSL"
            PropertyChanges {target: ipV4Items; visible:true}
            PropertyChanges {target: ipV6Items; visible:false}
            PropertyChanges {target: urlItems; visible:false}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: portItems; visible:true}
            PropertyChanges {target: portItems; anchors.left: passwordItems.left}
            PropertyChanges {target: portItems; anchors.leftMargin: 0}
            PropertyChanges {target: port; text : "443"}
            PropertyChanges {target: registerButton; anchors.left: saveCheckItems.left}
            PropertyChanges {target: registerButton; anchors.leftMargin: 0}
        },
        State{
            name : "IPv6"
            PropertyChanges {target: ipV4Items; visible:false}
            PropertyChanges {target: ipV6Items; visible:true}
            PropertyChanges {target: urlItems; visible:false}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: portItems; visible:true}
            PropertyChanges {target: portItems; anchors.left: ipV6Items.right}
            PropertyChanges {target: portItems; anchors.leftMargin: 60}
            PropertyChanges {target: port; text : "80"}
            PropertyChanges {target: registerButton; anchors.left: portItems.right}
            PropertyChanges {target: registerButton; anchors.leftMargin: 60}
        },
        State{
            name : "IPv6+SSL"
            PropertyChanges {target: ipV4Items; visible:false}
            PropertyChanges {target: ipV6Items; visible:true}
            PropertyChanges {target: urlItems; visible:false}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: portItems; visible:true}
            PropertyChanges {target: portItems; anchors.left: ipV6Items.right}
            PropertyChanges {target: portItems; anchors.leftMargin: 60}
            PropertyChanges {target: port; text : "443"}
            PropertyChanges {target: registerButton; anchors.left: portItems.right}
            PropertyChanges {target: registerButton; anchors.leftMargin: 60}
        },
        State{
            name : "URL"
            PropertyChanges {target: ipV4Items; visible: false}
            PropertyChanges {target: ipV6Items; visible:false}
            PropertyChanges {target: urlItems; visible:true}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: s1DdnsItems; visible:false}
            PropertyChanges {target: portItems; visible:true}
            PropertyChanges {target: portItems; anchors.left: urlItems.right}
            PropertyChanges {target: portItems; anchors.leftMargin: 60}
            PropertyChanges {target: port; text : "80"}
            PropertyChanges {target: registerButton; anchors.left: portItems.right}
            PropertyChanges {target: registerButton; anchors.leftMargin: 60}
        },
        State{
            name : "URL+SSL"
            PropertyChanges {target: ipV4Items; visible: false}
            PropertyChanges {target: ipV6Items; visible:false}
            PropertyChanges {target: urlItems; visible:true}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: portItems; visible:true}
            PropertyChanges {target: portItems; anchors.left: urlItems.right}
            PropertyChanges {target: portItems; anchors.leftMargin: 60}
            PropertyChanges {target: port; text : "443"}
            PropertyChanges {target: registerButton; anchors.left: portItems.right}
            PropertyChanges {target: registerButton; anchors.leftMargin: 60}
        },
        State{
            name : "DDNS/P2P"
            PropertyChanges {target: ipV4Items; visible: false}
            PropertyChanges {target: ipV6Items; visible:false}
            PropertyChanges {target: urlItems; visible:false}
            PropertyChanges {target: ddnsItems; visible:true}
            PropertyChanges {target: s1DdnsItems; visible:false}
            PropertyChanges {target: portItems; visible:false}
            PropertyChanges {target: registerButton; anchors.left: dtlsOff.right}
            PropertyChanges {target: registerButton; anchors.leftMargin: 60}
        },
        State{
            name : "S1 DDNS"
            PropertyChanges {target: ipV4Items; visible: false}
            PropertyChanges {target: ipV6Items; visible:false}
            PropertyChanges {target: urlItems; visible:false}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: s1DdnsItems; visible:true}
            PropertyChanges {target: portItems; visible:false}
            PropertyChanges {target: registerButton; anchors.left: s1DdnsItems.right}
            PropertyChanges {target: registerButton; anchors.leftMargin: 60}
        },
        State{
            name : "S1 SIP"
            PropertyChanges {target: ipV4Items; visible: false}
            PropertyChanges {target: ipV6Items; visible:false}
            PropertyChanges {target: urlItems; visible:false}
            PropertyChanges {target: ddnsItems; visible:false}
            PropertyChanges {target: s1DdnsItems; visible:true}
            PropertyChanges {target: portItems; visible:false}
            PropertyChanges {target: registerButton; anchors.left: s1DdnsItems.right}
            PropertyChanges {target: registerButton; anchors.leftMargin: 60}
        }
    ]

    // Connection info
    Rectangle{
        id : connectionInfoRect
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 25

        height: 20 + 64
        color: "transparent"

        property int rowBottomMargin : 14

        // Address type
        Item{
            id: addressTypeItems
            anchors.left: parent.left
            width: 256
            height: parent.height
            WisenetLabel{
                id : ipTypeText;

                anchors.left: parent.left
                text: WisenetLinguist.ipType
                wrapMode: Text.Wrap
                width: manualDiscoveryView.defaultLabelWidth
            }
            WisenetComboBox{
                id : addressCombo
                anchors.right: parent.right
                width: manualDiscoveryView.defaultControlWidth
                model: versionManager.s1Support ? ["IPv4", "IPv4+SSL", "URL", "URL+SSL", "S1 DDNS", "S1 SIP"] : (
                                                      manualDiscoveryView.ipv6Support ? ["IPv4", "IPv4+SSL", "IPv6", "IPv6+SSL","URL", "URL+SSL", "DDNS/P2P"] : ["IPv4", "IPv4+SSL", "URL", "URL+SSL", "DDNS/P2P"])
                currentIndex: 0

                onCurrentValueChanged: {
                    manualDiscoveryView.state  = displayText
                }
            }
        }

        // ID
        Item{
            id: idItems
            anchors.left: addressTypeItems.right
            anchors.leftMargin: 60
            width: 256
            height: parent.height
            WisenetLabel{
                id : idText

                anchors.left: parent.left
                text: WisenetLinguist.id_
                width: manualDiscoveryView.defaultLabelWidth
            }

            WisenetTextBox{
                id : inputID
                anchors.right: parent.right
                text: localSettingViewModel.autoRegisterId
                width: manualDiscoveryView.defaultControlWidth
            }
        }

        // Password
        Item{
            id: passwordItems
            anchors.left: idItems.right
            anchors.leftMargin: 60
            width: 256
            height: parent.height

            WisenetLabel{
                id : passwordText
                text: WisenetLinguist.password
                width: manualDiscoveryView.defaultLabelWidth
                wrapMode: Text.Wrap
            }

            WisenetPasswordBox{
                id : inputPassword
                text:localSettingViewModel.autoRegisterPassword
                width: manualDiscoveryView.defaultControlWidth
                anchors.left: passwordText.right
                showPasswordButtonVisible: false
            }
        }

        // Save password or not
        Item{
            id: saveCheckItems
            anchors.left: passwordItems.right
            anchors.leftMargin: 20
            width: savePassword.width + 12 + saveCheckQuestionButton.width
            height: parent.height

            WisenetCheckBox2{
                id: savePassword

                anchors.left: parent.left
                width: 150
                text: WisenetLinguist.saveIdPassword
                textWrapMode: Text.Wrap
                checked: localSettingViewModel.autoRegister
            }
            WisenetSaveCheckQuestionButton{
                id: saveCheckQuestionButton
                width: 16
                height: 16
                anchors.left: savePassword.right
                anchors.leftMargin: 12
            }
        }

        // IP
        Item{
            id: ipV4Items
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin

            width: startIpText.width + 5 + startIp.width + endIpText.width + endIp.width
            height: 16

            WisenetLabel{
                id : startIpText
                text: "IP"

                width: manualDiscoveryView.defaultLabelWidth
                anchors.left: parent.left
            }
            IpControl{
                id : startIp
                text: ""

                width: manualDiscoveryView.defaultControlWidth
                anchors.left: startIpText.right
                anchors.leftMargin: 5
            }
            Text{
                id : endIpText
                text : "~"

                width: 30
                anchors.left: startIp.right
                anchors.leftMargin: 10
                color: WisenetGui.contrast_04_light_grey
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            IpControl{
                id : endIp
                text: startIp.text
                width: manualDiscoveryView.defaultControlWidth
                anchors.left: endIpText.right
                anchors.leftMargin: 10
            }
        }

        // IP v6
        Item{
            id: ipV6Items
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin

            width: 256
            height: 16

            WisenetLabel{
                id : ipv6Text
                text: "IP"

                width: manualDiscoveryView.defaultLabelWidth
                anchors.left: parent.left
            }
            Ipv6Control{
                id : ipv6
                text: ""

                width: manualDiscoveryView.defaultControlWidth
                anchors.right: parent.right
            }
        }

        // URL
        Item{
            id: urlItems
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin

            width: 256
            height: 16

            WisenetLabel{
                id : urlText
                text: "URL"

                width: manualDiscoveryView.defaultLabelWidth
                anchors.left: parent.left
            }
            WisenetTextBox{
                id : urlEdit
                text: ""

                width: manualDiscoveryView.defaultControlWidth
                anchors.right: parent.right
            }
        }

        // DDNS/P2P
        Item{
            id: ddnsItems

            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin

            width: 256
            height: 16

            WisenetLabel{
                id : ddnsText
                text: WisenetLinguist.productId
                width: manualDiscoveryView.defaultLabelWidth
                anchors.left: parent.left
                wrapMode: Text.Wrap
            }
            WisenetTextBox{
                id : ddnsEdit
                width: manualDiscoveryView.defaultControlWidth
                anchors.right: parent.right
                text: ""
            }

        }

        // DTLS (only Wisenet DDNS)
        WisenetLabel{
            id : dtlsText
            text: WisenetLinguist.dtls
            height: 16
            width: manualDiscoveryView.defaultLabelWidth
            wrapMode: Text.Wrap
            anchors.left: ddnsItems.right
            anchors.leftMargin: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin
            visible: ddnsItems.visible
        }
        WisenetRadioButton{
            id: dtlsOn
            checked: true
            anchors.left: dtlsText.right
            anchors.leftMargin: 5
            anchors.bottom: parent.bottom
            visible: ddnsItems.visible && !versionManager.s1Support
            text: WisenetLinguist.on + "(" + WisenetLinguist.securityFirst +")"
        }
        WisenetRadioButton{
            id: dtlsOff
            checked: false
            anchors.left: dtlsOn.right
            anchors.leftMargin: 45
            anchors.bottom: parent.bottom
            visible: dtlsOn.visible
            text: WisenetLinguist.off + "(" + WisenetLinguist.performanceFirst +")"
        }

        // S1 DDNS & SIP
        Item{
            id: s1DdnsItems

            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin

            width: 388
            height: 16

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
                anchors.leftMargin: 40
            }


            WisenetMacAddressBox{
                id : s1DdnsEdit

                height: 17
                width: manualDiscoveryView.defaultControlWidth

                anchors.left: serialRadioButton.right
                anchors.leftMargin: 28
                anchors.bottom: serialRadioButton.bottom
                anchors.bottomMargin: 9

                visible: macRadioButton.checked
            }

            WisenetTextBox{
                id : serialNumberTextBox
                width: manualDiscoveryView.defaultControlWidth

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

        // Port
        Item{
            id: portItems

            anchors.left: ipV4Items.left
            anchors.leftMargin: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin

            width: 256
            height: 16

            WisenetLabel{
                id : portText

                anchors.left: parent.left
                text: WisenetLinguist.port
                width: manualDiscoveryView.defaultLabelWidth
                wrapMode: Text.Wrap
            }
            PortControl{
                id : port
                width: manualDiscoveryView.defaultControlWidth
                anchors.right: parent.right
            }
        }

        WisenetButton2 {
            id: registerButton
            width: 144
            height: 28

            anchors.left: portItems.right
            anchors.leftMargin: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: connectionInfoRect.rowBottomMargin - 6
            text : WisenetLinguist.register

            onClicked: {
                if(manualDiscoveryView.checkInput() === false)
                    msgDialog.visible = true
                else{
                    var paramItem;
                    if(addressCombo.currentText == "IPv4" || addressCombo.currentText == "IPv4+SSL")
                        paramItem = startIp.text;
                    else if(addressCombo.currentText == "IPv6" || addressCombo.currentText == "IPv6+SSL")
                        paramItem = ipv6.text;
                    else if(addressCombo.currentText == "URL" || addressCombo.currentText =="URL+SSL")
                        paramItem = urlEdit.text
                    else if(addressCombo.currentText == "S1 DDNS" || addressCombo.currentText == "S1 SIP")
                    {
                        if(macRadioButton.checked)
                            paramItem = s1DdnsEdit.macAddress
                        else
                            paramItem = serialNumberTextBox.text
                    }
                    else
                    {
                        paramItem = ddnsEdit.text
                    }

                    localSettingViewModel.saveDeviceCredential(savePassword.checked, inputID.text, inputPassword.text)

                    myManualDiscoveryViewModel.startRegister(addressCombo.displayText,
                                                             inputID.text,
                                                             inputPassword.text,
                                                             paramItem, endIp.text,
                                                             port.text,
                                                             dtlsOn.checked);
                }
            }
        }
    }

    // Table area
    WisenetTable {
        id: deviceListTable
        anchors.top: connectionInfoRect.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 18

        columns: deviceListColumns
        cellDelegate: deviceListChooser
        tableModel : myManualDiscoveryViewModel
        contentColor: "transparent"
        popupDepth : true
        headerBackgroundColor: WisenetGui.contrast_08_dark_grey
    }


    WisenetMessageDialog{
        id: msgDialog
        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    WisenetMessageDialog{
        id : msg
        width: 400
        height: 200
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

}
