import QtQuick 2.0
import QtQuick.Controls 2.15

Button{
    id: button
    text : ""
    implicitWidth: 100
    //width: contentItem.implicitWidth + leftPadding + rightPadding
    leftPadding: 0

    hoverEnabled: true
    font.pixelSize:  12
    checkable: true
    property alias textHAlignment : txt.horizontalAlignment

    background: Rectangle {
        color: WisenetGui.transparent
    }

    palette.buttonText: {
        if(!enabled)
            return WisenetGui.contrast_06_grey
        else
            return WisenetGui.contrast_01_light_grey
    }

    contentItem: Text {
        id : txt
        text: button.text
        width: button.width - button.leftPadding - button.rightPadding
        height: button.height
        wrapMode: Text.Wrap
        font: button.font
        color: checked? WisenetGui.contrast_00_white : (hovered? WisenetGui.contrast_01_light_grey : WisenetGui.contrast_06_grey)
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
       // elide: Text.ElideRight

    }
}
