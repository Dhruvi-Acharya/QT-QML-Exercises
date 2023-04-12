import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQml 2.15
import QtQml.Models 2.15
import QtQuick.Controls.Universal 2.0
import Wisenet.Qmls 0.1

Rectangle {
    id: layoutNavigator
    color: "#303030"

    height:24
    width: parent.width - 55 - 100 - 100
    property int maxWidth: parent.width - 55 - 100 - 100

    // signal 정의
    signal addLayout(string msg);

    Connections{
        target: layoutNavigator
        function onAddLayout(){
            layoutTabBehaviors.addTab(tabListView.count + 1)
            tabListView.currentIndex = tabListView.count - 1
        }
    }

    Component.onCompleted: {
        addLayout("");
    }

    ListView {
        id: tabListView
        spacing: 1
        height: 24
        width: {
            if(tabListView.contentWidth + 100 >= layoutNavigator.maxWidth)
                return layoutNavigator.maxWidth - 100
            else
                return contentWidth
        }

        //interactive: false
        orientation: ListView.Horizontal

        focus: true

        property int positionViewIndex : 0

        onCurrentIndexChanged:{
            //console.log("onCurrentIndexChanged " + currentIndex)

            var item = tabListView.itemAtIndex(currentIndex)
            // signal 송신
            mainWindow.layoutChanged(item.tabText);
        }

        onWidthChanged: {
            //console.log("tabListView.onWidthChanged() width: " + width + ", contentWidth: " + contentWidth)
            for(var i = 0; i < tabListView.count; i++ )
            {
                var item = tabListView.itemAtIndex(i)
                if (item !== null)
                    item.resizeLayoutTab()
            }

            adjustButtonAreaLocation()
        }

        onCountChanged: {
            //console.log("tabListView.onCountChanged() contentWidth: " + contentWidth)
            adjustButtonAreaLocation()
        }

        // 버튼 위치, visibility
        function adjustButtonAreaLocation() {
            /*console.log("adjustButtonAreaLocation() buttonArea:" + buttonArea.width +
                        ", tabListView.width:" + tabListView.width +
                        ", tabListView.x:" + tabListView.x +
                        ", tabListView.contentWidth:" + tabListView.contentWidth +
                        ", layoutNavigator.width:"+ layoutNavigator.width +
                        ", buttonArea.x:"+ buttonArea.x )*/

            if(tabListView.width <= 0){
                buttonArea.visibleMovebuttons(false);
                return;
            }

            if(buttonArea.width + tabListView.contentWidth > layoutNavigator.width){
                buttonArea.visibleMovebuttons(true);
            }
            else{
                buttonArea.visibleMovebuttons(false);
            }
        }

        function selectLayout(layoutName){
            console.log("selectLayout() : " + layoutName)
            for(var i = 0; i < tabListView.count; i++ )
            {
                var item = tabListView.itemAtIndex(i)

                if(item === null)
                    console.log("item is null: " + layoutName)

                if(item.tabText === layoutName){
                    tabListView.currentIndex = i
                    break
                }
            }
        }


        displaced: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
        }

        // https://doc.qt.io/qt-5/qml-qtqml-models-delegatemodel.html
        model: DelegateModel {
            //! [0]
            id: visualModel
            model: layoutTabModel

            delegate: DropArea {
                id: delegateRoot
                //required property color color;
                required property string tabText;

                // Icon width랑 일치시킴
                width: layoutTab.width

                // List view height랑 같게
                height: 24

                onEntered: function(drag) {
                    //console.log("drag " + drag.source)
                    //console.log("tabListView.currentIndex " + tabListView.currentIndex)
                    //console.log("icon.visualIndex " + layoutTab.visualIndex)

                    visualModel.items.move((drag.source as Icon).visualIndex, layoutTab.visualIndex)
                }

                function resizeLayoutTab(){
                    layoutTab.resizeLayoutTab()
                }

                property int visualIndex: DelegateModel.itemsIndex
                //property int parentItemCount: DelegateModel.count

                LayoutTab {
                    id: layoutTab

                    width: {
                        resizeLayoutTab()
                    }

                    function resizeLayoutTab()
                    {
                        width = tabListView.width / tabListView.count

                        if(width <= 0 || width >= 100)
                            width = 100

                        if(width <= 50)
                            width = 50

                        //console.log("layoutTab.resizeLayoutTab() text: " + tabText + ", width: "+  width)
                    }

                    dragParent: tabListView
                    visualIndex: delegateRoot.visualIndex
                    isFocused: tabListView.currentItem == delegateRoot
                    tabText : delegateRoot.tabText
                    parentItemCount : visualModel.count
                    Layout.minimumWidth: 60
                }
            }
        }
    }

    // 버튼 부분
    Rectangle{
        id: buttonArea
        height: 24
        width: 50
        anchors.left: tabListView.right
        color: "#303030"

        // < > 버튼 출력 여부 결정
        function visibleMovebuttons(param){
            //console.log("visibleMovebuttons param:" + param)
            if(param === false)
            {
                moveLeftButton.visible = false
                moveRightButton.visible = false
                width = 50
            }
            else
            {
                width = 100
                moveLeftButton.visible = true
                moveRightButton.visible = true
            }
        }

        RowLayout{
            anchors.fill: parent
            spacing: 1

            Image{
                id: moveLeftButton
                source: moveLeftMouseArea.containsPress? "tab_icon/left_arrow_p.png":
                                                         moveLeftMouseArea.containsMouse? "tab_icon/left_arrow_o.png" : "tab_icon/left_arrow.png"
                width:24
                height:24

                Layout.preferredHeight: 24
                Layout.preferredWidth: 24

                MouseArea{
                    id: moveLeftMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        console.log("left_arrow: " + tabListView.positionViewIndex)

                        if(tabListView.positionViewIndex == 0)
                            return

                        tabListView.positionViewIndex -= 1
                        tabListView.positionViewAtIndex(tabListView.positionViewIndex, ListView.Beginning)
                    }
                }
            }

            Image{
                id: moveRightButton
                source: moveRightMouseArea.containsPress? "tab_icon/right_arrow_p.png":
                                                          moveRightMouseArea.containsMouse? "tab_icon/right_arrow_o.png" : "tab_icon/right_arrow.png"
                width:24
                height:24

                Layout.preferredHeight: 24
                Layout.preferredWidth: 24

                MouseArea{
                    id: moveRightMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        console.log("right_arrow: " + tabListView.positionViewIndex)

                        if(tabListView.positionViewIndex >= tabListView.count)
                            return

                        tabListView.positionViewIndex += 1
                        tabListView.positionViewAtIndex(tabListView.positionViewIndex, ListView.End)
                    }
                }
            }

            Image{
                id: addTabButton
                source: addTabMouseArea.containsPress? "tab_icon/add_layout_p.png" :
                                                       addTabMouseArea.containsMouse? "tab_icon/add_layout_o.png" : "tab_icon/add_layout.png"

                width:24
                height:24

                Layout.preferredHeight: 24
                Layout.preferredWidth: 24

                MouseArea{
                    id: addTabMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        addLayout("")
                    }
                }
            }

            Image{
                id: popTabComboButton
                source: {
                    if(contextMenu.opened === true)
                        return "tab_icon/pop_layoutcombo_p.png"

                    if(popTabComboMouseArea.containsPress)
                        return "tab_icon/pop_layoutcombo_p.png"
                    else if(popTabComboMouseArea.containsMouse)
                        return "tab_icon/pop_layoutcombo_o.png"
                    else
                        return "tab_icon/pop_layoutcombo.png"
                }
                width:24
                height:24

                Layout.preferredHeight: 24
                Layout.preferredWidth: 24

                MouseArea{
                    id: popTabComboMouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    hoverEnabled: true

                    onClicked: {
                        console.log("contextMenu.visible " + contextMenu.visible)

                        contextMenu.popup(popTabComboMouseArea, 0, 24)
                    }

                    onPressAndHold: {
                        if (mouse.source === Qt.MouseEventNotSynthesized)
                            contextMenu.popup(popTabComboMouseArea, 0, 24)
                    }

                    // 색깔 customizing
                    // https://doc.qt.io/archives/qt-5.7/qtquickcontrols2-customize.html#customizing-button
                    Menu {
                        id: contextMenu

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
                            model: layoutTabModel
                            MenuItem {
                                id: menuItem
                                height: 24

                                contentItem: Text{
                                    id: contentItem
                                    text: tabText
                                    color: "white"
                                }

                                background: Rectangle{
                                    anchors.fill: parent
                                    color: menuItem.hovered ? "#F06200" : "#606060"
                                }

                                onClicked: {
                                    console.log("menu : " + tabText)
                                    tabListView.selectLayout(tabText)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
