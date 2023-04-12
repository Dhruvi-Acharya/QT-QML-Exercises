import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import QtQml.Models 2.15
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.UserLayoutTreeSourceModel 1.0
import Wisenet.Tree 1.0
import "EventRule.js" as EventRule
Column{
    id : root

    property int defContentHeight: 30
    property int resourceDeviceType: 0
    property int propertyListViewHeight:5

    property bool isTrigger: true

    property var prevItemGuidList
    property var prevItemAll

    property int prevAlarmoutDuration : 0

    property string strAlarmIn : "AlarmInput"
    property string strNetworkAlarmIn : "NetworkAlarmInput"
    property variant strSystemEvent : ["DevicePowerStatus",
        "DeviceFanStatus", "DeviceStorageStatus", "DeviceRecordingStatus", "DeviceSystemStatus", "Firmware", "Overload", "DeviceSystemConnected", "DeviceSystemDisconnected" ]

    property string strAlertAlarm : "AlertAlarm"
    property string strAlarmOut : "Alarm output"
    property string strLayoutChange : "Open layout"
    property string strEmail: "E-mail"

    function isSystemEvent(eventName){
        for(var item in strSystemEvent){
            if(strSystemEvent[item] === eventName){
                return true;
            }
        }
        return false;
    }

    signal loadPrevisouChceckSignal()
    signal clearAllPreviousSignal()

    anchors.fill: parent
    spacing: 5   
    onVisibleChanged: {
        if(visible == false){
            prevItemGuidList = prevItemAll  = null
            prevAlarmoutDuration =0
            clearAllPreviousSignal()
        }


        remianEventListViewItem.visible = !visible
        if(visible){
            loadPrevisouChceckSignal()
        }

    }

    function loadPreviousCheck(){
        var mapGuidIter = addEventRuleViewModel.getEventGuidList()
        console.log("[AddEventView] loadPreviousCheck start : " + root.isTrigger + "------------------------------------")
        var itemGuidList = new Map()

        for(var item in mapGuidIter){
            if(root.isTrigger && (item === root.strAlarmOut || item === root.strAlertAlarm || item === root.strLayoutChange))
                continue;
            if(!root.isTrigger && !(item === root.strAlarmOut || item === root.strAlertAlarm || item === root.strLayoutChange || item === root.strEmail))
                continue;
            console.log("[AddEventView] loadPreviousCheck event start : " + item )

            var tempItem = mapGuidIter[item]

            var vectorList = []

            for(var item2 in tempItem){
                vectorList.push(tempItem[item2]);
                console.log("[AddEventView] loadPreviousCheck :" + item + " -------" + tempItem[item2])
            }
            console.log("[AddEventView] loadPreviousCheck event end : " + item )
            itemGuidList.set(item, vectorList)
        }

        console.log("[AddEventView] loadPreviousCheck] end : " + root.isTrigger + "------------------------------------")
        return itemGuidList
    }

    function loadPreviousAllCheck(){
        var mapGuidIter = addEventRuleViewModel.getEventTriggerAll()

        var itemGuidList = new Map()

        for(var item in mapGuidIter){
            console.log("[AddEventView] loadPreviousAllCheck1:" + item + " " + mapGuidIter[item] + " isTrigger:" + root.isTrigger)

            itemGuidList.set(item, mapGuidIter[item])
        }

        return itemGuidList
    }

    function loadPreviousAlarmoutDuration(){
        console.log("[AddEventView] loadPreviousAlarmoutDuration prev:" + addEventRuleViewModel.getAlarmoutDuration())
        var alarmoutValue = addEventRuleViewModel.getAlarmoutDuration()
        var alarmout = 0
        if(alarmoutValue === -1)
            alarmout = 5
        else if(alarmoutValue === 0)
            alarmout = 0
        else if(alarmoutValue === 5)
            alarmout = 1
        else if(alarmoutValue === 10)
            alarmout = 2
        else if(alarmoutValue === 20)
            alarmout = 3
        else if(alarmoutValue === 30)
            alarmout = 4

        console.log("[AddEventView] loadPreviousAlarmoutDuration:" + alarmout + " alarmoutValue:" + alarmoutValue + " isTrigger:" + root.isTrigger)
        return alarmout
    }

    function getPreviousCheck(){
        var itemGuidList = new Map()
        for (var i = 0 ; i < enabledEventView.count ; i++) {
            var item = enabledEventView.itemAtIndex(i);
            if (item) {

                var vectorList = []
                if(item.selectionTree.selectedDeviceValue){
                    item.selectionTree.selectedDeviceValue.forEach(function(item2){
                        console.debug("[AddEventView] getPreviousCheck item.currentEventName:" + item.currentEventName + " " + item2.uuid + " isTrigger:" + root.isTrigger)
                        vectorList.push(item2.uuid);
                    });
                }
                console.debug("[AddEventView] getPreviousCheck index:" + i + " eventName:"+ item.currentEventName  + " isTrigger:" + root.isTrigger)
                itemGuidList.set(item.currentEventName, vectorList)
            }
        }
        return itemGuidList
    }

    function getPreviousAllCheck(){
        var itemGuidList = new Map()
        for (var i = 0 ; i < enabledEventView.count ; i++) {
            var item = enabledEventView.itemAtIndex(i);
            if (item) {
                itemGuidList.set(item.currentEventName, item.selectionTree.allDevice)
                console.debug("[AddEventView] getPreviousAllCheck index:" + i + " eventName:"+ item.currentEventName + " " + item.selectionTree.allDevice  + " isTrigger:" + root.isTrigger)
            }
        }
        return itemGuidList
    }

    function getPreviousAlarmOutDuration(){
        for (var i = 0 ; i < enabledEventView.count ; i++) {
            var item = enabledEventView.itemAtIndex(i);
            if (item && item.currentEventName === root.strAlarmOut) {
                 console.debug("[AddEventView] getPreviousAlarmOutDuration : " + item.getAlamoutDuration() + " isTrigger:" + root.isTrigger)
                return item.getAlamoutDuration()
            }
        }
        return 0
    }
    function getPreviousAlarmOutDurationIndex(){
        for (var i = 0 ; i < enabledEventView.count ; i++) {
            var item = enabledEventView.itemAtIndex(i);
            if (item && item.currentEventName === root.strAlarmOut) {

                return item.getAlamoutDurationIndex()
            }
        }
        return 0
    }

    function selectDeviceTree(currentEventName, selectionTree, selectionSourceModel, selectionProxyModel, guid){
        console.debug("[AddEventView] selectDeviceTree  : "+ currentEventName)

        var modelIndex = selectionSourceModel.getModelIndex(guid, selectionTree.treeItemSelectionModelVal)
        var proxyIndex = selectionProxyModel.mapFromSource(modelIndex)
        if(currentEventName === root.strAlertAlarm){
            selectionTree.treeItemSelectionModelVal.select(proxyIndex,  ItemSelectionModel.ClearAndSelect)
        }
        else if(currentEventName === root.strLayoutChange){
            selectionTree.treeItemSelectionModelVal.select(proxyIndex,  ItemSelectionModel.ClearAndSelect)
        }
        else if(currentEventName === root.strEmail){
            selectionTree.treeItemSelectionModelVal.select(proxyIndex,  ItemSelectionModel.Select)
        }
        else{
            console.debug("[AddEventView] selectDeviceTree ItemSelectionModel.Select" + " isTrigger:" + root.isTrigger)
            selectionTree.treeItemSelectionModelVal.select(proxyIndex,  ItemSelectionModel.Select)
        }
    }

    function clickTriggerEvent(eventName){
        prevItemGuidList = getPreviousCheck()

        console.debug("[AddEventView] onClickTriggerEvent1:" + eventName + " isTrigger:" + root.isTrigger)
        if(root.isTrigger){
            prevItemAll = getPreviousAllCheck()
            addEventRuleViewModel.appendEventList(eventName)
        }
        else{
             prevAlarmoutDuration = getPreviousAlarmOutDurationIndex()
            console.debug("[AddEventView] onClickTriggerEvent2:" + prevAlarmoutDuration + " isTrigger:" + root.isTrigger)
            addEventRuleViewModel.appendActionList(eventName)
        }
        remianEventListViewItem.visible = false
    }

    Component{
        id : remainEventDelegate

        Rectangle {
            x : 1
            width: root.width -2
            height: root.defContentHeight
            color: WisenetGui.contrast_08_dark_grey

            Row{
                spacing: 10
                leftPadding: 15
                anchors{
                    verticalCenter: parent.verticalCenter
                }

                Image{
                    id: eventImage
                    width: 16
                    height: 16
                    sourceSize: Qt.size(width, height)
                    source: EventRule.getResourceIcon(modelData)
                }

                WisenetText {
                    id : eventName

                    property string eventNameKey : modelData

                    font.pixelSize: 12

                    width: root.width
                    text: addEventRuleViewModel.translate(modelData)
                }
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.debug("[EventActionView] clickTriggerEvent4:" + eventName.text + " isTrigger:" + root.isTrigger)
                    root.clickTriggerEvent(eventName.eventNameKey);

                }
            }
        }
    }

    Component{
        id : enabledEventDelegate

        Item{
            id : enabledEventDelegateItem
            x : 1
            width: eventNameRect.width
            height: eventNameRect.height + resoureRect.height + 5

            property int propertyTreeHeight : 250

            property var selectionTree: selectCurrentTree()
            property var selectionSourceModel: selectCurrentSourceModel()
            property var selectionProxyModel: selectCurrentProxyModel()
            property alias currentEventName : eventNameRect.currentEventName

            function selectCurrentTree(){
                if(eventNameRect.currentEventName === root.strAlertAlarm)
                    return alertAlarmTree
                else if(eventNameRect.currentEventName === root.strAlarmOut)
                    return alarmOutTree
                else if(eventNameRect.currentEventName==root.strLayoutChange)
                    return layoutChangeTree
                else if((eventNameRect.currentEventName === root.strAlarmIn) || eventNameRect.currentEventName === root.strNetworkAlarmIn)
                    return alarmInTree
                else if ((eventNameRect.currentEventName === root.strEmail) || (eventNameRect.currentEventName === "Email"))
                    return emailTree

                else if(root.isSystemEvent(eventNameRect.currentEventName))
                    return systemTree
                else
                    return multiChannelTree
            }
            function selectCurrentSourceModel(){
                if(eventNameRect.currentEventName === root.strAlertAlarm)
                    return alertAlarmSourceModel
                else if(eventNameRect.currentEventName === root.strAlarmOut)
                    return alarmOutSourceModel
                else if(eventNameRect.currentEventName === root.strLayoutChange)
                    return layoutChangeSourceModel
                else if((eventNameRect.currentEventName === root.strAlarmIn) || eventNameRect.currentEventName === root.strNetworkAlarmIn)
                    return alarmInSourceModel
                else if ((eventNameRect.currentEventName === root.strEmail) || (eventNameRect.currentEventName === "Email"))
                    return emailSourceModel

                else if(root.isSystemEvent(eventNameRect.currentEventName))
                    return systemSourceModel
                else
                    return multiChannelSourceModel

            }
            function selectCurrentProxyModel(){
                if(eventNameRect.currentEventName === root.strAlertAlarm)
                    return alertAlarmProxyModel
                else if(eventNameRect.currentEventName === root.strAlarmOut)
                    return alarmOutProxyModel
                else if(eventNameRect.currentEventName === root.strLayoutChange)
                    return layoutChangeProxyModel
                else if((eventNameRect.currentEventName === root.strAlarmIn) || eventNameRect.currentEventName === root.strNetworkAlarmIn)
                    return alarmInProxyModel
                else if ((eventNameRect.currentEventName === root.strEmail) || (eventNameRect.currentEventName === "Email"))
                    return emailTreeProxyModel

                else if(root.isSystemEvent(eventNameRect.currentEventName))
                    return systemProxyModel
                else
                    return multiChannelProxyModel
            }
            function getAlamoutDurationIndex(){
                if(eventNameRect.currentEventName === root.strAlarmOut)
                    return alarmDurationRect.alarmOutDurationValue
                else
                    return 0
            }

            function getAlamoutDuration(){
                if(eventNameRect.currentEventName === root.strAlarmOut)
                {
                    if(alarmDurationRect.alarmOutDurationValue === 0)
                        return 0
                    else if(alarmDurationRect.alarmOutDurationValue === 1)
                        return 5
                    else if(alarmDurationRect.alarmOutDurationValue === 2)
                        return 10
                    else if(alarmDurationRect.alarmOutDurationValue === 3)
                        return 20
                    else if(alarmDurationRect.alarmOutDurationValue === 4)
                        return 30
                    else
                        return -1
                }
                else
                    return ""
            }

            function selectAlarmoutDuration(duration){

            }

            function selectDevice(tree){
                if(tree === selectionTree){
                    var find = false;
                    if(root.prevItemGuidList !== null){
                        for (var [key, value] of root.prevItemGuidList) {
                            if(key === currentEventName){
                                for(var va of value){
                                    selectDeviceTree(currentEventName, selectionTree, selectionSourceModel, selectionProxyModel, va)
                                }
                                find = true
                                break;
                            }
                        }
                    }
                    else{
                        tree.reset()
                    }
                    if(find === false){
                        tree.reset()
                    }
                    if(root.isTrigger && (root.prevItemAll !== null)){
                        for(var [key, value] of root.prevItemAll){
                            if(key === currentEventName && value === true){
                                tree.reset()
                            }
                        }
                    }
                }
                if(currentEventName == root.strAlarmOut){
                    alarmDurationRect.alarmOutDurationAlias.duration = root.prevAlarmoutDuration
                }
            }

            Rectangle {
                id : eventNameRect
                width: root.width -1
                height: root.defContentHeight
                color: "transparent"
                border.color: WisenetGui.contrast_01_light_grey
                border.width: 1

                property alias currentEventName: eventName.eventNameKey

                Row{
                    spacing: 10
                    leftPadding: 15
                    anchors{
                        verticalCenter: parent.verticalCenter
                    }

                    Image{
                        id: eventImage
                        width: 16
                        height: 16
                        sourceSize: Qt.size(width, height)
                        source: EventRule.getResourceIcon(modelData)
                    }

                    WisenetText {
                        id : eventName
                        property string eventNameKey: modelData
                        font.pixelSize: 12
                        width: eventNameRect.width - removeEvent.implicitWidth - eventImage.width - foldButton.width
                        text: addEventRuleViewModel.translate(modelData)
                    }
                }
                WisenetTransparentButton{
                    id : removeEvent
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: "X"
                    implicitWidth: 20

                    onClicked: {
                        console.debug("[EventActionView] removeEvent:" + eventName.text + " height:" +height + " isTrigger:" + root.isTrigger)
                        if(root.isTrigger)
                            addEventRuleViewModel.removeEventList(eventName.eventNameKey)
                        else
                            addEventRuleViewModel.removeActionList(eventName.eventNameKey)
                    }
                }
                WisenetTransparentButton{
                    id : foldButton
                    anchors.right: removeEvent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: "v"
                    implicitWidth: 20
                    onClicked: {
                        console.debug("fold Click:" + text)
                        if(text === "^"){
                            text = "v"
                            resoureRect.height = propertyTreeHeight
                            resoureRect.visible = true
                            root.propertyListViewHeight += propertyTreeHeight
                        }
                        else{
                            text = "^"
                            resoureRect.height = 0
                            resoureRect.visible = false
                            root.propertyListViewHeight -= propertyTreeHeight
                        }
                    }
                }
            }
            Rectangle{
                id : resoureRect
                width: parent.width
                anchors.top: eventNameRect.bottom
                height: propertyTreeHeight
                visible: true

                property color borderColor : WisenetGui.contrast_06_grey
                property color bgColor : WisenetGui.contrast_08_dark_grey

                function isChannelEvent(eventName){
                    if((eventName === root.strAlarmIn) || (eventName === root.strNetworkAlarmIn))
                        return false
                    else if(root.isSystemEvent(eventNameRect.currentEventName))
                        return false
                    else if((eventName === root.strAlarmOut) || (eventName === root.strAlertAlarm) || (eventName === root.strLayoutChange))// || (eventName === root.strEmail)))
                        return false
                    else
                        return true
                }

                //trigger - multi channel
                Item{
                    anchors.fill: parent
                    visible: resoureRect.isChannelEvent(eventNameRect.currentEventName)

                    Connections{
                        target: root
                        onLoadPrevisouChceckSignal: {
                            selectDevice(multiChannelTree)
                        }
                        onClearAllPreviousSignal:{
                            multiChannelTree.treeItemSelectionModelVal.clear()
                        }
                    }

                    ResourceTreeProxyModel{
                        id: multiChannelProxyModel
                        sourceModel: DevicePhysicalTreeSourceModel{
                            id: multiChannelSourceModel
                        }
                        Component.onCompleted: {
                            if(resoureRect.isChannelEvent(eventNameRect.currentEventName))
                            {
                                multiChannelSourceModel.readDevices(false)
                                doSort(Qt.AscendingOrder)
                                selectDevice(multiChannelTree)
                            }
                        }
                    }

                    WisenetDeviceSelectionTree{
                        id: multiChannelTree
                        anchors.fill: parent
                        treeModel: multiChannelProxyModel
                        bgColor: resoureRect.bgColor
                        borderColor: resoureRect.borderColor
                    }
                }
                //trigger - multi alarm In
                Item{
                    anchors.fill: parent
                    visible: (eventNameRect.currentEventName === root.strAlarmIn || eventNameRect.currentEventName ===root.strNetworkAlarmIn)
                    Connections{
                        target: root
                        onLoadPrevisouChceckSignal: {
                            selectDevice(alarmInTree)
                        }
                        onClearAllPreviousSignal:{
                            alarmInTree.treeItemSelectionModelVal.clear()
                        }
                    }
                    ResourceTreeProxyModel{
                        id: alarmInProxyModel
                        sourceModel: DeviceAlarmInTreeSourceModel{
                            id: alarmInSourceModel
                        }
                        Component.onCompleted: {
                            if((eventNameRect.currentEventName === root.strAlarmIn || eventNameRect.currentEventName ===root.strNetworkAlarmIn)){
                                alarmInSourceModel.readAlarmIn(eventNameRect.currentEventName ==root.strNetworkAlarmIn)
                                doSort(Qt.AscendingOrder)
                                selectDevice(alarmInTree)
                            }
                        }
                    }

                    WisenetDeviceSelectionTree{
                        id: alarmInTree
                        anchors.fill: parent
                        treeModel: alarmInProxyModel
                        bgColor: resoureRect.bgColor
                        borderColor: resoureRect.borderColor
                    }
                }
                //trigger - multi system event
                Item{
                    anchors.fill: parent
                    visible: (root.isSystemEvent(eventNameRect.currentEventName))
                    Connections{
                        target: root
                        onLoadPrevisouChceckSignal: {
                            selectDevice(systemTree)
                        }
                        onClearAllPreviousSignal:{
                            systemTree.treeItemSelectionModelVal.clear()
                        }
                    }
                    ResourceTreeProxyModel{
                        id: systemProxyModel
                        sourceModel: DeviceEventTreeSourceModel{
                            id: systemSourceModel
                        }
                        Component.onCompleted: {
                            if(root.isSystemEvent(eventNameRect.currentEventName)){
                                systemSourceModel.readDevice()
                                doSort(Qt.AscendingOrder)
                                selectDevice(systemTree)
                            }
                        }
                    }

                    WisenetDeviceSelectionTree{
                        id: systemTree
                        anchors.fill: parent
                        treeModel: systemProxyModel
                        bgColor: resoureRect.bgColor
                        borderColor: resoureRect.borderColor
                    }
                }
                //action - single channel
                Item{
                    anchors.fill: parent
                    visible: (eventNameRect.currentEventName === root.strAlertAlarm)
                    Connections{
                        target: root
                        onLoadPrevisouChceckSignal: {
                            selectDevice(alertAlarmTree)
                        }
                        onClearAllPreviousSignal:{
                            alertAlarmTree.treeItemSelectionModelVal.clear()
                        }
                    }
                    ResourceTreeProxyModel{
                        id: alertAlarmProxyModel
                        sourceModel: DevicePhysicalTreeSourceModel{
                            id: alertAlarmSourceModel
                        }
                        Component.onCompleted: {
                            if(eventNameRect.currentEventName === root.strAlertAlarm){
                                alertAlarmSourceModel.readDevices(false)
                                doSort(Qt.AscendingOrder)
                                selectDevice(alertAlarmTree)
                            }
                        }
                    }

                    WisenetDeviceSelectionTree{
                        id: alertAlarmTree
                        anchors.fill: parent
                        treeModel: alertAlarmProxyModel
                        bgColor: resoureRect.bgColor
                        borderColor: resoureRect.borderColor
                        //state : "AlertAlarm"
                        state : "AlarmAlert"

                        MouseArea{
                            anchors.fill: parent

                            onPressed: {
                                if(mouse.modifiers & Qt.ShiftModifier){
                                    mouse.accepted = true
                                }
                                else{
                                    mouse.accepted = false
                                }
                            }
                        }
                    }
                }
                // LayoutChange
                Item{
                    anchors.fill: parent
                    visible: (eventNameRect.currentEventName === root.strLayoutChange)
                    Connections{
                        target: root
                        onLoadPrevisouChceckSignal: {
                            console.log("[AddEventView] onLoadPrevisouChceckSignal layoutChangeTree" + " isTrigger:" + root.isTrigger)
                            selectDevice(layoutChangeTree)
                        }
                        onClearAllPreviousSignal:{
                            layoutChangeTree.treeItemSelectionModelVal.clear()
                        }
                    }
                    ResourceTreeProxyModel{
                        id: layoutChangeProxyModel
                        sourceModel: UserLayoutTreeSourceModel{
                            id: layoutChangeSourceModel
                        }
                        Component.onCompleted: {
                            layoutChangeSourceModel.readLayouts();
                            selectDevice(layoutChangeTree)
                        }
                    }

                    WisenetDeviceSelectionTree{
                        id: layoutChangeTree
                        anchors.fill: parent
                        treeModel: layoutChangeProxyModel
                        bgColor: resoureRect.bgColor
                        borderColor: resoureRect.borderColor
                        //state : "Layout"
                        state : "Layout_singleSelection"
                        Component.onCompleted: {
                            console.log("[AddEventView] WisenetDeviceSelectionTree layoutChangeTree Component.onCompleted " + eventNameRect.currentEventName + " isTrigger:" + root.isTrigger)
                            layoutChangeTree.expandAll()
                            selectDevice(layoutChangeTree)
                        }
                        MouseArea{
                            anchors.fill: parent

                            onPressed: {
                                if(mouse.modifiers & Qt.ShiftModifier){
                                    mouse.accepted = true
                                }
                                else{
                                    mouse.accepted = false
                                }
                            }
                        }
                    }
                }
                //action - multi alarmout
                Item{
                    anchors.fill: parent
                    visible: (eventNameRect.currentEventName === root.strAlarmOut)
                    Connections{
                        target: root
                        onLoadPrevisouChceckSignal: {
                            selectDevice(alarmOutTree)
                        }
                        onClearAllPreviousSignal:{
                            alarmOutTree.treeItemSelectionModelVal.clear()
                        }
                    }
                    ResourceTreeProxyModel{
                        id: alarmOutProxyModel
                        sourceModel: DeviceAlarmOutTreeSourceModel{
                            id: alarmOutSourceModel
                        }
                        Component.onCompleted: {
                            if(eventNameRect.currentEventName === root.strAlarmOut){
                                alarmOutSourceModel.readAlarmOut()
                                doSort(Qt.AscendingOrder)
                                selectDevice(alarmOutTree)
                            }
                        }
                    }
                    Item{
                        id : alarmoutTreeItem
                        width: parent.width
                        height: parent.height - root.defContentHeight
                        anchors.left: parent.left
                        anchors.top: parent.top
                    }

                    WisenetDeviceSelectionTree{
                        id: alarmOutTree
                        anchors.fill: alarmoutTreeItem
                        treeModel: alarmOutProxyModel
                        bgColor: resoureRect.bgColor
                        borderColor: resoureRect.borderColor
                        state : "AlarmOut"
                    }
                    Rectangle{
                        id : alarmDurationRect
                        anchors.top: alarmoutTreeItem.bottom
                        width: parent.width
                        height: root.defContentHeight

                        color: resoureRect.bgColor
                        border.width: 1
                        border.color: resoureRect.borderColor

                        property alias alarmOutDurationValue : addEventRuleAlarmDurationView.duration
                        property alias alarmOutDurationAlias : addEventRuleAlarmDurationView


                        AddEventRuleDurationView{
                            id : addEventRuleAlarmDurationView
                            anchors.fill: parent
                            isEnable : true
                            isAlarmDurationView: true
                            sliderFrom: 0
                            sliderTo: 5
                            duration: addEventRuleAlarmDurationView.duration
                            comboBoxModel: [WisenetLinguist.off,
                                "5 " + WisenetLinguist.sec,
                                "10  " + WisenetLinguist.sec,
                                "20 " + WisenetLinguist.sec,
                                "30 " + WisenetLinguist.sec,
                                WisenetLinguist.continious]
                        }
                    }
                }
                //action - email
                Item{
                    anchors.fill: parent
                    visible: (eventNameRect.currentEventName === root.strEmail)
                    Connections{
                        target: root
                        onLoadPrevisouChceckSignal: {
                            selectDevice(emailTree)
                        }
                        onClearAllPreviousSignal:{
                            emailTree.treeItemSelectionModelVal.clear()
                        }
                    }
                    ResourceTreeProxyModel{
                        id: emailTreeProxyModel
                        sourceModel: DeviceEmailTreeSourceModel{
                            id: emailSourceModel
                        }
                        Component.onCompleted: {
                            if(eventNameRect.currentEventName === root.strEmail){
                                emailSourceModel.readEmail()
                                doSort(Qt.AscendingOrder)
                                selectDevice(emailTree)
                            }
                        }
                    }
                    Item{
                        id : emailTreeItem
                        width: parent.width
                        height: parent.height - root.defContentHeight
                        anchors.left: parent.left
                        anchors.top: parent.top
                    }

                    WisenetDeviceSelectionTree{
                        id: emailTree
                        anchors.fill: emailTreeItem
                        treeModel: emailTreeProxyModel
                        bgColor: resoureRect.bgColor
                        borderColor: resoureRect.borderColor
                        state : "Email"
                    }
//                    Rectangle{
//                        id : emailAdditionalReceipientRect
//                        anchors.top: emailTreeItem.bottom
//                        width: parent.width
//                        height: root.defContentHeight + 15

//                        color: resoureRect.bgColor
//                        border.width: 1
//                        border.color: resoureRect.borderColor

//                        Column{
//                            WisenetLabel {
//                                id: emailAdditionalReceipientLabel
//                                text: WisenetLinguist.emailAdditionalReceipient
//                            }
//                            WisenetTextBox {
//                                id: emailAdditionalReceipientTextbox
//                                width: 355
//                            }
//                        }
//                    }
                }

            }

            Component.onCompleted: {
                console.debug("[AddEventView] Component.onCompleted prev:" + root.propertyListViewHeight + " height:" +height + currentEventName + " isTrigger:" + root.isTrigger)
                root.propertyListViewHeight += height
                console.debug("[AddEventView] Component.onCompleted after:" + root.propertyListViewHeight + " height:" +height + currentEventName + " isTrigger:" + root.isTrigger)

            }
            Component.onDestruction: {
                console.debug("[AddEventView] Component.onDestruction prev:" + root.propertyListViewHeight + " height:" +height + currentEventName + " isTrigger:" + root.isTrigger)
                root.propertyListViewHeight -= height
                console.debug("[AddEventView] Component.onDestruction prev:" + root.propertyListViewHeight + " height:" +height + currentEventName + " isTrigger:" + root.isTrigger)
            }
        }
    }


    Item{
        id : remianEventListViewItem
        width: parent.width
        height: parent.height
        visible: false
        Rectangle{
            id : selectDeviceRect
            width: parent.width
            height: root.defContentHeight
            color: WisenetGui.contrast_09_dark
            anchors.top: parent.top
            border.width: 1
            border.color: WisenetGui.contrast_00_white
            WisenetText{
                width: parent.width
                anchors.fill: parent
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment : Text.AlignHCenter
                color: WisenetGui.contrast_02_light_grey
                text : WisenetLinguist.selectEventType
            }
        }
        Rectangle{
            visible: remianEventListViewItem.visible
            color: WisenetGui.contrast_08_dark_grey
            anchors.top: selectDeviceRect.bottom
            anchors.left: parent.left
            width: parent.width
            height: parent.height - selectDeviceRect.height
            border.width: 1
            border.color: WisenetGui.contrast_06_grey
            ListView{
                id : remianEventListView
                anchors.left: parent.left
                anchors.leftMargin: 1
                anchors.top: parent.top
                anchors.topMargin: 1
                width: parent.width -2
                height: parent.height -2

                clip: true
                focus: true

                model: root.isTrigger? addEventRuleViewModel.remainingEventList : addEventRuleViewModel.exceptActionList
                delegate: remainEventDelegate
                spacing: 0

            }
        }


    }

    Item{

        function calculateHeight(){
            var bottomSize = actionButton.visible? actionButton.height: 0
            if(root.height - bottomSize > root.propertyListViewHeight)
                return root.propertyListViewHeight
            else
                return (root.height - bottomSize)
        }

        width: parent.width
        height: calculateHeight()

        ListView{
            id : enabledEventView
            width: parent.width
            height: parent.height

            clip: true
            visible: !remianEventListViewItem.visible
            model: root.isTrigger? addEventRuleViewModel.addedEventList : addEventRuleViewModel.addedActionList
            delegate: enabledEventDelegate
            focus: true
            spacing: 5
            ScrollBar.vertical: WisenetScrollBar { id : verticalScroll}
        }
    }
    WisenetTransparentButton{
        id : actionButton
        font.pixelSize: 12
        width: parent.width
        height: root.defContentHeight
        text : qsTr("+ ") + (root.isTrigger? WisenetLinguist.addTrigger : WisenetLinguist.addAction)
        visible: !(root.isTrigger? addEventRuleViewModel.isFullTrigger : addEventRuleViewModel.isFullAction) && !remianEventListViewItem.visible
        useBorder: (root.propertyListViewHeight >0)? false : true
        onClicked:
        {
            remianEventListViewItem.visible = true
        }
    }
}
