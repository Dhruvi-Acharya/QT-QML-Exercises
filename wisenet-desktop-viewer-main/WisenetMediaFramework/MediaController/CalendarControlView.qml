import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Wisenet.MediaController 1.0
import "qrc:/WisenetStyle/"

Item {
    id: calendarControl
    width: 329
    height: 322
    /*
    width: calendar.width
    height: calendar.height
    */

    property Calendar calendar: timelineCalendar

    function updateSelectedDate(date) {
        timelineCalendar.updateSelectedDate(date)
    }

    Rectangle {
        anchors.fill: parent
        color: WisenetGui.contrast_09_dark//  WisenetGui.transparent
        border.width: 1
        border.color: WisenetGui.contrast_06_grey

        WisenetCalendar {
            id: timelineCalendar
            useMultiSelection: false
            backgroundColor: WisenetGui.contrast_09_dark
            anchors.centerIn: parent

            Component.onCompleted: {
                calendar.calendarTimelineModel.connectMediaControlModelSignals(mediaControlModel)
                mediaControlModel.visibleStartTime = mediaControlModel.visibleStartTime
                mediaControlModel.visibleEndTime = mediaControlModel.visibleEndTime
                singleSelectionCompleted(false)
            }

            Connections {
                target: mediaControlModel
                enabled: mediaControlModel.isMultiTimelineMode
                function onCalendarMinimumDateChanged() {
                    calendar.minimumDate = mediaControlModel.calendarMinimumDate
                }
                function onCalendarMaximumDateChanged() {
                    calendar.maximumDate = mediaControlModel.calendarMaximumDate
                }
            }

            Connections {
                target: calendar.calendarTimelineModel
                enabled: !mediaControlModel.isMultiTimelineMode
                function onTimelineUpdated() {
                    calendar.minimumDate = new Date(mediaControlModel.recordingStartTime)
                    calendar.maximumDate = new Date(mediaControlModel.recordingEndTime)
                }
            }

            Connections {
                target: mediaController
                enabled: !mediaControlModel.isMultiTimelineMode
                function onFocusedViewingGridItemChanged() {
                    refreshCalendar(true)   // SingleTimelineMode에서 선택 채널 변경 시 캘린더 갱신
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    if(focusedItemIsCamera)
                        calendarContextMenu.popup()
                }
            }

            onVisibleYearMonthChanged: {
                console.log("onVisibleYearMonthChanged(), year :", calendar.visibleYear, "month :", calendar.visibleMonth+1)
                refreshCalendar(true)
            }

            onSingleSelectionCompleted: {
                console.log("onSingleSelectionCompleted(seekRequest), seekRequest :", seekRequest)
                mediaControlModel.calendarSelectedDate = calendar.selectedDate

                var selectionStartTime = calendar.calendarTimelineModel.getDayStartTimeMsec(calendar.selectedDate)
                var selectionEndTime = calendar.calendarTimelineModel.getDayEndTimeMsec(calendar.selectedDate)
                var rangeDelta = selectionEndTime - selectionStartTime - mediaControlModel.visibleTimeRange  // 표시 범위 변화량
                var startTimeDelta = selectionStartTime - mediaControlModel.visibleStartTime    // 표시 시작 시간 변화량
                mediaControlModel.setVisibleTimeRange(startTimeDelta, rangeDelta)

                var item = focusedViewingGridItem
                if(!seekRequest || !item || !item.isVideoItem)
                    return

                if(item.mediaPlaySpeed >= 0)
                    requestSeektoRecordExist(selectionStartTime) // 바뀐 날짜의 시작시간으로 seek
                else
                    requestSeektoRecordExist(selectionEndTime) // 바뀐 날짜의 시작시간으로 seek
            }
        }

        Image {
            y: 11
            width: 16
            height: 16
            anchors.right: parent.right
            anchors.rightMargin: 9
            sourceSize: Qt.size(width, height)
            source: closeButtonArea.pressed ? WisenetImage.popup_close_press :
                    closeButtonArea.containsMouse ? WisenetImage.popup_close_hover : WisenetImage.popup_close_default
            MouseArea {
                id: closeButtonArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    calendarControl.visible = false
                }
            }
        }
    }

    CalendarContextMenu {
        id: calendarContextMenu
    }
}
