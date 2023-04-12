import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Setup 1.0

WisenetPopupWindowBase {
    id: root
    visible: false

    title: WisenetLinguist.addEventRule
    minmaxButtonVisible: false
    minimumWidth: WisenetGui.addEventRuleWindowDefaultWidth
    minimumHeight: WisenetGui.addEventRuleWindowDefaultHeight
    width: WisenetGui.addEventRuleWindowDefaultWidth
    height: WisenetGui.addEventRuleWindowDefaultHeight
    applyButtonText: WisenetLinguist.ok
    property alias addEventRuleViewModelAlias : addEventRuleViewModel

    function readRule(){
        addEventRuleViewModel.readRule(eventGuid);
        eventRuleScheduleView.comboBoxModel = addEventRuleViewModel.scheduleNameList();

        if(eventGuid && (eventGuid.length > 0)){
            eventTriggerView.prevItemAll = eventTriggerView.loadPreviousAllCheck()
            eventTriggerView.prevItemGuidList = eventTriggerView.loadPreviousCheck()

            eventActionView.prevItemGuidList = eventActionView.loadPreviousCheck()
            console.log("[AddEventRuleView] readRule() prev")
            eventActionView.prevAlarmoutDuration = eventActionView.loadPreviousAlarmoutDuration()
            console.log("[AddEventRuleView] readRule() next")

            eventRuleScheduleView.currentComboIndex = eventRuleScheduleView.comboBox.indexOfValue(addEventRuleViewModel.getSchedule())
        }
        else{
            eventTriggerView.prevItemGuidList = eventActionView.prevItemGuidList = null
            eventTriggerView.prevItemAll = null
            eventActionView.prevAlarmoutDuration = 0

            eventGuid = ""
            eventRuleScheduleView.currentComboIndex = eventRuleScheduleView.comboBox.indexOfValue("Always")
        }
    }


    property string eventGuid : ""

    onVisibleChanged: {
        if(visible == false){
            eventGuid = ""
        }
        if(visible)
            readRule()
        console.log("[AddEventRuleView] onVisibleChanged eventGuid:" + eventGuid)
        root.title = ((eventGuid && eventGuid.length >0 )? WisenetLinguist.editEventRule : WisenetLinguist.addEventRule)
        eventTriggerView.visible =eventActionView.visible = visible
    }

    onApplyButtonClicked:{
        var str = ruleName.text
        if(str.replace(/\s/g, '').length === 0){
            msgDialog.message = WisenetLinguist.enterTheRuleName
            msgDialog.visible = true
        }
        else if(addEventRuleViewModel.addedEventList.length ===0){
            msgDialog.message = WisenetLinguist.addAnEventTrigger
            msgDialog.visible = true
        }
        else{
            addEventRuleViewModel.clearEventTree();
            addEventRuleViewModel.ruleName = ruleName.text
            addEventRuleViewModel.duration = addEventRuleDurationView.duration
            addEventRuleViewModel.schedule = eventRuleScheduleView.currentSchedule

            var actionList = eventActionView.getPreviousCheck()
            var triggerList = eventTriggerView.getPreviousCheck()

            var triggerAll = eventTriggerView.getPreviousAllCheck()

            var alarmOutDuration = eventActionView.getPreviousAlarmOutDuration()

            console.log("[AddEventRuleView] onApplyButtonClicked" + alarmOutDuration)


            for (var [key, value] of actionList) {                
                console.log("[AddEventRuleView] actionList " + "key: " + key + " value: " + value)

                addEventRuleViewModel.appendEventTree(false, key, false , value)
            }

            for (var [key, value] of triggerList) {
                console.log("[AddEventRuleView] triggerList " + "key: " + key + " value: " + value)

                addEventRuleViewModel.appendEventTree(true, key, triggerAll.get(key), value)
            }

            addEventRuleViewModel.saveRule(alarmOutDuration)
            root.close()
        }
    }

    AddEventRuleViewModel{
        id : addEventRuleViewModel
    }

    WisenetMessageDialog{
        id: msgDialog
        message: WisenetLinguist.enterTheRuleName
        cancelButtonText: WisenetLinguist.close
        applyButtonVisible: false
    }


    Column{
        id: mainLayout
        property int layoutMargin: 10
        anchors.fill: parent
        anchors.margins: layoutMargin
        spacing: 20
        topPadding: 20
        bottomPadding: 20
        leftPadding: 50
        rightPadding: 50

        WisenetTextBox{
            id : ruleName
            text: addEventRuleViewModel.ruleName
            placeholderText: WisenetLinguist.ruleName
            focus: true
        }

        Rectangle{
            id : mainRect
            width: parent.width - parent.leftPadding - parent.rightPadding
            height: parent.height - ruleName.height - parent.topPadding - parent.bottomPadding
            color: WisenetGui.transparent

            Row{
                id : rowId
                anchors.fill: parent
                spacing: 20

                Rectangle{
                    width : (parent.width/2) - rowId.spacing
                    height: parent.height
                    color: WisenetGui.transparent

                    Column{
                        anchors.fill: parent
                        spacing: 10
                        Rectangle{
                            width: parent.width
                            height: 40
                            color: WisenetGui.transparent
                            WisenetText{
                                anchors.fill: parent
                                text: WisenetLinguist.eventTrigger
                            }
                        }

                        Rectangle{
                            width: parent.width
                            height: parent.height - 100
                            color: WisenetGui.transparent
                            AddEventView{
                                id : eventTriggerView
                                anchors.fill: parent
                                visible: false

                            }
                        }
                        Rectangle {
                            width: parent.width
                            height:40
                            color: WisenetGui.transparent
                            AddEventRuleDurationView{
                                id : addEventRuleDurationView
                                anchors.fill: parent

                                isEnable : addEventRuleViewModel.isAvailableDuration
                                duration : addEventRuleViewModel.duration
                                isAlarmDurationView: false
                                sliderFrom: 3
                                sliderTo: 5
                                comboBoxModel: ["3 " + WisenetLinguist.sec,
                                    "4 " + WisenetLinguist.sec,
                                    "5 " + WisenetLinguist.sec]

                            }
                        }
                    }
                }

                Rectangle {
                    width: 1
                    height: parent.height
                    color:  WisenetGui.setupPageLine
                }

                Rectangle{
                    width : (parent.width/2) - rowId.spacing
                    height: parent.height
                    color: WisenetGui.transparent

                    Column{
                        anchors.fill: parent
                        spacing: 10
                        Rectangle{
                            width: parent.width
                            height: 40
                            color: WisenetGui.transparent
                            WisenetText{
                                anchors.fill: parent
                                text: WisenetLinguist.eventAction

                            }
                        }
                        Rectangle{
                            width: parent.width
                            height: parent.height - 100
                            color: WisenetGui.transparent
                            AddEventView{
                                id : eventActionView
                                isTrigger : false
                                anchors.fill: parent
                                visible: false
                            }
                        }

                        Rectangle {
                            width: parent.width
                            height: 40
                            color: WisenetGui.transparent
                            AddEventRuleScheduleView{
                                id: eventRuleScheduleView
                                anchors.fill: parent

                                //isEnable: addEventRuleViewModel. // 언제 가능하게 할지?
                            }
                        }
                    }
                }
            }
        }
    }
}
