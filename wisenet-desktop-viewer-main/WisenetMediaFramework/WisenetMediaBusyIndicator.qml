import QtQuick 2.12
import QtQuick.Controls 2.12

BusyIndicator {
    id: control

    property alias sourceWidth: spinnerImage.sourceSize.width
    property alias sourceHeight: spinnerImage.sourceSize.height

    contentItem: Item {
        implicitWidth: parent.width
        implicitHeight: parent.height

        Item {
            id: item
            anchors.fill: parent
            opacity: control.running ? 1 : 0

            Behavior on opacity {
                OpacityAnimator {
                    duration: 200
                }
            }

            RotationAnimator {
                target: item
                running: control.visible && control.running
                from: 0
                to: 360
                duration: 1000
                property bool _fastLoop: false
                onFinished: {
                    _fastLoop = !_fastLoop;
                    if (_fastLoop) {
                        duration = 400;
                        loops = 2;
                    }
                    else {
                        duration = 800;
                        loops = 1;
                    }

                    if (control.running) {
                        start();
                    }
                }
            }

            Image {
                id: spinnerImage
                //source: "images/spinner.png"
                source: "images/loading.svg"
                sourceSize: "72x72"
                anchors.fill: parent
            }
        }
    }
}
