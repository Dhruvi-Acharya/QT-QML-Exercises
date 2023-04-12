import QtQuick 2.15
import WisenetStyle 1.0

TextInput{
    id: textField
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignLeft
    color: WisenetGui.white
    selectByMouse: true
    selectionColor: WisenetGui.dimgray
    font.pixelSize: 10

    text: ""
    focus: true
    visible: false

    activeFocusOnPress: false

    // 마우스 커서 변경 코드 - 동작안함... 추후 더 확인
    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: Qt.IBeamCursor

        onContainsMouseChanged: {
            console.log("onContainsMouseChanged " + containsMouse);
        }
    }
}
