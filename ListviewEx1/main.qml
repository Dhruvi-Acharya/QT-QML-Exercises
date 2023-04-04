import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 150
    height: 150
    visible: true
    title: qsTr("Hello World")

    ListModel{
        id: lmodel
        ListElement{
            name: "Dhruvi"
            team: "A"
        }
        ListElement{
            name: "Nidhi"
            team: "A"
        }
        ListElement{
            name: "Nimi"
            team: "A"
        }
        ListElement{
            name: "Margi"
            team: "A"
        }
        ListElement {
            name: "Khushboo"
            team: "A"
        }
        ListElement{
            name: "Dhruvi"
            team: "B"
        }
        ListElement{
            name: "Nidhi"
            team: "B"
        }
        ListElement{
            name: "Nimi"
            team: "B"
        }
        ListElement{
            name: "Margi"
            team: "B"
        }
        ListElement {
            name: "Khushboo"
            team: "B"
        }
        ListElement{
            name: "Dhruvi"
            team: "C"
        }
        ListElement{
            name: "Nidhi"
            team: "C"
        }
        ListElement{
            name: "Nimi"
            team: "C"
        }
        ListElement{
            name: "Margi"
            team: "C"
        }
        ListElement {
            name: "Khushboo"
            team: "C"
        }

    }

    Component{
        id: ldelegate

        Rectangle {
            color: ListView.view.currentIndex === index ? "grey" : "transparent"
            anchors {left: parent.left; right: parent.right}
            implicitHeight: txt.implicitHeight


            Text{
                id: txt
                text: model.name
                font.pixelSize: 22

                property bool expanded: false
                height: expanded? implicitHeight : 0

                Behavior on height {
                    NumberAnimation{
                        duration: 2000
                    }
                }

                Component.onCompleted: console.log("Welcome " + model.index + " " + model.name) // when component created
                Component.onDestruction: console.log("By " + model.index + " " + model.name) // when component destroyed
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    lmodel.insert(index,{name: "Item" + lmodel.count, team: "A"})
                    parent.ListView.view.currentIndex = index
                }
            }
        }
    }

    ListView{
        id: lv
        model: lmodel
        delegate: ldelegate
        anchors.fill: parent
        clip: true
        focus: true
        cacheBuffer: 20 // cache to save memory only load

        onCurrentIndexChanged: console.log(currentIndex)

        header: Rectangle{
            anchors {left: parent.left; right: parent.right}
            color: "pink"
            height: 10
        }

        footer: Rectangle{
            anchors {left: parent.left; right: parent.right}
            color: "pink"
            height: 10
        }

//        highlight: Rectangle{
//            anchors {left: parent.left; right: parent.right}
//            color: "lightgrey"

//        }

        highlightRangeMode: ItemView.ApplyRange

        currentIndex: 5

        section.property: "team"
        section.criteria: ViewSection.FirstCharacter
        section.delegate: Rectangle{
            implicitHeight: txt_sec.implicitHeight
            width: parent.width
            color: "green"
            Text {
                id: txt_sec
                text: section
                anchors.centerIn: parent
                font.pixelSize: 20
            }
        }

        populate: Transition {
            NumberAnimation {properties: "x,y"; duration: 300}
        }

        add: Transition {
            PropertyAction{
                property: "tarnsformOrigin"
                value: Item.TopLeft
            }
            NumberAnimation{
                property: "opacity"
                from: 0
                to: 1.0
                duration: 200
            }
            NumberAnimation{
                property: "scale"
                from: 0
                to: 1.0
                duration: 200
            }
        }

        displaced: Transition {
            PropertyAnimation{properties: "opacity,scale"; duration: 200}
            NumberAnimation{properties: "x,y"; duration: 200}
        }


    }

    Text{
        id: label
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: "<b>" + lv.currentItem + "</b>"
        font.pixelSize: 16
    }
}
