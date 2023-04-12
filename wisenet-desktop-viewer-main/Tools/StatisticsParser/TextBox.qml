import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle{
    id: root
    color: "transparent"
    height: 17
    width: 228

    signal editingFinished()
    signal accepted()
    property alias textField: textField
    property alias text: textField.text
    property alias readOnly : textField.readOnly
    property alias echomode : textField.echoMode
    property alias placeholderText: textField.placeholderText
    property alias fontSize : textField.font.pixelSize
    property alias maximumLength: textField.maximumLength

    function setTextFocus()
    {
        textField.forceActiveFocus()
    }

    TextField{
        id: textField
        enabled: root.enabled
        anchors.fill: parent
        anchors.margins: 3
        anchors.topMargin: 0
        leftInset: 0
        rightInset: 0
        topInset: 0
        bottomInset: 0
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0
        background: Rectangle{
            anchors.fill: parent
            color: "transparent"
        }

        hoverEnabled: true
        selectByMouse: true
        color: !enabled ? "#909090" :
                hovered ? "#FFFFFF" : "#E5E5E5"
        placeholderTextColor: enabled ? "#909090" : "#5C5C5C"
        passwordCharacter: "*"
        onEditingFinished: root.editingFinished()
        onAccepted: root.accepted();
        font.pixelSize: 12
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        selectionColor: "#CC601A"
        selectedTextColor: "#FFFFFF"
    }

    Rectangle{
        enabled: root.enabled
        color: "#909090"
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
    }
}
