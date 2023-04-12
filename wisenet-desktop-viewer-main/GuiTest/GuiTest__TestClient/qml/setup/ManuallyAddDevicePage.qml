import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../customControls" as Res
import BppTable 0.1
import Wisenet.ManualDiscovery 0.1

Page{
    id: root

    anchors.fill: parent
    implicitWidth: 500
    implicitHeight: 400

    ColumnLayout {
        id : mainLayout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5

        Rectangle {
            color: 'transparent'
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 160
            Layout.fillWidth: true
            Layout.rightMargin: 10
            Layout.topMargin: 10
            GridLayout{
                id : grid
                enabled: scanBtn.visible
                opacity: scanBtn.visible? 1 : 0.2
                anchors.fill: parent
                columns: 4
                columnSpacing: 5
                rowSpacing: 3

                Res.MyText{ text: "IP Type" }
                Res.MyCombobox{
                    Layout.fillWidth: true
                    model : ["IP", "URL", "DDNS"]

                    onCurrentValueChanged: {
                        if(displayText == "IP"){
                            startIpText.visible= true
                            startIp.visible = true

                            //endIpText.text = "End IP"
                            endIp.visible = true
                        }
                        else{
                            startIpText.visible= false
                            startIp.visible = false

                            //endIpText.text = ""
                            endIp.visible = false
                        }
                    }
                }

                Res.MyText{}
                Res.MyText{}

                Res.MyText{
                    id : startIpText
                    text: "Start IP"
                }

                Rectangle {
                    id : addressText
                    visible: !startIpText.visible
                    color: "transparent"
                    width: 80
                    height: 30
                    Layout.margins: 0
                    Text{
                        id : txtItem
                        width: addressText.width - questionImage.width
                        height: addressText.height
                        anchors.left: parent.left
                        text: "Address"
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignRight
                        color: "#888888"
                        font.bold: true
                    }

                    Image {
                        id: questionImage
                        width: addressText.height
                        height: addressText.height
                        source: "../customControls/icon/questionMark.png"
                        fillMode: Image.PreserveAspectFit
                        clip: true
                        anchors.right: parent.right
                        opacity: 0.8

                    }
                }

                Res.MyIpEdit{
                    id : startIp
                    text: "0.0.0.0"
                    placeholderText: "Start address"
                    onTextChanged: {
                        endIp.text = startIp.text == "" ?"0.0.0.0" : startIp.text

                    }

                }
                Res.MyEditBox{
                    id : addressEdit
                    text: ""
                    placeholderText: " IP / Hostname / RTSP link / UDP link"
                    visible: !startIp.visible
                }

                Res.MyText{ text: "Login" }
                Res.MyEditBox{
                    id : inputDeviceID
                    text:"admin"}

                Res.MyText{
                    id : endIpText
                    text: "End IP" }
                Res.MyIpEdit{
                    id : endIp
                    text: "0.0.0.255"
                    placeholderText: "End address"
                    onTextChanged: {
                    }
                }
                Res.MyText{id : emptyControl; visible: !endIp.visible}


                Res.MyText{ text: "Password" }
                Res.MyPasswordEditBox{
                    id : inputDevicePassword
                    text:"5tkatjd!"}

                Res.MyText{ text: "Port" }
                SpinBox{
                    id : port
                    value: 80
                    from : 0
                    to : 65535
                    editable: true
                }
            }
        }

        Res.MyButton{
            id : scanBtn
            visible: true
            text : "Add"
            normalColor: "#ff6c00"
            hoverColor: "#ff7d10"
            pressColor: "#f06200"
            Layout.leftMargin: 10
            implicitWidth: 80
            onClicked: {
                //visible  = false
                //console.log("scan button start:" + startIp.text + " end:" + endIp.text + " port:" + port.textFromValue(port.value, port.locale))
                _manualDiscoveryModel.startDiscovery(startIp.text, endIp.text, port.textFromValue(port.value, port.locale))
            }
        }
        RowLayout{
            Layout.leftMargin : 10
            Layout.rightMargin: 10
            visible: !scanBtn.visible
            ColumnLayout{
                Label{
                    text: "Scanning online hosts..."
                    color: "orange"
                    font.pixelSize: 10
                    font.bold: true
                }

                Res.MyProgressBar{ }
            }

            Res.MyButton{
                id : stopBtn
                text : "Stop"
                implicitWidth: 80
                onClicked: {
                    scanBtn.visible = true
                }

            }
        }
        CompGrid {
            id: bGrid
            Layout.fillWidth: true
            Layout.fillHeight: true

            dataHeight: 30
            dateFormat: "MMM yyyy"
            showOptionsButton: false
            withMultiselection: true
            withMultiselectionMobileMode: false

            Component {
                id: cellDelegate

                Rectangle {
                    id: cellContainer
                    implicitHeight: bGrid.dataHeight
                    color: bGrid.getCellBk(row, highlight)
                    visible: model.visible

                    CellText {
                        visible: view === Enums.CellView.SimpleText
                        text: bGrid.formatDisplay(display, dataType, 2)
                        horizontalAlignment: bGrid.getAlign(dataType)
                        //font.capitalization: Font.AllUppercase //Font.Capitalize
                    }
                    Image {
                        visible: view === Enums.CellView.TypeView
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit
                        //sourceSize.width: 30
                        height: 20;//parent.height - 10
                        source: display === 1 ? "camera.png" : "recorder.png"
                    }

                    CellFa {
                        visible: view === Enums.CellView.StatusView
                        text: getStatus(display)
                        color: getStatusColor(display)
                        horizontalAlignment: Text.Center
                        BusyIndicator {
                            running: display === ManualDiscoveryModel.Registering
                            height: 30
                            anchors.fill: parent
                        }
                    }
                    CellClicker {
                        grid: bGrid

                    }

                    function getStatus(status) {
                        switch(status) {
                        case ManualDiscoveryModel.Ready:
                            return Fa.fa_exclamation
                        case ManualDiscoveryModel.Registered:
                            return Fa.fa_check
                        case ManualDiscoveryModel.ConnectionFailed:
                            return Fa.fa_times
                        case ManualDiscoveryModel.AuthFailed:
                            return Fa.fa_lock
                        default:
                            return ""
                        }
                    }
                    function getStatusColor(status) {
                        switch(status) {
                        case ManualDiscoveryModel.Ready:
                            return "orange"
                        case ManualDiscoveryModel.Registered:
                            return "green"
                        case ManualDiscoveryModel.ConnectionFailed:
                            return "red"
                        case ManualDiscoveryModel.AuthFailed:
                            return "red"
                        default:
                            return "white"
                        }
                    }
                }
            }

            cellDelegate: cellDelegate

            //fromListModel: modColumns
            fromArray: [
                { role: _manualDiscoveryModel.roleIpAddress, title: "IP Address", minWidth: 120, dataType: BTColumn.String },
                { role: _manualDiscoveryModel.rolePort, title: "Port", width: 80, dataType: BTColumn.Int },
                { role: _manualDiscoveryModel.roleStatus, title: "Status", width: 120, dataType: BTColumn.Int, view: Enums.CellView.StatusView }
            ]
        }

        Res.MyButton{
            id : applyBtn
            visible: true
            text : "Register"
            normalColor: "#ff6c00"
            hoverColor: "#ff7d10"
            pressColor: "#f06200"
            Layout.leftMargin: 10
            implicitWidth: 80
            onClicked: {
                _manualDiscoveryModel.startRegister(inputDeviceID.text, inputDevicePassword.text);
            }
        }
    }
    Component.onCompleted: {
        _manualDiscoveryModel.setGenericTableModel(bGrid.gridDataModel);
        console.log("manuual component oncompleted")
    }
}
