import QtQuick 2.15
import QtQuick.Controls 2.5
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Qt.labs.qmlmodels 1.0
import Wisenet.Setup 1.0
import "qrc:/WisenetStyle/"

Page{
    id: root

    property int xMargin : 15
    property int yMargin: 20
    visible: true

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    property int currentRowNum : -1

    function reset(){
        console.log("[ScheduleView] reset")
        eventScheduleViewModel.reset()
    }

    function allUncheckedDisplay(){
        eventScheduleTable.allCheckedStatus = false
    }

    function openAddSchedule(){
        openAddSchedulePopup.scheduleGuid=""
        openAddSchedulePopup.visible = true
        root.allUncheckedDisplay()
    }

    onVisibleChanged: {
        if(visible==true) {
            console.log("[ScheduleView] onVisibleChanged reset()")
            if(eventScheduleViewModel.highlightRow == -1) { // ScheduleView가 처음으로 실행됐을 때
                console.log("[ScheduleView] onVisibleChanged reset() for the first time")
                reset()
                eventScheduleViewModel.setHighlightRow(0)
                scheduleTable.showSchedule(eventScheduleViewModel.scheduleTable)
            }
        }
    }

    onCurrentRowNumChanged: {
        if (currentRowNum == 0) {
            disableRect.visible = true
            tableRect.enabled = false
        }
        else {
            disableRect.visible = false
            tableRect.enabled = true
        }
    }

    WisenetMessageDialog {
        id: enterNameWarning
        visible: false

        applyButtonVisible: false
        message: WisenetLinguist.enterNameMessage

        onCancelButtonClicked:
            enterNameWarning.close()
    }

    WisenetMessageDialog{
        id: sameNameScheduleDialog
        visible: false

        applyButtonVisible: false
        message: WisenetLinguist.sameNameScheduleExists

        onCancelButtonClicked: {
            sameNameScheduleDialog.close()
        }
    }

    WisenetMessageDialog{
        id: usedScheduleDialog
        visible: false

        applyButtonVisible: false
        message: WisenetLinguist.eventScheduleDeleteErrorMessage

        onCancelButtonClicked: {
            usedScheduleDialog.close()
        }
    }

    WisenetPopupWindowBase {
        id: openAddSchedulePopup
        visible: false

        width: WisenetGui.addEventScheduleWindowDefaultWidth
        height: WisenetGui.addEventScheduleWindowDefaultHeight

        title: WisenetLinguist.addEventSchedule
        minmaxButtonVisible: false

        minimumWidth: 450
        minimumHeight: 200
        applyButtonText: WisenetLinguist.ok
        property string scheduleGuid: ""

        onVisibleChanged:{
            if(visible==false)
            {
                title = WisenetLinguist.addEventSchedule
                scheduleGuid = ""
            }
            else
            {
                eventScheduleViewModel.readSchedule(scheduleGuid)

                if(scheduleGuid && (scheduleGuid.length > 0)){ // 이름 편집
                    title = WisenetLinguist.editEventSchedule
                    scheduleGuid = eventScheduleViewModel.getScheduleId(currentRowNum)
                    scheduleNameEntry.text = eventScheduleViewModel.getScheduleName(currentRowNum)
                }
                else{ // 새로운 스케줄 추가
                    scheduleNameEntry.text= ""
                    scheduleGuid = ""
                }
            }
        }

        onApplyButtonClicked:{
            console.log("[ScheduleView] openAddSchedulePopup onApplyButtonClicked scheduleName.text: ", scheduleNameEntry.text)

            // 공백으로만 이루어진 이름일 경우 리턴한다
            var str = scheduleNameEntry.text
            if (str.replace(/\s/g, '').length == 0){
                enterNameWarning.show()
                return
            }

            if(scheduleGuid && (scheduleGuid.length > 0)) { // schedule 이름 edit하는 경우
                eventScheduleViewModel.setScheduleName(scheduleNameEntry.text)
                eventScheduleViewModel.saveSchedule()
            }
            else {// 새로운 스케줄 추가
                eventScheduleViewModel.addSchedule(scheduleNameEntry.text)
                scheduleTable.showSchedule(eventScheduleViewModel.scheduleTable)
            }

            scheduleNameEntry.text=""
            openAddSchedulePopup.close()
        }

        WisenetTextBox{
            id: scheduleNameEntry
            anchors.centerIn: parent
            text: ""
            placeholderText: WisenetLinguist.scheduleName
            focus: true
        }
    }

    EventScheduleViewModel {
        id: eventScheduleViewModel

        onResultMessage:{ //DB 관련된 요청이 성공/실패했을 때 메세지를 띄우는 시그널을 모델에서 발생시킴
            if(msg == "Exist schedule name")
                sameNameScheduleDialog.visible = true
            else if(msg == "Used Schedule")
                usedScheduleDialog.visible = true
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }

        onDeleted:{ // 선택된 데이터 삭제되면 첫번째 데이터 하이라이트 후 스케줄 표 내용 보여줌
            console.log("ScheduleView::deleted signal from EventScheduleViewModel")
            eventScheduleViewModel.setHighlightRow(0);
            scheduleTable.showSchedule(eventScheduleViewModel.scheduleTable);
        }

        onExist:{ // 데이터가가 편집되거나 추가되는 데이터에 하이라이트되도록
            console.log("ScheduleView::exist signal from EventScheduleViewModel, now index is ", index)
            eventScheduleViewModel.setHighlightRow(index);
        }

        onHighlightRowChanged: {
            console.log("ScheduleView::highlightRowChanged signal from EventScheduleViewModel, now highlightRow is ", eventScheduleViewModel.highlightRow)
            currentRowNum = eventScheduleViewModel.highlightRow
        }
    }

    WisenetSetupApplyResultMessage{
        id: applyResultMessage
        anchors {
            right : divider.right
            rightMargin: 30
            verticalCenter: divider.bottom
        }
    }

    WisenetDivider{
        id : divider

        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 15

        label: WisenetLinguist.eventSchedule
    }


    WisenetSetupAskDialog {
        id: deleteDialog
        msg : WisenetLinguist.deleteEventScheduleConfirmMessage
        onAccepted: {
            eventScheduleViewModel.deleteSchedule()
            root.allUncheckedDisplay()
        }
        onRejected: {
            dontAskAgain = false
        }
    }

    Rectangle {
        id: buttonRect

        anchors.left: divider.left
        anchors.top: divider.bottom
        anchors.topMargin: 20

        color: "transparent"

        height: addScheduleButton.height
        width: 280

        WisenetSetupAddButton{
            id: addScheduleButton
            anchors.left: parent.left
            anchors.top: parent.top
            text : WisenetLinguist.add

            onClicked: {
                openAddSchedule()
            }
        }

        WisenetSetupDeleteButton{
            id : deleteScheduleButton

            anchors.left: addScheduleButton.right

            enabled: (eventScheduleViewModel.selectedRowCount > 0 )
            text : WisenetLinguist.deleteText
            onClicked: {
                if (!deleteDialog.dontAskAgain) {
                    deleteDialog.open();
                }
                else {
                    eventScheduleViewModel.deleteSchedule()
                    root.allUncheckedDisplay()
                }
            }
        }
    }

    ListModel{ // sort랑 splitter는 어떻게 쓰이는지
        id: eventScheduleColumns
        Component.onCompleted: {
            append({ "columnWidth": 0, "title": qsTr("ID"), "sort": 3, "splitter": 0})
            append({ "columnWidth": 38, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({ "columnWidth": listRect.width - 38, "title": WisenetLinguist.name, "sort": 1, "splitter": 0})
            append({ "columnWidth": 0, "title": "scheduleTable", "sort": 1, "splitter": 0})
        }
    }

    DelegateChooser {
        id: eventScheduleChooser
        DelegateChoice {
            id: checkDel
            column: 1
            delegate: WisenetSetupTableCheckRectangle{
                checkedValue: display
                onContainsMouseChanged:{
                    eventScheduleViewModel.setHoveredRow(row, containsMouse);
                }
                onClicked:{
                    eventScheduleViewModel.setCheckState(row, checkedValue)
                    eventScheduleViewModel.setHighlightRow(row);
                    scheduleTable.showSchedule(eventScheduleViewModel.scheduleTable)
                    if(display === false)
                        root.allUncheckedDisplay()
                }
            }
        }
        DelegateChoice{
            id: nameDel
            // column: 2
            delegate: WisenetSetupTableImageRect{
                text: display
                defaultImage:WisenetImage.calendar_Default
                hoveredImage:WisenetImage.calendar_Hover

                onPressed:{
                    eventScheduleViewModel.setHighlightRow(row);
                    currentRowNum = row;

                    var scheduleTableInfo = eventScheduleViewModel.scheduleTable;
                    scheduleTable.showSchedule(scheduleTableInfo);
                }
                onDoubleClicked:{
                    if(eventScheduleViewModel.getScheduleName(row) === "Always")
                        return;
                    openAddSchedulePopup.scheduleGuid = eventScheduleViewModel.getScheduleId(row)
                    openAddSchedulePopup.visible = true
                }
                onContainsMouseChanged:{
                    eventScheduleViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }
    }

    Rectangle {
        id: listRect

        anchors.left: divider.left
        anchors.top: buttonRect.bottom
        anchors.topMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        width: 230

        color: "transparent"

        WisenetTable {
            id: eventScheduleTable
            anchors.fill: listRect
            columns: eventScheduleColumns
            cellDelegate: eventScheduleChooser
            tableModel: eventScheduleViewModel
            contentColor: "transparent"

            onAllChecked:{
                console.log("[ScheduleView] All Schedule data checked", checked)
                eventScheduleViewModel.setAllCheckState(checked)
            }
        }
    }

    Rectangle {
        id: tableRect

        anchors.left: listRect.right
        anchors.leftMargin: 10
        anchors.top: listRect.top
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        color: WisenetGui.contrast_10_dark

        ScheduleTable {
            id: scheduleTable
            anchors.left: tableRect.left
        }

        Rectangle {
            id: disableRect
            anchors.fill: scheduleTable
            color: WisenetGui.contrast_10_dark
            opacity: 0.5
        }

        WisenetGrayButton {
            id: applyButton
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: tableRect.bottom
            anchors.bottomMargin: 10
            text: WisenetLinguist.apply
            onClicked: {
                    scheduleTable.applySchedule(currentRowNum)
            }
        }
    }
}
