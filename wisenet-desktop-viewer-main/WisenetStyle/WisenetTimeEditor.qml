import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 108
    height: 17

    property alias buttonVisible: buttonRect.visible
    property int hour: Number(hourTextField.text)
    property int minute: Number(minuteTextField.text)
    property int second: Number(secondTextField.text)
    property bool controlFocus: focusScope.activeFocus
    property TextField lastTextField: secondTextField
    property int leftPadding: 3
    property int textPixelSize: 12

    function setTimeText(hour, minute, second) {
        hourTextField.text = hour < 10 ? "0" + hour : hour
        minuteTextField.text = minute < 10 ? "0" + minute : minute
        secondTextField.text = second < 10 ? "0" + second : second
    }

    function getTimeText() {
        if(hour < 10)
            hourTextField.text = "0" + hour
        if(minute < 10)
            minuteTextField.text = "0" + minute
        if(second < 10)
            secondTextField.text = "0" + second

        return hourTextField.text + ":" + minuteTextField.text + ":" + secondTextField.text
    }

    onVisibleChanged: {
        if(visible)
            focusScope.focus = true
        else
            focusScope.focus = false
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter || event.key === Qt.Key_Escape)
            focusScope.focus = false
    }

    onTextPixelSizeChanged: {
        hourTextField.font.pixelSize = textPixelSize
        minuteTextField.font.pixelSize = textPixelSize
        secondTextField.font.pixelSize = textPixelSize
        separator1.font.pixelSize = textPixelSize
        separator2.font.pixelSize = textPixelSize
    }

    Rectangle {
        width: parent.width
        height: 1
        color: WisenetGui.contrast_06_grey
        anchors.bottom: parent.bottom
    }

    FocusScope {
        id: focusScope
        anchors.fill: parent

        Row {
            id: textRow
            height: 14
            anchors.left: parent.left
            anchors.leftMargin: root.leftPadding
            anchors.right: buttonRect.left
            spacing: 0

            component BaseTextField: TextField {
                height: 14
                padding: 0
                leftPadding: 0
                background: Rectangle {
                    color: WisenetGui.transparent
                }
                font.pixelSize: 12
                color: !enabled ? WisenetGui.contrast_06_grey : hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                selectionColor: WisenetGui.color_primary_deep_dark
                selectedTextColor: WisenetGui.contrast_00_white
                verticalAlignment: TextInput.AlignVCenter
                horizontalAlignment: TextInput.AlignHCenter
                validator: RegExpValidator { regExp: /^[0-9]?[0-9]$/ }
                inputMethodHints: Qt.ImhDigitsOnly
                selectByMouse: true
                text: "00"
                hoverEnabled: true

                Keys.onPressed: {
                    var time = Number(text)

                    if(event.key === Qt.Key_Up) {
                        time += 1
                        text = time < 10 ? "0" + String(time) : String(time)
                    }
                    else if(event.key === Qt.Key_Down && time !== 0) {
                        time -= 1
                        text = time < 10 ? "0" + String(time) : String(time)
                    }
                }
            }

            BaseTextField {
                id: hourTextField
                KeyNavigation.tab: minuteTextField

                onTextChanged: {
                    var time = Number(text)

                    if(time > 23)
                        text = "23"

                    if(!activeFocus && time < 10)
                        text = "0" + time
                }

                onActiveFocusChanged: {
                    if(activeFocus) {
                        lastTextField = hourTextField
                        selectAll()
                    }
                    else {
                        var time = Number(text)
                        if(time < 10)
                            text = "0" + time
                    }
                }
            }

            BaseTextField {
                id: separator1
                text: ":"
                enabled: false
                color: !root.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_02_light_grey
            }

            BaseTextField {
                id: minuteTextField
                KeyNavigation.tab: secondTextField

                onTextChanged: {
                    var time = Number(text)

                    if(time > 59)
                        text = "59"

                    if(!activeFocus && time < 10)
                        text = "0" + time
                }

                onActiveFocusChanged: {
                    if(activeFocus) {
                        lastTextField = minuteTextField
                        selectAll()
                    }
                    else {
                        var time = Number(text)
                        if(time < 10)
                            text = "0" + time
                    }
                }
            }

            BaseTextField {
                id: separator2
                text: ":"
                enabled: false
                color: !root.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_02_light_grey
            }

            BaseTextField {
                id: secondTextField
                KeyNavigation.tab: hourTextField

                onTextChanged: {
                    var time = Number(text)

                    if(time > 59)
                        text = "59"

                    if(!activeFocus && time < 10)
                        text = "0" + time
                }

                onActiveFocusChanged: {
                    if(activeFocus) {
                        lastTextField = secondTextField
                        selectAll()
                    }
                    else {
                        var time = Number(text)
                        if(time < 10)
                            text = "0" + time
                    }
                }
            }
        }

        Rectangle {
            id: buttonRect
            width: upButton.width
            height: upButton.height * 2 + 2
            anchors.right: parent.right
            anchors.rightMargin: 3
            color: WisenetGui.transparent

            Button {
                id: upButton
                width: 8
                height: 6
                anchors.right: parent.right
                anchors.top: parent.top
                hoverEnabled: true
                background: Rectangle {
                    anchors.fill: parent
                    color: WisenetGui.transparent
                    Canvas {
                        id: upButtonCanvas
                        anchors.fill: parent
                        contextType: "2d"
                        onPaint: {
                            context.reset();
                            context.moveTo(0, height);
                            context.lineTo(width, height);
                            context.lineTo(width / 2, 0);
                            context.closePath();
                            context.fillStyle = !upButton.enabled ? WisenetGui.contrast_07_grey :
                                                 upButton.hovered ? WisenetGui.contrast_03_light_grey : WisenetGui.contrast_05_grey
                            context.fill();
                        }
                    }
                }

                onHoveredChanged: {
                    upButtonCanvas.requestPaint()
                }

                onEnabledChanged:  {
                    upButtonCanvas.requestPaint()
                }

                onClicked: {
                    var time = Number(lastTextField.text)
                    lastTextField.text = String(time + 1)
                }
            }

            Button {
                id: downButton
                width: 8
                height: 6
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                hoverEnabled: true
                background: Rectangle {
                    anchors.fill: parent
                    color: WisenetGui.transparent
                    Canvas {
                        id: downButtonCanvas
                        anchors.fill: parent
                        contextType: "2d"
                        onPaint: {
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = !downButton.enabled ? WisenetGui.contrast_07_grey :
                                                 downButton.hovered ? WisenetGui.contrast_03_light_grey : WisenetGui.contrast_05_grey
                            context.fill();
                        }
                    }
                }

                onHoveredChanged: {
                    downButtonCanvas.requestPaint()
                }

                onEnabledChanged:  {
                    downButtonCanvas.requestPaint()
                }

                onClicked: {
                    var time = Number(lastTextField.text)
                    if(time !== 0)
                        lastTextField.text = String(time - 1)
                }
            }
        }
    }
}
