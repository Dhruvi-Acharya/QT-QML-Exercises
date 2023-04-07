import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Component.onCompleted: {
        light1.state = "on"
    }

    function toggle(parent)
    {
        if(parent.state === "on")
        {
            parent.state = "off"
        }
        else{
            parent.state = "on"
        }
    }

    function enforce(primary,secondary)
    {
        if(primary.state === "on")
        {
            secondary.state = "off"
        }
        else{
            secondary.state = "on"
        }
    }

    Row {
        //        width: 200
        //        height: 100
        anchors.centerIn: parent
        spacing: 50

        Light{
            id: light1
            width: 100
            height: 100
            state: "off"
            onStateChanged: enforce(light1,light2)

            MouseArea {
                anchors.fill: parent
                onClicked: toggle(parent)
            }

        }

        Light{
            id: light2
            width: 100
            height: 100
            state: "off"
            onStateChanged: enforce(light2,light1)

            MouseArea{
                anchors.fill: parent
                onClicked: toggle(parent)
            }
        }
    }
}
