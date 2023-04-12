import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

/////////////////////////////////////////////////////////////
// 파일 저장 알림 팝업 (TODO design)
// 명시적 x button 추가 필요
// mouse over시 auto hide timer reset 동작 필요
/////////////////////////////////////////////////////////////
Rectangle {
    id: fileSavedPopup
    width: messageGroup.implicitWidth + 22
    height:50
    visible: opacity > 0.01
    opacity: 0

    property string message
    property string fileDir

    color: WisenetGui.contrast_09_dark
    border.color: WisenetGui.contrast_06_grey
    border.width: 1
    //radius : height*0.2

    property bool bShow: false

    signal openFolder();
    Row {
        id: messageGroup
        anchors {
            centerIn: parent
            topMargin: 11
            bottomMargin: 11
        }
        height: parent.height
        spacing: 4

        Text {
            id: message
            height: parent.height
            width: implicitWidth + 30
            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 12
            text: fileSavedPopup.message
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        WisenetMediaTextButtonItem {
            width: textImplicitWidth + 20
            height: 28
            text: WisenetLinguist.openFolder
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                closeAnim.start();
                fileSavedPopup.openFolder();
            }
            onHoveredChanged: {
                if (hovered) {
                    autoHideTimer.stop();
                }
                else {
                    autoHideTimer.restart();
                }
            }
        }
    }

    function show(msg, dirPath)
    {
        fileSavedPopup.message = msg
        fileSavedPopup.fileDir = dirPath
        autoHideTimer.restart();
        if (!visible) {
            closeAnim.stop();
            openAnim.restart();
        }
    }

    NumberAnimation{
        id:closeAnim
        target: fileSavedPopup;
        properties:"opacity";
        to:0
        duration:500
    }

    NumberAnimation{
        id:openAnim
        target: fileSavedPopup;
        properties:"opacity";
        to:0.8
        duration:100
    }

    Timer {
        id: autoHideTimer
        interval: 5000
        repeat: false
        onTriggered: {
            closeAnim.start();
        }
    }
}
