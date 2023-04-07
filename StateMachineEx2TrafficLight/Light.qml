import QtQuick 2.12

Item {
    id: element
    property color colorOn: "red"
    property color colorOff: "grey"

    Rectangle{
        id: rect
        anchors.fill: parent
        radius: width
        color: "blue"
    }

    states: [
        State {
            name: "on"
            PropertyChanges {
                target: rect
                color: colorOn

            }
        },
        State {
            name: "off"
            PropertyChanges {
                target: rect
                color: colorOff
            }
        }
    ]

}
