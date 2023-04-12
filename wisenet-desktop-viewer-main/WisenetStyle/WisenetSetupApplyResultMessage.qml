import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import Wisenet.Define 1.0

Rectangle {
    id: applyResultMessage
    color: WisenetGui.contrast_09_dark
    border.color: WisenetGui.contrast_06_grey
    border.width: 1
    width: applyResultMessageText.width
    height: applyResultMessageText.height
    visible: false

    function showMessage(message, isHide = true)
    {
        autoHideTimer.stop();
        closeAnim.stop();

        applyResultMessageText.text = message;
        visible = true
        opacity = 0.8
        autoHideTimer.isHide = isHide
        autoHideTimer.start()
    }

    Text {
        id: applyResultMessageText
        anchors.centerIn: parent
        width: implicitWidth+30
        height: implicitHeight+10
        font.pixelSize: 14
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: WisenetGui.contrast_02_light_grey
    }
    Timer {
        id: autoHideTimer
        interval: 1500
        repeat: false
        property bool isHide : true
        onTriggered: {
            if(isHide)
                closeAnim.start();
        }
    }
    NumberAnimation{
        id:closeAnim
        target: applyResultMessage;
        properties:"opacity";
        to:0
        duration:1500
        onFinished: {
            applyResultMessage.visible = false;
            applyResultMessage.opacity = 0;
        }
    }
}
