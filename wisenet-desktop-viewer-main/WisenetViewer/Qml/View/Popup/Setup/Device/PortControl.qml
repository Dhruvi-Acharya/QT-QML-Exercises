import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
Column{
    id : root
    width:  228
    height: 20
    spacing: 0

    property alias text: control.text
    TextInput {
        id : control
        width: parent.width
        height: 18
        Layout.alignment: Qt.AlignLeft
        text: Number(80)
        font.pixelSize: 12

        selectByMouse: true
        color: !enabled ? WisenetGui.contrast_06_grey :
                hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
        selectionColor: WisenetGui.color_primary_deep_dark
        selectedTextColor: WisenetGui.white
        validator: IntValidator {bottom: 1; top: 65535}
        inputMethodHints: Qt.ImhFormattedNumbersOnly
    }

    Rectangle {
        width: parent.width
        height: 1
        //y : control.bottom
        color: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
    }
}


