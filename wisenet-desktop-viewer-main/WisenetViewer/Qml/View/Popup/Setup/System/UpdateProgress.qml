import QtQuick 2.0
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import QtQuick.Dialogs 1.3
import WisenetLanguage 1.0
import Wisenet.Setup 1.0

ProgressBar {
    id: progressBar
    width: 175
    height: 8
    anchors.verticalCenter: parent.verticalCenter

    from: 0
    to: 100

    background: Rectangle {
        color: WisenetGui.contrast_07_grey
    }

    contentItem: Item {
        anchors.fill: parent
        Rectangle {
            width: progressBar.visualPosition * parent.width
            height: parent.height
            color: WisenetGui.contrast_01_light_grey
        }
    }
}
