import QtQuick 2.12
import QtQuick.Controls 2.12

ComboBox {
    id: control
    height: 15
    width: parent.width

    //리스트
    delegate: ItemDelegate {
        width: control.width

        highlighted: control.highlightedIndex === index
        contentItem: Text {
            text: modelData
            color: "white"
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            leftPadding: 10

        }
        background: Rectangle{
            anchors.fill: parent
            color: parent.highlighted ? "#f06200" : "#505050"
        }
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = control.pressed ? "#989898" : "#7b7b7b";
            context.fill();
        }
    }

    contentItem: Text {
        leftPadding: 10
        text: control.displayText
        font: control.font
        color: "white"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 30
        color: control.hovered ? "#989898" : "#505050"
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: "#606060"
            radius: 2
        }
    }
}
