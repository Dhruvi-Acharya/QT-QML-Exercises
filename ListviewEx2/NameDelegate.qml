import QtQuick 2.12

Text {
    id: txt
    clip: true
    text: model.name
    font.pixelSize: 24
    property bool expanded: false
    height: expanded ? implicitHeight : 0
    Behavior on height {
        NumberAnimation { duration: 200}
    }
}
