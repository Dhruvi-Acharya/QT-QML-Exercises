import QtQuick 2.15
import "qrc:/WisenetStyle/"

// https://github.com/Furkanzmc/QML-Loaders/blob/master/Loaders/PulseLoader.qml
Item {
    // ----- Public Properties ----- //

    property alias barCount: repeater.model
    property color color: WisenetGui.contrast_04_light_grey
    property int spacing: 5

    property bool running: true
    property real yScale: 1.7

    id: root
    onRunningChanged: {
        if (barCount !== repeater.count || timer._barIndex <= barCount - 1) {
            return;
        }

        for (var barIndex = 0; barIndex < barCount; barIndex++) {
            if (running) {
                if (repeater.itemAt(barIndex)) {
                    repeater.itemAt(barIndex).playAnimation();
                }
            }
            else {
                if (repeater.itemAt(barIndex)) {
                    repeater.itemAt(barIndex).pauseAnimation();
                }
            }
        }
    }

    Repeater {
        id: repeater
        delegate: Component {
            Rectangle {
                width: (root.width / root.barCount) - root.spacing
                height: root.height
                x: index * width + root.spacing * index
                transform: Scale {
                    id: rectScale
                    origin {
                        x: width / 2
                        y: height / 2
                    }
                }
                transformOrigin: Item.Center
                color: root.color

                SequentialAnimation {
                    id: anim
                    loops: Animation.Infinite

                    NumberAnimation { target: rectScale; property: "yScale"; from: 1; to: root.yScale; duration: 300 }
                    NumberAnimation { target: rectScale; property: "yScale"; from: root.yScale; to: 1; duration: 300 }
                    PauseAnimation { duration: root.barCount * 150 }
                }

                function playAnimation() {
                    if (anim.running == false) {
                        anim.running = true;
                    }

                    if (anim.paused) {
                        anim.paused = false;
                    }
                }

                function pauseAnimation() {
                    if (anim.running) {
                        anim.paused = true;
                    }
                }
            }
        }
    }

    Timer {
        // ----- Private Properties ----- //
        property int _barIndex: 0

        id: timer
        interval: 80
        repeat: true
        onTriggered: {
            if (_barIndex === root.barCount) {
                stop();
            }
            else {
                repeater.itemAt(_barIndex).playAnimation();
                if (root.running === false) {
                    repeater.itemAt(_barIndex).pauseAnimation();
                }

                _barIndex++;
            }
        }
        Component.onCompleted: start()
    }
}
