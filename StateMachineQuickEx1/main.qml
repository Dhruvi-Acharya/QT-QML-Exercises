import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import QtQml.StateMachine 1.0 as DSM

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    DSM.StateMachine{
        running: true
        initialState: state1
        DSM.State{
            id: state1
            onEntered: label.text = "State 1"
            DSM.SignalTransition{
                targetState: state2
                signal: btn.clicked
            }
        }
        DSM.State{
            id: state2
            onEntered: label.text = "State 2"
            DSM.SignalTransition {
                targetState: state3
                signal: btn.clicked
            }
        }
        DSM.State {
            id: state3
            onEntered: label.text = "State 3"
            DSM.SignalTransition {
                targetState: state1
                signal: btn.clicked
            }
        }
    }

    Column{
        spacing: 10
        anchors.centerIn: parent
        Label{
            id: label
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Label"
        }
        Button{
            id: btn
            text: "Click Me"
        }
    }
}
