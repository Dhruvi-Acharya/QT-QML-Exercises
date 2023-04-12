import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

Item {
    id: root
    width: 228
    height: 20
    focus: true

    property alias text: textfield.text
    property alias selectedText : textfield.selectedText
    property alias maximumLength : textfield.maximumLength
    property bool enterAfterFocusChanged : false

    function selectAll() {
        textfield.selectAll();
        textfield.focus = true;
    }
    signal canceled();
    signal entered();

    TextField {
        id: textfield
        anchors.fill: parent
        anchors.margins: 3
        anchors.topMargin: 0
        height: 14
        padding: 0
        leftPadding: 0
        focus: true
        background: Rectangle {
            color: WisenetGui.transparent
        }
        font.pixelSize: 12
        color: WisenetGui.contrast_02_light_grey
        selectionColor: WisenetGui.color_primary_deep_dark
        selectedTextColor: WisenetGui.white
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignLeft
        selectByMouse: true

        validator: RegExpValidator{
            regExp: /[^`]+/
        }

        onAccepted: {
            parent.entered();
        }
        Keys.onEscapePressed: {
            parent.canceled();
        }

        onFocusChanged: {
            if(enterAfterFocusChanged && focus == false)
                parent.entered();
        }
    }

    Rectangle {
        width: parent.width
        height: 1
        color: WisenetGui.contrast_06_grey
        anchors.bottom: parent.bottom
    }
}
