import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property string ip: "0.0.0.0"

    Layout.fillWidth: true
    Layout.preferredWidth: 150
    Layout.minimumWidth: 150
    Layout.preferredHeight: 30
    Layout.minimumHeight: 30

    color: WisenetGui.black

    onIpChanged: {
        var array = ip.split(".", 4)

        for (var i = 0; i < array.length; i++) {
            listModel.setProperty(i, "field", array[i])
        }
    }

    QtObject {
        id: _private

        function updateIP() {
            var ip = ""
            for (var i = 0; i < listModel.count; i++) {
                if (i != 0)
                    ip += "."
                ip += listModel.get(i).field
            }

            root.ip = ip
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: listModel
        orientation: ListView.Horizontal

        ListModel {
            id: listModel
            ListElement { field: "0" }
            ListElement { field: "0" }
            ListElement { field: "0" }
            ListElement { field: "0" }
        }

        delegate:
        FocusScope {
            width: root.width / 4
            height: root.height
            property alias cursorPosition: textInput.cursorPosition
            property alias text: textInput.text

            onActiveFocusChanged: {
                if (activeFocus) {
                    listView.currentIndex = index
                }
            }

            TextInput {
                id: textInput
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignHCenter
                focus: true
                text: field
                //font.pointSize: 15
                color: WisenetGui.white
                selectByMouse: true
                selectionColor: WisenetGui.orange

                validator: RegExpValidator { regExp: /(2(5[0-5]|[0-4]\d))|[0-1]?\d{1,2}/ }

                onTextChanged: {
                    if (text.length == 2) {
                        if(text.charAt(0) ==="0")
                            text = text.charAt(1)

                        if (Number(text) * 10 > 255) {
                            listView.incrementCurrentIndex()
                        }
                    }
                    else if (text.length == 3) {
                        listView.incrementCurrentIndex()
                    }

                    if (text !==listModel.get(index).field) {
                        listModel.setProperty(index, "field", text)
                        _private.updateIP()
                    }
                }

                Keys.onPressed: {
                    if (event.key === Qt.Key_Backspace) {
                        if (cursorPosition == 0) {
                            listView.decrementCurrentIndex()
                        }
                    }
                    else if (event.key === Qt.Key_Enter ||
                             event.key === Qt.Key_Return)
                    {
                        listView.incrementCurrentIndex()
                    }
                    else if (event.key === Qt.Key_Space) {
                        listView.incrementCurrentIndex()
                    }
                    else if (event.key === Qt.Key_Period) {
                        listView.incrementCurrentIndex()
                    }
                }
            }

            TextInput {
                anchors.verticalCenter: parent.verticalCenter
                visible: index > 0
                text: "."
                readOnly: true
                color: textInput.color
                selectByMouse: true
                selectionColor: textInput.selectionColor
            }
        }

        Keys.onLeftPressed: {
            var oldIndex = listView.currentIndex;
            listView.decrementCurrentIndex()

            var newIndex = listView.currentIndex;
            if (oldIndex != newIndex)
                listView.currentItem.cursorPosition = listView.currentItem.text.length
        }

        Keys.onRightPressed: {
            var oldIndex = listView.currentIndex;
            listView.incrementCurrentIndex()

            var newIndex = listView.currentIndex;
            if (oldIndex != newIndex)
                listView.currentItem.cursorPosition = 0
        }
    }
}
