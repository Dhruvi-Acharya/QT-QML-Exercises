import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetLanguage 1.0

Rectangle {
    id: textSearchResultView

    property int scrollRightMargin : 15
    property int zoomStep : 10
    property int textItemViewTopMargin : 18
    property alias currentIndex : stackView.currentIndex
    property var textSearchGridView: gridListView
    property int textWidth : 260
    property int textHeight : 340

    property int prevStepValue: 0
    property bool increaseDecision: false

    signal zoomInOutStepValue(var stepValue)
    function setZoomInOutStepValue(stepValue){
        if (stepValue > prevStepValue)
            textSearchResultView.increaseDecision = true
        else
            textSearchResultView.increaseDecision = false

        textSearchResultView.prevStepValue = stepValue

//        console.info("=====> stepValue: ", stepValue)
//        console.info("=====> increaseDecision: ", textSearchResultView.increaseDecision)
    }

    function search(allDevice, devices, keyword, from, to, isWholeWord, isCaseSensitive) {
        gridListView.search(allDevice, devices, keyword, from, to, isWholeWord, isCaseSensitive)
    }

    StackLayout {
        id: stackView
        anchors.fill: parent

        Rectangle{
            id: textItemView
            color: WisenetGui.contrast_09_dark

            Rectangle{
                id: upperBorder

                height:1
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                color: WisenetGui.contrast_08_dark_grey
            }

            Flickable{
                id: scrollArea

                anchors.fill: parent
                anchors.topMargin: textItemViewTopMargin

                contentHeight: gridListView.tableModel.totalCount / (textSearchResultView.width / textSearchResultView.textWidth) * (textSearchResultView.textHeight + 62)
                interactive: false

                ScrollBar.vertical: WisenetScrollBar {
                    id: verticalScrollBar

                    parent: scrollArea

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    stepSize: verticalScrollBar.visualSize / 4
                }

                clip: true

                // for Keyboard event
                focus: true
                Keys.onUpPressed: verticalScrollBar.decrease()
                Keys.onDownPressed: verticalScrollBar.increase()

                Flow {
                    id: textFlow

                    anchors.fill: parent
                    anchors.leftMargin: 1
                    spacing: 0

                    Repeater{
                        id: textRepeater
                        model: gridListView.tableModel

                        TextItem{
                            id: textItem                           
                            viewModel: gridListView.listModel
                            // 슬라이더 움직임에 대한 줌인, 줌아웃 결정
                            increaseDecision: textSearchResultView.increaseDecision
                            // 슬라이더 움직임에 대한 줌인, 줌아웃 설정값
                            zoomInOutStepValue: textSearchResultView.prevStepValue
                        }
                    }
                }

                MouseArea{
                    anchors.fill: parent

                    onPressed: {
                        mouse.accepted = false
                    }

                    onWheel: {
                        if (wheel.modifiers & Qt.ControlModifier){
                            console.log("onWheel", wheel.pixelDelta.y, wheel.angleDelta.y)

                            if (wheel.angleDelta.y > 0)
                            {
                                if (gridListView.listModel.textDataHeight + zoomStep <= 340) {
                                    textSearchResultView.zoomInOutStepValue(gridListView.listModel.textDataHeight + zoomStep)
                                }
                            }
                            else
                            {
                                if (gridListView.listModel.textDataHeight - zoomStep >= 260) {
                                    textSearchResultView.zoomInOutStepValue(gridListView.listModel.textDataHeight - zoomStep)
                                }
                            }

                            wheel.accepted = true
                        }
                        else
                        {
                            if (wheel.angleDelta.y > 0)
                                verticalScrollBar.decrease()
                            else
                                verticalScrollBar.increase()

                            wheel.accepted = false
                        }
                    }
                }
            }
        }

        TextSearchGridView {
            id: gridListView
            supportPlayback: true
            supportBookmark: true
        }

        onCurrentIndexChanged: {
            if (currentIndex === 0) {
                gridListView.visible = false
            }
            else {
                gridListView.visible = true
            }
        }
    }
}
