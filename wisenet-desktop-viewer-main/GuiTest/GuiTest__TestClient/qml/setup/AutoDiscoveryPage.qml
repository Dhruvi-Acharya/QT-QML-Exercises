import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import Wisenet.Qmls 0.1
import BppTable 0.1

Page {
    id: autoDiscoveryPage
    anchors.fill: parent
    implicitWidth: 500
    implicitHeight: 400

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            id: inputLayout
            Layout.fillWidth: true            

             Label {
                id: txtID
                font.bold: true
                color: "orange"
                text: " Login ID "
            }          
                
            TextField {
                id : inputDeviceID
                focus : true
                implicitWidth: 200
                placeholderText: qsTr("Enter Device ID")
                text : "admin"
                Layout.fillWidth:true
            }          

             Label {
                id: txtPassword
                font.bold: true
                color: "orange"
                text: " Password "
            }
            TextField{
                id : inputDevicePassword
                implicitWidth: 200
                placeholderText: qsTr("Enter Device Password")
                text : "5tkatjd!"
                echoMode: TextInput.Password
                focus : true
				Layout.fillWidth:true
            }
        }
        RowLayout {
            id: refreshLayout
            Layout.fillWidth: true            
        Connections {
                target: _autoDiscoveryModel
                function onScanningStatusChanged(status) {
                    console.log("onScanningStatusChanged", status);
                    if (_autoDiscoveryModel.scanningStatus) {
                        refreshButton.checked = true;
                        scanningAnimation.start();
                    }
                    else {
                        refreshButton.checked = false;
                        scanningAnimation.stop();
                    }
                }
                function onDeviceCountChanged(count) {
                    console.log("onDeviceCountChanged", count);
                    txtInfo.tableRowCount = count;
                }
            }

            RoundButton {
                id: refreshButton
                checkable: true
                onClicked: {
                    console.log("checked=", checked);
                    if (!_autoDiscoveryModel.scanningStatus) {
                        console.log("Refresh")
                        _autoDiscoveryModel.startDiscovery()
                        scanningAnimation.start()
                    }
                    else {
                        console.log("Cancel")
                        _autoDiscoveryModel.cancelDiscovery()
                        scanningAnimation.stop()
                    }
                }

                Rectangle {
                 radius: parent.radius
                 anchors.fill: parent
                 color: "transparent"
                 CellFa {
                     text: Fa.fa_sync_alt
                     color: "orange"
                 }

                 RotationAnimation on rotation {
                     id: scanningAnimation
                     from: 0
                     to: 360
                     loops: Animation.Infinite
                     alwaysRunToEnd : true
                     duration: 1000
                 }
                }
            }
             Text {
                property int tableRowCount : bGrid.rows();
                id: txtInfo
                //anchors.right : parent.right
                Layout.alignment: Qt.AlignRight
                font.bold: true
                color: "orange"
                text: "Total " + tableRowCount + " devices"
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
                            running: display === AutoDiscoveryModel.Registering
                            height: 30
                            anchors.fill: parent
                        }
                    }
                    CellClicker {
                        grid: bGrid

                    }

                    function getStatus(status) {
                        switch(status) {
                        case AutoDiscoveryModel.Ready:
                            return Fa.fa_exclamation
                        case AutoDiscoveryModel.Registered:
                            return Fa.fa_check
                        case AutoDiscoveryModel.ConnectionFailed:
                            return Fa.fa_times
                        case AutoDiscoveryModel.AuthFailed:
                            return Fa.fa_lock
                        default:
                            return ""
                        }
                    }
                    function getStatusColor(status) {
                        switch(status) {
                        case AutoDiscoveryModel.Ready:
                            return "orange"
                        case AutoDiscoveryModel.Registered:
                            return "green"
                        case AutoDiscoveryModel.ConnectionFailed:
                            return "red"
                        case AutoDiscoveryModel.AuthFailed:
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
                { role: _autoDiscoveryModel.roleModelName, title: "Model Name", minWidth: 120, dataType: BTColumn.String },
                { role: _autoDiscoveryModel.roleIpAddress, title: "IP Address", minWidth: 120, dataType: BTColumn.String },
                { role: _autoDiscoveryModel.roleMacAddress, title: "MAC Address", minWidth: 120, dataType: BTColumn.String },
                { role: _autoDiscoveryModel.rolePort, title: "Port", width: 80, dataType: BTColumn.Int },
                { role: _autoDiscoveryModel.roleStatus, title: "Status", width: 120, dataType: BTColumn.Int, view: Enums.CellView.StatusView }
             ]
        }

        Button {
                id: registerButton
                width: implicitWidth
                checkable: true       
                //text : "Add " + bGrid.tview.highlightRow +" Device" +  ((bGrid.tview.highlightRow > 1) ? "s" : ""))
                text : "Add Device"

                onClicked: {
                    _autoDiscoveryModel.startRegister(inputDeviceID.text, inputDevicePassword.text);
                }
            }

    }

    Component.onCompleted: {
        _autoDiscoveryModel.setGenericTableModel(bGrid.gridDataModel);
        console.log("component oncompleted")
    }

}
