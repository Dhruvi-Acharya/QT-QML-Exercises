import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

ToolTip {
    id: control

    leftPadding: 5
    rightPadding: 5
    topPadding: 3
    bottomPadding: 3 + 5
    font.pixelSize: 11

    contentItem: Text {
        text: control.text
        font: control.font
        color: WisenetGui.contrast_08_dark_grey
        padding: 2
    }

    background: Rectangle {
        color: WisenetGui.transparent
        Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: 5
            color: WisenetGui.contrast_02_light_grey
            radius: 2
        }
        Canvas {
            width: 8
            height: 8
            x: -control.x + control.parent.width/2 - width/2
            anchors.bottom: parent.bottom
            contextType: "2d"
            onPaint: {
                var context = getContext("2d")
                context.reset();
                context.moveTo(0, 0);
                context.lineTo(width, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = WisenetGui.contrast_02_light_grey
                context.fill();
            }
        }
    }
}
