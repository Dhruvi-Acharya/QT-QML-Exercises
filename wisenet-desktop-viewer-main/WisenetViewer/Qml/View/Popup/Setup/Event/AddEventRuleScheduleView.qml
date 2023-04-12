import QtQuick 2.0
import WisenetStyle 1.0
import WisenetLanguage 1.0

Item {
    id: root
    anchors.fill: parent

    property bool isEnable: true

    property alias comboBox: combo
    property alias comboBoxModel: combo.model
    property alias currentSchedule: combo.currentText
    property alias currentComboIndex: combo.currentIndex


    WisenetText{
        id: scheduleText
        text: WisenetLinguist.schedule
        enabled: isEnable
        opacity: enabled? 1 : 0.3
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    WisenetComboBox {
        id: combo
        enabled: isEnable
        opacity: enabled? 1 : 0.3

        width: 108
        anchors.verticalCenter:  parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10

        onCurrentTextChanged: {

        }
    }
}
