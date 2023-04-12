import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import WisenetStyle 1.0
import QtQuick.Dialogs 1.3
import WisenetLanguage 1.0
import Wisenet.Setup 1.0
import Wisenet.Define 1.0
import "qrc:/"

Page {
    id: root
    bottomPadding: 10
    anchors.fill: parent
    implicitWidth: WisenetGui.popupSecondWindowDefaultWidth
    implicitHeight: WisenetGui.popupSecondWindowDefaultHeight
    onVisibleChanged: if(visible) reset()
    onCancelDeviceMainteanceFirmwareUpdate :reset()
    background: Rectangle{
        anchors.fill: parent
        color: "transparent"
    }

    property bool isFirmwareView : true
    property color backgroundColor: WisenetGui.color_setup_Background
    property string nameFilter :  WisenetLinguist.uploadFiles + " (*.img)"
    property int defaultCellHeight: 36


    signal cancelSetupQuit()
    signal cancelDeviceMainteanceFirmwareUpdate()
    signal cancelDeviceMainteanceConfigRestore()
    signal cancelDeviceMainteanceConfigBackup()
    signal cancelDeviceMainteanceChangePassword()
    signal cancelSubTree(var index)
    signal cancelSubTreeReject()

    function reset(){

        if(isFirmwareView) {
            deviceConfigViewModel.reset(DeviceConfigViewModel.FirmwareUpdateViewType)
        }
        else {
            deviceConfigViewModel.reset(DeviceConfigViewModel.ConfigRestoreViewType)
        }

        nameFilter = WisenetLinguist.uploadFiles + (isFirmwareView?" (*.img)":" (*.bin *.dat)")
        console.log("device firmware update reset - " + nameFilter)
    }
    function allUncheckedDisplay(){
        deviceListTable.allCheckedStatus = false
    }

    function cancelEmit(index){
        switch(index){
        case WisenetSetupTree.SubPageIndex.SetupQuit : cancelSetupQuit(); break;
        case WisenetSetupTree.SubPageIndex.DeviceMainteanceFirmwareUpdate : cancelDeviceMainteanceFirmwareUpdate(); break;
        case WisenetSetupTree.SubPageIndex.DeviceMainteanceConfigRestore : cancelDeviceMainteanceConfigRestore(); break;
        case WisenetSetupTree.SubPageIndex.DeviceMainteanceConfigBackup : cancelDeviceMainteanceConfigBackup(); break;
        case WisenetSetupTree.SubPageIndex.DeviceMainteanceChangePassword : cancelDeviceMainteanceChangePassword(); break;
        default : cancelSubTree(index); break;

        }
    }


    function askCancelFirmwareUpdate(index){
        if(deviceConfigViewModel.isFirmwareUpdating){
            cancelDialog.cancelIndex = index
            cancelDialog.open()
        }
        else{
            cancelEmit(index)
        }
    }

    Connections{
        target: deviceListTable
        onAllChecked:{
            console.log("all checked", checked)
            deviceConfigViewModel.setAllCheckState(checked)
        }
    }

    ListModel{
        id: deviceListColumns
        //Sort 0은 정렬 안함. 1은 Descending, 2는 Ascending, 3은 사용 안함
        //Spliter 0은 컬럼 크기 조정 안함, 1은 컬럼 크기 조정 함
        Component.onCompleted: {
            append({ "columnWidth": 0, "title": WisenetLinguist.deviceId, "sort": 0, "splitter": 0, "visible":false})
            append({ "columnWidth": 40, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({ "columnWidth": isFirmwareView ? 0 : 85, "title": WisenetLinguist.model, "sort": 0, "splitter": isFirmwareView ? 0 : 1, "visible":isFirmwareView?false:true})
            append({ "columnWidth": isFirmwareView ? 150 : 110, "title": WisenetLinguist.device, "sort": 0, "splitter": 1})
            append({ "columnWidth": 0, "title": WisenetLinguist.chId, "sort": 0, "splitter": 0, "visible":false})
            append({ "columnWidth": 0, "title": "channel model", "sort": 0, "splitter": 0, "visible":false})
            append({ "columnWidth": isFirmwareView ? 150 : 0, "title": WisenetLinguist.channel, "sort": 0, "splitter": 1})
            append({ "columnWidth": 120, "title": WisenetLinguist.ip, "sort": 0, "splitter": 1})
            append({ "columnWidth": 150, "title": WisenetLinguist.currentVersion, "sort": 3, "splitter": 1})
            append({ "columnWidth": isFirmwareView ? 240 : 300, "title": isFirmwareView ? WisenetLinguist.recentVersion : WisenetLinguist.path, "sort": 0, "splitter": 1})
            append({ "columnWidth": 0, "title": "Server update", "sort": 0, "splitter": 0, "visible":false})
            append({ "columnWidth": isFirmwareView ? 120 : 150, "title": WisenetLinguist.status, "sort": 0, "splitter": 1})
            console.log("device firmware update deviceListColumns Component.onCompleted")
        }
    }
    DelegateChooser {
        id: deviceListChooser

        DelegateChoice {
            id : delegateChoiceCheck
            column: 1
            delegate:WisenetSetupTableCheckRectangle{
                implicitWidth: 300
                checkedValue: display
                onContainsMouseChanged: {
                    deviceConfigViewModel.setHoveredRow(row, containsMouse);
                }
                onClicked: {
                    deviceConfigViewModel.setCheckState(row, checkedValue)
                    deviceConfigViewModel.setHighlightRow(row, modifier);
                    if(display === false)
                        root.allUncheckedDisplay()
                }
            }
        }

        DelegateChoice {
            column: 9//isFirmwareView? 9 : 6
            delegate: Rectangle {
                id: delegateBtn
                width: 300
                implicitWidth: 300
                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press : hoveredRole ? WisenetGui.color_primary : "transparent"
                WisenetText{
                    id : pathText
                    width: delegateBtn.width - 20
                    visible: true //(text ===  "-") ? true : false

                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: display===undefined ? "" : display
                    color: paintRole ? WisenetGui.color_secondary : WisenetGui.contrast_04_light_grey
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onContainsMouseChanged: {
                            deviceConfigViewModel.setHoveredRow(row, containsMouse)
                            if(containsMouse) {
                                pathTextTooltip.visible = true
                            }
                            else {
                                pathTextTooltip.visible = false
                            }
                        }
                        onPressed: {
                            deviceConfigViewModel.setHighlightRow(row, mouse.modifiers)
                        }
                    }

                    WisenetMediaToolTip {
                        id: pathTextTooltip
                        text: pathText.text
                        delay: 200
                    }
                }

                function isFileSelectVisible(row){
                    if(pathText.text === "-")
                        return true
                    if(!root.isFirmwareView)
                        return false

                    var isServer = deviceConfigViewModel.isServerUpdateMode(row)
                    console.log("isFileSelectVisible row : ", row, " isServer: ", isServer)
                    if(isServer){
                        return true
                    }
                    else{
                        return false
                    }
                }

                FileSelectButton{
                    id : fileSelectBtn
                    x : pathText.visible ? pathText.width : pathText.width/2
                    //visible: isFileSelectVisible(row)

                    visible: ((pathText.text == "-")? true : (!root.isFirmwareView? false : (serverUpdateRole?true:false)))

                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            fileDialog.visible = true;
                        }
                    }
                }
                FileRemoveButton{
                    x : pathText.visible ? pathText.width : pathText.width/2
                    visible: !fileSelectBtn.visible
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20
                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            deviceConfigViewModel.deletePath(row)
                            if(isFirmwareView) {
                                console.log("removed update file")
                                //deviceConfigViewModel.setServerUpdateMode(row, true)
                                deviceConfigViewModel.fillRecentFwVersion(row)
                            }
                        }
                    }
                }

                WisenetTableCellLine{
                    anchors.bottom: parent.bottom
                }

                FileDialog {
                    id: fileDialog
                    title: "Please choose a file"
                    nameFilters: [ nameFilter, WisenetLinguist.allFiles + " (*)" ]
                    folder: shortcuts.home
                    onAccepted: {
                        console.log("You chose: " + fileDialog.fileUrl + " " + Qt.platform.os.toString())
                        if(isFirmwareView) {
                            console.log("uploaded update file")
                            deviceConfigViewModel.setServerUpdateMode(row, false)
                        }

                        var path = filePathConveter.getPathByUrl(fileDialog.fileUrl)
                        deviceConfigViewModel.setSameFile(sameModelCheckBox.checked, row, path)
                    }
                    onRejected: {
                        console.log("Canceled")
                    }
                }
            }
        }
        DelegateChoice {
            column: 11//isFirmwareView? 10 : 7
            delegate: Rectangle {
                id: delegateStatus
                width: 270
                implicitWidth: 300
                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press : hoveredRole ? WisenetGui.color_primary : "transparent"

                WisenetTableCellText{
                    id : cellText
                    visible: isNaN(display)
                    text: deviceConfigViewModel.translate(display)
                    color: highlightRole ?  WisenetGui.contrast_00_white :hoveredRole ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                }

                SetupProgress{
                    visible: !isNaN(display)
                    value: deviceConfigViewModel.convertType(display)
                }

                WisenetTableCellLine{
                    anchors.bottom: parent.bottom
                }
                MouseArea {
                    anchors.fill : parent
                    hoverEnabled: true
                    onPressed: {
                        deviceConfigViewModel.setHighlightRow(row, modifier);
                    }
                    onContainsMouseChanged: {
                        deviceConfigViewModel.setHoveredRow(row, containsMouse);
                    }
                }
            }
        }

        DelegateChoice {
            id : defaultDele
            delegate: WisenetSetupTableDefaultRect{
                implicitWidth: 300
                text: display === undefined ? "" : display
                onPressed: {
                    deviceConfigViewModel.setHighlightRow(row, modifier);
                }
                onContainsMouseChanged: {
                    deviceConfigViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }
    }

    Rectangle{
        color: WisenetGui.transparent
        anchors.fill: parent
        Column {
            id: mainLayout
            anchors.fill: parent
            spacing: 10
            topPadding: 10

            WisenetCheckBox{
                id : sameModelCheckBox
                text : WisenetLinguist.applyToSameModel
                checked: true
                width: 200
            }

            Rectangle {
                width: parent.width
                height: parent.height -  sameModelCheckBox.height - (mainLayout.spacing * 2) - mainLayout.topPadding

                color: "transparent"
                WisenetTable {
                    id: deviceListTable
                    anchors.fill: parent
                    enabled: !deviceConfigViewModel.isFirmwareUpdating;
                    columns: deviceListColumns
                    cellDelegate: {
                        implicitWidth: 300
                        implicitHeight: 300
                        return deviceListChooser
                    }
                    tableModel : deviceConfigViewModel
                }
            }
        }
    }
    WisenetSetupAskDialog {
        id: cancelDialog
        msg :  WisenetLinguist.warningFirmwareUpdateInProgress + WisenetLinguist.firmwareUpdateCancelMessage
        dontAskVisible : false
        property int cancelIndex: WisenetSetupTree.SubPageIndex.DeviceMainteanceFirmwareUpdate
        onAccepted: {
            console.log("cancelDialog onAccepted")
            deviceConfigViewModel.cancelFirmwareUpdate()
            cancelEmit(cancelIndex)
        }
        onRejected: {
            cancelSubTreeReject()
        }
    }
    footer:
        WisenetDialogButtonBox{
        id: registerButton
        okText: deviceConfigViewModel.isFirmwareUpdating? WisenetLinguist.cancel :
                 (root.isFirmwareView?WisenetLinguist.update:WisenetLinguist.restore)
        onAccepted: {
            if(deviceConfigViewModel.selectedRowCount > 0){
                if(deviceConfigViewModel.isFirmwareUpdating)
                    askCancelFirmwareUpdate(WisenetSetupTree.SubPageIndex.DeviceMainteanceFirmwareUpdate)
                else {
                    if(deviceConfigViewModel.isSetFilePath()){
                        if(isFirmwareView){
                            deviceConfigViewModel.startUpdate(0)
                        }
                        else {
                            if(deviceConfigViewModel.isConfigRestoreRequirePassword())
                                passwordRequired.showDialog()
                            else
                                deviceConfigViewModel.startConfigRestore(true, "")
                        }
                    }
                    else{
                        msgDialog.message = WisenetLinguist.pleaseSelectFilePath
                        msgDialog.show()
                    }
                }
            }
            else{
                msgDialog.message = WisenetLinguist.pleaseSelectDevice
                msgDialog.show()
            }

        }
        /*onRejected: {
                askCancelFirmwareUpdate(WisenetSetupTree.SubPageIndex.DeviceMainteanceFirmwareUpdate)
            }*/
    }
    DeviceConfigPasswordRequired{
        id : passwordRequired

        onPasswordEntered:{
            deviceConfigViewModel.startConfigRestore(true, password.toString())
        }
    }
    WisenetMessageDialog{
        id: msgDialog
        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }
}
