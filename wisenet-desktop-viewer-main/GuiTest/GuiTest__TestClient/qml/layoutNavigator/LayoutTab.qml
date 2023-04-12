import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

Rectangle {
    id: layoutTab
    required property Item dragParent
    property ListView parentListView : dragParent as ListView

    property int visualIndex: 0
    property int parentItemCount: 1
    property string tabText
    property bool isFocused

    height: 24
    anchors {
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }

    // 선택 됐을 때 색상
    color: isFocused ? "#606060" : "#383838"

    // 모퉁이 둥글게
    radius: 1

    function closeTab(name){
        console.log("closeTab :" + name)

        layoutTabBehaviors.closeTab(name)

    }

    function closeAllButThis(name){
        console.log("closeAllButThis :" + name)

        layoutTabBehaviors.closeAllButThis(name)

        parentListView.currentIndex = 0
    }

    RowLayout{
        width: parent.width
        anchors.centerIn: parent

        Text {
            id: layoutText
            // 남은 공간 꽉채움 -- 이걸 안하면 빈공간에서 Flickable 동작
            // Mouse Area를 Fill로 잡기 때문
            Layout.fillHeight: true
            // 남은 공간 꽉채움
            Layout.fillWidth: true

            color: "white"

            text: layoutTab.tabText

            // 길어질 경우 오른쪽 자르기 ...표시
            elide: Text.ElideRight

            // Text 정렬
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment :Text.AlignVCenter

            // https://doc.qt.io/qt-5/qml-qtquick-drag.html
            MouseArea{
                id: dragAndClickArea

                acceptedButtons: Qt.LeftButton | Qt.RightButton
                property bool held: false

                anchors.fill: parent
                hoverEnabled: true

                // 클릭 했을 때 index 선택
                onClicked: {
                    //console.log("text " + layoutTab.visualIndex)
                    //console.log("isFocused " + layoutTab.isFocused)
                    //console.log("parentListView.currentIndex " + parentListView.currentIndex)

                    parentListView.currentIndex = visualIndex
                }

                drag.target: layoutTab
                drag.axis: Drag.XAxis

                pressAndHoldInterval: 50

                onPressed: {
                    //console.log("onPressed")
                    if(mouse.button & Qt.RightButton) {
                        //console.log("onPressed-right")

                        layoutContextMenu.layoutName = layoutTab.tabText
                        layoutContextMenu.popup(dragAndClickArea, 10, 24)
                    }
                }

                // 마우스를 누르고 유지
                onPressAndHold: {
                    //console.log("onPressAndHold")
                    held = true
                }

                // 마우스를 뗌
                onReleased:{
                    //console.log("onReleased")
                    held = false
                }
            }

            ToolTip {
                id: layoutTooltip
                parent: dragAndClickArea
                visible: dragAndClickArea.containsMouse && layoutText.truncated
                delay: 200
                text: layoutText.text
            }
        }

        Image {
            // 탭 x 버튼 image
            source: {
                if(layoutTab.isFocused)
                {
                    if(closeTabMouseArea.containsPress)
                        return "tab_icon/close_tab_sp.png"
                    else if(closeTabMouseArea.containsMouse)
                        return "tab_icon/close_tab_so.png"
                    else
                        return "tab_icon/close_tab_s.png"
                }
                else{
                    if(closeTabMouseArea.containsPress)
                        return "tab_icon/close_tab_p.png"
                    else if(closeTabMouseArea.containsMouse)
                        return "tab_icon/close_tab_o.png"
                    else
                        return "tab_icon/close_tab.png"
                }
            }

            width:24
            height:24
            Layout.preferredHeight: 24
            Layout.preferredWidth: 24

            MouseArea{
                id: closeTabMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    //console.log("closeTab view")
                    layoutTabBehaviors.closeTab(layoutTab.tabText)
                }
            }
        }

        ListModel{
            id: menuModel
            ListElement{
                subTitle: "Close"
                deleteAction: "layout"
            }

            ListElement{
                subTitle: "Close all but this"
                deleteAction: "all"
            }
        }

        Menu {
            id: layoutContextMenu
            property string layoutName

            // maximum size 정하기
            height: {
                if ( contentHeight > 500)
                    return 500
                else
                    return contentHeight + 20
            }

            background: Rectangle {
                implicitWidth: 200
                color: "#606060"
                border.color: "#606060"
                radius: 2
            }

            Repeater {
                model: menuModel
                MenuItem {
                    id: menuItem
                    required property string subTitle
                    required property string deleteAction
                    height: 24

                    contentItem: Text{
                        id: contentItem
                        text: subTitle
                        color: "white"
                    }

                    background: Rectangle{
                        anchors.fill: parent
                        color: menuItem.hovered ? "#F06200" : "#606060"
                    }

                    onClicked: {
                        if(menuItem.deleteAction == "all"){
                            console.log("menu clicked :" + menuItem.deleteAction)
                            layoutTab.closeAllButThis(layoutContextMenu.layoutName)
                        }
                        else{
                            console.log("menu clicked :" + menuItem.deleteAction)
                            layoutTab.closeTab(layoutContextMenu.layoutName)
                        }

                    }
                }
            }
        }

    }

    // 드래그 동작
    states: [
        State {
            when: dragAndClickArea.held
            ParentChange {
                target: layoutTab
                parent: layoutTab.dragParent
            }

            AnchorChanges {
                target: layoutTab
                anchors.horizontalCenter: undefined
            }

            StateChangeScript{
                script:  parentListView.currentIndex = visualIndex
            }
        }
    ]

    Drag.active: dragAndClickArea.drag.active
    Drag.hotSpot.x: layoutTab.width / 2
}
