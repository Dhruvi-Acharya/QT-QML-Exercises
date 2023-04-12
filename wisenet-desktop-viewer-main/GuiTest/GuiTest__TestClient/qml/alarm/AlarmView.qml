import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: alarmlistView
    anchors.fill: parent
    clip: true

    delegate: Rectangle {
        id: alarmArea
        //anchors.left: parent.left
        //anchors.right: parent.right
        width: parent.width
        height: 56
        color: {
            if(alarmMouseArea.containsMouse)
                return "#484848"
            else
                return "#2d2d2d"
        }
        border.color: "#4f4f4f"
        border.width: 1

        Flow {
            id: alarmLayout
            anchors.fill: parent
            anchors.rightMargin: 2
            anchors.leftMargin: 5
            anchors.bottomMargin: 4
            anchors.topMargin: 4
            spacing: 2

            Rectangle {
                id: colorRect
                x: 2
                y: 2
                width: 15
                height: 15
                color: colorCode
            }

            Text {
                id: channelName
                x: 20
                width: parent.width - 18
                height: 15
                text: channel
                font.pixelSize: 12
                color: "white"
            }

            Text {
                id: occurrenceTime
                width: parent.width - 3
                height: 15
                text: time
                font.pixelSize: 12
                color: "white"
            }

            Text {
                id: alarmType
                width: parent.width - 3
                height: 15
                text: alarmName
                font.pixelSize: 12
                color: "white"
            }
        }

        MouseArea
        {
            id: alarmMouseArea
            hoverEnabled: true
            anchors.fill: parent
        }

        ToolTip {
            id: alarmTooltip
            x: alarmArea.x + 20
            y: alarmArea.y + alarmArea.height
            visible: alarmMouseArea.containsMouse
            delay: 200
            text: {
                channelName.text + "\r\n\r\n" + occurrenceTime.text + "\r\n\r\n" + alarmType.text;
            }
        }

    }
    ScrollBar.vertical: ScrollBar {}
    model: alarmModel

    add: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
        NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
    }

    displaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }

        // ensure opacity and scale values return to 1.0
        NumberAnimation { property: "opacity"; to: 1.0 }
        NumberAnimation { property: "scale"; to: 1.0 }
    }
}
