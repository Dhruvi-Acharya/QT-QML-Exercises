import QtQuick 2.15
import QtQuick.Controls 2.15

Button{
    id: control

    property var imageNormal
    property var imageSelected
    property var imageHover
    property var imageDisabled
    property var imageSelectedHover


    property var imageChecked
    property var imageCheckedHover

    property alias sourceWidth : image.sourceSize.width
    property alias sourceHeight : image.sourceSize.height

    property alias checkSourceWidth : checkedImage.sourceSize.width
    property alias checkSourceHeight : checkedImage.sourceSize.height

    checkable: true

    width: 32
    height: 28

    background: Rectangle {
        implicitWidth: image.width
        implicitHeight: image.width
        color: WisenetGui.transparent
    }

    Image {
        id: image

        width: 28
        height: 28

        anchors.left: parent.left
        anchors.top: parent.top

        source: !control.enabled ? control.imageDisabled :
                                   control.hovered? ((control.checked && control.checkable) ? control.imageSelectedHover : control.imageHover) :
                                                    ((control.checked && control.checkable) ? control.imageSelected : control.imageNormal)
    }

    Image {
        id: checkedImage
        width: 16
        height: 16
        visible: control.checked
        source: control.hovered ? control.imageCheckedHover : control.imageChecked

        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
