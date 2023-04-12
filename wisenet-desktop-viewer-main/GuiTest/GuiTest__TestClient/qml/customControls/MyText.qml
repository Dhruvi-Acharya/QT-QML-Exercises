import QtQuick 2.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    property alias text: txtItem.text

    color: "transparent"
    Layout.minimumWidth: 80
    Layout.preferredWidth: 80
    Layout.maximumWidth: 80

    Layout.minimumHeight: 30
    Layout.maximumHeight: 30
    Layout.preferredHeight: 30
    Text{
        id : txtItem
        anchors.fill: parent
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        color: "#888888"
        font.bold: true
    }
}

/*
import QtQuick 2.12
import QtQuick.Controls 2.12

Label {
    text: qsTr("Label")
    color: "#888888"
    font.bold: true
    horizontalAlignment: Text.AlignRight
    verticalAlignment: Text.AlignVCenter
    width: 70
}
*/
