import QtQuick 2.15
import "qrc:/WisenetStyle/"

Rectangle {
    id: sleepModePanel
    visible: opacity > 0.01
    color: WisenetGui.contrast_11_bg
    opacity: 0

    function updateStatus(isOn) {
        if (isOn) {
            hideAnim.stop();
            sleepModeLoader.visible = false;
            opacity = 0.5;
        }
        else {
            hideAnim.restart();
            sleepModeLoader.visible = true;
        }
    }

    ClassicLoader {
        id: sleepModeLoader
        anchors.centerIn: parent

        visible:false
        running:visible
        //useCircle:false

        property real maxWidth: 320
        property real calcWidth: visible?parent.width/8:0
        width: visible?Math.min(calcWidth, maxWidth):0
        height: width
    }

    NumberAnimation{
        id: hideAnim
        target: sleepModePanel;
        properties:"opacity";
        to: 0
        duration:1000
        onStopped: {
            sleepModeLoader.visible = false;
        }
    }

}
