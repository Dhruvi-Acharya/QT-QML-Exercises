import QtQuick 2.0
import QtQuick.Controls 2.15

Button{
    id: button
    property color hoverColor: "#585858"
    property color pressColor: "#484848"
    property color normalColor: "#505050"

    font.capitalization: Font.Capitalize
    font.bold: true

    background: Rectangle{
        id: background
        color: normalColor
        radius: 2
    }

    contentItem: Text {
        text: button.text
        font: button.font
        opacity: enabled ? 1.0 : 0.3
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    states: [
        State {
            name: "Hovering"
            PropertyChanges {
                target: background
                color: hoverColor
            }
        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: background
                color: pressColor
            }
        },
        State {
            name: "Normal"
            PropertyChanges {
                target: background
                color:normalColor
            }
        }
    ]
    MouseArea{
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
