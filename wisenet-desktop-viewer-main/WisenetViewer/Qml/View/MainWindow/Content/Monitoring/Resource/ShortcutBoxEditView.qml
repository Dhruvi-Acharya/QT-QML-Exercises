import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import QtQuick.Layouts 1.15
import Wisenet.Shortcut 1.0

Popup{
    id: shortcutBoxEditView
    width: 300; height: !licenseActivated? 420 : 450
    background: Rectangle {
        id: backRect
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1
    }

    topPadding: 20
    leftPadding: 20
    rightPadding: 20
    bottomPadding: 20

    property int buttonWidthSize: 120
    property int buttonHeightSize: 28
    property int shortcutUseCount
    property bool licenseActivated

    onVisibleChanged: updateShortcutInfo()

    function updateShortcutInfo(){
        var count = 0;
        const useList = shortcutBoxListModel.getData()

        for (const menuUse of useList) {
            if(menuUse)
                count +=1
        }

        licenseActivated = shortcutBoxListModel.getLicenseActivated()
        shortcutUseCount = count
        console.log("ShortcutBoxEditView shortcutUseCount", shortcutUseCount)
    }

    Connections {
        target:shortcutBoxListModel
        onSettingChanged: updateShortcutInfo()
    }

    function getMenuName(index){
        switch(index)
        {
        case ShortcutBoxListModel.Search:
            return WisenetLinguist.search
        case ShortcutBoxListModel.EventSearch:
            return WisenetLinguist.eventSearch
        case ShortcutBoxListModel.AISearch:
            return WisenetLinguist.objectSearch
        case ShortcutBoxListModel.SmartSearch:
            return WisenetLinguist.smartSearch
        case ShortcutBoxListModel.Setup:
            return WisenetLinguist.settings
        case ShortcutBoxListModel.Register:
            return WisenetLinguist.register
        case ShortcutBoxListModel.PriorityAlarm:
            return WisenetLinguist.alertAlarm
        case ShortcutBoxListModel.Volume:
            return WisenetLinguist.volume
        case ShortcutBoxListModel.FullScreen:
            return WisenetLinguist.fullScreen
        case ShortcutBoxListModel.LayoutClear:
            return WisenetLinguist.clearScreen
        case ShortcutBoxListModel.ChannelName:
            return WisenetLinguist.channelName
        case ShortcutBoxListModel.Status:
            return WisenetLinguist.videoStatus
        case ShortcutBoxListModel.TextSearch:
            return WisenetLinguist.textSearch
        case ShortcutBoxListModel.HealthCheck:
            return "Health Check"
        case ShortcutBoxListModel.Masking:
            return WisenetLinguist.masking
        default:
            console.log("got number more than the number of shortcut menu", index)
            return ""
        }
    }

    function getResourceIcon(index)
    {
        switch(index)
        {
        case ShortcutBoxListModel.Search:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/search_default.svg"
        case ShortcutBoxListModel.EventSearch:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/event_search_default.svg"
        case ShortcutBoxListModel.AISearch:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/ai_search_default.svg"
        case ShortcutBoxListModel.SmartSearch:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/smart_search_default.svg"
        case ShortcutBoxListModel.Setup:
            return  "qrc:/Assets/Icon/Monitoring/ShortcutBox/setup_default.svg"
        case ShortcutBoxListModel.Register:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/add_device_default.svg"
        case ShortcutBoxListModel.PriorityAlarm:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/priority_alarm_hover.svg"
        case ShortcutBoxListModel.Volume:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/volume1_default.svg"
        case ShortcutBoxListModel.FullScreen:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/full_screen_default.svg"
        case ShortcutBoxListModel.LayoutClear:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/clear_screen_default.svg"
        case ShortcutBoxListModel.ChannelName:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/channel_name_default.svg"
        case ShortcutBoxListModel.Status:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/video_info_default.svg"
        case ShortcutBoxListModel.TextSearch:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/text_search_default.svg"
        case ShortcutBoxListModel.HealthCheck:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/dashboard_default.svg"
        case ShortcutBoxListModel.Masking:
            return "qrc:/Assets/Icon/Monitoring/ShortcutBox/masking_default.svg"
        default:
            console.log("got number more than the number of shortcut menu", index)
            return ""
        }
    }

    WisenetLabel {
        id: label
        width: 109; height: 16
        anchors {top: parent.top; left: parent.left;}
        Layout.alignment: Qt.AlignLeft
        text: WisenetLinguist.editShortcutBox
        defaultPixelSize: 14
        color: WisenetGui.contrast_02_light_grey
    }

    Rectangle {
        id: checkboxListArea
        anchors {top:label.bottom; topMargin:15; bottom:buttonArea.top; bottomMargin: 15;
            left:parent.left; right: parent.right}
        color: WisenetGui.transparent

        ListView{
            id: checkboxRepeater
            anchors.fill: parent
            spacing: 10
            interactive: false
            model: shortcutBoxListModel
            delegate: WisenetCheckBox{
                id: checkboxItem
                text: getMenuName(menuEnum)
                checked: inUse
                iconSource: getResourceIcon(menuEnum)
                onClicked:{
                    if(checked)
                        shortcutUseCount += 1
                    else
                        shortcutUseCount -= 1

                    if(shortcutUseCount > 0)
                        inUse= checked
                    else{ // 원복
                        checkboxItem.checked = !checked
                        shortcutUseCount += 1
                    }
                }
            }
        }
    }

    Rectangle {
        id: buttonArea
        width: applyButton.width + cancelButton.width + 10; height: applyButton.height
        anchors {bottom: parent.bottom; horizontalCenter: parent.horizontalCenter}
        color: WisenetGui.transparent

        WisenetOrangeButton {
            id: applyButton
            visible: true
            anchors.left: parent.left
            text: WisenetLinguist.ok
            onClicked: {
                console.log("ShortcutBoxEditView applyButton clicked")
                shortcutBoxListModel.saveShortcutSetting()
                shortcutBoxEditView.close()
            }
        }

        WisenetGrayButton {
            id: cancelButton
            anchors {left:applyButton.right; leftMargin: 10; right: parent.right}
            text: WisenetLinguist.cancel
            onClicked: {
                shortcutBoxEditView.close()
            }
        }
    }
}
