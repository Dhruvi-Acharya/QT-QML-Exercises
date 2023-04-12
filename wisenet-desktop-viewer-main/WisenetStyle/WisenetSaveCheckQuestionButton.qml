import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetLanguage 1.0

Image{
    id: root
    width: 16
    height: 16
    sourceSize: "16x16"

    source: mouseArea.containsMouse ? WisenetImage.setup_device_question_hover : WisenetImage.setup_device_question_normal

    MouseArea{
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true
    }

    ToolTip{
        id: toolTip

        x: mouseArea.x
        y: mouseArea.y

        visible: mouseArea.containsMouse

        width: 220
        height: rule1.height + 20

        background: Rectangle {
            color: WisenetGui.contrast_08_dark_grey
            border.color: WisenetGui.contrast_07_grey
            border.width: 1
        }

        Rectangle{
            color: "transparent"
            anchors.fill: parent
            anchors.topMargin: 3
            anchors.bottomMargin: 3
            anchors.leftMargin: 3
            anchors.rightMargin: 3

            Text{
                id: rule1

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                width: 215
                text: WisenetLinguist.savedAuthenticationMessage
                wrapMode: Text.WordWrap
                color: WisenetGui.contrast_03_light_grey
                font.pixelSize: 11
            }
        }
    }

}
