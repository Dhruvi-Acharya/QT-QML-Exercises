import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Wisenet.DeviceSelectionTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import WisenetLanguage 1.0

Rectangle{
    id: textSearchFilterView
    visible: true
    color: WisenetGui.contrast_09_dark

    property int itemDefaultLeftMargin: 24

    signal initialize()
    signal search(bool allDevice, var channels, string keyword, date from, date to, bool isWholeWord, bool isCaseSensitive)
    signal resetListPosition()

    function reset(){
        eventSearchFilterTree.reset()
        deviceSeletectionTree.reset()
        calendarControlView.resetMultiSelection()
        var currentDateTime = new Date()
        fromTime.setTimeText(currentDateTime.getHours()-1, currentDateTime.getMinutes(), currentDateTime.getSeconds())
        toTime.setTimeText(23,59,59)
    }

    function setChannelFilter(channels)
    {
        console.debug("setchannelFilter: " + channels)
        deviceSeletectionTree.selectChannels(channels)
    }

    function setDateFilter(from, to)
    {
        updateCalendarLimitDate()

        textSearchFromDateEditor.setDate(from)
        textSearchToDateEditor.setDate(to)

        fromTime.setTimeText(from.getHours(),from.getMinutes(),from.getSeconds())
        toTime.setTimeText(to.getHours(),to.getMinutes(),to.getSeconds())

        updateCalendar()
    }

    function searchByFilter()
    {
        var keyword = keywordInputTextBox.text

        var from = new Date(textSearchFromDateEditor.year, textSearchFromDateEditor.month - 1, textSearchFromDateEditor.day, fromTime.hour, fromTime.minute, fromTime.second)
        var to = new Date(textSearchToDateEditor.year, textSearchToDateEditor.month - 1, textSearchToDateEditor.day, toTime.hour, toTime.minute, toTime.second)

        var isCaseSensitive= keywordCheckBox1.checked
        var isWholeWord = keywordCheckBox2.checked

        var channels = [];

        if(deviceSeletectionTree.selectedDeviceValue !== null && deviceSeletectionTree.selectedDeviceValue.length > 0){
            deviceSeletectionTree.selectedDeviceValue.forEach(function(item){
                channels.push(item.uuid)
            })
        }

        search(deviceSeletectionTree.allDevice, channels, keyword, from, to, isWholeWord, isCaseSensitive)
    }

    function updateCalendar()
    {
        var fromDate = new Date(textSearchFromDateEditor.year,
                                textSearchFromDateEditor.month-1,
                                textSearchFromDateEditor.day,
                                0,0,0)

        var toDate = new Date(textSearchToDateEditor.year,
                              textSearchToDateEditor.month-1,
                              textSearchToDateEditor.day,
                              0,0,0)

        calendarControlView.calendarTimelineModel.setSelectionStartDate(fromDate)
        calendarControlView.calendarTimelineModel.setSelectionEndDate(toDate)
        calendarControlView.updateVisibleYearMonth()
    }

    function updateCalendarLimitDate()
    {
        var currentDate = new Date()
        calendarControlView.minimumDate = new Date(currentDate.getFullYear() - 2,0,1,0,0,0)
        currentDate.setDate(currentDate.getDate() + 1)
        calendarControlView.maximumDate = currentDate
    }

    function initializeTree()
    {
        textSearchDeviceTreeSourceModel.initializeTree()
    }

    Component.onCompleted: {
        reset();
    }

    Column{
        id: searchCondition
        spacing: 0

        Rectangle{
            height: 1
            width: textSearchFilterView.width
            color: WisenetGui.contrast_08_dark_grey
        }

        //장치
        Rectangle{
            id: deviceRectangle
            width: textSearchFilterView.width
            height: 68
            Layout.alignment: Qt.AlignTop
            color: WisenetGui.contrast_09_dark

            Text{
                id: deviceText
                width: parent.width - 48
                leftPadding: 24
                topPadding: 12
                elide: Text.ElideRight
                color: WisenetGui.contrast_04_light_grey
                text: WisenetLinguist.device
                font.pixelSize: 12
            }

            //all또는 선택된 camera들 가로로 보여주기
            Text{
                id: cameraNames
                width: parent.width - 48
                leftPadding: 24
                topPadding: 39
                elide: Text.ElideRight
                color: WisenetGui.contrast_01_light_grey
                text: WisenetLinguist.all
                font.pixelSize: 12
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    //deviceSeletectionTree.initalize()
                    deviceViewPopup.open()
                }
            }
        }

        Rectangle{
            width: textSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        // Date & Time Area
        Rectangle{
            id: dateTimeRectangle
            width: textSearchFilterView.width
            height: 134
            color: WisenetGui.contrast_09_dark

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    calendarViewPopup.open()
                }
            }

            Text{
                id: dateTimeText
                width: parent.width - 48
                leftPadding: 24
                topPadding: 12
                elide: Text.ElideRight
                color: WisenetGui.contrast_04_light_grey
                text: WisenetLinguist.dateAndTime
                font.pixelSize: 12
            }

            Rectangle{
                id: fromRectangle
                x: 24
                y: 40
                width: 88
                height: 76
                color: WisenetGui.contrast_09_dark

                WisenetStateImageButton{
                    id: calendarViewPopupButton
                    x: 0
                    y: 0
                    width: 17
                    height: 17
                    defaultImageSource: WisenetImage.calendar_Default
                    hoverImageSource: WisenetImage.calendar_Hover
                    selectImageSource: WisenetImage.calendar_Select
                    disableImageSource: WisenetImage.calendar_Disable

                    onClicked: {
                        calendarViewPopup.open()
                    }
                }

                Text{
                    id: fromText
                    y:2
                    width: 63
                    leftPadding: 23
                    topPadding: 0
                    elide: Text.ElideRight
                    color: WisenetGui.contrast_04_light_grey
                    text: WisenetLinguist.from
                    font.pixelSize: 12
                }

                WisenetDateEditor{
                    id: textSearchFromDateEditor
                    x: 7
                    y: 26
                    width: 82

                    onControlFocusChanged: {
                        if(!controlFocus){
                            updateCalendarLimitDate()
                            var fromDate = new Date(year,month-1,day,0,0,0)
                            var toDate = new Date(textSearchToDateEditor.year,
                                                  textSearchToDateEditor.month-1,
                                                  textSearchToDateEditor.day,
                                                  0,0,0)
                            if(fromDate.getTime() > calendarControlView.maximumDate.getTime()){
                                fromDate = calendarControlView.maximumDate; fromDate.setHours(0);fromDate.setMinutes(0);fromDate.setSeconds(0);
                                setDate(fromDate)
                            }
                            if(fromDate.getTime() < calendarControlView.minimumDate.getTime()){
                                fromDate = calendarControlView.minimumDate; fromDate.setHours(0);fromDate.setMinutes(0);fromDate.setSeconds(0);
                                setDate(fromDate)
                            }

                            if(fromDate.getTime() > toDate.getTime()){
                                textSearchToDateEditor.setDate(fromDate)
                            }

                            if(fromDate.getTime() === toDate.getTime()){
                                var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                if(ftime > ttime){
                                    toTime.setTimeText(fromTime.hour, fromTime.minute,fromTime.second)
                                }
                            }

                            updateCalendar();

                        }
                        console.debug("conrtolFocus changed: " + controlFocus)
                    }
                }

                Rectangle{
                    x: 0
                    y: 42
                    width: fromRectangle.width
                    height: 1
                    color: WisenetGui.contrast_06_grey
                }

                Rectangle{
                    x: 0
                    y: 55
                    width: fromRectangle.width
                    height: 21
                    color: WisenetGui.contrast_09_dark
                    border.color: WisenetGui.contrast_06_grey
                    border.width: 1

                    WisenetTimeEditor{
                        id: fromTime
                        anchors.leftMargin: 16
                        anchors.topMargin: 4
                        anchors.fill: parent
                        buttonVisible: false

                        onControlFocusChanged: {
                            if(!controlFocus){

                                var fromDate = new Date(textSearchFromDateEditor.year,
                                                        textSearchFromDateEditor.month-1,
                                                        textSearchFromDateEditor.day,
                                                        0,0,0)
                                var toDate = new Date(textSearchToDateEditor.year,
                                                      textSearchToDateEditor.month-1,
                                                      textSearchToDateEditor.day,
                                                      0,0,0)

                                if(fromDate.getTime() === toDate.getTime()){
                                    var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                    var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                    if(ftime > ttime){
                                        toTime.setTimeText(fromTime.hour, fromTime.minute,fromTime.second)
                                    }
                                }
                            }
                        }
                    }
                }

            }

            Rectangle{
                id: toRectangle
                x: 130
                y: 40
                width: 88
                height: 76
                color: WisenetGui.contrast_09_dark

                Text{
                    id: toText
                    y:2
                    width: 63
                    leftPadding: 0
                    topPadding: 0
                    elide: Text.ElideRight
                    color: WisenetGui.contrast_04_light_grey
                    text: WisenetLinguist.to
                    font.pixelSize: 12
                }

                WisenetDateEditor{
                    id: textSearchToDateEditor
                    x: 7
                    y: 26
                    width: 82

                    onControlFocusChanged: {
                        if(!controlFocus){
                            updateCalendarLimitDate()

                            var toDate = new Date(year,month-1,day,0,0,0)
                            var fromDate = new Date(textSearchFromDateEditor.year,
                                                  textSearchFromDateEditor.month-1,
                                                  textSearchFromDateEditor.day,
                                                  0,0,0)

                            if(toDate.getTime() > calendarControlView.maximumDate.getTime()){
                                console.debug("toDate is more then maximumDate. toDate:" + toDate + ", maximumDate" + calendarControlView.maximumDate)
                                toDate = calendarControlView.maximumDate; toDate.setHours(0);toDate.setMinutes(0);toDate.setSeconds(0);
                                setDate(toDate)
                            }
                            if(toDate.getTime() < calendarControlView.minimumDate.getTime()){
                                toDate = calendarControlView.minimumDate; toDate.setHours(0);toDate.setMinutes(0);toDate.setSeconds(0);
                                setDate(toDate)
                            }

                            if(fromDate.getTime() > toDate.getTime()){
                                textSearchFromDateEditor.setDate(toDate)
                            }

                            if(fromDate.getTime() === toDate.getTime()){
                                var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                if(ftime > ttime){
                                    fromTime.setTimeText(toTime.hour, toTime.minute,toTime.second)
                                }
                            }

                            updateCalendar();
                        }
                    }


                }

                Rectangle{
                    x: 0
                    y: 42
                    width: toRectangle.width
                    height: 1
                    color: WisenetGui.contrast_06_grey
                }

                Rectangle{
                    x: 0
                    y: 55
                    width: toRectangle.width
                    height: 21
                    color: WisenetGui.contrast_09_dark
                    border.color: WisenetGui.contrast_06_grey
                    border.width: 1

                    WisenetTimeEditor{
                        id: toTime
                        anchors.leftMargin: 16
                        anchors.topMargin: 4
                        anchors.fill: parent
                        buttonVisible: false

                        onControlFocusChanged: {
                            if(!controlFocus){

                                var fromDate = new Date(textSearchFromDateEditor.year,
                                                        textSearchFromDateEditor.month-1,
                                                        textSearchFromDateEditor.day,
                                                        0,0,0)
                                var toDate = new Date(textSearchToDateEditor.year,
                                                      textSearchToDateEditor.month-1,
                                                      textSearchToDateEditor.day,
                                                      0,0,0)

                                if(fromDate.getTime() === toDate.getTime()){
                                    var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                    var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                    if(ftime > ttime){
                                        fromTime.setTimeText(toTime.hour, toTime.minute,toTime.second)
                                    }
                                }
                            }
                        }
                    }
                }
            }

        }

        Rectangle{
            width: textSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        Rectangle{
            id: eventKeywordRectangle
            width: textSearchFilterView.width
            height: 174
            color: WisenetGui.transparent

            Text{
                id: keywordText
                width: parent.width - 48
                leftPadding: 24
                topPadding: 12
                elide: Text.ElideRight
                color: WisenetGui.contrast_04_light_grey
                text: WisenetLinguist.keyword
                font.pixelSize: 12
            }

            Rectangle{
                id: keywordRectangle
                x: 24
                y: 40

                WisenetTextBox {
                    id: keywordInputTextBox
                    anchors.topMargin: 30
                    height: 30
                    width: 170
                    Layout.alignment: Qt.AlignVCenter
                    placeholderText: WisenetLinguist.inputKeyword
                    visible: true
                }

                WisenetCheckBox {
                    id: keywordCheckBox1
                    anchors.top: keywordInputTextBox.bottom
                    anchors.topMargin: 25
                    text: WisenetLinguist.caseSensitivity
                    enabled: true

                    onCheckedChanged: {
                        keywordCheckBox1.checked = this.checked
                    }
                }

                WisenetCheckBox {
                    id: keywordCheckBox2
                    anchors.top: keywordCheckBox1.bottom
                    anchors.topMargin: 25
                    text: WisenetLinguist.wholeWord
                    enabled: true

                    onCheckedChanged: {
                        keywordCheckBox2.checked = this.checked
                    }
                }
            }
        }

        Rectangle{
            width: textSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        Rectangle{
            id: searchRectangle
            width: textSearchFilterView.width
            height: 58
            color: WisenetGui.transparent

            WisenetOrangeButton{
                id: searchButton
                text: WisenetLinguist.search
                x: 24
                y: 15
                width: 119
                height: 28

                onClicked: {
                    searchByFilter()
                    resetListPosition()
                }
            }

            WisenetGrayButton{
                id: resetButton
                x: 153
                y: 15
                width: 66
                height: 28

                Text {
                    id: resetButtonText
                    anchors.fill: parent
                    anchors.leftMargin: 18
                    anchors.rightMargin: 17
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: lineCount == 1 ? Text.AlignHCenter : Text.AlignLeft
                    width: 66
                    height: 24
                    color: WisenetGui.contrast_00_white
                    font.pixelSize: 12

                    fontSizeMode: Text.HorizontalFit
                    minimumPixelSize: 8
                    wrapMode: Text.Wrap
                    text: WisenetLinguist.reset
                }

                onClicked: {
                    textSearchFilterView.reset();
                }
            }
        }
    }


    Rectangle{
        id: eventSearchFilterBorder
        x: textSearchFilterView.width - 1
        y: 0
        width: 1
        height: textSearchFilterView.height
        color: WisenetGui.contrast_08_dark_grey
    }

    //장치선택뷰
    Popup {
        id: deviceViewPopup
        x: textSearchFilterView.width
        y: deviceRectangle.y
        width: 300
        height: 580
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: textSearchDeviceTreeProxyModel
            sourceModel: DeviceSelectionTreeSourceModel{
                id: textSearchDeviceTreeSourceModel

                onInvalidate: textSearchDeviceTreeProxyModel.invalidate()
            }

            Component.onCompleted: {
                doSort(Qt.AscendingOrder)
            }
        }

        contentItem: WisenetDeviceSelectionTree{
            id: deviceSeletectionTree
            anchors.fill: parent
            treeModel: textSearchDeviceTreeProxyModel

            onSelectedDeviceChanged: {
                console.debug("onSelectedDeviceChanged .....")
                if(allDevice === true){
                    cameraNames.text = WisenetLinguist.all
                }else if(selectedDeviceValue === null || selectedDeviceValue.length === 0){
                    cameraNames.text = WisenetLinguist.none
                }else {
                    if(selectedDeviceValue.length === 1){
                        cameraNames.text = selectedDeviceValue[0].displayName
                    }else{
                        cameraNames.text = selectedDeviceValue.length.toString() + " " + WisenetLinguist.channels
                    }
                }
            }

        }

        onOpenedChanged: {
            if(opened){
                deviceSeletectionTree.expandAll()
            }
        }
    }

    //날짜선택뷰
    Popup {
        id: calendarViewPopup
        x: textSearchFilterView.width
        //y: date.y
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
            id: calendarControlView
            anchors.centerIn: parent
            useMultiSelection: true

            onMultiSelectionCompleted:{
                console.debug("onMultiSelectionCompleted -- ")
                textSearchFromDateEditor.setDate(calendarControlView.calendarTimelineModel.selectionStartDate)
                textSearchToDateEditor.setDate(calendarControlView.calendarTimelineModel.selectionEndDate)
            }
        }

        onOpenedChanged: {
            if(opened){
                updateCalendarLimitDate()
            }
        }
    }


    //이벤트타입선택뷰
    Popup {
        id: eventTypeViewPopup
        x: textSearchFilterView.width
        //y: date.y
        width: 300
        height: 570
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: eventSearchFilterTreeProxyModel
            sourceModel: EventFilterTreeSourceModel{
                id: eventSearchFilterTreeSourceModel
                Component.onCompleted: {
                    setAllEventData()
                }
            }
        }


        contentItem: WisenetEventFilterTree{
            id: eventSearchFilterTree
            anchors.fill: parent
            treeModel: eventSearchFilterTreeProxyModel

            onSelectedEventChanged: {
                if(allEvent === true){
                    eventTypeModel.clear()
                }else if(selectedEventValue === null || selectedEventValue.length === 0){
                    eventTypeModel.clear()
                }else {
                    eventTypeModel.clear()
                    selectedEventValue.forEach(function(item) {
                        eventTypeModel.append({"uuid" : item.uuid})
                    })
                }
            }
        }

        onOpenedChanged: {
            if(opened){
                eventSearchFilterTree.expandAll()
            }
        }

    }

}

