import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ListModel{
        id: lmodel
        ListElement{name: "A"; team: "A"}
        ListElement{name: "B"; team: "B"}
        ListElement{name: "C"; team: "C"}
        ListElement{name: "D"; team: "A"}
        ListElement{name: "E"; team: "B"}
        ListElement{name: "F"; team: "C"}
        ListElement{name: "G"; team: "A"}
        ListElement{name: "H"; team: "B"}
    }

    Component{
        id: ldelegate
        Rectangle{
//            color: "lightblue"
//            color: ListView.isCurrentItem?"#157efb":"#53d769"
            color: ListView.isCurrentItem ? "blue" : "skyblue"
            border.width: 2
            border.color: "black"
            radius: 10
            height: txt.implicitHeight + 3
            width: parent.width

            Text {
                id: txt
                font.pixelSize: 24
                text: model.name
                anchors.horizontalCenter: parent.horizontalCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked:
                {
                    parent.ListView.view.currentIndex = index
                    lmodel.insert(index,{name: "New Item"}) // new item added at index position
                    //                        Listview.view.currenIndex = index
                    console.log("Item "+ model.index + " Clicked " + model.team)
                    console.log("The item is clicked" + model.index + model.team)
                    lmodel.append({"name": "Append "+index}) // new item appended at end of the list
                    lmodel.remove(index)
                }
            }
        }

    }

    Rectangle {
        anchors.centerIn: parent
        width: 200
        height: 300
        color: "grey"

        ListView {
            id: lv1
            anchors.fill: parent
            model: lmodel
            delegate: ldelegate
            clip: true
            spacing: 3

            add: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 1000 }
                NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 2000 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 400;  }
            }
        }


//                ListView {
//                    id: lv2
//                    clip: true
//                    anchors.fill: parent
//                    model: 100
//                    focus: true
//                    currentIndex: 4

//                    delegate: Component{
//                        Rectangle {
//                            width: 50
//                            radius: 10
//                            height: txt.implicitHeight
//                            anchors.horizontalCenter: parent.horizontalCenter
//        //                    color: ListView.view.currentIndex === index ? "yellow" : "transparent"
//                            color: ListView.isCurrentItem?"#157efb":"#53d769"

//        //                    color: ListView.iscurrentItem ? "blue" : "yellow"
//                            Text {
//                                anchors.horizontalCenter: parent.horizontalCenter
//                                id: txt
//                                text: index
//                                font.pixelSize: 18

//                            }

//                            MouseArea{
//                                anchors.fill: parent
//                                onClicked: {
//                                    parent.ListView.view.currentIndex = index
//                                }
//                            }
//                        }
//                    }
//                }

    }
}
