import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import QtQuick.Dialogs 1.3
import Wisenet.Setup 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

WisenetPopupWindowBase{
    id: deviceRegisterViewWindow

    property int imageSize: 12
    property int mainTitleHeight: 40
    minimumWidth: WisenetGui.popupSecondWindowDefaultWidth
    minimumHeight: WisenetGui.popupSecondWindowDefaultHeight

    component TabTitle: Rectangle{
        id: tabTitle

        property alias title: titleText.text
        property bool checked: false

        signal clicked()

        width: titleText.contentWidth
        height: 16
        color: "transparent"


        Text{
            id: titleText

            anchors.fill: parent
            color: tabTitle.checked ? WisenetGui.contrast_01_light_grey : WisenetGui.contrast_06_grey

            verticalAlignment: Text.AlignVCenter

            font.pixelSize: 14
        }

        MouseArea{
            anchors.fill: parent

            onClicked: tabTitle.clicked()
        }
    }

    component TabSeperator: Rectangle{
        id: tabSeperator

        width: seperator.contentWidth + 20
        height: 16
        color: "transparent"


        Text{
            id: seperator

            text: "|"

            anchors.fill: parent
            color: WisenetGui.contrast_06_grey

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            font.pixelSize: 14
        }
    }

    title : WisenetLinguist.addDevice
    cancelButtonText: WisenetLinguist.close
    applyButtonText: WisenetLinguist.register
    minmaxButtonVisible: false

    onVisibleChanged: {
        autoDiscoveryButton.checked = true
        autoDiscovery.visible  = visible
    }

    onApplyButtonClicked: autoDiscovery.register()

    AutoDiscoveryViewModel {
        id: myAutoDiscoveryViewModel
    }
    ManualDiscoveryViewModel {
        id: myManualDiscoveryViewModel
    }

    Connections{
        target: myAutoDiscoveryViewModel
        function onDeviceRegisterCompleted(success){
            console.log("onDeviceRegisterCompleted " + success)
            applyResultMessage.showMessage(success? WisenetLinguist.registerCompleted : WisenetLinguist.registerFailed)
        }
        function onDeviceRegistering(){
            applyResultMessage.showMessage(WisenetLinguist.registering, false)
        }
    }
    Connections{
        target: myManualDiscoveryViewModel
        function onDeviceRegisterCompleted(success){
            console.log("onDeviceRegisterCompleted " + success)
            applyResultMessage.showMessage(success? WisenetLinguist.registerCompleted : WisenetLinguist.registerFailed)
        }
        function onDeviceRegistering(){
            applyResultMessage.showMessage(WisenetLinguist.registering, false)
        }
    }

    WisenetSetupApplyResultMessage{
        id: applyResultMessage
        anchors {
            right : deviceRegisterViewWindow.header.right
            rightMargin: 30
            top : deviceRegisterViewWindow.header.top
            topMargin: 30
        }
    }

    Rectangle{
        id : mainRect

        property int defaultPadding: 34

        anchors.fill: parent
        anchors.topMargin: 5
        anchors.leftMargin: defaultPadding
        anchors.rightMargin: defaultPadding

        color: "transparent"

        Rectangle{
            id : registerMenuRect

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 5
            height: 30
            color: "transparent"

            Row{
                id : buttonRow
                anchors.top: parent.top
                anchors.left: parent.left

                TabTitle {
                    id : autoDiscoveryButton

                    title: WisenetLinguist.auto
                    onClicked: {
                        autoDiscoveryButton.checked = true
                        autoDiscovery.visible = true
                    }
                }

                TabSeperator{}

                TabTitle {
                    id : manualButton

                    checked: !autoDiscoveryButton.checked
                    title: WisenetLinguist.manual

                    onClicked: {
                        autoDiscoveryButton.checked = false
                        autoDiscovery.visible = false
                    }
                }
            }

            Rectangle{
                anchors.right: parent.right
                anchors.top: parent.top
                color: "transparent"
                width: 220
                height: 30
                visible: autoDiscovery.visible

                Item{
                    anchors.left: parent.left
                    anchors.top: parent.top
                    visible : (Qt.platform.os === "windows") && !versionManager.s1Support
                    //visible: true
                    WisenetRadioButton{
                        id: ipv4RadioButton
                        checked: true
                        text: "IPv4"
                        font.pixelSize: 12
                        onCheckedChanged: {
                            if(checked){
                                myAutoDiscoveryViewModel.cancelDiscovery()
                                myAutoDiscoveryViewModel.ipv6Mode = false
                            }
                        }
                    }

                    WisenetRadioButton{
                        id:  ipv6RadioButton
                        text: "IPv6"
                        font.pixelSize: 12
                        anchors.left: ipv4RadioButton.right
                        anchors.leftMargin: 30
                        onCheckedChanged: {
                            if(checked){
                                myAutoDiscoveryViewModel.cancelDiscovery()
                                myAutoDiscoveryViewModel.ipv6Mode = true
                            }
                        }
                    }
                    Rectangle{
                        color: WisenetGui.contrast_07_grey
                        width: 1
                        height: 14
                        anchors.left: ipv6RadioButton.right
                        anchors.leftMargin: 30
                    }
                }
/*
                WisenetImageAndTextButton{
                    id : ipModeButton
                    anchors.right: parent.left
                    anchors.top: parent.top
                    txt : myAutoDiscoveryViewModel.ipv6Mode?"Ipv6":"Ipv4"
                    normalImage: "qrc:/Assets/Icon/setup/refreshDefault.svg"
                    pressImage: "qrc:/Assets/Icon/setup/refreshPress.svg"
                    hoverImage: "qrc:/Assets/Icon/setup/refreshHover.svg"
                    normalTextColor: WisenetGui.contrast_04_light_grey
                    hoverTextColor: WisenetGui.white
                    pressTextColor: WisenetGui.contrast_06_grey
                    imageWidth: 16
                    imageHeight: 16
                    visible : (Qt.platform.os === "windows") && !versionManager.s1Support
                    height: 16
                    width: 50
                    onButtonClicked: {

                    }
                }
                */

                WisenetImageAndTextButton{
                    id : refreshButton
                    anchors.right: parent.right
                    anchors.top: parent.top
                    txt : WisenetLinguist.refresh
                    normalImage: "qrc:/Assets/Icon/setup/refreshDefault.svg"
                    pressImage: "qrc:/Assets/Icon/setup/refreshPress.svg"
                    hoverImage: "qrc:/Assets/Icon/setup/refreshHover.svg"
                    normalTextColor: WisenetGui.contrast_04_light_grey
                    hoverTextColor: WisenetGui.white
                    pressTextColor: WisenetGui.contrast_06_grey
                    visible: !myAutoDiscoveryViewModel.scanningStatus
                    imageWidth: 16
                    imageHeight: 16
                    height: 16
                    width: 70                    
                    onButtonClicked: {
                        myAutoDiscoveryViewModel.startDiscovery(false)
                    }
                }
            }
        }

        // it's located outside of registerMenuRect to adjust top margin
        WisenetMediaBusyIndicator {
            id: streamChaningIndicator

            anchors.top: parent.top
            anchors.right: parent.right

            sourceWidth: 26
            sourceHeight: 26
            width: 26
            height: 26
            running:true

            visible : myAutoDiscoveryViewModel.scanningStatus && autoDiscovery.visible
        }

        AutoDiscoveryPageView {
            id : autoDiscovery

            anchors.top: registerMenuRect.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            visible: false
            onVisibleChanged: {
                applyButtonVisible = visible
            }
        }

        ManualDiscoveryPageView {
            id : manualPage
            anchors.top: registerMenuRect.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            visible: (deviceRegisterViewWindow.visible && !autoDiscovery.visible)

            onVisibleChanged: {
                applyButtonVisible = !visible
            }
        }
    }
}
