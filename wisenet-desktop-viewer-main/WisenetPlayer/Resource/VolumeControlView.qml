import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import "qrc:/WisenetStyle"

Popup{
    id: volumeControlView
    width: 200
    height: 40

    background: Rectangle {
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1
    }

    // Default 값이 있음.
    // https://doc.qt.io/qt-5/qml-qtquick-controls2-popup.html
    topInset: 0
    leftInset: 0
    rightInset: 0
    bottomInset: 0
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    property url normalSource
    property url pressSource
    property url hoverSource

    Rectangle{
        anchors.fill: parent
        color: "transparent"

        Image{
            id: volumeButton
            width: 24
            height: 24
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter

            sourceSize: Qt.size(width, height)
            source: volumeButtonMouseArea.pressed ? volumeControlView.pressSource :
                    (volumeButtonMouseArea.containsMouse ? volumeControlView.hoverSource : volumeControlView.normalSource)

            MouseArea
            {
                id: volumeButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    gAudioManager.enable = !gAudioManager.enable
                    if (gAudioManager.volume === 0 && gAudioManager.enable) {
                        gAudioManager.volume = 50;
                    }
                }
            }
        }

        WisenetSlider {
            width: 130

            anchors.left: volumeButton.right
            anchors.leftMargin: 6

            from : 0
            to : 100
            value: gAudioManager.volume
            valid: gAudioManager.enable
            onValueChanged: {
                gAudioManager.volume = value;
                gAudioManager.enable = value > 0
            }
        }
    }
}

