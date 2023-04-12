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
        validator: RegExpValidator{regExp:/(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))/}

    }
    Rectangle {
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
    }
}
