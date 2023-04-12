import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Rectangle {
    id: layoutWarning
    height: 28
    width: messageText.width
    color: WisenetGui.contrast_09_dark
    border.color: WisenetGui.contrast_06_grey
    border.width: 1
    visible: opacity > 0.01
    opacity: 0
    property int hideSeconds: 2000
    property alias messageText: messageText.text

    function showMessage(msg)
    {
        messageText.text = msg
        closeAnim.stop();
        autoHideTimer.restart();
        openAnim.restart();
    }

    Text {
        id: messageText
        width: implicitWidth + 30
        height: parent.height
        color: WisenetGui.contrast_02_light_grey
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    NumberAnimation{
        id:closeAnim
        target: layoutWarning;
        properties:"opacity";
        to:0
        duration:500
    }

    NumberAnimation{
        id:openAnim
        target: layoutWarning;
        properties:"opacity";
        to:0.8
        duration:100
    }

    Timer {
        id: autoHideTimer
        interval: layoutWarning.hideSeconds
        repeat: false
        onTriggered: {
            closeAnim.start();
        }
    }
}
