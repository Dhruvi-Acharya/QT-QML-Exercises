import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 1.4

Rectangle{
    property alias text: txtInput.text
    property alias placeholderText : txtInput.placeholderText
    property alias enabled: txtInput.enabled
    Layout.fillWidth: true
    Layout.preferredWidth: 120
    Layout.minimumWidth: 90
    Layout.preferredHeight: 30
    Layout.minimumHeight: 30

    color: WisenetGui.black
    TextInput{
        property string placeholderText: ""
        id : txtInput
        anchors.fill: parent

        color: WisenetGui.white
        leftPadding: 5
        rightPadding: 5
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        selectByMouse: true
        selectionColor: WisenetGui.orange
        clip: true
        renderType: Text.NativeRendering
        maximumLength: 128
        enabled: true


        Text{
            anchors.fill: parent
            text : txtInput.placeholderText
            color: WisenetGui.gray
            visible: !txtInput.text
            leftPadding: 5
            verticalAlignment: TextInput.AlignVCenter
            horizontalAlignment: TextInput.AlignLeft
            font: txtInput.font
        }
    }
}
