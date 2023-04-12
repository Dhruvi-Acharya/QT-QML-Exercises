import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1280
    height: 720
    title: qsTr("Wisenet Ui Test")

    ColumnLayout{
        spacing: 0
        width: parent.width; height: parent.height

        Rectangle {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            Layout.preferredWidth: 1280
            Layout.preferredHeight: 50

            border.color: "#444444"
            border.width: 1

            RoundButton {
                id: addAlarmButton
                text: "+"
                anchors.bottom: emptyView.bottom
                anchors.bottomMargin: 8
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: addDeviceWindow.visible = true
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignCenter
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1280
            Layout.preferredHeight: 650

            border.color: "#444444"
            border.width: 1

            RowLayout {
                spacing: 0
                width: parent.width; height: parent.height

                Rectangle {
                    Layout.alignment: Qt.AlignLeft
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                    Layout.preferredWidth: 300
                    Layout.preferredHeight: 650

                    border.color: "#444444"
                    border.width: 1

                    ExplorerView {
                    }
                }

                Rectangle {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                    Layout.preferredWidth: 780
                    Layout.preferredHeight: 650

                    border.color: "#444444"
                    border.width: 1
                }

                Rectangle {
                    Layout.alignment: Qt.AlignRight
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 650

                    border.color: "#444444"
                    border.width: 1
                }
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            Layout.preferredWidth: 1280
            Layout.preferredHeight: 20

            border.color: "#444444"
            border.width: 1
        }
    }
}
