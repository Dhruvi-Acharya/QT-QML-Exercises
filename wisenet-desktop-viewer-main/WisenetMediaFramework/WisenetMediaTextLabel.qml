import QtQuick 2.15
import "qrc:/WisenetStyle/"

Item {
    id: root
    property alias text: textLabel.text
    property alias hAlignment: textLabel.horizontalAlignment
    property alias vAlignment: textLabel.verticalAlignment
    property alias textElide : textLabel.elide
    property alias textTruncated: textLabel.truncated
    property bool autoResize : false

    height:14
    width:80

    Text{
        id: textLabel
        color: WisenetGui.contrast_04_light_grey
        width: parent.width
        height: parent.height
        fontSizeMode: autoResize ? Text.VerticalFit : Text.FixedSize
        minimumPixelSize: 4
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }
}
