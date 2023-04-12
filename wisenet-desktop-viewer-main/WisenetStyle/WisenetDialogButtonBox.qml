import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetLanguage 1.0

Rectangle{

    color: WisenetGui.transparent
    implicitWidth: 350
    implicitHeight: 50
    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter


    property alias okEnabled : okId.enabled
    property alias okText: okId.text

    signal accepted()

    WisenetGrayButton {
        id : okId
        text: WisenetLinguist.apply
        x: (parent.width -  width ) / 2
        y : (parent.height - height) / 2
        onClicked: accepted()
    }

}
