import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
    id: control
    width: 228
    height: 17
    rightPadding: 3
    leftPadding: 3
    font.pixelSize: 12
    hoverEnabled: true

    property int itemTextMargin: 16

    delegate: ItemDelegate {
        width: control.width
        height: 20
        contentItem: Text {
            anchors.fill: parent
            anchors.leftMargin: itemTextMargin
            anchors.rightMargin: itemTextMargin
            text: modelData
            color: parent.pressed ? WisenetGui.contrast_02_light_grey :
                   parent.highlighted ? WisenetGui.contrast_01_light_grey : WisenetGui.contrast_02_light_grey
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            padding: 0
        }
        highlighted: control.highlightedIndex === index
        background: Rectangle{
            anchors.fill: parent
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            color: parent.pressed ? WisenetGui.color_primary_press :
                   parent.highlighted ? WisenetGui.color_primary : WisenetGui.transparent
        }
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 8
        height: 6
        contextType: "2d"
        onPaint: {
            var context = getContext("2d")
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = !control.enabled ? WisenetGui.contrast_07_grey :
                                 control.hovered ? WisenetGui.contrast_03_light_grey : WisenetGui.contrast_05_grey
            context.fill();
        }
    }
    onEnabledChanged: { canvas.requestPaint(); }
    onPressedChanged: { canvas.requestPaint(); }
    onHoveredChanged: { canvas.requestPaint(); }

    contentItem: Text {
        anchors.fill: parent
        leftPadding: control.leftPadding
        rightPadding: control.indicator.width + control.spacing
        text: control.displayText
        font: control.font
        color: !control.enabled ? WisenetGui.contrast_06_grey :
                control.hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        anchors.fill: parent
        color: WisenetGui.transparent
        Rectangle {
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
            color: control.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
        }
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 0

        contentItem: Rectangle {
            color: WisenetGui.transparent
            width: control.width
            implicitHeight: list.implicitHeight + 2
            ListView {
                id: list
                anchors.fill: parent
                anchors.topMargin: 1
                clip: true
                implicitHeight: contentHeight
                model: control.popup.visible ? control.delegateModel : null
                currentIndex: control.highlightedIndex
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: WisenetScrollBar{}
            }
        }

        background: Rectangle {
            color: WisenetGui.contrast_09_dark
            border.color: WisenetGui.contrast_07_grey
            border.width: 1
        }
    }
}
