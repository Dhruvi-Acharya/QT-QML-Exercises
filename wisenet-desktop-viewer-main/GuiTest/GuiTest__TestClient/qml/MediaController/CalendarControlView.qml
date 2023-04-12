import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Wisenet.Qmls 0.1

Item {
    id: calendarControl
    width: 250
    height: 370

    property Calendar calendar: the_calendar
    property bool isPinned: pinButton.state == "pin" ? true : false
    function autoHideTimerRestart() {
        autoHideTimer.restart()
    }
    function autoHideTimerStop() {
        autoHideTimer.stop()
    }

    CalendarTimelineModel {
        id: calendarTimeline
    }

    Timer {
        id: autoHideTimer
        interval: 3000
        running: false
        repeat: false
        onTriggered: {
            calendarControl.visible = false
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onContainsMouseChanged: {
            if(!containsMouse && pinButton.state === "unpin")
                autoHideTimer.restart()
            else
                autoHideTimer.stop()
        }
    }

    Rectangle {
        id: dayControl
        width: 250
        height: 120
        visible: false
        border.color: "black"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 1
            spacing: 0

            Rectangle {
                id: dayToolbar
                x: 0
                y: 0
                Layout.preferredHeight: 20
                Layout.preferredWidth: 250
                Layout.fillWidth: true
                Layout.fillHeight: false
                color: Qt.rgba(48/255,48/255,48/255,1)

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Qt.formatDate(the_calendar.selectedDate, "yyyy-MM-dd")
                    color: Qt.rgba(148/255,148/255,148/255,1)
                    font.bold: true
                }

                Rectangle {
                    width: 18
                    height: 18
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10

                    Image{
                        anchors.fill: parent
                        source: "icon/calendar_daycontrol_hide.png"
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            dayControl.visible = false
                        }
                    }
                }
            }

            Rectangle {
                x: 0
                y: 20
                Layout.preferredHeight: 100
                Layout.preferredWidth: 250
                Layout.fillWidth: true
                Layout.fillHeight: false

                Grid {
                    id: timeGrid
                    anchors.fill: parent
                    rows: 4
                    columns: 6

                    Repeater {
                        model: 24

                        Rectangle {
                            id: hourRect
                            width: timeGrid.width/6
                            height: timeGrid.height/4
                            color: calendarTimeline.hasRecordForDate(the_calendar.selectedDate)
                                   ? Qt.rgba(32/255,128/255,32/255,1)
                                   : Qt.rgba(48/255,48/255,48/255,1)

                            property date hourTime: new Date(the_calendar.selectedDate.getFullYear(),
                                                             the_calendar.selectedDate.getMonth(),
                                                             the_calendar.selectedDate.getDate(),
                                                             hourRect.Positioner.index)

                            property date startTime: new Date(the_calendar.selectedDate.getFullYear(),
                                                              the_calendar.selectedDate.getMonth(),
                                                              the_calendar.selectedDate.getDate(),
                                                              hourRect.Positioner.index+1)

                            border.color: Qt.rgba(200/255,113/255,8/255,1)
                            border.width: calendarTimeline.getTimelineStartTime() < startTime && hourTime <= calendarTimeline.getTimelineEndTime()
                                          ? 3 : 0

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                color: the_calendar.minimumDate < startTime && hourTime <= the_calendar.maximumDate
                                       ? Qt.rgba(241/255,241/255,241/255,1)
                                       : Qt.rgba(119/255,119/255,119/255,1)
                                text: hourRect.hourTime.toLocaleTimeString(Qt.locale(), "h ap")
                            }
                            Rectangle {
                                // right border
                                color: "black"
                                width: 1
                                height: parent.height
                                anchors.right: parent.right
                                visible: hourRect.Positioner.index % 6 == 5 ? false : true
                            }
                            Rectangle {
                                // top border
                                color: "black"
                                width: parent.width
                                height: 1
                                anchors.top: parent.top
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: monthControl
        y: 120
        width: 250
        height: 250

        Calendar{
            id: the_calendar
            anchors.fill: parent

            onSelectedDateChanged: {
                dayControl.visible = true
            }

            style: CalendarStyle {
                id: customCalendarStyle
                gridColor: Qt.rgba(18/255,18/255,18/255,1)

                background: Rectangle{
                    anchors.fill: parent
                    color:Qt.rgba(48/255,48/255,48/255,1)
                }

                // Title(year and month)
                navigationBar:Item{
                    height: the_calendar.height/8

                    Rectangle {
                        id: prevMonth
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        width: parent.height/2
                        height: width

                        Image{
                            anchors.fill: parent
                            source: "icon/calendar_prevmonth.png"
                        }

                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                if(the_calendar.minimumDate < new Date(the_calendar.visibleYear, the_calendar.visibleMonth, 1))
                                    the_calendar.showPreviousMonth()
                            }
                        }
                    }

                    Rectangle {
                        id: nextMonth
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        width: parent.height/2
                        height: width

                        Image{
                            anchors.fill: parent
                            source: "icon/calendar_nextmonth.png"
                        }

                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                if(the_calendar.maximumDate > new Date(the_calendar.visibleYear, the_calendar.visibleMonth + 1, 0))
                                    the_calendar.showNextMonth()
                            }
                        }
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: styleData.title
                        font.bold: true
                        color: Qt.rgba(242/255,242/255,242/255,1)
                    }
                }

                // Week
                dayOfWeekDelegate: Item{
                    //color: "transparent"
                    height: the_calendar.height/8
                    Label {
                        text: the_calendar.__locale.dayName(styleData.dayOfWeek, the_calendar.dayOfWeekFormat)
                        anchors.centerIn: parent
                        color: {
                            if(styleData.dayOfWeek === Locale.Sunday || styleData.dayOfWeek === Locale.Saturday)
                                color = Qt.rgba(255/255,61/255,61/255,1)
                            else
                                color = Qt.rgba(183/255,183/255,183/255,1)
                        }
                    }
                }

                // Day
                dayDelegate: Rectangle {
                    border.color: Qt.rgba(200/255,113/255,8/255,1)
                    border.width: calendarTimeline.getTimelineStartTime() <= styleData.date && styleData.date <= calendarTimeline.getTimelineEndTime()
                                  ? 3 : 0
                    color: calendarTimeline.hasRecordForDate(styleData.date)
                           ? Qt.rgba(32/255,128/255,32/255,1)
                           : Qt.rgba(48/255,48/255,48/255,1)

                    Label {
                        id: dayLabel
                        anchors.centerIn: parent
                        text: styleData.date.getDate()
                        font.bold: styleData.valid
                        color: !styleData.valid ? Qt.rgba(91/255,91/255,91/255,1)
                                                : (styleData.index === 0 || styleData.index % 7 == 0 || styleData.index % 7 == 6) ? Qt.rgba(255/255,61/255,61/255,1)
                                                                                                                                  : Qt.rgba(241/255,241/255,241/255,1)
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onContainsMouseChanged: {
                            if(!containsMouse && pinButton.state === "unpin")
                                autoHideTimer.restart()
                            else
                                autoHideTimer.stop()
                        }
                        onClicked: {
                            if(styleData.valid) {
                                dayControl.visible = !dayControl.visible
                                the_calendar.selectedDate = styleData.date
                            }
                        }
                    }
                }
            }
        }

        Button {
            id: pinButton
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 6
            anchors.rightMargin: 43
            width: 20
            height: 20

            state: "unpin"
            states: [
                State { name: "unpin" },
                State { name: "pin" }
            ]

            Image{
                anchors.fill: parent
                source: {
                    if(pinButton.state == "unpin")
                        sourec: "icon/calendar_pin_off.png"
                    else
                        sourec: "icon/calendar_pin_on.png"
                }
            }

            onClicked: {
                if(state == "unpin")
                    state = "pin"
                else
                    state = "unpin"
            }

            onHoveredChanged: {
                if(!hovered && pinButton.state === "unpin")
                    autoHideTimer.restart()
                else
                    autoHideTimer.stop()
            }
        }
    }
}
