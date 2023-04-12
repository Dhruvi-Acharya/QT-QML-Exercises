import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0
import "qrc:/"

Rectangle {
    id: layoutTab
    required property Item dragParent
    property ListView parentListView : dragParent as ListView

    property int visualIndex: 0
    property int parentItemCount: 1
    property string tabText
    property string uuid
    property bool isFocused
    property bool saveStatus
    property int itemType
    property bool isSequencePlaying

    // 공유레이아웃 여부
    readonly property bool isShared: layoutModel && layoutModel.ownerId && (layoutModel.loginId !== layoutModel.ownerId)

    property var layoutModel

    height: 40
    anchors {
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }

    signal makeNewLayout();
    signal tabClosedWithoutSave(var closedLayouts)

    // 선택 됐을 때 색상
    color: WisenetGui.layoutTabColor

    // 모퉁이 둥글게
    radius: 1

    function closeTab(layoutId){
        console.log("closeTab: " + layoutId)

        layoutNavigationViewModel.closeTab(layoutId)
        var closedLayouts = [];
        closedLayouts.push(layoutId)

        // 저장없이 닫았을 때 트리 원복
        layoutTab.tabClosedWithoutSave(closedLayouts)
    }

    function closeAllButThis(layoutId){
        console.log("closeAllButThis: " + layoutId)

        var closedLayouts = layoutNavigationViewModel.closeAllButThis(layoutId)
        console.log("closeAllButThis closed: " + closedLayouts)
        parentListView.currentIndex = 0

        // 저장없이 닫았을 때 트리 원복
        layoutTab.tabClosedWithoutSave(closedLayouts)
    }

    Rectangle{
        anchors.fill: parent
        color: layoutTab.color

        Rectangle {
            id: layoutText
            // 남은 공간 꽉채움 -- 이걸 안하면 빈공간에서 Flickable 동작
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: xButton.left
            anchors.bottom: borderLine.top
            anchors.rightMargin: 8
            color: layoutTab.color

            Image{
                id: layoutImage

                height: 18
                width: 18
                sourceSize: "18x18"

                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter

                source: {
                    if(itemType === ItemType.Sequence) {
                        return WisenetImage.sequence_main_icon_default
                    }
                    else if(itemType === ItemType.Layout) {
                        return WisenetImage.layout
                    }
                }
            }

            Text{
                id: innerText
//                anchors.fill: parent
                anchors.top: parent.top
                anchors.left: layoutImage.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                //anchors.rightMargin: star.visible ? 5 : 0
                font.pixelSize: 12
                color: isFocused ? WisenetGui.selectedLayoutBorderColor : WisenetGui.layoutTextColor

                text: (!saveStatus && layoutTab.itemType !== ItemType.Sequence) ? "* "+layoutTab.tabText : layoutTab.tabText

                // 길어질 경우 오른쪽 자르기 ...표시
                elide: Text.ElideRight

                // Text 정렬
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            /*
            Text{
                id: star

                visible: !saveStatus && layoutTab.itemType !== ItemType.Sequence

                width: 5
                anchors.top : parent.top
                anchors.bottom : parent.bottom
                anchors.left: innerText.right
                font.pixelSize: 18
                color: isFocused ? WisenetGui.selectedLayoutBorderColor : WisenetGui.layoutTextColor
                text: "*"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }*/

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
                        layoutContextMenu.uuid = layoutTab.uuid
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
        }

        WisenetImageButton{
            id: xButton

            visible: tabArea.containsMouse

            height: 12
            width: 12
            sourceHeight: 12
            sourceWidth: 12

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            imageHover: "qrc:/Assets/Icon/Header/layout_x_button_hover.svg"
            imageNormal: "qrc:/Assets/Icon/Header/layout_x_button.svg"

            onClicked: layoutTab.closeTab(layoutTab.uuid)
        }

        ToolTip {
            id: layoutTooltip
            visible: tabArea.containsMouse && innerText.truncated
            delay: 200
            text: innerText.text

            leftPadding: 5
            rightPadding: 5
            topPadding: 3 + 5
            bottomPadding: 3
            font.pixelSize: 11

            contentItem: Text {
                text: layoutTooltip.text
                font: layoutTooltip.font
                color: WisenetGui.contrast_08_dark_grey
                padding: 2
            }

            background: Rectangle {
                color: WisenetGui.transparent
                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: 5
                    color: WisenetGui.contrast_02_light_grey
                    radius: 2
                }
                Canvas {
                    width: 8
                    height: 8
                    x: -layoutTooltip.x + layoutTooltip.parent.width/2 - width/2
                    anchors.top: parent.top
                    contextType: "2d"
                    onPaint: {
                        var context = getContext("2d")
                        context.reset();
                        context.moveTo(0, height);
                        context.lineTo(width, height);
                        context.lineTo(width / 2, 0);
                        context.closePath();
                        context.fillStyle = WisenetGui.contrast_02_light_grey
                        context.fill();
                    }
                }
            }
        }

        MouseArea{
            id: tabArea
            anchors.fill: parent
            hoverEnabled: true

            onPressed: {
                mouse.accepted = false
            }
        }

        Rectangle{
            id: borderLine

            color: isFocused ? WisenetGui.selectedLayoutBorderColor : WisenetGui.layoutTabColor
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 1
        }

        WisenetMediaMenu {
            id: layoutContextMenu
            property string layoutName
            property string uuid
            width: 220

            WisenetMediaMenuItem{
                text : WisenetLinguist.newTab
                displayShotcutOnly: StandardKey.New
                onTriggered: layoutTab.makeNewLayout()
            }

            Action{
                text : WisenetLinguist.newWindow
                onTriggered: windowHandler.initializeNewWindow(false)
            }


            WisenetMediaMenuItem {
                text : WisenetLinguist.saveLayoutAs
                enabled: itemType !== ItemType.Sequence
                displayShotcutOnly : Qt.platform.os === "windows" ? "F12" : "Ctrl+Shift+S"
                onTriggered: {
                    layoutSaveAsDialog.text = layoutModel.name + "#";
                    layoutSaveAsDialog.saveAs = true
                    layoutSaveAsDialog.layout = layoutModel
                    layoutSaveAsDialog.showDialog();
                }
            }

            WisenetMediaMenuItem {
                text : WisenetLinguist.saveLayout
                enabled: !saveStatus && itemType !== ItemType.Sequence && !layoutModel.locked && !isShared    // 잠김, 공유 레이아웃 저장 비활성화
                displayShotcutOnly : StandardKey.Save
                onTriggered: {
                    if(layoutModel.isNewLayout())
                    {
                        layoutSaveAsDialog.text = "Layout"
                        layoutSaveAsDialog.saveAs = false
                        layoutSaveAsDialog.layout = layoutModel
                        layoutSaveAsDialog.showDialog();
                    }
                    else {
                        layoutModel.saveToCoreService()
                    }
                }
            }

            WisenetMediaMenuItem{
                text : WisenetLinguist.close
                displayShotcutOnly: "Ctrl+W"
                onTriggered: layoutTab.closeTab(layoutContextMenu.uuid)
            }

            Action{
                text : WisenetLinguist.closeAllButThis
                onTriggered: layoutTab.closeAllButThis(layoutContextMenu.uuid)
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
