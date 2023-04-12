import QtQuick 2.12
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.12

ProgressBar {
    id: control
    value: 0.5
    padding: 1

    Layout.fillWidth: true
    Layout.preferredWidth: 500
    Layout.minimumWidth: 500
    Layout.preferredHeight: 4
    Layout.minimumHeight: 4

    background: Rectangle {
        width: control.width
        height: control.height
            color: "black"
            radius: 3
        }

        contentItem: Item {
            width: parent.width
            height: parent.height

            Rectangle {
                width: control.visualPosition * parent.width
                height: parent.height
                radius: 2
                color: "orange"
            }
        }
    }

