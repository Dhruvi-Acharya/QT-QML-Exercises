import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 1.4

Rectangle{
    property alias text: txtInput.text
    property alias placeholderText : txtInput.placeholderText
    Layout.fillWidth: true
    Layout.preferredWidth: 200
    Layout.minimumWidth: 200
    Layout.preferredHeight: 30
    Layout.minimumHeight: 30

    color: "black"
    TextInput{
        property string placeholderText: ""
        id : txtInput
        anchors.fill: parent

        color: "white"
        leftPadding: 5
        rightPadding: 5
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        selectByMouse: true
        selectionColor: "orange"
        clip: true
        renderType: Text.NativeRendering
        maximumLength: 128

        Text{
            anchors.fill: parent
            text : txtInput.placeholderText
            color: "gray"
            visible: !txtInput.text
            leftPadding: 5
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignLeft
            font: txtInput.font
        }
    }
}
