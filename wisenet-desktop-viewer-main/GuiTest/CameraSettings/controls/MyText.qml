import QtQuick 2.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    property alias text: txtItem.text
    property alias font: txtItem.font
    property alias horizontalAlignment: txtItem.horizontalAlignment
    property int defaultWidth: 90
    property int defaultHeight: 30
    color: "#383838"
    Layout.preferredWidth: defaultWidth
    //Layout.maximumWidth: defaultWidth
    Layout.minimumWidth: defaultWidth
    Layout.minimumHeight: defaultHeight
    Layout.maximumHeight: defaultHeight
    Layout.preferredHeight: defaultHeight
    Text{
        id : txtItem
        anchors.fill: parent
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        color: "#888888"
        //font.bold: true
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
