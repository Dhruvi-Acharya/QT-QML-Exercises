import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetLanguage 1.0

WisenetPopupWindowBase {
    id: baseDialog
    width: 529
    height: 235
    minmaxButtonVisible: false
    resizeEnabled: false

    property bool applyClose: true
    property alias closeButtonVisible: closeButton.visible

    property alias message: messageText.text
    property alias messageAlign: messageText.horizontalAlignment

    onApplyButtonClicked: if(applyClose) baseDialog.close()

    header: Rectangle {
        color: WisenetGui.transparent
        height: 34
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onPressed: {
                pressedX = mouseX
                pressedY = mouseY
            }
            onPositionChanged: {
                var newX = baseDialog.x + mouseX - pressedX
                var newY = baseDialog.y + mouseY - pressedY

                if(newX+10 > Screen.desktopAvailableWidth)
                    newX = Screen.desktopAvailableWidth - 10
                if(newY+10 > Screen.desktopAvailableHeight)
                    newY = Screen.desktopAvailableHeight - 10

                baseDialog.x = newX
                baseDialog.y = newY
            }
        }
        Button {
            id: closeButton
            visible: true
            y: 11
            width: 16
            height: 16
            hoverEnabled: true
            background: Rectangle {
                color: WisenetGui.transparent
            }
            anchors.right: parent.right
            anchors.rightMargin: 9
            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: parent.pressed ? WisenetImage.popup_close_press :
                        parent.hovered ? WisenetImage.popup_close_hover : WisenetImage.popup_close_default
            }
            onClicked: {
                baseDialog.close()
            }
        }
    }

    Text {
        id: messageText
        anchors.fill: parent
        anchors.leftMargin: 36
        anchors.rightMargin: 36
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: lineCount == 1 ? Text.AlignHCenter : Text.AlignLeft
        font.pixelSize: 12
        color: WisenetGui.contrast_00_white
        wrapMode: Text.Wrap
    }
}
