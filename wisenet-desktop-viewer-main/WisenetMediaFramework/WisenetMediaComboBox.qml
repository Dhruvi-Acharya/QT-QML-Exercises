import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

ComboBox {
    id: control
    width: 228
    height: 17
    rightPadding: 3
    leftPadding: 3
    font.pixelSize: 12
    hoverEnabled: true

    delegate: ItemDelegate {
        width: control.width
        height: control.height
        contentItem: Text {
            anchors.fill: parent
            text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
            color: WisenetGui.contrast_02_light_grey
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            leftPadding: control.leftPadding
        }
        highlighted: control.highlightedIndex === index
        background: Rectangle{
            anchors.fill: parent
            anchors.margins: 1
            color: parent.highlighted ? WisenetGui.color_primary : WisenetGui.transparent
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
            color: control.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_08_dark_grey
        }
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight + padding * 2
        padding: 0

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            //ScrollIndicator.vertical: ScrollIndicator { }
            ScrollBar.vertical: WisenetMediaScrollBar{}
        }

        background: Rectangle {
            color: WisenetGui.contrast_09_dark
            border.color: WisenetGui.contrast_07_grey
            border.width: 1
        }
    }
}
