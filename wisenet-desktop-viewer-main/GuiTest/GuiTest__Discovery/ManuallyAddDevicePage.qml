import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15

Page {
    id: manuallyAddDevicePage
    anchors.fill: parent
    property int margin: 12

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: manuallyAddDevicePage.margin
        RowLayout {
            id: deviceType
            Layout.fillWidth: true
            Label { text: qsTr("DeviceType") }
            ComboBox {
                model: ["Wisenet Camera/Encoder", "Wisenet Recorder"]
            }
            Label { text: qsTr("Port") }

        }

        RowLayout {
            id: ipRange
            Layout.fillWidth: true
            Label { text: qsTr("IP Range") }
            Label { text: qsTr(" ~ ") }
        }

        RowLayout {
            id: login
            Layout.fillWidth: true
            Label { text: qsTr("Login") }
            Label { text: qsTr("Password") }
        }

        Pane {
            id: tableView
            Layout.minimumHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
