import QtQuick 2.3
import QtQuick.Layouts 1.15


RowLayout{
    property alias text:txt.text
    TextEdit {
        id : txt
        text: ""
        color: "#b8b8b8"
        font.pixelSize: 12
        Layout.fillWidth: true
        readOnly: true
        //wrapMode: Text.WordWrap
        selectByMouse: true
    }
}
