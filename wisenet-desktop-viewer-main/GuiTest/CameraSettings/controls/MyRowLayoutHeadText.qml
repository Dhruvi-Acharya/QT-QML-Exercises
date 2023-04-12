import QtQuick 2.0
import QtQuick.Layouts 1.15

RowLayout{
    property alias text:txt.text
    Text {
        id : txt
        text: ""
        color: "white"
        font.bold: true
        font.pixelSize: 20
        Layout.fillWidth: true
    }
}
