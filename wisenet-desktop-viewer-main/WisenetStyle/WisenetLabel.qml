import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

TextEdit {
    property int defaultPixelSize: 12
    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
    width: 100
    height: 20
    text: ""
    color: WisenetGui.contrast_04_light_grey
    font.pixelSize: defaultPixelSize
    readOnly: true
    textMargin: 1
}
