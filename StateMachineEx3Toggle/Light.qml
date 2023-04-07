import QtQuick 2.12

Item {
    property color colorOff: "grey"
    property color colorOn: "red"

    Rectangle {
        id: rectangle
        radius: width
        color: "red"
        anchors.fill: parent
    }

    states: [
        State {
            name: "on"
            PropertyChanges {
                target: rectangle
                color: colorOn
            }
        },
        State {
            name: "off"
            PropertyChanges {
                target: rectangle
                color: colorOff

            }
        }
    ]
}
