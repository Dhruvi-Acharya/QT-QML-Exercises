import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0

Button {
    id: wisenetAlldeviceCheckBoxButton

    property string selectedIcon : WisenetImage.all_device_select
    property string hoveredIcon : WisenetImage.all_device_hover
    property string normalIcon : WisenetImage.all_device_normal

    function changeCheckBoxText(checkBoxText){
        wisenetAlldeviceCheckBoxText.text = checkBoxText
    }

    signal checkStateChanged(var state)

    flat: true
    anchors.fill: parent
    hoverEnabled: true
    checkable: true
    background: Rectangle {
        color: WisenetGui.transparent
    }

    Image {
        id: wisenetAlldeviceCheckBoxButtonImage
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        width: 16
        height: 16
        sourceSize: Qt.size(16, 16)
        source: wisenetAlldeviceCheckBoxButton.normalIcon
    }

    Text{
        id: wisenetAlldeviceCheckBoxText
        text: WisenetLinguist.allDevices
        anchors.left: wisenetAlldeviceCheckBoxButtonImage.right
        anchors.right: parent.right
        anchors.leftMargin: 6
        anchors.rightMargin: 1
        anchors.verticalCenter: parent.verticalCenter

        color: WisenetGui.contrast_00_white
        font.pixelSize: 12
    }

    onHoveredChanged: {
        if(checked){
            wisenetAlldeviceCheckBoxButtonImage.source = wisenetAlldeviceCheckBoxButton.selectedIcon
        }else if(hovered){
            wisenetAlldeviceCheckBoxButtonImage.source = wisenetAlldeviceCheckBoxButton.hoveredIcon
        }else {
            wisenetAlldeviceCheckBoxButtonImage.source = wisenetAlldeviceCheckBoxButton.normalIcon
        }
    }

    onCheckedChanged: {
        console.debug("onCheckedChanged ............")
        if(checked){
            wisenetAlldeviceCheckBoxButtonImage.source = wisenetAlldeviceCheckBoxButton.selectedIcon
        }else if(hovered){
            wisenetAlldeviceCheckBoxButtonImage.source = wisenetAlldeviceCheckBoxButton.hoveredIcon
        }else {
            wisenetAlldeviceCheckBoxButtonImage.source = wisenetAlldeviceCheckBoxButton.normalIcon
        }

        checkStateChanged(checked)
    }

}
