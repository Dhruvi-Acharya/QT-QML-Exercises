import QtQuick 2.15
import QtQuick.Controls 2.15

Button{
    id: control

    property var imageNormal: ""
    property var imageSelected: ""
    property var imageHover: ""
    property var imageDisabled: ""

    property alias sourceWidth : image.sourceSize.width
    property alias sourceHeight : image.sourceSize.height

    background: Rectangle {
        implicitWidth: image.width
        implicitHeight: image.height
        color: WisenetGui.transparent
    }

    Image {
        id: image
        width: control.sourceWidth
        height: control.sourceHeight
        anchors.centerIn: parent
        source: !control.enabled ? control.imageDisabled :
                                   control.hovered? control.imageHover : ((control.checked && control.checkable) ? control.imageSelected : control.imageNormal)
    }
}
