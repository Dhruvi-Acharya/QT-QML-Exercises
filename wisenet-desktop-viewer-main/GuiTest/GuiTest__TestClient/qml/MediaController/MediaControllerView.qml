import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Wisenet.Qmls 0.1

// 미디어 컨트롤러의 Main View
Item {
    id: mediaController
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: 90
    state: "show"

    // 데이터 모델들
    MediaControlModel {
        id: mediaControlModel
    }

    TimeAreaModel {
        id: timeModel
    }

    DateAreaModel {
        id: dateAreaModel
    }

    GraphAreaModel {
        id: graphAreaModel
    }
    //

    // ShowHide 상태
    states: [
        State {
            name: "show"
            PropertyChanges {
                target: mediaController
                height: 90
            }
        },
        State {
            name: "hide"
            PropertyChanges {
                target: mediaController
                height: 0
            }
        }
    ]

    // ShowHide Animation
    Behavior on height {
        NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
    }

    // ShowHide Button
    Rectangle {
        id: showHideButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.top
        width: 32
        height: 10

        Image{
            anchors.fill: parent
            source: {
                if(showHideButtonMouseArea.containsMouse)
                {
                    if (mediaController.state =="hide")
                        source: "icon/arrow_up_o.png"
                    else
                        source: "icon/arrow_down_o.png"
                }
                else
                {
                    if (mediaController.state == "hide")
                        source: "icon/arrow_up_n.png"
                    else
                        source: "icon/arrow_down_n.png"
                }
            }
        }

        MouseArea {
            id: showHideButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked:
            {
                if (mediaController.state == "hide")
                {
                    mediaController.state = "show"
                    baseArea.visible = true;
                    showHideToolTip.text = "Hide Timeline"
                }
                else
                {
                    mediaController.state = "hide"
                    baseArea.visible = false;
                    showHideToolTip.text = "Show Timeline"
                }
            }
        }

        ToolTip {
            id: showHideToolTip
            parent: showHideButtonMouseArea
            visible: showHideButtonMouseArea.containsMouse
            delay: 1000
            text: "Hide Timeline"
        }
    }

    // Calendar
    CalendarControlView {
        id: calendarControl
        anchors.bottom: parent.top
        anchors.right: parent.right
        visible: false
        Component.onCompleted: {
            calendarControl.calendar.minimumDate = new Date(2021, 0, 10, 13)
            calendarControl.calendar.maximumDate = new Date() //today
        }
        onVisibleChanged: {
            if(!visible)
                calendarButton.state = "Off"
        }
    }

    Rectangle {
        id: baseArea
        anchors.fill: parent
        color: "transparent"

        Behavior on visible  {
            NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // Left Area
            Rectangle {
                id: leftArea
                Layout.alignment: Qt.AlignRight
                width: 180
                height: 90
                color: "transparent"
                border.color: "#444444"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // Speed Slider
                    Slider {
                        id: speedSlider
                        Layout.leftMargin: 3
                        Layout.rightMargin: 3
                        Layout.preferredHeight: 29
                        Layout.preferredWidth: 180
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        hoverEnabled: true
                        value: 0
                        from: -50
                        to: 50

                        property int lastValue : 0

                        // Custom background
                        background: Rectangle {
                            x: speedSlider.leftPadding
                            y: speedSlider.topPadding + speedSlider.availableHeight / 2 - height / 2
                            implicitWidth: 200
                            implicitHeight: 2
                            width: speedSlider.availableWidth
                            height: implicitHeight
                            radius: 2
                            color: "#565656"

                            Rectangle {
                                width: speedSlider.visualPosition * parent.width
                                height: parent.height
                                color: "#565656"
                                radius: 2
                            }
                        }

                        // Custom handle
                        handle: Rectangle {
                            x: speedSlider.leftPadding + speedSlider.visualPosition * (speedSlider.availableWidth - width)
                            y: speedSlider.topPadding + speedSlider.availableHeight / 2 - height / 2
                            implicitWidth: 16
                            implicitHeight: 16
                            radius: 8
                            color: "#303030"
                            border.color: "#919191"
                            border.width: 2
                        }

                        ToolTip {
                            id: speedToolTip
                            parent: speedSlider.handle
                            visible: speedSlider.hovered | speedSlider.pressed
                            text: "Paused"
                        }

                        onValueChanged: {
                            // Update toolTip text
                            var value = speedSlider.value

                            if(value < -45)
                                speedToolTip.text = "-16x"
                            else if(value < -35)
                                speedToolTip.text = "-8x"
                            else if(value < -25)
                                speedToolTip.text = "-4x"
                            else if(value < -15)
                                speedToolTip.text = "-2x"
                            else if(value < -5)
                                speedToolTip.text = "-1x"
                            else if(value < 5)
                                speedToolTip. text = "Paused"
                            else if(value < 15)
                                speedToolTip.text = "1x"
                            else if(value < 25)
                                speedToolTip.text = "2x"
                            else if(value < 35)
                                speedToolTip.text = "4x"
                            else if(value < 45)
                                speedToolTip.text = "8x"
                            else
                                speedToolTip.text = "16x"
                        }

                        onPressedChanged: {
                            // pressed 해제 시 마지막 value로 복귀
                            if(speedSlider.pressed == false)
                                speedSlider.value = lastValue
                        }

                        Behavior on value {
                            NumberAnimation {
                                duration: 300
                            }
                        }
                    }

                    // Media Control Buttons
                    RowLayout {
                        Layout.leftMargin: 3
                        Layout.rightMargin: 3
                        Layout.preferredHeight: 32
                        Layout.preferredWidth: 180
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        spacing: 1

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            Button {
                                id: previousChunkButton
                                anchors.fill: parent
                                flat: true

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Previous Chunk (Z)"

                                Image {
                                    source: "icon/mediacontrol_previous_chunk.png"
                                    anchors.fill: parent
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            Button {
                                id: speedDownButton
                                anchors.fill: parent
                                flat: true
                                state: "previousFrame"

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Previous Frame (Ctrl+Left)"

                                Image {
                                    anchors.fill: parent
                                    source: {
                                        if(speedDownButton.state == "previousFrame")
                                            sourec: "icon/mediacontrol_previous_frame.png"
                                        else
                                            sourec: "icon/mediacontrol_speed_down.png"
                                    }
                                }

                                onClicked: {
                                    if(speedDownButton.state == "speedDown"){
                                        var newValue = speedSlider.value - 10
                                        speedSlider.value = newValue
                                        speedSlider.lastValue = newValue

                                        speedToolTip.show(speedToolTip.text, 3000)
                                    }
                                }

                                states: [
                                    State {
                                        name: "previousFrame"
                                        PropertyChanges {
                                            target: speedDownButton
                                            ToolTip.text: "Previous Frame (Ctrl+Left)"
                                        }
                                    },
                                    State {
                                        name: "speedDown"
                                        PropertyChanges {
                                            target: speedDownButton
                                            ToolTip.text: "Speed Down (Ctrl+Left)"
                                        }
                                    }
                                ]
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            Button {
                                id: playPauseButton
                                anchors.fill: parent
                                flat: true
                                checked: true

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Play"

                                Image {
                                    anchors.fill: parent
                                    source: {
                                        if(playPauseButton.checked == true)
                                            sourec: "icon/mediacontrol_play.png"
                                        else
                                            sourec: "icon/mediacontrol_pause.png"
                                    }
                                }

                                onClicked: {
                                    if(speedDownButton.state == "speedDown")
                                        speedDownButton.state = "previousFrame"
                                    else
                                        speedDownButton.state = "speedDown"

                                    if(speedUpButton.state == "speedUp")
                                        speedUpButton.state = "nextFrame"
                                    else
                                        speedUpButton.state = "speedUp"

                                    if(playPauseButton.checked) {
                                        speedSlider.value = 0
                                        speedSlider.lastValue = 0
                                        playPauseButton.ToolTip.text = "Play"                                        
                                        mediaControlModel.requestDeviceMediaControl(mediaControlModel.pause)
                                    }
                                    else {

                                        speedSlider.value = 10
                                        speedSlider.lastValue = 10
                                        playPauseButton.ToolTip.text = "Pause"
                                        mediaControlModel.requestDeviceMediaControl(mediaControlModel.play)
                                    }

                                    speedToolTip.show(speedToolTip.text, 3000)
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            Button {
                                id: speedUpButton
                                anchors.fill: parent
                                flat: true
                                state: "nextFrame"

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Next Frame (Ctrl+Right)"

                                Image {
                                    anchors.fill: parent
                                    source: {
                                        if(speedUpButton.state == "nextFrame")
                                            sourec: "icon/mediacontrol_next_frame.png"
                                        else
                                            sourec: "icon/mediacontrol_speed_up.png"
                                    }
                                }

                                onClicked: {
                                    if(speedUpButton.state == "speedUp"){
                                        var newValue = speedSlider.value + 10
                                        speedSlider.value = newValue
                                        speedSlider.lastValue = newValue

                                        speedToolTip.show(speedToolTip.text, 3000)
                                    }
                                }

                                states: [
                                    State {
                                        name: "nextFrame"
                                        PropertyChanges {
                                            target: speedUpButton
                                            ToolTip.text: "Next Frame (Ctrl+Right)"
                                        }
                                    },
                                    State {
                                        name: "speedUp"
                                        PropertyChanges {
                                            target: speedUpButton
                                            ToolTip.text: "Speed Up (Ctrl+Right)"
                                        }
                                    }
                                ]
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            Button {
                                id: nextChunkButton
                                anchors.fill: parent
                                flat: true

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Next Chunk (X)"

                                Image {
                                    source: "icon/mediacontrol_next_chunk.png"
                                    anchors.fill: parent
                                }
                            }
                        }
                    }

                    // Digital Clock
                    Label {
                        id: digitalClock

                        Layout.leftMargin: 5
                        Layout.preferredHeight: 29
                        Layout.preferredWidth: 180
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        verticalAlignment: Text.AlignVCenter
                        font.family: "Tahoma"
                        font.weight: Font.DemiBold
                        font.bold: true
                        font.pointSize: 11
                        color: "#e46108"
                        text: new Date().toLocaleTimeString()

                        // Refresh Timer
                        Timer {
                            interval: 1000; running: true; repeat: true;
                            onTriggered: digitalClock.timeChanged()
                        }

                        // Update time text
                        function timeChanged() {
                            digitalClock.text = new Date().toLocaleTimeString()
                        }
                    }
                }
            }


            // Timeline Area
            Rectangle {
                id: centerArea
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                border.color: "#444444"
                border.width: 1

                TimelineControlView {
                    anchors.fill: parent
                }
            }


            // Right Area
            Rectangle {
                id: rightArea
                Layout.alignment: Qt.AlignRight
                width: 120
                height: 90
                color: "transparent"
                border.color: "#444444"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 3
                    spacing: 0

                    RowLayout {
                        spacing: 0
                        Layout.preferredHeight: 30

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredWidth: 30
                            color: "transparent"

                            // Mute Button
                            Button {
                                id: muteButton
                                anchors.fill: parent
                                flat: true
                                state: "muteOff"

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Toggle Mute (U)"

                                states: [
                                    State { name: "muteOn" },
                                    State { name: "muteOff" }
                                ]

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: {
                                        if(muteButton.state == "muteOn")
                                            sourec: "icon/mute_on.png"
                                        else
                                            sourec: "icon/mute_off.png"
                                    }
                                }

                                onClicked: {
                                    if(muteButton.state == "muteOn")
                                    {
                                        muteButton.state = "muteOff"
                                        volumeSlider.lastValue = volumeSlider.value
                                        volumeSlider.value = 0
                                    }
                                    else
                                    {
                                        muteButton.state = "muteOn"
                                        volumeSlider.value = volumeSlider.lastValue
                                    }

                                    volumeToolTip.show(volumeToolTip.text, 3000)
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.preferredWidth: 90
                            color: "transparent"

                            // Volume Slider
                            Slider {
                                id: volumeSlider
                                anchors.fill: parent
                                hoverEnabled: true
                                value: 0
                                from: 0
                                to: 100
                                stepSize: 1

                                property int lastValue: 100

                                // Custom background
                                background: Rectangle {
                                    x: volumeSlider.leftPadding
                                    y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                                    implicitWidth: 200
                                    implicitHeight: 2
                                    width: volumeSlider.availableWidth
                                    height: implicitHeight
                                    radius: 2
                                    color: "#565656"

                                    Rectangle {
                                        width: volumeSlider.visualPosition * parent.width
                                        height: parent.height
                                        color: "#565656"
                                        radius: 2
                                    }
                                }

                                // Custom handle
                                handle: Rectangle {
                                    x: volumeSlider.leftPadding + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                                    y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                                    implicitWidth: 16
                                    implicitHeight: 16
                                    radius: 8
                                    color: "#303030"
                                    border.color: "#919191"
                                    border.width: 2
                                }

                                ToolTip {
                                    id: volumeToolTip
                                    parent: volumeSlider.handle
                                    visible: volumeSlider.hovered | volumeSlider.pressed
                                    text: "Muted"
                                }

                                onValueChanged: {
                                    if(value === 0) {
                                        volumeToolTip.text = "Muted"
                                        muteButton.state = "muteOff"
                                    }
                                    else {
                                        volumeToolTip.text = volumeSlider.value + "%"
                                        muteButton.state = "muteOn"
                                    }
                                }
                            }
                        }
                    }

                    GridLayout {
                        rows: 2
                        columns: 2
                        rowSpacing: 0
                        columnSpacing: 0
                        Layout.preferredHeight: 60

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            // Jump to Live Button
                            Button {
                                id: liveButton
                                anchors.fill: parent
                                flat: true
                                state: "On"

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Jump to Live (L)"

                                states: [
                                    State { name: "On" },
                                    State { name: "Off" }
                                ]

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: {
                                        if(liveButton.state == "On")
                                            sourec: "icon/jumptolive_on.png"
                                        else
                                            sourec: "icon/jumptolive_off.png"
                                    }
                                }

                                onClicked: {
                                    if(liveButton.state == "On") {
                                        liveButton.state = "Off"
                                    }
                                    else {
                                        liveButton.state = "On"
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            // Synchronize Stream Button
                            Button {
                                id: syncStreamButton
                                anchors.fill: parent
                                flat: true
                                state: "On"

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Disable Stream Synchronization (S)"

                                states: [
                                    State {
                                        name: "On"
                                        PropertyChanges {
                                            target: syncStreamButton
                                            ToolTip.text: "Disable Stream Synchronization (S)"
                                        }
                                    },
                                    State {
                                        name: "Off"
                                        PropertyChanges {
                                            target: syncStreamButton
                                            ToolTip.text: "Synchronize Streams (S)"
                                        }
                                    }
                                ]

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: {
                                        if(syncStreamButton.state == "On")
                                            sourec: "icon/syncstream_on.png"
                                        else
                                            sourec: "icon/syncstream_off.png"
                                    }
                                }

                                onClicked: {
                                    if(syncStreamButton.state == "On") {
                                        syncStreamButton.state = "Off"
                                    }
                                    else {
                                        syncStreamButton.state = "On"
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            // Thumbnail Button
                            Button {
                                id: thumbnailButton
                                anchors.fill: parent
                                flat: true
                                state: "Off"

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Hide Thumbnails"

                                states: [
                                    State {
                                        name: "On"
                                        PropertyChanges {
                                            target: thumbnailButton
                                            ToolTip.text: "Hide Thumbnails"
                                        }
                                    },
                                    State {
                                        name: "Off"
                                        PropertyChanges {
                                            target: thumbnailButton
                                            ToolTip.text: "Show Thumbnails"
                                        }
                                    }
                                ]

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: {
                                        if(thumbnailButton.state == "On")
                                            sourec: "icon/thumbnail_on.png"
                                        else
                                            sourec: "icon/thumbnail_off.png"
                                    }
                                }

                                onClicked: {
                                    if(thumbnailButton.state == "On") {
                                        thumbnailButton.state = "Off"
                                    }
                                    else {
                                        thumbnailButton.state = "On"
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: "transparent"

                            // Calendar Button
                            Button {
                                id: calendarButton
                                anchors.fill: parent
                                flat: true
                                state: "Off"
                                hoverEnabled: true

                                ToolTip.delay: 1000
                                ToolTip.visible: hovered
                                ToolTip.text: "Hide Calendar"

                                states: [
                                    State {
                                        name: "On"
                                        PropertyChanges {
                                            target: calendarButton
                                            ToolTip.text: "Hide Calendar"
                                        }
                                    },
                                    State {
                                        name: "Off"
                                        PropertyChanges {
                                            target: calendarButton
                                            ToolTip.text: "Show Calendar"
                                        }
                                    }
                                ]

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    source: {
                                        if(calendarButton.state == "On")
                                            sourec: "icon/calendar_on.png"
                                        else
                                            sourec: "icon/calendar_off.png"
                                    }
                                }

                                onClicked: {
                                    if(calendarButton.state == "On") {
                                        calendarButton.state = "Off"
                                        calendarControl.visible = false
                                    }
                                    else {
                                        calendarButton.state = "On"
                                        calendarControl.visible = true
                                    }
                                }

                                onHoveredChanged: {
                                    if(!hovered && !calendarControl.isPinned)
                                        calendarControl.autoHideTimerRestart()
                                    else
                                        calendarControl.autoHideTimerStop()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
