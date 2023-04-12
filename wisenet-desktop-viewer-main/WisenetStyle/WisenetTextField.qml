import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: 228
    height: 17

    property alias textField: textfield
    property alias placeholderText: textfield.placeholderText

    TextField {
        id: textfield
        anchors.fill: parent
        anchors.margins: 3
        anchors.topMargin: 0
        height: 14
        padding: 0
        leftPadding: 0
        background: Rectangle {
            color: WisenetGui.transparent
        }
        font.pixelSize: 12
        color: WisenetGui.contrast_02_light_grey
        selectionColor: WisenetGui.color_primary_deep_dark
        selectedTextColor: WisenetGui.contrast_00_white
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        selectByMouse: true
    }

    Rectangle {
        width: parent.width
        height: 1
        color: WisenetGui.contrast_06_grey
        anchors.bottom: parent.bottom
    }
}

