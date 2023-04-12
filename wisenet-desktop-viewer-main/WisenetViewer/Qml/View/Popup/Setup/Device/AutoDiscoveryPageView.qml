import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import Wisenet.Define 1.0
import WisenetLanguage 1.0

import "DeviceRegister.js" as DeviceRegister

Rectangle {
    id: root

    function reset(){

        console.log("autoDiscoveryPage reset deviceListTable.verticalScrollPosition =", deviceListTable.verticalScrollPosition)
        myAutoDiscoveryViewModel.reset()
        deviceListTable.verticalScrollPosition = 0
        inputDeviceID.text = localSettingViewModel.autoRegisterId
        inputDevicePassword.text = localSettingViewModel.autoRegisterPassword
        savePassword.checked = localSettingViewModel.autoRegister

        myAutoDiscoveryViewModel.startDiscovery()
    }

    function getStatusColor(display){
        if(display==="IpConflict") return WisenetGui.color_accent;
        else if(display ==="InitDevicePassword") return WisenetGui.color_orange2;
        else return WisenetGui.contrast_04_light_grey;
    }

    function allUncheckedDisplay(){
        deviceListTable.allCheckedStatus = false
    }

    function register(){
        localSettingViewModel.saveDeviceCredential(savePassword.checked, inputDeviceID.text, inputDevicePassword.text)
        myAutoDiscoveryViewModel.startRegister(inputDeviceID.text, inputDevicePassword.text);
    }

    onVisibleChanged: {
        if(visible) reset();
        else{
            console.log("autoDiscoveryPage visible false")
            // myAutoDiscoveryViewModel.reset()
        }
    }

    Connections{
        target: deviceListTable
        onAllChecked:{
            console.log("all checked", checked)
            myAutoDiscoveryViewModel.setAllCheckState(checked)
        }
    }

    Connections{
        target: myAutoDiscoveryViewModel
        onIpv6ModeChanged:{
            console.log("onIpv6ModeChanged", isIpv6)
            myAutoDiscoveryViewModel.reset()
            deviceListTable.verticalScrollPosition = 0
            singleShotTimer.running = true
        }
    }

    Timer {
        id: singleShotTimer
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            myAutoDiscoveryViewModel.startDiscovery()
        }
    }

    ListModel {
        id: deviceListColumns

        Component.onCompleted: {
            append({ "columnWidth": 0, "title": "IsHttps", "sort": 0, "splitter": 0})
            append({ "columnWidth": 50, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({ "columnWidth": 210, "title": WisenetLinguist.model, "sort": 0, "splitter": 1})
            append({ "columnWidth": 300, "title": WisenetLinguist.ip, "sort": 0, "splitter": 1})
            append({ "columnWidth": 200, "title": WisenetLinguist.macAddress, "sort": 0, "splitter": 1})
            append({ "columnWidth": 120, "title": WisenetLinguist.port, "sort": 0, "splitter": 1})
            append({ "columnWidth": 270, "title": WisenetLinguist.status, "sort": 0, "splitter": 1, "textColor":1})
        }
    }

    DelegateChooser {
        id: deviceListChooser


        DelegateChoice {
            id : delegateChoice
            column: 1
            delegate:WisenetSetupTableCheckRectangle{
                checkedValue: display
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"
                onContainsMouseChanged: {
                    myAutoDiscoveryViewModel.setHoveredRow(row, containsMouse);
                }
                onClicked: {
                    myAutoDiscoveryViewModel.setCheckState(row, checkedValue)
                    myAutoDiscoveryViewModel.setHighlightRow(row, modifier);
                    if(display === false)
                        root.allUncheckedDisplay()

                }
            }
        }
        DelegateChoice {
            column: 6
            delegate:WisenetSetupTableDefaultRect{
                text: myAutoDiscoveryViewModel.translate(display)
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"
                textColor: getStatusColor(display)
                onPressed: {
                    myAutoDiscoveryViewModel.setHighlightRow(row, modifier);
                }
                onContainsMouseChanged: {
                    myAutoDiscoveryViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }

        DelegateChoice {
            id : delegateChoice2
            delegate:WisenetSetupTableDefaultRect{
                text: display
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"
                onPressed: {
                    myAutoDiscoveryViewModel.setHighlightRow(row, modifier);
                }
                onContainsMouseChanged: {
                    myAutoDiscoveryViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }
    }

    Rectangle
    {
        color: WisenetGui.contrast_09_dark
        anchors.fill: parent
        ColumnLayout {
            id: mainLayout
            anchors.fill: parent

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"

                WisenetTable {
                    id: deviceListTable
                    anchors.fill: parent
                    columns: deviceListColumns
                    cellDelegate: deviceListChooser
                    tableModel : myAutoDiscoveryViewModel
                    contentColor: "transparent"
                    popupDepth : true
                    headerBackgroundColor: WisenetGui.contrast_08_dark_grey
                }
            }

            Rectangle{
                id : duplicateId
                visible: true
                Layout.alignment: Qt.AlignRight
                width: 140
                height: 11
                color: "transparent"
                Text{
                    function getWarningMessage(){
                        var msg = ""
                        /*console.log("[AutoDiscoveryPageView] getWarningMessage" + myAutoDiscoveryViewModel.needPasswordCount
                                    + " " + myAutoDiscoveryViewModel.conflictIpCount)*/
                        if(myAutoDiscoveryViewModel.needPasswordCount > 0)
                            msg = WisenetLinguist.initialPasswordNeedMessage.arg(myAutoDiscoveryViewModel.needPasswordCount)
                        if(myAutoDiscoveryViewModel.conflictIpCount > 0)
                            msg += ((myAutoDiscoveryViewModel.needPasswordCount > 0)?" " : "") + WisenetLinguist.ipConflictMessage.arg(myAutoDiscoveryViewModel.conflictIpCount)
                        return msg
                    }

                    anchors.fill: parent
                    text: getWarningMessage()
                    horizontalAlignment: Text.AlignRight
                    color: WisenetGui.color_accent
                    font.pixelSize:  11
                }
            }

            Rectangle {
                color: WisenetGui.transparent
                Layout.preferredWidth: root.width
                Layout.preferredHeight: 40
                Layout.fillWidth: true
                //Layout.rightMargin: 10
                //Layout.topMargin: 10

                Rectangle{
                    color: WisenetGui.transparent
                    anchors.bottom:  parent.bottom
                    anchors.left: parent.left
                    width: parent.width - 298
                    height: 30

                    Row{
                        anchors.fill: parent

                        spacing: 20

                        Row{
                            spacing: 1
                            height: parent.height
                            WisenetLabel {
                                id: txtID
                                text: WisenetLinguist.id_
                                width: 35
                                wrapMode: Text.Wrap
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            WisenetTextBox{
                                id : inputDeviceID
                                text:localSettingViewModel.autoRegisterId
                                width: 228
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                        Row{
                            spacing: 1
                            height: parent.height
                            WisenetLabel {
                                id: txtPassword
                                text: WisenetLinguist.password
                                width: 60
                                wrapMode: Text.Wrap
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            WisenetPasswordBox{
                                id : inputDevicePassword
                                text : localSettingViewModel.autoRegisterPassword
                                width: 228
                                anchors.verticalCenter: parent.verticalCenter
                                showPasswordButtonVisible: false
                                onAccepted: {
                                    if (inputDeviceID.text.length > 0 && inputDevicePassword.text.length > 0) {
                                        root.register();
                                    }
                                }
                            }
                        }
                        Row{
                            spacing: 10
                            height: parent.height
                            WisenetCheckBox2{
                                id: savePassword
                                text: WisenetLinguist.saveIdPassword
                                width: 190
                                height: 16
                                textWrapMode: Text.Wrap
                                anchors.verticalCenter: parent.verticalCenter
                                checked: localSettingViewModel.autoRegister
                            }
                            WisenetSaveCheckQuestionButton{
                                width: 16
                                height: 16
                                anchors.leftMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                }

                Rectangle{
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    color: "transparent"
                    width: 298
                    height: 30
                    visible: !myAutoDiscoveryViewModel.ipv6Mode
                    Row{
                        anchors.fill: parent
                        spacing: 10
                        WisenetGrayButton{
                            width:  144
                            height: parent.height


                            text : WisenetLinguist.initialPassword
                            onClicked: {
                                console.log("Initial password clicked :", myAutoDiscoveryViewModel.selectedRowCount)
                                if(!myAutoDiscoveryViewModel.checkInitDeviceSelect()){
                                    msgDialog.message = WisenetLinguist.pleaseSelectInitialPasswordDevice
                                    msgDialog.visible = true
                                }
                                else
                                    initialPassword.show()
                            }
                        }
                        WisenetGrayButton{
                            width:  144
                            height: parent.height

                            text : WisenetLinguist.ipConfiguration
                            onClicked: {
                                if(myAutoDiscoveryViewModel.selectedRowCount > 0){
                                    console.log("[AutoDiscoveryPageView] myAutoDiscoveryViewModel.selectedRowCount:" + myAutoDiscoveryViewModel.selectedRowCount)
                                    ipConfiguration.show()
                                }
                                else{
                                    msgDialog.message = WisenetLinguist.pleaseSelectDevice
                                    msgDialog.visible = true
                                }
                            }
                        }
                    }
                }
            }

            InitialPassword{
                id : initialPassword
                width: 421
                height: 300

            }
            IpConfiguration{
                id : ipConfiguration
                width: 710
                height: 556
            }

            WisenetMessageDialog{
                id: msgDialog
                visible: false
                applyButtonVisible: false
                cancelButtonText: WisenetLinguist.close
            }
        }
    }
}
