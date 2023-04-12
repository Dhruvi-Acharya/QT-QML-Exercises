import QtQuick 2.15
import Wisenet.Qmls 0.1

// 시간 및 눈금 표시 영역
Item {
    Component.onCompleted: {
        timeModel.refreshTimeDataList(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime, timelineControlView.width)
    }

    Row {
        id: itemRow
        width: parent.width
        height: parent.height
        spacing: 0
        clip: true

        // timeModel의 데이터를 차례로 그림
        Repeater {
            model: timeModel
            delegate: rectDelegate
        }
    }

    Component {
        id: rectDelegate

        // 눈금 1칸 범위의 Rectangle
        Rectangle {
            id: rect
            width: model.rectWidth
            height: itemRow.height
            color: "Transparent"
            opacity: {
                if(model.rectWidth !== 0)
                    return 1
                else
                    return 0
            }

            // 눈금 선
            Rectangle {
                width: 1
                height: model.lineHeight
                anchors.right: rect.right
                color: "White"
                opacity: {
                    if(model.lineHeight !== 0)
                        return 1
                    else
                        return 0
                }

                Behavior on height {
                    NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo }
                }

                Behavior on opacity {
                    NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo }
                }
            }

            // 시간 Text
            Text {
                anchors.horizontalCenter: rect.right
                horizontalAlignment: Text.AlignHCenter
                y: model.lineHeight + 1
                text: model.timeText
                color: "White"
                clip: true
                opacity: {
                    if(model.timeText !== "")
                        return 1
                    else
                        return 0
                }

                Behavior on y {
                    NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo }
                }

                Behavior on opacity {
                    NumberAnimation { duration: timelineControlView.animationDuration; easing.type: Easing.OutExpo }
                }
            }
        }
    }
}
