import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    TrafficLight{
        id: trafficlight
        anchors.centerIn: parent
    }

    Row {
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter
        id: row
        y: 10
        height: 100

        Button {
            id: btnGo
            text: qsTr("Go")
            onClicked: trafficlight.state = "go"
        }

        Button {
            id: btnStop
            text: qsTr("Stop")
            onClicked: trafficlight.state = "stop"
        }

        Button {
            id: btnToggle
            text: qsTr("Toggle")
            onClicked: {
                if(trafficlight.state == "go")
                {
                    trafficlight.state = "stop"
                }
                else{
                    trafficlight.state = "go"
                }
            }
        }
    }
}
