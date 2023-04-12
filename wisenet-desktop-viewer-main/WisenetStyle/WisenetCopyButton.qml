import QtQuick 2.0

Rectangle {
    id: rec
    color: "transparent"
    signal release

    width: 80
    height: 20
    radius: 1


    Image {
        id: uncheckIcon
        width: 20
        height: 20
        source: "Icon/unchecked.png"
        fillMode: Image.PreserveAspectFit
        clip: true
        anchors.top: parent.top
        anchors.left: parent.left
        opacity: 1
    }

    Image {
        id: checkIcon
        width: 20
        height: 20
        source: "Icon/checked.png"
        fillMode: Image.PreserveAspectFit
        clip: true
        anchors.top: parent.top
        anchors.left: parent.left
        opacity: 0
    }


    Text {
        anchors.leftMargin: 10
        id: copyButton
        text: qsTr("copy")
        anchors.left: uncheckIcon.right
        anchors.centerIn: parent
        font.pixelSize: 10
        color: "white"
    }
    Text {
        anchors.leftMargin: 10
        id: copiedButton
        text: qsTr("copied")
        anchors.left: uncheckIcon.right
        anchors.centerIn: parent
        font.pixelSize: 10
        color: "white"
        opacity: 0.0
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onReleased: {
            parent.release()
            seqAnimation.start()
        }
    }
    SequentialAnimation{
        id : seqAnimation
        ParallelAnimation{
            NumberAnimation{
                target: copyButton
                property: "opacity"
                duration: 0
                to : 0
            }

            NumberAnimation{
                target: copiedButton
                property: "opacity"
                duration: 0
                to : 1
            }

            NumberAnimation{
                target: uncheckIcon
                property: "opacity"
                duration: 0
                to : 0
            }

            NumberAnimation{
                target: checkIcon
                property: "opacity"
                duration: 0
                to : 1
            }

        }
        NumberAnimation{
            duration: 2000
        }

        ParallelAnimation{
            NumberAnimation{
                target: copiedButton
                property: "opacity"
                duration: 500
                to : 0
            }
            NumberAnimation{
                target: checkIcon
                property: "opacity"
                duration: 500
                to : 0
            }
        }
        ParallelAnimation{
            NumberAnimation{
                target: copyButton
                property: "opacity"
                duration: 500
                to : 1
            }
            NumberAnimation{
                target: uncheckIcon
                property: "opacity"
                duration: 500
                to : 1
            }
        }
    }
}
