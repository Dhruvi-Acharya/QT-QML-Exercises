import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Wisenet.DeviceSelectionTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import WisenetLanguage 1.0

Rectangle{
    id: eventSearchFilterView
    visible: true
    color: WisenetGui.contrast_09_dark

    property int itemDefaultLeftMargin: 24

    signal search(date from, date to, bool allChannel, var channels, bool allEvent, var events)
    signal resetListPosition()

    function reset(){
        eventSearchFilterTree.reset()
        deviceSeletectionTree.reset()
        calendarControlView.resetMultiSelection()
        fromTime.setTimeText(0,0,0)
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

        eventSearchFromDateEditor.setDate(from)
        eventSearchToDateEditor.setDate(to)

        fromTime.setTimeText(from.getHours(),from.getMinutes(),from.getSeconds())
        toTime.setTimeText(to.getHours(),to.getMinutes(),to.getSeconds())

        updateCalendar()
    }

    function searchByFilter()
    {
        var from = new Date(eventSearchFromDateEditor.year, eventSearchFromDateEditor.month - 1, eventSearchFromDateEditor.day, fromTime.hour, fromTime.minute, fromTime.second)
        var to = new Date(eventSearchToDateEditor.year, eventSearchToDateEditor.month - 1, eventSearchToDateEditor.day, toTime.hour, toTime.minute, toTime.second)

        var channels = [];

        if(deviceSeletectionTree.selectedDeviceValue !== null && deviceSeletectionTree.selectedDeviceValue.length > 0){
            deviceSeletectionTree.selectedDeviceValue.forEach(function(item){
                channels.push(item.uuid)
            })
        }

        var events = [];

        if(eventSearchFilterTree.selectedEventValue !== null && eventSearchFilterTree.selectedEventValue.length > 0){
            eventSearchFilterTree.selectedEventValue.forEach(function(item){
                events.push(item.uuid)
            })
        }

        search(from, to, deviceSeletectionTree.allDevice, channels, eventSearchFilterTree.allEvent, events)
    }

    function updateCalendar(){
        var fromDate = new Date(eventSearchFromDateEditor.year,
                                eventSearchFromDateEditor.month-1,
                                eventSearchFromDateEditor.day,
                                0,0,0)
        var toDate = new Date(eventSearchToDateEditor.year,
                              eventSearchToDateEditor.month-1,
                              eventSearchToDateEditor.day,
                              0,0,0)
        calendarControlView.calendarTimelineModel.setSelectionStartDate(fromDate)
        calendarControlView.calendarTimelineModel.setSelectionEndDate(toDate)
        calendarControlView.updateVisibleYearMonth()
    }

    function updateCalendarLimitDate(){
        var currentDate = new Date()
        calendarControlView.minimumDate = new Date(currentDate.getFullYear() - 2,0,1,0,0,0)
        currentDate.setDate(currentDate.getDate() + 1)
        calendarControlView.maximumDate = currentDate
    }

    function initializeTree(){
        eventSearchDeviceTreeSourceModel.initializeTree()
    }

    Component.onCompleted: {
        reset();
    }

    onHeightChanged: {

        if(eventTypeListView.contentHeight < 100){
            eventTypeRectangle.height = 174
        }else{
            var maximumEventTypeRactangleHeight = eventSearchFilterView.height - deviceRectangle.height - dateTimeRectangle.height - searchRectangle.height - 84
            console.debug("ContentHeightChanged : " + eventTypeListView.contentHeight + ", maximumHeight : " + maximumEventTypeRactangleHeight)
            if(maximumEventTypeRactangleHeight > (eventTypeListView.contentHeight) ){
                eventTypeRectangle.height = eventTypeListView.contentHeight + 56
            }else {
                eventTypeRectangle.height = maximumEventTypeRactangleHeight + 56
            }
        }
    }

    Column{
        id: searchCondition
        spacing: 0

        Rectangle{
            height: 1
            width: eventSearchFilterView.width
            color: WisenetGui.contrast_08_dark_grey
        }

        //장치
        Rectangle{
            id: deviceRectangle
            width: eventSearchFilterView.width
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
            width: eventSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        // Date & Time Area
        Rectangle{
            id: dateTimeRectangle
            width: eventSearchFilterView.width
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
                    id: eventSearchFromDateEditor
                    x: 7
                    y: 26
                    width: 82

                    onControlFocusChanged: {
                        if(!controlFocus){
                            updateCalendarLimitDate()
                            var fromDate = new Date(year,month-1,day,0,0,0)
                            var toDate = new Date(eventSearchToDateEditor.year,
                                                  eventSearchToDateEditor.month-1,
                                                  eventSearchToDateEditor.day,
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
                                eventSearchToDateEditor.setDate(fromDate)
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

                                var fromDate = new Date(eventSearchFromDateEditor.year,
                                                        eventSearchFromDateEditor.month-1,
                                                        eventSearchFromDateEditor.day,
                                                        0,0,0)
                                var toDate = new Date(eventSearchToDateEditor.year,
                                                      eventSearchToDateEditor.month-1,
                                                      eventSearchToDateEditor.day,
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
                    id: eventSearchToDateEditor
                    x: 7
                    y: 26
                    width: 82

                    onControlFocusChanged: {
                        if(!controlFocus){
                            updateCalendarLimitDate()

                            var toDate = new Date(year,month-1,day,0,0,0)
                            var fromDate = new Date(eventSearchFromDateEditor.year,
                                                  eventSearchFromDateEditor.month-1,
                                                  eventSearchFromDateEditor.day,
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
                                eventSearchFromDateEditor.setDate(toDate)
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

                                var fromDate = new Date(eventSearchFromDateEditor.year,
                                                        eventSearchFromDateEditor.month-1,
                                                        eventSearchFromDateEditor.day,
                                                        0,0,0)
                                var toDate = new Date(eventSearchToDateEditor.year,
                                                      eventSearchToDateEditor.month-1,
                                                      eventSearchToDateEditor.day,
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
            width: eventSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        Rectangle{
            id: eventTypeRectangle
            width: eventSearchFilterView.width
            height: 174
            color: WisenetGui.transparent

            Text{
                id: eventTypeText
                width: parent.width - 48
                leftPadding: 24
                topPadding: 12
                elide: Text.ElideRight
                color: WisenetGui.contrast_04_light_grey
                text: WisenetLinguist.eventType
                font.pixelSize: 12
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    eventSearchFilterTreeSourceModel.updateAllEventData()
                    eventTypeViewPopup.open()
                }
            }

            Rectangle{
                id: eventList
                anchors.fill: parent
                anchors.topMargin: 42
                anchors.leftMargin: 24
                anchors.rightMargin: 24
                anchors.bottomMargin: 12
                color: WisenetGui.contrast_09_dark

                ListModel{
                    id: eventTypeModel
                }

                Text{
                    visible: eventTypeListView.model.count === 0 ? true : false
                    text: WisenetLinguist.all
                    color: WisenetGui.contrast_02_light_grey
                    font.pixelSize: 12
                }

                ListView{
                    id: eventTypeListView
                    visible: eventSearchFilterTree.allEvent === false ? true : false
                    anchors.fill: parent
                    spacing: 12
                    model: eventTypeModel

                    ScrollBar.vertical: WisenetScrollBar {
                        id: verticalScrollbar
                        width: 15
                    }

                    delegate: Rectangle{
                        width: parent ? parent.width : 0
                        height: 32
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_09_dark

                        Image{
                            id: eventTypeDelegateImage
                            x: 8
                            width: 16
                            height: 16
                            anchors.verticalCenter: parent.verticalCenter
                            sourceSize: Qt.size(width, height)
                            source: eventSearchFilterTree.getResourceIcon(uuid)
                        }

                        Text {
                            id: eventTypeDelegateText
                            anchors.left: eventTypeDelegateImage.right
                            anchors.leftMargin: 8
                            anchors.right: eventTypeCloseImage.left
                            anchors.rightMargin: 14
                            anchors.verticalCenter: parent.verticalCenter
                            verticalAlignment: Text.AlignVCenter

                            text: eventSearchFilterTree.getDisplayName(uuid)
                            color: WisenetGui.contrast_02_light_grey
                            font.pixelSize: 12
                            elide: Text.ElideRight
                        }

                        Image{
                            id: eventTypeCloseImage
                            anchors.right: parent.right
                            anchors.rightMargin: 14
                            width: 10
                            height: 10
                            anchors.verticalCenter: parent.verticalCenter
                            sourceSize: Qt.size(width, height)
                            source: "qrc:/WisenetStyle/Icon/eventsearch-eventtype-close-normal.svg"

                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    eventSearchFilterTree.unCheckItem(uuid)
                                    eventTypeModel.remove(index,1)
                                }
                            }
                        }
                    }

                    onContentHeightChanged: {
                        // 최대 크기는 77
                        if(contentHeight < 100){
                            eventTypeRectangle.height = 174
                        }else{
                            var maximumEventTypeRactangleHeight = eventSearchFilterView.height - deviceRectangle.height - dateTimeRectangle.height - searchRectangle.height - 84
                            console.debug("ContentHeightChanged : " + contentHeight + ", maximumHeight : " + maximumEventTypeRactangleHeight)
                            if(maximumEventTypeRactangleHeight > (contentHeight) ){
                                eventTypeRectangle.height = contentHeight + 56
                            }
                        }
                    }
                }
            }


        }

        Rectangle{
            width: eventSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        Rectangle{
            id: searchRectangle
            width: eventSearchFilterView.width
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
                    eventSearchFilterView.reset();
                }
            }

        }

    }


    Rectangle{
        id: eventSearchFilterBorder
        x: eventSearchFilterView.width - 1
        y: 0
        width: 1
        height: eventSearchFilterView.height
        color: WisenetGui.contrast_08_dark_grey
    }

    //장치선택뷰
    Popup {
        id: deviceViewPopup
        x: eventSearchFilterView.width
        y: deviceRectangle.y
        width: 300
        height: 580
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: eventSearchDeviceTreeProxyModel
            sourceModel: DeviceSelectionTreeSourceModel{
                id: eventSearchDeviceTreeSourceModel

                onInvalidate: eventSearchDeviceTreeProxyModel.invalidate()
            }

            Component.onCompleted: {
                doSort(Qt.AscendingOrder)
            }
        }

        contentItem: WisenetDeviceSelectionTree{
            id: deviceSeletectionTree
            anchors.fill: parent
            treeModel: eventSearchDeviceTreeProxyModel

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
        x: eventSearchFilterView.width
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
                eventSearchFromDateEditor.setDate(calendarControlView.calendarTimelineModel.selectionStartDate)
                eventSearchToDateEditor.setDate(calendarControlView.calendarTimelineModel.selectionEndDate)
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
        x: eventSearchFilterView.width
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

