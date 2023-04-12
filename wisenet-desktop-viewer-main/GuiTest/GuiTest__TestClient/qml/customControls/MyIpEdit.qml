import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 1.4

Rectangle{
    property alias text: txtInput.text
    property alias placeholderText : txtInput.placeholderText

    Layout.fillWidth: true
    Layout.preferredWidth: 160
    Layout.minimumWidth: 160
    Layout.preferredHeight: 30
    Layout.minimumHeight: 30

    color: "black"
    TextInput {
        property string placeholderText: ""

        id: txtInput
        anchors.fill: parent
        text: "0.0.0.0"
        color: "white"
        leftPadding: 5
        rightPadding: 5
        readOnly: false
        visible: true
        font.pixelSize: 15
        selectByMouse: true
        selectionColor: "orange"
        font.underline: false

        verticalAlignment: TextInput.AlignVCenter

        focus:true

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


        validator:
            RegExpValidator{
            regExp:/(?=(\b|\D))(((\d{1,2})|(1\d{1,2})|(2[0-4]\d)|(25[0-5]))\.){3}((\d{1,2})|(1\d{1,2})|(2[0-4]\d)|(25[0-5]))(?=(\b|\D))/
        }

    }
}
