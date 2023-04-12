import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle{
    id: wisenetPasswordBox
    color: "transparent"
    height: 17
    width: 228

    signal accepted()
    signal focusChanged()
    property alias textField: textField
    property alias text: textField.text
    property bool isEnabled : false
    property bool showPassword : false
    property bool showPasswordButtonVisible : true
    property alias placeholderText: textField.placeholderText
    property alias maximumLength: textField.maximumLength

    function setTextFocus()
    {
        textField.forceActiveFocus()
    }

    TextField{
        id: textField
        enabled: wisenetPasswordBox.enabled
        anchors.left: parent.left
        anchors.right: image.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
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
            regExp: /[0-9a-zA-Z`!@#$%^&*()-_=+\{\}\[\]\\|:;\'\"<>,\.\/?~]+/
        }

        hoverEnabled: true
        selectByMouse: true
        color: !enabled ? WisenetGui.contrast_06_grey :
                hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
        echoMode: showPassword ? TextInput.Normal : TextInput.Password
        placeholderTextColor: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
        passwordCharacter: "*"
        font.pixelSize: 12
        maximumLength: 64
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        selectionColor: WisenetGui.color_primary_deep_dark
        selectedTextColor: WisenetGui.white
        onTextChanged: text = text.replace(/\s+/g,'')
        onAccepted: wisenetPasswordBox.accepted();
        onFocusChanged: wisenetPasswordBox.focusChanged();
    }

    Image {
        id: image
        enabled: wisenetPasswordBox.enabled
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        visible: showPasswordButtonVisible
        source: showPassword ? (!enabled ? WisenetImage.hide_password_dim : buttonArea.containsMouse ? WisenetImage.hide_password_hover : WisenetImage.hide_password_normal) :
                               (!enabled ? WisenetImage.show_password_dim : buttonArea.containsMouse ? WisenetImage.show_password_hover :  WisenetImage.show_password_normal)
        sourceSize: "17x17"
        MouseArea{
            id: buttonArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                showPassword = !showPassword
            }
        }
    }

    Rectangle{
        enabled: wisenetPasswordBox.enabled
        color: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
    }
}
