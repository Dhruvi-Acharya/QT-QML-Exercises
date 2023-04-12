import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle{
    color: "#181818"
    height: 30
    width: parent.width
    radius: 3

    property alias text: textField.text
    property alias readOnly : textField.readOnly
    property alias echomode : textField.echoMode
    property alias placeholderText: textField.placeholderText

    TextField{
        id: textField
        anchors.fill: parent
        style: TextFieldStyle{
            placeholderTextColor: "gray"
            textColor: "white"
            background: Rectangle{
                anchors.fill: parent
                //color를  지정하면 cusor가 뒤로 가면서 앞에서 보이지 않음
                color: "transparent"
            }
        }
    }
}
