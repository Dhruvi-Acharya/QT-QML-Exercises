import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    color: "lightpink"
    // title: qsTr("Hello World")

    ListModel {
        id: lmodel
        ListElement {
            name: "Bill Jones"

        }
        ListElement {
            name: "Jane Doe"

        }
        ListElement {
            name: "John Smith"

        }
    }



    Component {
        id: delegate
        Column {
            id: wrapper
            opacity: PathView.isCurrentItem ? 1 : 0.5
            Rectangle {
                anchors.horizontalCenter: nameText.horizontalCenter
                width: 64; height: 64
                color: "black"
            }
            Text {
                id: nameText
                text: name
                font.pointSize: 16
            }
        }
    }

    PathView {
        anchors.fill: parent
        model: lmodel
        delegate: delegate
        path: Path {
            startX: 120; startY: 100
            PathQuad { x: 120; y: 20; controlX: 260; controlY: 75 }
            PathQuad { x: 120; y: 100; controlX: -20; controlY: 75 }
        }
    }
}
