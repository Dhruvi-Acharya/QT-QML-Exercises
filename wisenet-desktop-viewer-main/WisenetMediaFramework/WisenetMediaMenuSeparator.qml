import QtQuick 2.15
import QtQuick.Controls 2.15

MenuSeparator {
    id:seperator
    padding: 2
    verticalPadding: 0//padding + 4
    readonly property color seperatorColor: "#505050"

    height: visible ? implicitHeight : 0
    contentItem: Rectangle {
        implicitWidth: 200
        implicitHeight: 1
        color: seperator.seperatorColor
    }
}
