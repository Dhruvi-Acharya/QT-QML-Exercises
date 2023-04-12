import QtQuick 2.0
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import QtQuick.Dialogs 1.3
import WisenetLanguage 1.0
import Wisenet.Setup 1.0


Button{
    id: button
    //text : WisenetLinguist.remove
    implicitWidth: 100
    width: 100//contentItem.implicitWidth + leftPadding + rightPadding

    hoverEnabled: true
    font.pixelSize: 12



    background: Rectangle {
        color: WisenetGui.transparent
    }

    Image {
        id: deleteButtonImage
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        //
        width: 18
        height: 18
        sourceSize: Qt.size(18, 18)
        source: pressed?WisenetImage.setup_file_remove_default:WisenetImage.setup_file_remove_default
    }

    palette.buttonText: {
        if(!enabled)
            return WisenetGui.contrast_06_grey
        else
            return WisenetGui.contrast_01_light_grey
    }

    onHoveredChanged: {
        if(pressed){
            deleteButtonImage.source = WisenetImage.setup_file_remove_default
        }
        else if(hovered){
            deleteButtonImage.source = WisenetImage.setup_file_remove_default
        }
        else {
            deleteButtonImage.source = WisenetImage.setup_file_remove_default
        }
    }



    contentItem: Text {
        text: button.text
        font: button.font
        opacity: enabled ? 1.0 : 0.3
        color: WisenetGui.white
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        anchors.leftMargin: 5
        anchors.left: deleteButtonImage.right
    }
    Rectangle {
        anchors.fill: parent
        color: WisenetGui.transparent
    }
}
