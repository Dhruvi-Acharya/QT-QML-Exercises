import QtQuick 2.15

Item{
    id: button
    width : 80
    height : 30

    //normalColor: "#ff6c00"
    //hoverColor: "#ff7d10"
    //pressColor: "#f06200"

    property alias text : innerText.text
    property color hoverColor: "orange"
    property color pressColor: "red"
    property color color: "gray"
    property int fontSize: 11
    property color fontColor : "white"

    Rectangle {
        id: rectangleButton
        anchors.fill: parent
        radius: 5
        color: button.enabled ? button.color : "black"

        Text {
            id: innerText
            color: fontColor
            font.pixelSize:  fontSize
            anchors.centerIn: parent
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
}
