import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0

Column {
    id : col
    spacing: 5
    width: parent.width
    padding : 20
    property alias label: textLabel.text
    property alias font: textLabel.font

    signal editingFinished()
    property alias text: textField.text
    property alias readOnly : textField.readOnly
    property alias echomode : textField.echoMode
    property alias placeholderText: textField.placeholderText

    TextField{
        id: textField
        anchors.fill: parent
        style: TextFieldStyle{
            placeholderTextColor: WisenetGui.gray
            textColor: WisenetGui.textBoxTextColor
            background: Rectangle{
                anchors.fill: parent

                color: WisenetGui.transparent
            }
        }
        onEditingFinished: root.editingFinished()
    }

    Rectangle {
        border.width: 1
        height: 2
        width: parent.width - col.padding
        anchors.margins: 20
        border.color: WisenetGui.setupPageLine
    }
}
