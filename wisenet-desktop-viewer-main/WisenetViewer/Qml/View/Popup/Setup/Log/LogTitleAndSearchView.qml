import QtQuick 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0

Item {
    function fromDate(){
        var fromDate = new Date(logFromDateEditor.year,logFromDateEditor.month-1,logFromDateEditor.day,0,0,0)
        return fromDate
    }
    function toDate(){
        var toDate = new Date(logToDateEditor.year,logToDateEditor.month-1,logToDateEditor.day,23,59,59)
        return toDate
    }

    signal searchButonClicked()

    function changeTitle(title){
        titleText.text = title
    }


    function reset(){
        logCalendarControlView.resetMultiSelection()
    }

    function updateCalendar(){
        var fromDate = new Date(logFromDateEditor.year,
                                logFromDateEditor.month-1,
                                logFromDateEditor.day,
                                0,0,0)
        var toDate = new Date(logToDateEditor.year,
                              logToDateEditor.month-1,
                              logToDateEditor.day,
                              0,0,0)
        logCalendarControlView.calendarTimelineModel.setSelectionStartDate(fromDate)
        logCalendarControlView.calendarTimelineModel.setSelectionEndDate(toDate)
        logCalendarControlView.updateVisibleYearMonth()
    }

    function updateCalendarLimitDate(){
        var currentDate = new Date()
        logCalendarControlView.minimumDate = new Date(currentDate.getFullYear() - 2,0,1,0,0,0)
        currentDate.setDate(currentDate.getDate() + 1)
        logCalendarControlView.maximumDate = currentDate
    }

    Text{
        id: titleText
        x: 0
        y: 0
        width: parent.width
        height: 28
        verticalAlignment: Text.AlignBottom
        text: WisenetLinguist.eventLog
        font.pixelSize: 24
        color: WisenetGui.contrast_00_white
    }

    Rectangle{
        id: dateSearchArea
        x: 0
        y: titleText.height + 28
        width: parent.width
        height: 28
        color: WisenetGui.transparent

        WisenetStateImageButton{
            id: logCalendarViewPopupButton
            x: 0
            y: 2
            width: 20
            height: 20
            defaultImageSource: WisenetImage.calendar_Default
            hoverImageSource: WisenetImage.calendar_Hover
            selectImageSource: WisenetImage.calendar_Select
            disableImageSource: WisenetImage.calendar_Disable

            onClicked: {
                logCalendarViewPopup.open()
            }
        }

        Text{
            id: logFromText
            anchors.left: logCalendarViewPopupButton.right
            anchors.leftMargin: 13
            topPadding: 3
            width: 28
            height: parent.height
            elide: Text.ElideRight
            color: WisenetGui.contrast_04_light_grey
            text: WisenetLinguist.from
            font.pixelSize: 12

            onTruncatedChanged: {
                if(truncated){
                    while(truncated){
                        width += 1;
                    }
                }
            }
        }

        WisenetDateEditor{
            id: logFromDateEditor
            anchors.left: logFromText.right
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            width: 102

            onControlFocusChanged: {
                if(!controlFocus){
                    updateCalendarLimitDate()
                    updateCalendarLimitDate()
                    var fromDate = new Date(year,month-1,day,0,0,0)
                    var toDate = new Date(logToDateEditor.year,
                                          logToDateEditor.month-1,
                                          logToDateEditor.day,
                                          0,0,0)
                    if(fromDate.getTime() > logCalendarControlView.maximumDate.getTime()){
                        fromDate = logCalendarControlView.maximumDate; fromDate.setHours(0);fromDate.setMinutes(0);fromDate.setSeconds(0);
                        setDate(fromDate)
                    }
                    if(fromDate.getTime() < logCalendarControlView.minimumDate.getTime()){
                        fromDate = logCalendarControlView.minimumDate; fromDate.setHours(0);fromDate.setMinutes(0);fromDate.setSeconds(0);
                        setDate(fromDate)
                    }

                    if(fromDate.getTime() > toDate.getTime()){
                        logToDateEditor.setDate(fromDate)
                    }

                    updateCalendar();

                }
                console.debug("EventLog conrtolFocus changed: " + controlFocus)
            }
        }

        Text{
            id: logToText
            anchors.left: logFromDateEditor.right
            anchors.leftMargin: 32
            topPadding: 3
            width: 14
            height: parent.height
            elide: Text.ElideRight
            color: WisenetGui.contrast_04_light_grey
            text: WisenetLinguist.to
            font.pixelSize: 12

            onTruncatedChanged: {
                if(truncated){
                    while(truncated){
                        width += 1;
                    }
                }
            }
        }

        WisenetDateEditor{
            id: logToDateEditor
            anchors.left: logToText.right
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            width: 102

            onControlFocusChanged: {
                if(!controlFocus){
                    updateCalendarLimitDate()

                    var toDate = new Date(year,month-1,day,0,0,0)
                    var fromDate = new Date(logFromDateEditor.year,
                                          logFromDateEditor.month-1,
                                          logFromDateEditor.day,
                                          0,0,0)
                    if(toDate.getTime() > logCalendarControlView.maximumDate.getTime()){
                        toDate = logCalendarControlView.maximumDate; toDate.setHours(0);toDate.setMinutes(0);toDate.setSeconds(0);
                        setDate(toDate)
                    }
                    if(toDate.getTime() < logCalendarControlView.minimumDate.getTime()){
                        toDate = logCalendarControlView.minimumDate; toDate.setHours(0);toDate.setMinutes(0);toDate.setSeconds(0);
                        setDate(toDate)
                    }

                    if(fromDate.getTime() > toDate.getTime()){
                        logFromDateEditor.setDate(toDate)
                    }

                    updateCalendar();
                }
            }
        }

        WisenetGrayButton{
            id: logSearchButton
            text: WisenetLinguist.search
            anchors.left: logToDateEditor.right
            anchors.leftMargin: 24
            width: 144
            height: 28

            onClicked: {
                searchButonClicked()
            }
        }

    }

    //날짜선택뷰
    Popup {
        id: logCalendarViewPopup
        x: logCalendarViewPopupButton.x + logCalendarViewPopupButton.width
        y: logCalendarViewPopupButton.y + dateSearchArea.y
        width: 321
        height: 317
        modal: true
        focus: true
        background: Rectangle {
            color: WisenetGui.contrast_08_dark_grey
            border.width: 1
            border.color: WisenetGui.contrast_07_grey
        }

        contentItem: WisenetCalendar{
            id: logCalendarControlView
            anchors.centerIn: parent
            useMultiSelection: true

            onMultiSelectionCompleted:{
                logFromDateEditor.setDate(logCalendarControlView.calendarTimelineModel.selectionStartDate)
                logToDateEditor.setDate(logCalendarControlView.calendarTimelineModel.selectionEndDate)
            }
        }

        onOpenedChanged: {
            if(opened){
                updateCalendarLimitDate()
            }
        }
    }

}
