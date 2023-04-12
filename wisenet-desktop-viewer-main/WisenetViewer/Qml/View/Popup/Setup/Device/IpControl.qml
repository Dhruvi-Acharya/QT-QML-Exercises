import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0

Rectangle{
    id : root
    color: "transparent"
    width:  228
    height: 17
    property alias text : control.text

    TextInput {
        id : control
        width: parent.width
        height: 18
        Layout.alignment: Qt.AlignLeft
        text: root.text
        font.pixelSize: 12
        anchors.fill: parent
        selectByMouse: true
        color: !enabled ? WisenetGui.contrast_06_grey :
                hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
        selectionColor: WisenetGui.color_primary_deep_dark
        selectedTextColor: WisenetGui.white
        validator: RegExpValidator{regExp:/(?=(\b|\D))(((\d{1,2})|(1\d{1,2})|(2[0-4]\d)|(25[0-5]))\.){3}((\d{1,2})|(1\d{1,2})|(2[0-4]\d)|(25[0-5]))(?=(\b|\D))/}

    }
    Rectangle {
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
    }
}
