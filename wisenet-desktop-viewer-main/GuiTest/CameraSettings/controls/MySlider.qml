import QtQuick 2.12

Item {
    id: slider
    height: 26
    width: 400
    property real min: 1
    property real max: 50
    property real init: 1
    property real value: min + (max - min) * dragHandler.value

    property string name: "Slider"
    property color color: "#585858"
    property real minLabelWidth: 44


    DragHandler {
        id: dragHandler
        target: handle
        xAxis.minimum: Math.round(-handle.width / 2 + 3)
        xAxis.maximum: Math.round(foo.width - handle.width/2 - 3)
        property real value: (handle.x - xAxis.minimum) / (xAxis.maximum - xAxis.minimum)
    }

    Component.onCompleted: setValue(init)
    function setValue(v) {
       if (min < max)
          handle.x = Math.round( v / (max - min) *
                                (dragHandler.xAxis.maximum - dragHandler.xAxis.minimum)
                                + dragHandler.xAxis.minimum);
    }
    Rectangle {
        id:sliderName
        anchors.left: parent.left
        anchors.leftMargin: 16
        height: childrenRect.height
        width:70
        //width: Math.max(slider.minLabelWidth, childrenRect.width)
        anchors.verticalCenter: parent.verticalCenter
        color: "transparent"
        Text {
            text: slider.name
            font.pointSize: 10
            color: "white"
        }
    }
    Rectangle{
        id: foo
        width: parent.width - 8 - sliderName.width
        color: "#585858"
        height: 7
        radius: 3
        antialiasing: true
        border.color: Qt.darker(color, 1.2)
        anchors.left: sliderName.right
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 24
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            height: parent.height
            anchors.left: parent.left
            anchors.right: handle.horizontalCenter
            color: slider.color
            radius: 3
            border.width: 1
            border.color: Qt.darker(color, 1.3)
            opacity: 0.8
        }

        Image {
            id: handle
            source: "images/slider_handle.png"
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
