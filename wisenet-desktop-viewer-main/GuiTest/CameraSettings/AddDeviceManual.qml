import QtQuick 2.15
import QtQuick.Controls 2.15 as Control215
import QtQuick.Controls 1.4 as Control14
import QtQuick.Layouts 1.15
//import Qt.labs.qmlmodels 1.0

import "./controls" as Res

Rectangle{
    id: root

    implicitWidth: width
    color: "#383838"
    //anchors.margins: 15
    Layout.preferredWidth: 800
    Layout.preferredHeight: 600
    Layout.minimumHeight: 800
    Layout.minimumWidth: 600

    ColumnLayout {
        anchors.fill: parent
        width: parent.width
        height: parent.height
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

                            endIpText.text = "End IP"
                            endIp.visible = true
                        }
                        else{
                            startIpText.visible= false
                            startIp.visible = false

                            endIpText.text = ""
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
                    width: 90
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
                        source: "controls/images/questionMark.png"
                        fillMode: Image.PreserveAspectFit
                        clip: true
                        anchors.right: parent.right

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
                Res.MyEditBox{text:"admin"}

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
                Res.MyPasswordEditBox{text:"5tkatjd!"}

                Res.MyText{ text: "Port" }
                Control215.SpinBox{
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
            text : "Scan"
            normalColor: "#ff6c00"
            hoverColor: "#ff7d10"
            pressColor: "#f06200"
            Layout.leftMargin: 10
            implicitWidth: 80
            onClicked: {
                visible  = false
            }
        }
        RowLayout{
            Layout.leftMargin : 10
            Layout.rightMargin: 10
            visible: !scanBtn.visible
            ColumnLayout{
                Control215.Label{
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

        Rectangle {
            color: "#303030"
            width: parent.width
            height: parent.height - 300

            Layout.preferredWidth: 700
            Layout.preferredHeight: 300
            implicitWidth: 700
            implicitHeight: 300
            Layout.fillWidth: true
            Layout.fillHeight: true

            Control14.TableView{
                id : tableView

                anchors.fill: parent
                Layout.margins: 10
                Layout.topMargin: 50

                Control14.TableViewColumn {
                    id: addressColumn
                    title: "Address"
                    role: "address"
                    movable: false
                    resizable: false
                    width: tableView.viewport.width - statusColumn.width - checkColumn.width
                    delegate: itemDelegateText
                }

                Control14.TableViewColumn {
                    id: statusColumn
                    title: "Status"
                    role: "status"
                    movable: false
                    resizable: false
                    width: 200
                    delegate: itemDelegateText
                }

                Control14.TableViewColumn {
                    id: checkColumn
                    title: ""
                    role: "check"
                    movable: false
                    width: 50
                    delegate: itemDelegateCheck
                }
                headerDelegate: Rectangle{
                    color: styleData.containsMouse? "#383838":"#303030"
                    height: 20
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: "white"
                        text: styleData.value
                        font.pointSize: 10
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        anchors.fill: parent
                    }
                }

                rowDelegate: Rectangle{
                    id:rowRectangle
                    color:styleData.selected?(rowRectangleMouseArea.containsMouse?"#6C523E":"#6F4322"):
                                            (rowRectangleMouseArea.containsMouse?"#404040":"#303030")

                    height: 18

                    MouseArea {
                        id : rowRectangleMouseArea
                        hoverEnabled: true
                        anchors.fill: parent
                        onPressed: mouse.accepted = false
                    }
                }


                Component{
                    id: itemDelegateText
                    Rectangle{
                        color:"transparent"
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            color: "white"
                            elide: styleData.elideMode
                            text: styleData.value
                            font.pointSize: 10
                            font.bold: false
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            anchors.fill: parent
                        }
                    }
                }
                Component{
                    id: itemDelegateCheck
                    Rectangle{
                        color:"transparent"
                        Res.MyCheckBox{
                            checked: styleData.value
                            anchors.verticalCenter: parent.verticalCenter
                            text: ""
                            anchors.fill: parent
                        }
                    }
                }


                model: sourceModel

                ListModel {
                    id: sourceModel
                    ListElement {
                        address: "192.168.10.1"
                        status: "success"
                        check: false
                    }
                    ListElement {
                        address: "192.168.10.2"
                        status: "fail"
                        check: true
                    }
                    ListElement {
                        address: "192.168.10.3"
                        status: "success"
                        check: false
                    }
                }
            }
        }
    }
}

