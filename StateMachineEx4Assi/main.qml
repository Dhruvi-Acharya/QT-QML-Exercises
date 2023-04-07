import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.3

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    function fun()
    {
        if(light.isOn === "true")
        {
            light.isOn = "false"
        }
        else{
            light.isOn = "true"
        }
    }

    Column {
        width: 300
        height: 300
        spacing: 30
        anchors.centerIn: parent

        Light{
            anchors.horizontalCenter: parent.horizontalCenter
            id: light
            height: 200
            width: 200
        }


        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10

            Button{
                id: btnOn
                font.pixelSize: 20
                text: "On"
                onClicked: fun()
            }
            Button{
                id: btnOff
                font.pixelSize: 20
                text: "Off"
                onClicked: fun()
//                {
//                    console.log("button Clicked")
//                    light.isOn = "false"
//                }
            }
        }
    }
}
