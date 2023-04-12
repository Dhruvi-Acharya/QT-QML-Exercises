import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import QtQuick.Window 2.12
import Wisenet.Qmls 0.1

Page {
    anchors.fill: parent

    MainTreeSortFilterProxyModel {
        id: mainTreeFilterModel
        sourceModel: _mainTreeModel
    }

    ColumnLayout{
        id: treeLayout
        spacing: 0
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            TextInput {
                leftPadding: 10
                readOnly: false
                selectByMouse: true
                text: ""
                cursorVisible: true
                color: "white"
                font.bold: true
                overwriteMode: true
                focus: true
                font.pointSize: 15
                onTextChanged: {
                    //console.log("onTextChanged...");
                    mainTreeFilterModel.setFilterText(text);
                }
            }
        }

        TreeView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            headerVisible: false
            backgroundVisible: false
            alternatingRowColors: false

            model: mainTreeFilterModel
            //model: mainTreeFilterModel

            onActivated: {
                var item = mainTreeFilterModel.data(index);
                mainTreeFilterModel.activateModel(item);
            }

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

            MouseArea{
                id: treeMouse
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons : Qt.RightButton
                onClicked: {
                    console.log("tree right click" );
                    //_mainTreeModel.getSelectedIndex(mainTreeModel.styleData );
                    //_mainTreeModel.showDeviceContextMenu(index, mouseX, mouseY);
                    treeServerMenu.popup(mouseX, mouseY)
                }

                Menu {
                    id: treeServerMenu
                    MenuItem {
                        text: "Monitor"
                    }
                    MenuItem {
                        text: "Monitor in New Tab"
                    }
                    MenuItem {
                        text: "Monitor in New Window"
                    }
                    MenuItem {
                        text: "Rename"
                    }
                    MenuItem {
                        text: "Add Device..."
                        onTriggered: addDeviceWindow.visible = true
                    }
                    MenuItem {
                        text: "Camera Lists by Server..."
                    }
                    MenuItem {
                        text: "Server Logs..."
                    }
                    MenuItem {
                        text: "Server Diagnostics..."
                    }
                    MenuItem {
                        text: "Server Web Page..."
                    }
                    MenuItem {
                        text: "Server Settings..."
                    }
                }
                Menu {
                    id: treeDeviceMenu
                    MenuItem {
                        text: "Open"
                    }
                    MenuItem {
                        text: "Open in New Tab"
                    }
                    MenuItem {
                        text: "Open in New Window"
                    }
                    MenuItem {
                        text: "Delete"
                    }
                    MenuItem {
                        text: "Rename"
                    }
                    MenuItem {
                        text: "Check Camera Issues..."
                    }
                    /*
                MenuItem {
                    text: "Camera Rules..."
                }
                */
                    MenuItem {
                        text: "Camera Settings..."
                    }
                }
            }
        }
    }
}
