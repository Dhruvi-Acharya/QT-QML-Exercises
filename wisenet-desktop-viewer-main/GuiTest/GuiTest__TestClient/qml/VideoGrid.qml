import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Wisenet.Qmls 0.1

Item {
    id: videoGrid

//    Rectangle {
//        id: bg
//        anchors.fill: parent
//        border.width: 1
//        border.color: "green"
//        color: "transparent"

//        QVideoQuickItem{
//            id: videoItem
//            anchors.centerIn: parent
//            width: parent.width -5
//            height: parent.height -5
//        }
//        DropArea {
//            id: dropArea;
//            anchors.fill: bg
//            onDropped: {
//                console.log("ITEM Drop:" + drop.urls);
//                videoItem.openMedia(drop.urls);
//            }
//        }
//    }

    Grid {
        id: grid
        anchors.fill: parent
        columns: 2
        rows: 2
        spacing: 0
        Repeater {
            model: 4
            Rectangle {
                id: bg
                width: grid.width / grid.columns
                height: grid.height / grid.rows
                border.width: 1
                border.color: "green"
                color: "transparent"

                Rectangle {
                    id: videoItem
                    anchors.centerIn: parent
                    width: parent.width -5
                    height: parent.height -5
                }
/*
                QVideoQuickItem{
                    id: videoItem
                    anchors.centerIn: parent
                    width: parent.width -5
                    height: parent.height -5
                    Keys.onPressed: {
                        switch(event.key){
                        case Qt.Key_Delete:
                            console.log("delete key pressed");
                            videoItem.closeMedia();
                            break;
                        default:break;
                        }
                    }
                }
*/
                MouseArea {
                    id: mouseArea
                    anchors.fill: bg
                    onClicked: {
                        videoItem.focus = true;
                    }
                }

                DropArea {
                    id: dropArea;
                    anchors.fill: bg
                    onDropped: {
                        console.log("ITEM Drop:" + drop.urls);
                        //videoItem.openMedia(drop.urls, false);
                    }
                }
            }
        }

    }

}
