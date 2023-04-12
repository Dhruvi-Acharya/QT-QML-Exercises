import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle{
    id: root
    color: "transparent"
    height: 17
    width: 228

    signal editingFinished()
    signal accepted()
    signal backward();
    property alias textField: textField
    property alias text: textField.text
    property alias readOnly : textField.readOnly
    property alias echomode : textField.echoMode
    property alias placeholderText: textField.placeholderText
    property alias fontSize : textField.font.pixelSize
    property alias maximumLength: textField.maximumLength
    property alias validator: textField.validator
    property alias capitalization: textField.font.capitalization
    property alias horizontalAlignment: textField.horizontalAlignment
    property alias bottomBorderVisible: bottomBorder.visible

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

        validator: RegExpValidator{
            regExp: /[^`]+/
        }

        hoverEnabled: true
        selectByMouse: true
        color: !enabled ? WisenetGui.contrast_06_grey :
                hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
        placeholderTextColor: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
        passwordCharacter: "*"
        onEditingFinished: root.editingFinished()
        onAccepted: root.accepted();
        font.pixelSize: 12
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        selectionColor: WisenetGui.color_primary_deep_dark
        selectedTextColor: WisenetGui.white

        Keys.onPressed: {
            if(event.key === Qt.Key_Backspace && textField.length == 0)
            {
                root.backward()
            }
        }
    }

    Rectangle{
        id: bottomBorder
        enabled: root.enabled
        color: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
    }
}
