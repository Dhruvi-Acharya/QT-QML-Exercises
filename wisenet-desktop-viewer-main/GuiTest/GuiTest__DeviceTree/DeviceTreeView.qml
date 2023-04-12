import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15

import devicetreemodel.wisenet.com 1.0
import devicetreeitemtype.wisenet.com 1.0

Page {

    DeviceTreeModel {
        id: deviceTreeModel
    }

    TreeView {
        anchors.fill: parent
        headerVisible: false
        backgroundVisible: false
        alternatingRowColors: false

        model: deviceTreeModel

        style: TreeViewStyle {
            //backgroundColor: "#444444"
            textColor: "white"

            itemDelegate: Rectangle {

                color: "transparent"

                RowLayout{
                    anchors.fill: parent
//                    Image {
//                        source: "channel.png"
//                    }

                    Text{
                       color: "white"
                       elide: Text.ElideRight
                       font.pixelSize: 15
                       font.bold: true
                       text: styleData.value.name + " (" + styleData.value.ip + ")"
                    }
                }

            }

            rowDelegate: Rectangle {
                height: 30
                color: styleData.selected ? "darkorange" : "transparent"
            }
        }



        TableViewColumn {
            role: "deviceTree"
            title: "DeviceTree"
            width: parent.width
        }
    }
}
