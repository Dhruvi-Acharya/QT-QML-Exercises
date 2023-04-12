import QtQuick 2.12
import QtQuick.Controls 2.12

SpinBox {
    id: control
    value: 50
    editable: true

    property int indicatorWidth : 12
    property int indicatorHeight : 20
    property alias spinTxt: txtId.text

    textFromValue: function(value) {
        return value;
    }

    leftPadding: 0

    contentItem:
        Rectangle{
        color: "transparent"
        width: control.width - indicatorWidth
        height: control.height

        TextInput {
            id : txtId
            z: 2
            text: control.textFromValue(control.value, control.locale)
            onEditingFinished:{
                control.value = valueFromText(text, control.locale)
            }

            onTextChanged: {
                control.value = Number(txtId.text)
            }

            anchors.fill: parent

            font: control.font
            color: WisenetGui.white
            selectByMouse: true
            selectionColor: WisenetGui.orange
            selectedTextColor: WisenetGui.white
            verticalAlignment: Qt.AlignVCenter
            anchors.leftMargin: 20
            anchors.topMargin: 12
            validator: control.validator
            inputMethodHints: Qt.ImhFormattedNumbersOnly
        }
    }

    up.indicator: Rectangle {
        x: parent.width - width
        y : 0
        height: parent.height /2
        implicitWidth: indicatorWidth
        implicitHeight: indicatorHeight
        color: "transparent"
        Image {
            anchors.fill: parent
            sourceSize: "12x12"
            source: {
                return WisenetImage.spin_up
            }
        }
    }

    down.indicator: Rectangle {
        x: parent.width - width
        y : parent.height /2
        height: parent.height /2
        implicitWidth: indicatorWidth
        implicitHeight: indicatorHeight
        color: "transparent"
        Image {
            anchors.fill: parent
            sourceSize: "12x12"
            source: {
                return WisenetImage.spin_down
            }
        }
    }

    Rectangle{
        //enabled: root.enabled
        color: enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
    }

    background: Rectangle {
        implicitWidth: 40
        color: "transparent"
    }
}

