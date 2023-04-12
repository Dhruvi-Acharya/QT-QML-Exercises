import QtQuick 2.3
import QtQuick.Layouts 1.15

RowLayout{
    property alias text:txt.text
    Text {
        id : txt
        text: ""
        color: "#888888"
        font.bold: true
        font.pixelSize: 12
        Layout.fillWidth: true
    }
}
