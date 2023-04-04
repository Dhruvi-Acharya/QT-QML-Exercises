import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Component{
        id: listdelegate
        Item {
            width: ListView.view.width
            height: 50

            Column{
                Text {
                    text: "Name: " + name
                }
                Text {
                    text: "Number: " + number
                }
            }
        }
    }

    Component {
        id: listViewDelegate
        Rectangle {
            width: 100
            height: 50

            ListView.onPooled: rotationAnimation.pause()
            ListView.onReused: rotationAnimation.resume()

            Rectangle {
                id: rect
                anchors.centerIn: parent
                width: 40
                height: 5
                color: "green"

                RotationAnimation {
                    id: rotationAnimation
                    target: rect
                    duration: (Math.random() * 2000) + 200
                    from: 0
                    to: 359
                    running: true
                    loops: Animation.Infinite
                }
            }
        }
    }

    ListView{
        id: ll
        Component{
            id: lvdelegate
            Rectangle {
                width: ltext.width
                height: ltext.height
                color: ListView.isCurrentItem ? "grey" : "red"

                Text{
                    id: ltext
                    text: "Name: " + name + " Number: " + number
                    color: ListView.isCurrentItem ? "orange" : "yellow"
                }
            }
        }

        anchors.centerIn: parent
        width: 200
        height: 400

        model: ContactModel {}
        //        delegate: Text { text: name + ": " + number}
        delegate: listdelegate
        //        delegate: lvdelegate
        //        delegate: listViewDelegate
        highlight: Rectangle {color: "lightgreen"; radius: 10}
//        highlightRangeMode: ItemView.ApplyRange
        focus: true
        orientation: ListView.Vertical
        layoutDirection: "RightToLeft"
        verticalLayoutDirection: ItemView.BottomToTop
        flickableDirection: Flickable.HorizontalAndVerticalFlick

        add: Transition {
            NumberAnimation{
                properties: "x,y"
                from: 100
                duration: 5000
            }

        }
        displaced: Transition {
            NumberAnimation{
                properties: "x,y"
                from: 100
                duration: 5000
            }

        }

        snapMode: ListView.SnapToItem
        highlightRangeMode: ItemView.StrictlyEnforceRange
    }

//    ListView {
//        width: 240; height: 320
//        model: ContactModel {}

//        delegate: Rectangle {
//            width: 100; height: 30
//            border.width: 1
//            color: "lightsteelblue"
//            Text {
//                anchors.centerIn: parent
//                text: name
//            }
//        }

//        add: Transition {
//            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 4000 }
//            NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 4000 }
//        }


//        displaced: Transition {
//            NumberAnimation { properties: "x,y"; duration: 4000; easing.type: Easing.OutBounce }
//        }

//        focus: true
//        Keys.onSpacePressed: model.insert(0, { "name": "Item " + model.count })
//    }
}
