import QtQuick 2.12

Item {
    id: item1
    Rectangle{
        width: 120
        height: 225
        color: "black"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Column{
            anchors.centerIn: parent
            spacing: 10

            Light{
                id: redlight
                colorOn: "red"
                state: "on"
                colorOff: "darkred"
                width: 100
                height: 100
            }
            Light{
                id: greenlight
                colorOn: "limegreen"
                state: "on"
                colorOff: "darkgreen"
                width: 100
                height: 100
            }
        }


    }
    states: [
        State {
            name: "stop"
            PropertyChanges {
                target: redlight
                state: "on"
            }
            PropertyChanges {
                target: greenlight
                state: "off"
            }
        },
        State {
            name: "go"
            PropertyChanges {
                target: redlight
                state: "off"
            }
            PropertyChanges {
                target: greenlight
                state: "on"

            }
        }
    ]

}
