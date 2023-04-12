import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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
        spacing: 5

        Rectangle {
            color: 'transparent'
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 100
            Layout.fillWidth: true
            Layout.rightMargin: 10
            Layout.topMargin: 10

            GridLayout{
                id : grid
                enabled: scanBtn.visible
                opacity: scanBtn.visible? 1 : 0.2
                anchors.fill: parent
                columns: 4
                columnSpacing: 10
                rowSpacing: 5

                Res.MyText{ text: "Start IP" }
                Res.MyIpEdit{
                    id : startIp

                    onTextChanged: {
                        endIp.text = startIp.text
                    }
                }

                Res.MyText{ text: "Login" }
                Res.MyEditBox{text:"admin"}

                Res.MyText{ text: "End IP" }
                Res.MyIpEdit{
                    id : endIp
                    text: "0.0.0.255"
                    onTextChanged: {

                    }
                }

                Res.MyText{ text: "Password" }
                Res.MyPasswordEditBox{text:"5tkatjd!"}

                Res.MyText{ text: "Port" }
                SpinBox{
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
        Rectangle {
            color: "#303030"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
