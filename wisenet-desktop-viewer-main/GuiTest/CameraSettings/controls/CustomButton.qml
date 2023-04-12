import QtQuick 2.0
Item{
    id: button

    property alias text : innerText.text
    property color hoverColor: "orange"
    property color pressColor: "red"

    width : 80
    height : 30

    Rectangle {
        id: rectangleButton
        anchors.fill: parent
        radius: 5
        color: button.enabled ? button.color : "grey"

        Text {
            id: innerText
            font.pointSize: fontSize
            anchors.centerIn: parent
        }
    }

    MouseArea {
        hoverEnabled: true
        anchors.fill: button
        onEntered: { button.state='Hovering'}
        onExited: { button.state='Normal'}
        onClicked: { button.clicked();}
        onPressed: { button.state="Pressed" }
        onReleased: {
            if (containsMouse)
                button.state="Hovering";
            else
                button.state="";
        }
    }
    states: [
        State {
            name: "Hovering"
            PropertyChanges {
                target: rectangleButton
                color: hoverColor
            }
        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: rectangleButton
                color: pressColor
            }
        },
        State {
            name: "Normal"
            PropertyChanges {
                target: rectangleButton
                color: "grey"
            }
        }
    ]
}
