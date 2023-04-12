import QtQuick 2.15
import QtQuick.Controls 2.15

Button{
    id: control

    property alias fontColor: textInButton.color
    contentItem: Text {
        id: textInButton
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: WisenetGui.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        id: background
        color: WisenetGui.transparent
    }

    states: [
        State {
            name: "Hovering"
            PropertyChanges {
                target: control
                fontColor: WisenetGui.hoverColor
            }
        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: control
                fontColor: WisenetGui.pressColor
            }
        },
        State {
            name: "Normal"
            PropertyChanges {
                target: control
                fontColor: WisenetGui.textColor
            }
        }
    ]
    MouseArea {
        hoverEnabled: true
        anchors.fill: control
        onEntered: { control.state='Hovering'}
        onExited: { control.state='Normal'}
        onClicked: { control.clicked();}
        onPressed: { control.state="Pressed" }
        onReleased: {
            if (containsMouse)
                control.state="Hovering";
            else
                control.state="";
        }
    }
}
