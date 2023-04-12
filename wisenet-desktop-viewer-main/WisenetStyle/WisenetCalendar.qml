import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import Wisenet.MediaController 1.0

Calendar {
    id: calendar
    width: 293//330
    height: 293//324

    property CalendarTimelineModel calendarTimelineModel: calendarTimeline
    property bool useMultiSelection: false
    property color backgroundColor: WisenetGui.contrast_08_dark_grey
    signal multiSelectionCompleted()
    signal singleSelectionCompleted(bool seekRequest)
    signal visibleYearMonthChanged()

    function resetMultiSelection(){
        console.debug("resetMultiSelection...")
        var currentDate = new Date()
        calendarTimeline.selectionStartDate = currentDate
        calendarTimeline.selectionEndDate = currentDate
        multiSelectionCompleted()
    }

    function updateVisibleYearMonth(){
        if(calendar.visibleYear !== calendarTimeline.selectionStartDate.getFullYear()){

            if(calendar.visibleYear > calendarTimeline.selectionStartDate.getFullYear()){
                var previousYearCount = calendar.visibleYear - calendarTimeline.selectionStartDate.getFullYear()
                for(var previousYear = 0; previousYear < previousYearCount; previousYear++){
                    calendar.showPreviousYear()
                }
            }else{
                var nextYearCount = calendarTimeline.selectionStartDate.getFullYear() - calendar.visibleYear
                for(var nextYear = 0; nextYear < nextYearCount; nextYear++){
                    calendar.showNextYear()
                }
            }
        }

        if(calendar.visibleMonth !== calendarTimeline.selectionStartDate.getMonth()){

            if(calendar.visibleMonth > calendarTimeline.selectionStartDate.getMonth()){
                var previousMonthCount = calendar.visibleMonth - calendarTimeline.selectionStartDate.getMonth()
                for(var previousMonth = 0; previousMonth < previousMonthCount; previousMonth++){
                    calendar.showPreviousMonth()
                }
            }else{
                var nextMonthCount = calendarTimeline.selectionStartDate.getMonth() - calendar.visibleMonth
                for(var nextMonth = 0; nextMonth < nextMonthCount; nextMonth++){
                    calendar.showNextMonth()
                }
            }
        }

        visibleYearMonthChanged()
    }

    function isUndefinedDateTime(date){
        if(date === undefined){
            //console.debug("isUndefinedDateTime- Date is undefined")
            return true
        }

        if(date.getFullYear() === 1970
                && date.getMonth() === 0
                && date.getDate() === 1){
            //console.debug("isUndefinedDateTime - true Date: " + date)
            return true
        }
        //console.debug("isUndefinedDateTime - false Date: " + date)
        return false
    }

    function getUndefinedDateTime(){
        return new Date(1970,0,1,0,0,0)
    }

    function updateSelectedDate(date) {
        if(date.getFullYear() === calendar.selectedDate.getFullYear() &&
                date.getMonth() === calendar.selectedDate.getMonth() &&
                date.getDate() === calendar.selectedDate.getDate())
            return  // 같은 날짜가 이미 선택되어 있으면 리턴

        // 신규 날짜 선택
        var selectedDate = new Date(date.getFullYear(), date.getMonth(), date.getDate(), 0, 0, 0)

        calendar.visibleYear = selectedDate.getFullYear()
        calendar.visibleMonth = selectedDate.getMonth()

        if(!useMultiSelection) {
            calendarTimeline.selectionStartDate = selectedDate
            calendar.selectedDate = selectedDate
            singleSelectionCompleted(false) // seek는 하지 않음
        }
        else {
            calendarTimeline.selectionStartDate = selectedDate
            calendarTimeline.selectionEndDate = selectedDate
            multiSelectionCompleted()
        }
    }

    CalendarTimelineModel {
        id: calendarTimeline
    }

    style: CalendarStyle {
        id: customCalendarStyle
        gridColor: WisenetGui.transparent
        gridVisible: false

        background: Rectangle{
            anchors.fill: parent
            color: backgroundColor
        }

        // Title(year and month)
        navigationBar: Item{
            id: navigationArea
            height: 60//calendar.height * 2 / 9

            MouseArea {
                // for block background mouse action
                anchors.fill: parent
                acceptedButtons: Qt.MiddleButton | Qt.LeftButton | Qt.RightButton
                onWheel: {}
            }

            Rectangle {
                width: 270
                height: 17
                anchors.top: parent.top
                anchors.topMargin: 12
                anchors.horizontalCenter: parent.horizontalCenter
                color: WisenetGui.transparent

                Button {
                    id: prevMonthButton
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    width: 14
                    height: 14
                    hoverEnabled: true
                    background: Rectangle { color: WisenetGui.transparent }
                    enabled: {
                        if(calendar.minimumDate < new Date(calendar.visibleYear, calendar.visibleMonth, 1))
                            return true
                        else
                            return false
                    }

                    Image {
                        anchors.fill: parent
                        sourceSize: Qt.size(width, height)
                        source: {
                            if(!parent.enabled)
                                return WisenetImage.calendar_prev_month_dim
                            else if(parent.pressed)
                                return WisenetImage.calendar_prev_month_press
                            else if(parent.hovered)
                                return WisenetImage.calendar_prev_month_over
                            else
                                return WisenetImage.calendar_prev_month
                        }
                    }

                    onClicked: {
                        calendar.showPreviousMonth()
                        visibleYearMonthChanged()
                    }
                }

                Button {
                    id: nextMonthButton
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    width: 14
                    height: 14
                    hoverEnabled: true
                    background: Rectangle { color: WisenetGui.transparent }
                    enabled: {
                        if(calendar.maximumDate > new Date(calendar.visibleYear, calendar.visibleMonth + 1, 0))
                            return true
                        else
                            return false
                    }

                    Image {
                        anchors.fill: parent
                        sourceSize: Qt.size(width, height)
                        source: {
                            if(!parent.enabled)
                                return WisenetImage.calendar_next_month_dim
                            else if(parent.pressed)
                                return WisenetImage.calendar_next_month_press
                            else if(parent.hovered)
                                return WisenetImage.calendar_next_month_over
                            else
                                return WisenetImage.calendar_next_month
                        }
                    }

                    onClicked: {
                        calendar.showNextMonth()
                        visibleYearMonthChanged()
                    }
                }

                Rectangle {
                    width: monthComboBox.width + yearComboBox.width + todayButton.width + 37
                    height: 17
                    color: WisenetGui.transparent
                    anchors.horizontalCenter: parent.horizontalCenter

                    WisenetComboBox {
                        id: yearComboBox
                        anchors.left: parent.left
                        width: 49
                        height: 17
                        itemTextMargin: 10
                        model: getYearModel(calendar.minimumDate, calendar.maximumDate)
                        displayText: calendar.visibleYear
                        currentIndex: indexOfValue(calendar.visibleYear)
                        onActivated: {
                            calendar.visibleYear = currentValue
                        }

                        function getYearModel(minimumDate, maximumDate) {
                            var startYear = minimumDate.getFullYear()
                            var endYear = maximumDate.getFullYear()

                            var years = []
                            for(var i=startYear ; i<=endYear ; i++) {
                                years.push(i)
                            }

                            return years
                        }
                    }

                    WisenetComboBox {
                        id: monthComboBox
                        anchors.left: yearComboBox.right
                        anchors.leftMargin: 16
                        width: 40
                        height: 17
                        itemTextMargin: 10
                        model: getMonthModel(calendar.minimumDate, calendar.maximumDate, calendar.visibleYear)
                        displayText: calendar.visibleMonth + 1
                        currentIndex: indexOfValue(calendar.visibleMonth + 1)
                        onActivated: {
                            calendar.visibleMonth = currentValue - 1
                        }

                        function getMonthModel(minimumDate, maximumDate, visibleYear) {
                            var startMonth = 1
                            var endMonth = 12

                            if(minimumDate.getFullYear() === visibleYear)
                                startMonth = minimumDate.getMonth() + 1
                            if(maximumDate.getFullYear() === visibleYear)
                               endMonth =  maximumDate.getMonth() + 1

                            var months = []
                            for(var i=startMonth ; i<=endMonth ; i++) {
                                months.push(i)
                            }

                            return months
                        }
                    }

                    Button {
                        id: todayButton
                        height: 17
                        anchors.right: parent.right
                        text: WisenetLinguist.today
                        leftPadding: 3
                        rightPadding: 3
                        font.pixelSize: 12
                        palette.buttonText: WisenetGui.contrast_01_light_grey
                        background: Rectangle { color: WisenetGui.transparent }
                        Rectangle {
                            width: parent.width
                            height: 1
                            anchors.bottom: parent.bottom
                            color: WisenetGui.contrast_06_grey
                        }

                        onClicked: {
                            var today = new Date()
                            today = new Date(today.getFullYear(), today.getMonth(), today.getDate(), 0, 0, 0)
                            calendar.visibleYear = today.getFullYear()
                            calendar.visibleMonth = today.getMonth()

                            // 오늘 날짜를 선택까지 하도록 수정
                            if(!useMultiSelection) {
                                calendarTimeline.selectionStartDate = today
                                calendar.selectedDate = today
                                singleSelectionCompleted(true)
                            }
                            else {
                                calendarTimeline.selectionStartDate = today
                                calendarTimeline.selectionEndDate = today
                                multiSelectionCompleted()
                            }
                        }
                    }
                }
            }
        }

        // Week
        dayOfWeekDelegate: Item {
            height: 33//calendar.height/9

            MouseArea {
                // for block background mouse action
                anchors.fill: parent
                acceptedButtons: Qt.MiddleButton | Qt.LeftButton | Qt.RightButton
                onWheel: {}
            }

            Label {
                text: calendar.__locale.dayName(styleData.dayOfWeek, calendar.dayOfWeekFormat)===undefined ? "" : calendar.__locale.dayName(styleData.dayOfWeek, calendar.dayOfWeekFormat)
                anchors.centerIn: parent
                font.pixelSize: 10
                color: WisenetGui.contrast_06_grey
            }
        }

        // Day
        dayDelegate: Rectangle {
            id: dayRect
            anchors.fill: parent
            color: "transparent"

            Rectangle {
                id: selectionRect
                width: isSelectionDate(calendarTimeline.selectionStartDate) || isSelectionDate(calendarTimeline.selectionEndDate) ? 30 : parent.width
                height: 30
                anchors.centerIn: parent
                radius: isSelectionDate(calendarTimeline.selectionStartDate) || isSelectionDate(calendarTimeline.selectionEndDate) ? 15 : 0
                color: isInSelection() || (!useMultiSelection && border.width != 0) ? WisenetGui.color_primary : WisenetGui.transparent
                border.width: isSelectionDate(calendarTimeline.selectionStartDate) ? 1 : 0
                border.color: WisenetGui.color_primary
                //visible: useMultiSelection


                function isInSelection(){
                    if(isUndefinedDateTime(calendarTimeline.selectionStartDate) || isUndefinedDateTime(calendarTimeline.selectionEndDate)){
                        //console.debug("isSelectionDate - undefinded" + "styleData : " + styleData.date)
                        return false
                    }

                    var startDate = calendarTimeline.selectionStartDate
                    var endDate = calendarTimeline.selectionEndDate
                    var curDate = styleData.date
                    startDate.setHours(0); startDate.setMinutes(0); startDate.setSeconds(0); startDate.setMilliseconds(0);
                    endDate.setHours(0); endDate.setMinutes(0); endDate.setSeconds(0); endDate.setMilliseconds(0);
                    curDate.setHours(0); curDate.setMinutes(0); curDate.setSeconds(0); curDate.setMilliseconds(0);

                    if((startDate.getTime() <= curDate.getTime()) && (endDate.getTime() >= curDate.getTime())){

                        //console.debug("isSelectionDate - in a section start:" + startDate + ",end:" + endDate +"styleData : " + curDate)
                        return true
                    }

                    //console.debug("isSelectionDate - out of a section start:" + startDate + ",end:" + endDate +"styleData : " + curDate)
                    return false
                }

                function isSelectionDate(date){

                    if(!isUndefinedDateTime(date)
                            && !isUndefinedDateTime(styleData.date)
                            && (date.getFullYear() === styleData.date.getFullYear())
                            && (date.getMonth() === styleData.date.getMonth())
                            && (date.getDate() === styleData.date.getDate())){
                        //console.debug("isSelectionDate - true: " + date + ", styleData : " + styleData.date)
                        return true
                    }
                    //console.debug("isSelectionDate - false: " + date + ", styleData : " + styleData.date)
                    return false
                }

            }

            Rectangle {
                width: parent.width / 2
                height: 30
                color: WisenetGui.color_primary
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: selectionRect.isSelectionDate(calendarTimeline.selectionStartDate) && !isUndefinedDateTime(calendarTimeline.selectionEndDate) && !selectionRect.isSelectionDate(calendarTimeline.selectionEndDate)
            }

            Rectangle {
                width: parent.width / 2
                height: 30
                color: WisenetGui.color_primary
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                visible: selectionRect.isSelectionDate(calendarTimeline.selectionEndDate) && !isUndefinedDateTime(calendarTimeline.selectionStartDate) && !selectionRect.isSelectionDate(calendarTimeline.selectionStartDate)
            }

            Label {
                id: dayLabel
                anchors.centerIn: parent
                text: styleData.date.getDate()===undefined ? "" : styleData.date.getDate()
                color: {
                    if(!styleData.valid)
                        return WisenetGui.contrast_07_grey

                    if(selectionRect.color !== WisenetGui.transparent)
                        return WisenetGui.contrast_00_white
                    else
                        return getDateRecordColor(calendarTimeline.refreshFlag, styleData.date)===undefined ? WisenetGui.contrast_00_white : getDateRecordColor(calendarTimeline.refreshFlag, styleData.date)
                }
                font.pixelSize: 12
            }

            Image {
                width: 3
                height: 3
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: selectionRect.bottom
                anchors.bottomMargin: 3
                source: selectionRect.color == WisenetGui.transparent ? WisenetImage.calendar_today : WisenetImage.calendar_today_selected
                sourceSize: Qt.size(width, height)
                visible: {
                    var today = new Date()
                    return today.getFullYear() === styleData.date.getFullYear()
                            && today.getMonth() === styleData.date.getMonth()
                            && today.getDate() === styleData.date.getDate();
                }
            }

            function getDateRecordColor(flag, time) {
                if(calendarTimeline.hasRecordForDate(time))
                    return WisenetGui.color_primary
                else
                    return WisenetGui.contrast_04_light_grey
            }

            MouseArea {
                // for block background mouse action
                anchors.fill: parent
                acceptedButtons: Qt.MiddleButton | Qt.RightButton
                onWheel: {}
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton

                onClicked: {
                    if(!styleData.valid)
                        return

                    if(!useMultiSelection) {
                        calendarTimeline.selectionStartDate = styleData.date
                        calendar.selectedDate = styleData.date
                        singleSelectionCompleted(true)
                        return;
                    }

                    if(isUndefinedDateTime(calendarTimeline.selectionStartDate)
                            || (!isUndefinedDateTime(calendarTimeline.selectionStartDate) && !isUndefinedDateTime(calendarTimeline.selectionEndDate))) {
                        calendarTimeline.selectionStartDate = styleData.date
                        calendarTimeline.selectionEndDate = getUndefinedDateTime();
                        //console.debug("calendar Timeline click- 1 start: " + calendarTimeline.selectionStartDate + ", end: " + calendarTimeline.selectionEndDate)
                    }
                    else {

                        if(calendarTimeline.selectionStartDate <= styleData.date){
                            calendarTimeline.selectionEndDate = styleData.date
                            //console.debug("calendar Timeline click- 2 start: " + calendarTimeline.selectionStartDate + ", end: " + calendarTimeline.selectionEndDate)
                        }else{
                            calendarTimeline.selectionEndDate = calendarTimeline.selectionStartDate
                            calendarTimeline.selectionStartDate = styleData.date
                            //console.debug("calendar Timeline click- 3 start: " + calendarTimeline.selectionStartDate + ", end: " + calendarTimeline.selectionEndDate)
                        }

                        multiSelectionCompleted()
                    }
                }
            }
        }
    }
}
