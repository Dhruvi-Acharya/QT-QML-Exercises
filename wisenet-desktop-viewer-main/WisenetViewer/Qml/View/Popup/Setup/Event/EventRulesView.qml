import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Qt.labs.qmlmodels 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Setup 1.0
import "EventRule.js" as EventRule

Page{
    id : root
    property int xMargin : 15
    property int yMargin: 20
    property color backgroundColor: WisenetGui.color_setup_Background
    property int defaultCellHeight: 36

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    function openAddEventRule(){
        addEventRuleView.eventGuid = ""
        addEventRuleView.visible = true
        root.allUncheckedDisplay()
    }

    function reset(){
        console.log("eventRulesView reset")
        eventRulesViewModel.reset()
    }

    function allUncheckedDisplay(){
        eventRulesTable.allCheckedStatus = false
    }

    onVisibleChanged: if(visible == true) reset()

    EventRulesViewModel{
        id : eventRulesViewModel
    }

    Connections{
        target: eventRulesTable
        onAllChecked:{
            console.log("all checked", checked)
            eventRulesViewModel.setAllCheckState(checked)
        }
    }
    Connections{
        target: eventRulesViewModel
        function onResultMessage(success, msg){
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }
    }

    Connections{
        target: addEventRuleView.addEventRuleViewModelAlias
        function onResultMessage(success, msg){
            applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)
        }
    }

    ListModel {
        id: eventRulesColumns

        Component.onCompleted: {
            append({ "columnWidth": 0, "title": qsTr("ID"), "sort": 3, "splitter": 0})
            append({ "columnWidth": 50, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({ "columnWidth": 100, "title": WisenetLinguist.use, "sort": 3, "splitter": 1})
            append({ "columnWidth": 255, "title": WisenetLinguist.name, "sort": 1, "splitter": 1})
            append({ "columnWidth": 280, "title": WisenetLinguist.eventTrigger, "sort": 3, "splitter": 1})
            append({ "columnWidth": 280, "title": WisenetLinguist.eventAction, "sort": 3, "splitter": 1})
        }
    }

    DelegateChooser {
        id: eventRulesChooser
        DelegateChoice {
            id : checkDele
            column: 1
            delegate: WisenetSetupTableCheckRectangle {
                checkedValue: display
                onContainsMouseChanged: {
                    eventRulesViewModel.setHoveredRow(row, containsMouse);
                }
                onClicked: {
                    eventRulesViewModel.setCheckState(row, checkedValue)
                    eventRulesViewModel.setHighlightRow(row, modifier);
                    if(display === false)
                        root.allUncheckedDisplay()
                }
            }
        }

        DelegateChoice {
            column: 2
            delegate: Rectangle {
                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

                WisenetToggleButton{
                    id : useId
                    //text: display
                    checked: display
                    color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"
                    myColor: checked?(highlightRole ?  WisenetGui.color_secondary :hoveredRole ? WisenetGui.color_secondary_bright : WisenetGui.color_secondary) :
                                      (highlightRole ?  WisenetGui.contrast_00_white :hoveredRole ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey)

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    onContainsMouseChanged: {
                        eventRulesViewModel.setHoveredRow(row, containsMouse);
                    }

                    onClicked: {
                        useId.checked = !useId.checked
                        eventRulesViewModel.editRule(row, column, useId.checked)
                    }
                }

                WisenetTableCellLine{
                    anchors.bottom: parent.bottom
                }
            }
        }

        DelegateChoice {
            column: 3
            delegate:WisenetSetupTableDefaultRect{
                text: display
                onPressed: {
                    eventRulesViewModel.setHighlightRow(row, modifier);
                }
                onDoubleClicked: {
                    addEventRuleView.eventGuid = eventRulesViewModel.getRuleId(row)
                    addEventRuleView.visible = true
                }
                onContainsMouseChanged: {
                    eventRulesViewModel.setHoveredRow(row, containsMouse);
                }
            }
        }
        DelegateChoice {
            column: 6
        }
        DelegateChoice {
            delegate: Rectangle {
                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

                Row{
                    spacing: 10
                    anchors{
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    Repeater{
                        model : display

                        Image{
                            id: eventImage
                            width: 16
                            height: 16
                            anchors.verticalCenter: parent.verticalCenter
                            sourceSize: Qt.size(width, height)
                            source: EventRule.getResourceIcon(modelData)
                        }
                    }
                }

                WisenetTableCellLine{
                    anchors.bottom: parent.bottom
                }
                MouseArea {
                    anchors.fill : parent
                    hoverEnabled: true

                    onPressed: {
                        eventRulesViewModel.setHighlightRow(row, mouse.modifiers);
                    }
                    onDoubleClicked: {
                        addEventRuleView.eventGuid = eventRulesViewModel.getRuleId(row)
                        addEventRuleView.visible = true
                    }
                    onContainsMouseChanged: {
                        eventRulesViewModel.setHoveredRow(row, containsMouse);
                    }
                }
            }
        }
    }

    WisenetDivider{
        id : divider
        x : xMargin
        y : 20
        Layout.fillWidth: true
        label: WisenetLinguist.eventRules
    }
    WisenetSetupApplyResultMessage{
        id: applyResultMessage
        anchors {
            right : divider.right
            rightMargin: 30
            verticalCenter: divider.bottom
        }
    }

    Row{
        id: buttonLayout
        Layout.fillWidth: true
        anchors.left: divider.left
        anchors.top: divider.bottom
        anchors.topMargin: 20

        WisenetSetupAddButton{
            text : WisenetLinguist.add
            onClicked: {
                console.log("[EventRulesView] x :" + root.x + " y:" + root.y)
                addEventRuleView.eventGuid = ""
                addEventRuleView.visible = true
                root.allUncheckedDisplay()
            }
        }

        WisenetSetupDeleteButton{
            text : WisenetLinguist.deleteText
            enabled: (eventRulesViewModel.selectedRowCount > 0)
            onClicked: {
                if (!deleteDialog.dontAskAgain) {
                    deleteDialog.open();
                }
                else {
                    eventRulesViewModel.deleteRule()
                    root.allUncheckedDisplay()
                }
            }
        }
    }

    WisenetSetupAskDialog {
        id: deleteDialog
        msg : WisenetLinguist.deleteEventRuleConfirmMessage
        onAccepted: {
            eventRulesViewModel.deleteRule()
            root.allUncheckedDisplay()
        }
        onRejected: {
            dontAskAgain = false
        }
    }
    Rectangle{
        anchors.left: buttonLayout.left
        anchors.top: buttonLayout.bottom
        width: parent.width - (xMargin *2) - 10
        height: parent.height - buttonLayout.y - buttonLayout.height - buttonRect.height
        color: "transparent"

        WisenetTable {
            id: eventRulesTable
            anchors.fill: parent
            columns: eventRulesColumns
            cellDelegate: eventRulesChooser
            tableModel : eventRulesViewModel
            contentColor: "transparent"
        }
    }
    Rectangle{
        id : buttonRect
        width: parent.width
        height: 40
        color: "transparent"
    }
    AddEventRuleView{
        id : addEventRuleView
        visible: false
        width: WisenetGui.addEventRuleWindowDefaultWidth
        height: WisenetGui.addEventRuleWindowDefaultHeight
        onVisibleChanged: {
            if(visible){
                console.log("[EventRulesView] AddEventRuleView x : " + x + " y:" + y +
                            " setup x:" + setupMainId.x +  " y:" +setupMainId.y +
                            " root x:" + root.x +  " y:" +root.y)
            }
        }
    }
}

