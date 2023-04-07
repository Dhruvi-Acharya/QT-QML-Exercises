import QtQuick 2.12

Item {
    id: item1
    property bool isOn

    onIsOnChanged: {
        if(isOn)
        {
            console.log("ON")
            state = "on"
        }
        else
        {
            console.log("OFF")
            state = "off"
        }
    }

    width: 200
    height: 200

    Rectangle{
        id: rectangle
        anchors.fill: parent
        color: "grey"
        radius: width
    }
    states: [

        State {
            name: "on"
            PropertyChanges {
                target: rectangle
                color: "limegreen"
            }
        },
        State {
            name: "off"
            PropertyChanges {
                target: rectangle
                color: "red"
            }
        }
    ]
}
