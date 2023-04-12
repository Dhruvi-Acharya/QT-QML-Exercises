import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Window 2.15

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1280
    height: 720
    title: qsTr("Device discovery test")

    Material.theme: Material.Dark
    Material.accent: Material.DeepOrange

    ListView {
        id: emptyView
        anchors.fill: parent
    }

    RoundButton {
        id: addAlarmButton
        text: "+"
        anchors.bottom: emptyView.bottom
        anchors.bottomMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: addDeviceWindow.visible = true
    }

    AddDeviceWindow {
        id : addDeviceWindow
        transientParent: mainWindow
        visible: false
        //x: Math.round((parent.width - width) / 2)
        //y: Math.round((parent.height - height) / 2)
    }
}
