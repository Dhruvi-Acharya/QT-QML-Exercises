import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15


Window {
    id: addDeviceWindow
    title: qsTr("Add device")
    width: 800
    minimumWidth: 640
    maximumWidth: 1280
    height: 600
    minimumHeight: 480
    maximumHeight: 800
    flags: Qt.Dialog
    modality: Qt.NonModal
    opacity: 1.0
    //transientParent: null

    onVisibleChanged: {
        console.log("visible changed")
        if (visible == true)
            _autoDiscoveryModel.startDiscovery()
    }

    Page {
        id: mainPage
        anchors.fill: parent
        header: TabBar {
            id: bar
            width: parent.width
            TabButton {
                text: qsTr("Auto Discovery")
                width: implicitWidth
            }
            TabButton {
                text: qsTr("Add Manually")
                width: implicitWidth
            }
        }
        StackLayout {
            //width: parent.width
            anchors.fill: parent
            currentIndex: bar.currentIndex
            AutoDiscoveryPage {
            }
            ManuallyAddDevicePage {
            }
        }
    }
}
