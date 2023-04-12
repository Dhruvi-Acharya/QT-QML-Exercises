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
    property int xMargin : 24
    property int yMargin: 20
    property color backgroundColor: WisenetGui.color_setup_Background
    property int defaultCellHeight: 36

    property int eventWidth: 400

    anchors.fill: parent
    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    onVisibleChanged: {
        if(visible===true)
        {
            console.log( "EVENTNOTIFICATIONSVIEW VISIBLE TRUE" )
            showallEventCheckBox.checked = eventNotificationsViewModel.isCheckedShowAll("event")
            showallSystemCheckBox.checked = eventNotificationsViewModel.isCheckedShowAll("system")
            eventsRepeater.model = eventNotificationsViewModel.getEventsList("event") // To load dynamic events
        }
    }

    function gotoPage( pageName ){
        buttonLayout.resetButton();
        if( pageName === "event")
        {
            eventButton.checked=true
        }
        else if( pageName === "system" )
        {
            systemButton.checked=true
        }
        else
        {
            eventButton.checked=true
        }
    }

    AddEventRuleViewModel{
        id : addEventRuleViewModel
    }

    WisenetDivider{
        id : divider
        x : xMargin
        y : 20
        Layout.fillWidth: true
        label: WisenetLinguist.eventNotifications
    }

    Row {
        id: buttonLayout
        Layout.fillWidth: true
        anchors.left: divider.left
        anchors.top: divider.bottom
        anchors.topMargin: 20

        property int textWidth: 130
        property int textHeight: 30

        function resetButton(){
            eventButton.checked = systemButton.checked = false
        }

        WisenetSetupTextButton {
            id : eventButton
            width: buttonLayout.textWidth
            height: buttonLayout.textHeight
            textHAlignment : Text.AlignHCenter
            text: WisenetLinguist.event
            checked: true

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    gotoPage( "event" )
                }
            }
        }
        //SetupTabLine{ y : 10}
        Rectangle{
            width: 1
            height: 15
            y: 10

            color: WisenetGui.contrast_08_dark_grey
        }

        WisenetSetupTextButton {
            id : systemButton
            leftPadding:10
            width: buttonLayout.textWidth
            height: buttonLayout.textHeight
            textHAlignment : Text.AlignHCenter
            text: WisenetLinguist.system
            checked: false
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    gotoPage( "system" )
                }
            }
        }
    }
    EventNotificationsViewModel{
        id: eventNotificationsViewModel
    }

    function saveSetting(){
        console.log( "saveEventsList() +++")
        eventNotificationsViewModel.saveSettings( showallEventCheckBox.checked, showallSystemCheckBox.checked );
        console.log( "saveEventsList() ---")
    }

    Rectangle{
        anchors.left: buttonLayout.left
        anchors.top: buttonLayout.bottom
        width: parent.width - (xMargin *2) - 10
        height: (((parent.height - buttonLayout.y) - buttonLayout.height) - root.footer.height) - 20
        color: "transparent"

        Rectangle{
            id:pageBG
            color:"transparent"
            x:0
            y:15
            width:parent.width-x
            height:parent.height-y

            Rectangle {
                id : eventPage
                width:parent.width
                height:parent.height
                x:0
                y:0
                visible: eventButton.checked
                color:"transparent"
                Column{
                width:parent.width
                height:parent.height
                    spacing:15
                    WisenetCheckBox{
                        id : showallEventCheckBox
                        text : WisenetLinguist.showAllNotifications
                        checked: true
                        //checked: eventNotificationsViewModel.isCheckedShowAll("event")
                        width: 200
                        signal checkAll(bool isCheck);
                        onClicked: checkAll( checked );
                    }
                    Rectangle{
                        color: WisenetGui.contrast_07_grey
                        height: 1
                        width:eventPage.width
                    }
                    Rectangle{
                        // To give a empty space
                        color: WisenetGui.transparent
                        height: 1
                        width:eventPage.width
                    }
                    Rectangle{
                        id: eventListBG
                        width:parent.width
                        height:parent.height
                        x:15
                        color:"transparent"
                        Flickable{
                            width:parent.width
                            height:parent.height
                            contentWidth: width
                            contentHeight: eventsList.height
                            ScrollBar.vertical: WisenetScrollBar {
                                id: verticalScrollbar
                            }
                            clip:true
                            Flow{
                                x:0
                                y:0
                                width:parent.width
                                //height:parent.height
                                id:eventsList
                                spacing: 20
                                Repeater{
                                    id:eventsRepeater
                                    model: eventNotificationsViewModel.getEventsList("event")
                                    delegate:WisenetCheckBox{
                                        id : eventItem
                                        text : addEventRuleViewModel.translate(modelData)
                                        checked: eventNotificationsViewModel.isCheckedEvent(modelData);
                                        width: eventWidth
                                        iconSource: EventRule.getResourceIcon(modelData)
                                        onCheckedChanged: {
                                            if(checked===false)
                                            {
                                                showallEventCheckBox.checked=false;
                                            }
                                            eventNotificationsViewModel.setEventNotification(modelData,checked);
                                        }
                                        Component.onCompleted: {
                                            if(checked===false)
                                            {
                                                showallEventCheckBox.checked=false;
                                            }
                                        }

                                        Connections{
                                            target:showallEventCheckBox
                                            onCheckAll:eventItem.checked = isCheck;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Rectangle {
                id : systemPage
                anchors.fill: parent
                visible: systemButton.checked
                color:"transparent"
                Column{
                    width:parent.width
                    height:parent.height
                    spacing:15
                    WisenetCheckBox{
                        id : showallSystemCheckBox
                        text : WisenetLinguist.showAllNotifications
                        checked: eventNotificationsViewModel.isCheckedShowAll("system")
                        width: 200
                        signal checkAll(bool isCheck);
                        onClicked: checkAll( checked );
                    }
                    Rectangle{
                        color: WisenetGui.contrast_07_grey
                        height: 1
                        width:systemPage.width
                    }
                    Rectangle{
                        // To give a empty space
                        color: WisenetGui.transparent
                        height: 1
                        width:eventPage.width
                    }
                    Rectangle{
                        id: systemListBG
                        x:15
                        width:parent.width
                        height:parent.height
                        color:"transparent"
                        Flow{
                            anchors.fill: systemListBG
                            id:systemList
                            spacing: 20
                            Repeater{
                            model: eventNotificationsViewModel.getEventsList("system")
                            delegate:WisenetCheckBox{
                                id : systemItem
                                text : addEventRuleViewModel.translate(modelData)
                                checked: eventNotificationsViewModel.isCheckedEvent(modelData);
                                width: eventWidth
                                iconSource: EventRule.getResourceIcon(modelData)
                                onCheckedChanged: {
                                    if(checked===false)
                                    {
                                        showallSystemCheckBox.checked=false;
                                    }
                                    eventNotificationsViewModel.setEventNotification(modelData,checked);
                                }
                                Component.onCompleted: {
                                    if(checked===false)
                                    {
                                        showallSystemCheckBox.checked=false;
                                    }
                                }
                                Connections{
                                    target:showallSystemCheckBox
                                    onCheckAll:{
                                        systemItem.checked = isCheck;
                                    }
                                }
                            }
                            }
                        }
                    }
                }
            }
        }
    }

    footer:
        WisenetDialogButtonBox{

        onAccepted: {
            console.log( "[EventNotifications] Save +++")
            saveSetting();
            console.log( "[EventNotifications] Save ---")
        }
    }
}

