import QtQuick 2.15
import QtQuick.Controls 2.5
import WisenetStyle 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import WisenetLanguage 1.0

Rectangle {
    id: shortcutBoxView
    height: buttonFlow.height

    signal openEventSearch()
    signal openAiSearch()
    signal openSmartSearch()
    signal openTextSearch()
    signal openSetup()
    signal openEventPanel()
    signal fullScreen()
    signal clearScreen()
    signal toggleVideoStatus()
    signal toggleChannelNameVisible()
    signal openInitializeDeviceCredential()
    signal openP2pRegister()
    signal openDashboard()
    signal showEditButton()
    signal hideEditButton()
    signal openMasking()

    property alias videoStatus : videoStatusButton.checked
    property alias channelNameVisible : channelNameVisibleButton.checked
    property alias eventSearchButtonEnabled : eventSearchButton.enabled
    property alias addDeviceButtonEnabled : addDeviceButton.enabled
    property alias setupButtonEnabled : setupButton.enabled
    property alias priorityAlarmButtonEnabled : priorityAlarmButton.enabled
    property alias volumeControlButtonEnabled : volumeControlButton.enabled
    property alias dashboardButtonEnabled : dashboardButton.enabled
    property bool shortcutMouseHovered: false

    function checkHovered(){
        if(shortcutBackMouseArea.containsMouse ||
                searchButton.buttonHovered ||
                eventSearchButton.buttonHovered ||
                aiSearchButton.buttonHovered ||
                smartSearchButton.buttonHovered ||
                textSearchButton.buttonHovered ||
                dashboardButton.buttonHovered ||
                setupButton.buttonHovered ||
                addDeviceButton.buttonHovered ||
                priorityAlarmButton.buttonHovered ||
                volumeControlButton.isHovered ||
                fullScreenButton.buttonHovered ||
                clearScreenButton.buttonHovered ||
                channelNameVisibleButton.buttonHovered ||
                videoStatusButton.buttonHovered ||
                maskingButton.buttonHovered)
        {
            shortcutMouseHovered = true
            showEditButton()
        }
        else {
            shortcutMouseHovered = false
            hideEditButton()
        }
    }

    function setShortcutVisible(index, inUse){
        console.log("ShortcutBoxView.setShortcutVisible() ", index, inUse)

        if(index === 0)
            searchButton.visible = inUse
        else if(index === 1)
            eventSearchButton.visible = inUse
        else if(index === 2)
            aiSearchButton.visible = inUse
        else if(index === 3)
            smartSearchButton.visible = inUse
        else if(index === 4)
            setupButton.visible = inUse
        else if(index === 5)
            addDeviceButton.visible = inUse
        else if(index === 6)
            priorityAlarmButton.visible = inUse
        else if(index === 7)
            volumeControlButton.visible = inUse
        else if(index === 8)
            fullScreenButton.visible = inUse
        else if(index === 9)
            clearScreenButton.visible = inUse
        else if(index === 10)
            channelNameVisibleButton.visible = inUse
        else if(index === 11)
            videoStatusButton.visible = inUse
        else if(index === 12)
            textSearchButton.visible = inUse
        else if(index === 13)
            dashboardButton.visible = inUse
        else if(index === 14) {
            if(shortcutBoxListModel.getLicenseActivated()) {
                console.log("ShortcutBoxView::licenseActivated")
                maskingButton.visible = inUse
            }
            else
                maskingButton.visible = false
        }
        else
            return
    }

    onVisibleChanged: {
        if(visible)
            shortcutBoxListModel.loadShortcutSetting()
    }

    component ShortcutButton: Rectangle{
        id: shortCutRect

        color: button.hovered ? WisenetGui.color_primary : "transparent"
        width:40
        height:40
        property string imageNormal
        property string imageHover
        property string imageSelected
        property string imageDisabled
        property string tooltipText
        property bool checked
        property int shortcutIndex
        property alias buttonHovered: button.hovered
        property bool isHovered: false
        signal clicked()

        WisenetImageButton {
            id: button

            anchors.centerIn: parent
            width: 30
            height: 30
            sourceWidth: 30
            sourceHeight: 30

            imageNormal: shortCutRect.imageNormal
            imageHover: shortCutRect.imageHover
            imageSelected: shortCutRect.imageSelected
            imageDisabled: shortCutRect.imageDisabled

            checked: shortCutRect.checked
            onClicked: shortCutRect.clicked()
            onHoveredChanged: {
                parent.isHovered=button.hovered
                checkHovered()
            }

            WisenetMediaToolTip {
                id: shortcutTooltip
                delay: 500
                visible: parent.hovered
                text: shortCutRect.tooltipText
            }
        }
    }

    MouseArea {
        id: shortcutBackMouseArea
        anchors.fill: parent
        hoverEnabled:true
        onContainsMouseChanged: checkHovered()
    }

    Flow{
        id: buttonFlow
        width: parent.width

        ShortcutButton {
            id: searchButton
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/search_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/search_hover.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/search_dim.svg"

            onClicked: searchMenu.visible = true
            tooltipText: WisenetLinguist.search
        }

        ShortcutButton{
            id: eventSearchButton
            visible : false
            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/event_search_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/event_search_hover.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/event_search_dim.svg"
            onClicked: shortcutBoxView.openEventSearch()
            tooltipText: WisenetLinguist.eventSearch
        }

        ShortcutButton{
            id: aiSearchButton
            enabled: eventSearchButton.enabled
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/ai_search_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/ai_search_hover.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/ai_search_dim.svg"

            onClicked: shortcutBoxView.openAiSearch()
            tooltipText: WisenetLinguist.objectSearch
        }

        ShortcutButton{
            id: smartSearchButton
            enabled: eventSearchButton.enabled
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/smart_search_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/smart_search_hover.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/smart_search_dim.svg"

            onClicked: shortcutBoxView.openSmartSearch()
            tooltipText: WisenetLinguist.smartSearch
        }

        ShortcutButton{
            id: textSearchButton
            enabled: eventSearchButton.enabled
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/text_search_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/text_search_hover.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/text_search_dim.svg"

            onClicked: shortcutBoxView.openTextSearch()
            tooltipText: WisenetLinguist.textSearch
        }

        ShortcutButton{
            id: maskingButton
            visible: false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/masking_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/masking_hover.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/ShortcutBox/masking_selected.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/masking_dim.svg"

            onClicked: shortcutBoxView.openMasking()
            tooltipText: WisenetLinguist.masking
        }

        ShortcutButton{
            id: dashboardButton
            visible: false
            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/dashboard_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/dashboard_hover.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/ShortcutBox/dashboard_press.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/dashboard_dim.svg"

            onClicked: shortcutBoxView.openDashboard()
            tooltipText: WisenetLinguist.dashboard
        }

        ShortcutButton{
            id: setupButton
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/setup_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/setup_hover.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/setup_dim.svg"

            onClicked: shortcutBoxView.openSetup()
            tooltipText: WisenetLinguist.settings
        }

        ShortcutButton{
            id: addDeviceButton
            visible : false

            checked: addDeviceMenu.visible

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/add_device_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/add_device_hover.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/ShortcutBox/add_device_press.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/add_device_dim.svg"

            onClicked: addDeviceMenu.visible = true
            tooltipText: WisenetLinguist.register
        }

        ShortcutButton{
            id: priorityAlarmButton
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/priority_alarm_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/priority_alarm_hover.svg"
            imageDisabled: "qrc:/Assets/Icon/Monitoring/ShortcutBox/priority_alarm_dim.svg"

            onClicked: shortcutBoxView.openEventPanel()
            tooltipText: WisenetLinguist.alertAlarm
        }

        ShortcutButton{
            id: volumeControlButton
            visible : false

            property bool volumeEnabled: gAudioManager && gAudioManager.enable
            imageNormal:{
                if (!volumeEnabled)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/mute_default.svg"
                if (gAudioManager.volume <= 33)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume1_default.svg"
                if (gAudioManager.volume <= 66)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume2_default.svg"
                return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume_default.svg"
            }
            imageHover:{
                if (!volumeEnabled)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/mute_hover.svg"
                if (gAudioManager.volume <= 33)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume1_hover.svg"
                if (gAudioManager.volume <= 66)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume2_hover.svg"
                return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume_hover.svg"
            }
            imageSelected:{
                if (!volumeEnabled)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/mute_press.svg"
                if (gAudioManager.volume <= 33)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume1_select.svg"
                if (gAudioManager.volume <= 66)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume2_select.svg"
                return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume_press.svg"
            }
            imageDisabled:{
                if (!volumeEnabled)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/mute_dim.svg"
                if (gAudioManager.volume <= 33)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume1_dim.svg"
                if (gAudioManager.volume <= 66)
                    return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume2_dim.svg"
                return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume_dim.svg"
            }

            checked: volumeControlView.visible

            onClicked: {
                volumeControlView.x = volumeControlButton.x
                volumeControlView.y = volumeControlButton.y + 30 + 4
                volumeControlView.open()
            }
            tooltipText: WisenetLinguist.volume
        }

        ShortcutButton{
            id: fullScreenButton
            visible : false

            checked: wisenetViewerMainWindow.viewerMode === MainViewModel.FullScreen

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/full_screen_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/full_screen_hover.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/ShortcutBox/full_screen_press.svg"

            onClicked: shortcutBoxView.fullScreen()

            tooltipText: WisenetLinguist.fullScreen
        }

        ShortcutButton{
            id: clearScreenButton
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/clear_screen_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/clear_screen_hover.svg"

            onClicked: shortcutBoxView.clearScreen()
            tooltipText: WisenetLinguist.clearScreen
        }

        ShortcutButton{
            id: channelNameVisibleButton
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/channel_name_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/channel_name_hover.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/ShortcutBox/channel_name_press.svg"
            onClicked: {
                shortcutBoxView.toggleChannelNameVisible()
                console.log("channelNameVisibleButton checked : " + channelNameVisibleButton.checked)
                wisenetViewerMainWindow.displayOsd = channelNameVisibleButton.checked
            }

            tooltipText: WisenetLinguist.channelName
        }

        ShortcutButton{
            id: videoStatusButton
            visible : false

            imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/video_info_default.svg"
            imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/video_info_hover.svg"
            imageSelected: "qrc:/Assets/Icon/Monitoring/ShortcutBox/video_info_press.svg"

            onClicked: {
                shortcutBoxView.toggleVideoStatus()
                console.log("channelNameVisibleButton checked : " + videoStatusButton.checked)
                wisenetViewerMainWindow.displayVideoStatus = videoStatusButton.checked
            }
            tooltipText: WisenetLinguist.videoStatus
        }
    }


    WisenetMediaMenu{
        id: addDeviceMenu

        x: addDeviceButton.x
        y: addDeviceButton.y + 34
        width: 200


        Action{
            text : WisenetLinguist.autoRegister
            onTriggered: {
                if(!localSettingViewModel.autoRegister)
                    shortcutBoxView.openInitializeDeviceCredential()
                else
                    autoDiscoveryViewModel.startDiscoveryAndRegister("Nvr", localSettingViewModel.autoRegisterId, localSettingViewModel.autoRegisterPassword)
            }
        }

        Action {
            text : WisenetLinguist.ddnsAndP2pRegister
            onTriggered: shortcutBoxView.openP2pRegister()
        }
    }


    VolumeControlView{
        id: volumeControlView
        normalSource: volumeControlButton.imageNormal
        pressSource: volumeControlButton.imageSelected
        hoverSource: volumeControlButton.imageHover
    }

    WisenetMediaMenu{
        id: searchMenu
        x: searchButton.x
        y: searchButton.y + 34
        width: 200

        Action{
            text: WisenetLinguist.eventSearch
            onTriggered: shortcutBoxView.openEventSearch()
        }

        Action {
            text: WisenetLinguist.objectSearch
            onTriggered: shortcutBoxView.openAiSearch()
        }

        Action{
            text: WisenetLinguist.smartSearch
            onTriggered: shortcutBoxView.openSmartSearch()
        }

//        Action{
//            text: WisenetLinguist.textSearch
//            onTriggered: shortcutBoxView.openTextSearch()
//        }
    }

    /*
    ShortcutButton{
        id: emptyButton

        visible: eventListViewModel.eventTestVisible

        anchors.top: volumeControlButton.bottom
        anchors.left: videoStatusButton.right
        anchors.topMargin: 6
        anchors.leftMargin: 13

        imageNormal: "qrc:/Assets/Icon/Monitoring/ShortcutBox/event_search_default.svg"
        imageHover: "qrc:/Assets/Icon/Monitoring/ShortcutBox/event_search_hover.svg"

        onClicked: {
            //notificationViewModel.test2()
            eventListViewModel.testPriorityAlarm()
            gc()
        }
        tooltipText: "gc()"
    }*/
}

