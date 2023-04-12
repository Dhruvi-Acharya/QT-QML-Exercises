import QtQuick 2.15

Item {
    id: root

    // public
    property string text: ''
    property bool checked: false

    signal clicked(bool checked)

    property string iconSource : ''
    property int iconSize: 16
    property int iconMargin: 8

    // private
    property real padding: 0.1

    width: height + text.width
    height: 16
    opacity: enabled? 1: 0.3

    Item { // check box
        id: rectangle
        width: root.height
        height: root.height

        anchors {verticalCenter: parent.verticalCenter; leftMargin: padding * root.height}

        Image { // check
            visible: checked
            source: (!root.enabled ? WisenetImage.check_on_dim :
                     root.pressed ? WisenetImage.check_on_press : WisenetImage.check_on)
            sourceSize.width: root.height
            sourceSize.height: root.height
            anchors.fill: parent
        }

        Image{ // uncheck
            visible: !checked
            source: (!root.enabled ? WisenetImage.check_off_dim :
                     root.pressed ? WisenetImage.check_off_press : WisenetImage.check_off)
            sourceSize.width: root.height
            sourceSize.height: root.height
            anchors.fill: parent
        }
    }

    Image{
        id:imageIcon
        visible: root.iconSource!==''
        anchors{
            left:rectangle.right
            leftMargin: iconMargin
            verticalCenter: rectangle.verticalCenter
        }
        width: root.iconSize
        height: root.iconSize
        sourceSize:Qt.size(iconSize,iconSize)
        source: root.iconSource
    }

    Text {
        id: text
        text: root.text
        anchors {
            left: root.iconSource===''?rectangle.right:imageIcon.right
            verticalCenter: rectangle.verticalCenter
            leftMargin: padding * root.height + 5
            rightMargin: padding * root.height
            topMargin: padding * root.height
            bottomMargin: padding * root.height
        }
        font.pixelSize: root.height * 0.8
        color: !root.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_01_light_grey
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
        onClicked: {
            checked = !checked
            root.clicked(checked)
        }
        onEntered: { if(!checked) root.state='Hovering'}
        onExited: { if(!checked) root.state='Normal'}
    }
}
