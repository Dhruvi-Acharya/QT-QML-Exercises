import QtQuick 2.12
import QtQuick.Controls 2.12

BusyIndicator {
    id: control

    property int tempwidth : 30
    property int tempheight : 30

    contentItem: Item {
        implicitWidth: tempwidth
        implicitHeight: tempheight

        Item {
            id: item
            x: 0
            y: 0
            width: parent.width
            height: parent.height

            RotationAnimator {
                target: item
                running: control.visible && control.running
                from: 0
                to: 360
                loops: Animation.Infinite
                duration: 500
            }

            Repeater {
                id: repeater
                model: 6

                Rectangle {
                    x: item.width / 2 - width / 2
                    y: item.height / 2 - height / 2
                    implicitWidth: 5
                    implicitHeight: 5
                    radius: 5
                    color: WisenetGui.orange
                    transform: [
                        Translate {
                            y: -Math.min(item.width, item.height) * 0.5 + 5
                        },
                        Rotation {
                            angle: index / repeater.count * 360
                            origin.x: 3
                            origin.y: 3
                        }
                    ]
                }
            }
        }
    }
    background: Rectangle{
        color: "transparent"
    }
}
