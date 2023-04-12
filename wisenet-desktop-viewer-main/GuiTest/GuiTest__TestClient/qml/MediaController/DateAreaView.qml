import QtQuick 2.15
import QtQml 2.15
import Wisenet.Qmls 0.1

// 날짜 표시 영역
Item {
    Component.onCompleted: {
        dateAreaModel.refreshDateDataList(mediaControlModel.visibleStartTime, mediaControlModel.visibleEndTime, timelineControlView.width)
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
            height: itemRow.height
            color: {
                if(model.evenNumberStyle)
                    return "Gray"
                else
                    return "DarkGray"
            }

            Text {
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
                text: model.text
                color: "Black"
                clip: true
                visible: model.width > contentWidth + 10
            }
        }
    }
}
