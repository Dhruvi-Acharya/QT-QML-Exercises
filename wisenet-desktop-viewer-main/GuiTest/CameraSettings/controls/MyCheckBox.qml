import QtQuick 2.0

Item {
    id: root

// public
    property string text:    'text'
    property bool   checked: false

// private
    property real padding: 0.1

    width: height + text.width ;  height: 15
    opacity: enabled? 1: 0.3

    Item { // check box
        id: rectangle
        width: root.height
        height: root.height
        anchors {verticalCenter: parent.verticalCenter; leftMargin: padding * root.height}

        Image { // check
            visible: checked
            source: "images/checked.png"
            anchors.fill: parent
        }

        Image{ // uncheck
            visible: !checked
            source: "images/unchecked.png"
            anchors.fill: parent
        }
    }

    Text {
        id: text
        text: root.text
        anchors {left: rectangle.right;  verticalCenter: rectangle.verticalCenter;  margins: padding * root.height}
        font.pixelSize: root.height * 0.8
        color: checked ? "#e8e8e8" :  "#b8b8b8"
    }

    //uncheck states
    states: [
        State {
            name: "Hovering"
            PropertyChanges {
                target: text
                color: "#c8c8c8"
            }
        },
        State {
            name: "Normal"
            PropertyChanges {
                target: text
                color: "#b8b8b8"
            }
        }
    ]

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: checked = !checked
        onEntered: { if(!checked) root.state='Hovering'}
        onExited: { if(!checked) root.state='Normal'}
    }
}
