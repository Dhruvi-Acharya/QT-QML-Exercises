import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3

import "tree"
import "setup"
import "MediaController"
import "alarm"

Item {
    anchors.fill: parent
    // 크기 변경 관련 properties
    property int leftSplitterPreviousX
    property int rightSplitterPreviousX
    property int leftMinimumWidth: 220
    property int leftMaximumWidth: 450
    property int rightMinimumWidth: 150
    property int rightMaximumWidth: 400

    // signal 수신
    Connections{
        target: mainWindow
        function onLayoutChanged(msg){
            console.log("MonitoringView onLayoutChanged : " + msg)

            stackLayout.setLayout(msg)
        }
    }

    // Left splitter 동작
    MouseArea
    {
        id: leftSplitterMouseArea
        x: leftSplitter.x
        width: 3
        height: rowLayout.height
        cursorShape: leftArea.state == "HIDE" ? Qt.ArrowCursor : Qt.SplitHCursor

        onPressed: {
            leftSplitterPreviousX = mouseX
            console.log("onPressed")
        }

        onMouseXChanged: {

            var dx = mouseX - leftSplitterPreviousX

            if(leftArea.state == "HIDE")
            {
                console.log("hide return")
                return
            }

            if(leftMaximumWidth <= leftArea.Layout.preferredWidth + dx)
                return

            if(leftMinimumWidth >= leftArea.Layout.preferredWidth + dx)
                return

            leftAreaAnimationBehavior.enabled = false
            leftArea.Layout.preferredWidth = leftArea.width + dx
            leftAreaAnimationBehavior.enabled = true
        }
    }

    // Right splitter 동작
    MouseArea
    {
        id: rightSplitterMouseArea
        x: rightArea.x
        width: 3
        height: rowLayout.height
        cursorShape: rightArea.state == "HIDE" ? Qt.ArrowCursor : Qt.SplitHCursor

        onPressed: {
            rightSplitterPreviousX = mouseX
            console.log("onPressed")
        }

        onMouseXChanged: {
            var dx = rightSplitterPreviousX - mouseX

            if(rightArea.state == "HIDE")
            {
                console.log("hide return")
                return
            }

            if(rightMaximumWidth <= rightArea.Layout.preferredWidth + dx)
                return

            if(rightMinimumWidth >= rightArea.Layout.preferredWidth + dx)
                return

            rightAreaAnimationBehavior.enabled = false
            rightArea.Layout.preferredWidth = rightArea.width + dx
            rightAreaAnimationBehavior.enabled = true
        }
    }

    // 열기 닫기 버튼
    property string arrowLeftOver : "icon/arrow_left_o.png"
    property string arrowLeftNormal : "icon/arrow_left_n.png"
    property string arrowRightOver : "icon/arrow_right_o.png"
    property string arrowRightNormal : "icon/arrow_right_n.png"


    // Center Area
    Rectangle {
        color: "transparent"
        anchors.fill: parent

        border.color: "#444444"
        border.width: 1

        RowLayout {
            id: rowLayout
            spacing: 0
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: mediaController.top
                rightMargin: 3
            }

            Rectangle {
                id: leftArea
                state : "OPEN"
                color: "transparent"

                // 잘 안먹음..
                //Layout.maximumWidth: 400
                //Layout.minimumWidth: 100
                Layout.fillHeight: true

                // 초기 사이즈
                Layout.preferredWidth: 300

                // width를 줄일 때 애니메이션
                Behavior on Layout.preferredWidth  {
                    id: leftAreaAnimationBehavior
                    NumberAnimation {
                        // Easing - https://doc.qt.io/qt-5/qml-qtquick-propertyanimation.html
                        duration: 300; easing.type: Easing.InOutQuad
                    }
                }

                Behavior on visible  {
                    NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
                }

                ExplorerView {
                    id: treeView
                    anchors.fill: parent

                    anchors{
                        rightMargin: 5
                    }
                }

                Rectangle {
                    id: leftSplitter

                    anchors {
                        right: parent.right
                        rightMargin: 1
                        top: parent.top
                        bottom: parent.bottom
                    }

                    width: 1
                    color: "#444444"
                    Layout.fillHeight: true
                }
            }

            Rectangle {
                id: centerArea
                height: 200
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true

                // 영상 모니터링 screen 영영
                StackLayout{
                    id: stackLayout
                    anchors.fill: parent

                    function setLayout(layoutName) {

                        for(var i = 0; i < stackLayout.count; i++ )
                        {
                            var item = stackLayout.itemAt(i)

                            if(item.layoutText === layoutName){
                                console.log("setLayout " + i)
                                currentIndex = i
                                break
                            }
                        }
                    }

                    Repeater {
                        id: stackRepeater
                        model: layoutTabModel
//                        Rectangle {
//                            id: layoutRectangle
//                            color: "#101010"
//                            property string layoutText: tabText

//                            Button {
//                                anchors.centerIn: parent
//                                text: tabText

//                                onClicked: {
//                                    console.log("mainWindow uuid : " + windowGuid)
//                                }
//                            }
//                        }
                        VideoGrid {
                            id: videoGrid
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                }

                Image{
                    id: foldLeftButtonImage
                    x: centerArea.anchors.leftMargin + 1
                    y: centerArea.anchors.verticalCenterOffset
                    width: 10
                    height: 32
                    anchors.verticalCenter: centerArea.verticalCenter

                    source: {
                        if(foldLeftButtonMouseArea.containsMouse)
                        {
                            if(leftArea.state == "OPEN")
                                return arrowLeftOver
                            else
                                return arrowRightOver
                        }
                        else
                        {
                            if(leftArea.state == "OPEN")
                                return arrowLeftNormal
                            else
                                return arrowRightNormal
                        }
                    }

                    MouseArea
                    {
                        hoverEnabled: true
                        anchors.fill: parent
                        id: foldLeftButtonMouseArea
                        onClicked:
                        {
                            console.log("foldLeftButton")

                            if (leftArea.state == "OPEN")
                            {
                                leftArea.state = "HIDE"
                                leftSplitterPreviousX = leftArea.Layout.preferredWidth
                                leftArea.Layout.preferredWidth  = 0
                                leftArea.Layout.visible = false;
                                treeView.visible = false;
                            }
                            else
                            {
                                leftArea.state = "OPEN"
                                leftArea.Layout.preferredWidth  = leftSplitterPreviousX
                                leftArea.Layout.visible = true;
                                treeView.visible = true;
                            }
                        }
                    }
                }

                Image
                {
                    id: foldRightButton
                    width: 10
                    height: 32
                    anchors.verticalCenter: centerArea.verticalCenter
                    anchors.right: centerArea.right
                    anchors.verticalCenterOffset: 0
                    anchors.rightMargin: 2

                    source: {
                        if(foldRightButtonMouseArea.containsMouse)
                        {
                            if(rightArea.state == "OPEN")
                                return arrowRightOver
                            else
                                return arrowLeftOver
                        }
                        else
                        {
                            if(rightArea.state == "OPEN")
                                return arrowRightNormal
                            else
                                return arrowLeftNormal
                        }
                    }

                    MouseArea
                    {
                        hoverEnabled: true
                        anchors.fill: parent
                        id: foldRightButtonMouseArea
                        onClicked:
                        {
                            console.log("foldRightButton")

                            if (rightArea.state == "OPEN")
                            {
                                rightArea.state = "HIDE"
                                rightSplitterPreviousX = rightArea.Layout.preferredWidth
                                rightArea.Layout.preferredWidth  = 0
                                rightArea.Layout.visible = false;
                                alarmView.visible = false;
                            }
                            else
                            {
                                rightArea.state = "OPEN"
                                rightArea.Layout.preferredWidth = rightSplitterPreviousX
                                rightArea.Layout.visible = true;
                                alarmView.visible = true;
                            }
                        }
                    }
                }

            }

            Rectangle {
                id: rightArea
                state : "OPEN"

                color: "transparent"
                Layout.fillHeight: true

                // 초기 사이즈
                Layout.preferredWidth: 200

                // width를 줄일 때 애니메이션
                Behavior on Layout.preferredWidth  {
                    id: rightAreaAnimationBehavior
                    NumberAnimation {
                        // Easing - https://doc.qt.io/qt-5/qml-qtquick-propertyanimation.html
                        duration: 300; easing.type: Easing.InOutQuad
                    }
                }

                Behavior on visible  {
                    NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
                }

                Rectangle {
                    id: rightSplitter

                    anchors {
                        left: rightArea.left
                        leftMargin: 1
                        top: rightArea.top
                        bottom: rightArea.bottom
                    }

                    width: 1
                    color: "#444444"
                    Layout.fillHeight: true
                }

                AlarmPanel {
                    id: alarmView
                    anchors.fill: parent
                    anchors.leftMargin: 5

                    Behavior on visible  {
                        NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
                    }
                }
            }
        }

        MediaControllerView {
            id: mediaController
        }
    }
}
