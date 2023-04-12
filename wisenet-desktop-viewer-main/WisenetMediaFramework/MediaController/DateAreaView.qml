import QtQuick 2.15
import QtQml 2.15
import Wisenet.MediaController 1.0
import "qrc:/WisenetStyle/"

// 날짜 표시 영역
Item {
    Component.onCompleted: {
        dateAreaModel.refreshDateDataList(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime, graphWidth)
    }

    Row {
        id: itemRow
        width: parent.width
        height: parent.height
        spacing: 0
        clip: true

        // dateAreaModel의 데이터를 차례대로 표시
        Repeater {
            model: dateAreaModel
            delegate: rectDelegate
        }
    }

    Component {
        id: rectDelegate
        Rectangle {
            id: rect
            width: model.width
            height: itemRow.height - 1
            y: 1
            color: WisenetGui.transparent

            Rectangle {
                //visible: rect.x != 0
                width: 1
                height: 9
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                color: WisenetGui.contrast_04_light_grey
            }

            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                text: supportDuration ? model.timeText : model.dateText + " " + model.timeText
                font.pixelSize: 11
                leftPadding: 8
                rightPadding: 8
                color: WisenetGui.contrast_04_light_grey
                clip: true
                elide: Text.ElideRight
                //visible: model.width > contentWidth + 10
            }

            // 날짜 더블클릭 시 Zoom In
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                onDoubleClicked: {
                    var rangeDelta = model.endTime - model.startTime - mediaControlModel.visibleTimeRange  // 표시 범위 변화량
                    var startTimeDelta = model.startTime - mediaControlModel.visibleStartTime    // 표시 시작 시간 변화량

                    timelineControlView.animationDuration = 500;
                    mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)
                }
            }
        }
    }
}
