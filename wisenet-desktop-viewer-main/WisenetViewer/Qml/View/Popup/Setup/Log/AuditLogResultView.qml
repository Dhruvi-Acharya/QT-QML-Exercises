import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0
import Wisenet.AuditLogViewModel 1.0
import Wisenet.UserFilterTreeSourceModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import Wisenet.DeviceFilterTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import WisenetLanguage 1.0

import "qrc:/"
import WisenetMediaFramework 1.0

Rectangle{
    id: auditLogResultView
    visible: true
    color: backgroundColor

    property color backgroundColor: WisenetGui.contrast_09_dark
    property color tableBorderColor: WisenetGui.contrast_07_grey
    property color tableHeaderBackgroundColor: WisenetGui.contrast_08_dark_grey
    property color tableLowBorderColor: WisenetGui.contrast_08_dark_grey

    property var tableModel: auditLogViewModel

    function search(from, to){
        auditLogViewModel.search(from, to)
        auditLogResultTable.initializeSortState(0)
    }

    function useExportButton(visible){
        exportLogButton.visible = visible
    }

    signal exportButtonClicked()

    //Table 모델 및 Delegate
    ListModel{
        id: auditLogColumns
        //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
        //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
        Component.onCompleted: {
            append({ "columnWidth": 200, "title": WisenetLinguist.dateAndTime, "sort": 1, "splitter": 1, "filter":0})
            append({ "columnWidth": 100, "title": WisenetLinguist.user, "sort": 0, "splitter": 1, "filter":1})
            append({ "columnWidth": 150, "title": WisenetLinguist.deviceName, "sort": 0, "splitter": 1, "filter":1})
            append({ "columnWidth": 120, "title": WisenetLinguist.logType, "sort": 0, "splitter": 1, "filter":1})
            append({ "columnWidth": 500, "title": WisenetLinguist.description, "sort": 3, "splitter": 1, "filter":2})
        }
    }

    DelegateChooser {
        id: auditLogChooser
        DelegateChoice{
            column: 4
            delegate: Rectangle {
                implicitHeight: 30
                color: hoveredRole ? WisenetGui.contrast_08_dark_grey : backgroundColor

                MouseArea {
                  hoverEnabled: true
                  anchors.fill: parent
                  onContainsMouseChanged: {
                      auditLogViewModel.setHoveredRow(row, containsMouse);
                  }
                }

                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    text: displayRole
                    color: WisenetGui.contrast_04_light_grey
                    font.pixelSize: 12
                }

                Rectangle{
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    height: 1
                    width: parent.width
                    color: tableLowBorderColor
                }
            }
        }
        DelegateChoice{
            delegate: Rectangle {
                implicitHeight: 30
                color: hoveredRole ? WisenetGui.contrast_08_dark_grey : backgroundColor

                MouseArea {
                  hoverEnabled: true
                  anchors.fill: parent
                  onContainsMouseChanged: {
                      auditLogViewModel.setHoveredRow(row, containsMouse);
                  }
                }

                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    text: displayRole
                    color: WisenetGui.contrast_04_light_grey
                    font.pixelSize: 12
                }

                Rectangle{
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    height: 1
                    width: parent.width
                    color: tableLowBorderColor
                }
            }
        }

    }

    AuditLogViewModel {
        id: auditLogViewModel

        onSearchStarted: {
            console.debug("searchStarted")
            resultTimer.restart();
        }

        onSearchFinished: {
            //userList
            userFilterTreeSourceModel.setUserData(userList)
            deviceFilterTreeSourceModel.setDevices(cameraList)
            auditLogFilterTreeSourceModel.setEventData(logList)
            userFilterTree.reset()
            deviceFilterTree.reset()
            auditLogFilterTree.reset()
            descriptionFilterSearchBox.reset()

            resultTimer.stop()
            pulseView.close()
        }
    }

    // 200ms 동안 결과를 못받은 경우, timer 구동
    Timer {
        id: resultTimer
        interval: 200
        repeat: false
        onTriggered: {
            pulseView.open();
        }
    }

    WisenetTable {
        id: auditLogResultTable
        anchors.fill: parent
        anchors.bottomMargin: 77

        columns: auditLogColumns
        cellDelegate: auditLogChooser
        tableModel : auditLogViewModel
        lastSortIndex : 0

        borderColor: tableBorderColor
        headerBackgroundColor: tableHeaderBackgroundColor

        onFilterImageClicked: {
            if(1 === index){
                userFilterView.x = x
                userFilterView.y = y
                userFilterView.open()
            }else if(2 === index){
                deviceFilterView.x = x
                deviceFilterView.y = y
                deviceFilterView.open()
            }else if(3 === index){
                auditLogFilterView.x = x
                auditLogFilterView.y = y
                auditLogFilterView.open()
            }else if(4 === index){
                descriptionFilterView.x = x
                descriptionFilterView.y = y
                descriptionFilterView.open()
            }
        }
    }

    Rectangle {
        id: totalresult
        anchors.left: parent.left
        anchors.top: auditLogResultTable.bottom
        anchors.topMargin: 10
        height: 16
        width: parent.width/3
        color: WisenetGui.transparent

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            text: auditLogViewModel.totalCount.toLocaleString(Qt.locale('en_US'),'f',0) + " " + WisenetLinguist.results
            color: WisenetGui.contrast_04_light_grey
        }
    }

    WisenetImageAndTextButton{
        id: exportLogButton
        anchors.right: parent.right
        anchors.top: auditLogResultTable.bottom
        anchors.topMargin: 10
        height: 14
        width: 60
        visible: false

        onButtonClicked: {
            exportButtonClicked()
        }
    }

    function filterAll(){
        var selectedUsers = [];
        if(userFilterTree.selectedUserValue !== null){
            userFilterTree.selectedUserValue.forEach(function(item){
                selectedUsers.push(item.uuid)
            })
        }

        var selectedChannels = [];
        if(deviceFilterTree.selectedDeviceValue !== null){
            deviceFilterTree.selectedDeviceValue.forEach(function(item){
                selectedChannels.push(item.uuid)
            })
        }

        var selectedEvents = [];
        if(auditLogFilterTree.selectedEventValue !== null){
            auditLogFilterTree.selectedEventValue.forEach(function(item){
                selectedEvents.push(item.uuid)
            })
        }

        auditLogViewModel.filter(selectedUsers, selectedChannels,selectedEvents,descriptionFilterSearchBox.lastDisplayText)
    }

    //검색 중 표시
    Popup {
        id: pulseView

        width: 160
        height: 160
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose

        background: Rectangle{
            color: WisenetGui.transparent
        }

        /*contentItem: PurseLoader {
            id: processingIndicator
            barCount: 5
            visible:true
            running:true
            anchors.centerIn: parent
            property real maxWidth: 72
            property real calcWidth: visible?parent.width/8:0
            width: visible?parent.width:0
            height: visible?parent.width*9/16:0
            spacing: width/12
        }*/
        contentItem: WisenetMediaBusyIndicator {
            id: processingIndicator
            visible:true
            running:true
            anchors.centerIn: parent
            width: 160
            height: 160
        }
    }

    //사용자 필터
    Popup {
        id: userFilterView
        x: 0
        y: 0
        width: 300
        height: 500
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: userFilterTreeProxyModel
            sourceModel: UserFilterTreeSourceModel{
                id: userFilterTreeSourceModel
            }
        }

        contentItem: WisenetUserFilterTree{
            id: userFilterTree
            anchors.fill: parent
            treeModel: userFilterTreeProxyModel
            visibleImage: false
            onSelectedUserChanged: {
                console.debug("AuditLog table user filter onSelectedEventChanged .....")
                auditLogResultView.filterAll()
            }
        }

        onOpenedChanged: {
            if(opened){
                userFilterTree.expandAll()
            }
        }

    }

    //장치 필터
    Popup {
        id: deviceFilterView
        x: 0
        y: 0
        width: 300
        height: 500
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: deviceFilterDeviceTreeProxyModel
            sourceModel: DeviceFilterTreeSourceModel{
                id: deviceFilterTreeSourceModel
            }

            Component.onCompleted: {
                deviceFilterTreeSourceModel.setSelectableDeviceItem(true)
                doSort(Qt.AscendingOrder)
            }

        }

        contentItem: WisenetDeviceSelectionTree{
            id: deviceFilterTree
            anchors.fill: parent
            treeModel: deviceFilterDeviceTreeProxyModel
            onSelectedDeviceChanged: {
                console.debug("AuditLog table filter onSelectedDeviceChanged")
                auditLogResultView.filterAll()
            }
        }

        onOpenedChanged: {
            if(opened){
                deviceFilterTree.expandAll()
            }
        }

    }

    //이벤트 필터
    Popup {
        id: auditLogFilterView
        x: 0
        y: 0
        width: 300
        height: 500
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: auditLogFilterTreeProxyModel
            sourceModel: EventFilterTreeSourceModel{
                id: auditLogFilterTreeSourceModel
            }
        }


        contentItem: WisenetEventFilterTree{
            id: auditLogFilterTree
            anchors.fill: parent
            treeModel: auditLogFilterTreeProxyModel
            visibleImage: false
            onSelectedEventChanged: {
                console.debug("AuditLog table filter onSelectedEventChanged .....")
                auditLogResultView.filterAll()
            }
        }

        onOpenedChanged: {
            if(opened){
                auditLogFilterTree.expandAll()
            }
        }
    }

    //Description 필터
    Popup {
        id: descriptionFilterView
        x: 0
        y: 0
        width: 255
        height: 31
        modal: true
        focus: true

        contentItem: WisenetSearchFilter{
            id: descriptionFilterSearchBox
            focus: true
            anchors.fill: parent

            onSearchFilterTextChanged: {
                console.debug("auditLog table filter descriptionFilter Search Text: " + text)
                auditLogResultView.filterAll()
            }

        }

    }
}
