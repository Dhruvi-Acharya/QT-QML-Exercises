import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import Wisenet.Qmls 0.1

Page {
    anchors.fill: parent

    MainTreeModel {
        id: mainTreeModel
    }

    TreeView {
        anchors.fill: parent
        headerVisible: false
        backgroundVisible: false
        alternatingRowColors: false

        model: mainTreeModel

        style: TreeViewStyle {
            textColor: "white"
            itemDelegate: TreeCellItemDelegate {
                id: itemD
                height: 30
            }
            rowDelegate: Rectangle {
                id: rowD
                height: 30
                color: styleData.selected ? "darkorange" : "transparent"
            }
        }
        TableViewColumn {
            role: "display"
            title: "DeviceTree"
            width: parent.width
        }
    }
}
