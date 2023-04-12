import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import WisenetStyle 1.0

Rectangle{
    id: utilityView

    property int topMargin: 11
    property int imageButtonLeftMargin: 9
    property int imageButtonWidth: 18
    color: WisenetGui.layoutTabColor

    width: (imageButtonLeftMargin + imageButtonWidth) * 4 + separator.width + 20
    height: 40

    signal openNotificationPopup()
    signal openLogoutMenu()
    signal openResourceUsagePopup(var positionX, var positionY)

    Image{
        id: cpuButton
        width: 18
        height: 18

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: topMargin
        sourceSize: "18x18"

        source: cpuButtonMouseArea.containsMouse ? ((resourceUsageViewModel.cpuUsage >= 90 || resourceUsageViewModel.memoryUsage >= 90) ? "qrc:/Assets/Icon/Header/cpu_warning_hover.svg" :
                                                                                                                                          ((resourceUsageViewModel.cpuUsage >= 60 && resourceUsageViewModel.cpuUsage < 90) || (resourceUsageViewModel.memoryUsage >= 60 && resourceUsageViewModel.memoryUsage < 90)) ? "qrc:/Assets/Icon/Header/cpu_caution_hover.svg" :
                                                                                                                                                                                                                                                                                                                       "qrc:/Assets/Icon/Header/cpu_normal_hover.svg")
                                                 :
                                                   ((resourceUsageViewModel.cpuUsage >= 90 || resourceUsageViewModel.memoryUsage >= 90) ? "qrc:/Assets/Icon/Header/cpu_warning_default.svg" :
                                                                                                                                          ((resourceUsageViewModel.cpuUsage >= 60 && resourceUsageViewModel.cpuUsage < 90) || (resourceUsageViewModel.memoryUsage >= 60 && resourceUsageViewModel.memoryUsage < 90)) ? "qrc:/Assets/Icon/Header/cpu_caution_default.svg" :
                                                                                                                                                                                                                                                                                                                       "qrc:/Assets/Icon/Header/cpu_normal_default.svg")



        MouseArea{
            id: cpuButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                console.log("resourceButton :",x,y)
                utilityView.openResourceUsagePopup(cpuButton.x, cpuButton.y + 20)
            }
        }
    }

    Image {
        id: notificationButton

        width: 18
        height: 18

        anchors.left: cpuButton.right
        anchors.leftMargin: imageButtonLeftMargin
        anchors.top: parent.top
        anchors.topMargin: topMargin

        sourceSize: "18x18"
        source: notificationViewModel.eventListModel.count === 0 ? "qrc:/Assets/Icon/Header/notification_disabled.svg" :
                                                                   notificationButtonMouseArea.containsMouse ? "qrc:/Assets/Icon/Header/notification_pressed.svg" :
                                                                                                               "qrc:/Assets/Icon/Header/notification_enabled.svg"
        MouseArea
        {
            id: notificationButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                if(menuViewModel.LoginStatus && notificationViewModel.eventListModel.count > 0)
                    utilityView.openNotificationPopup()
            }
        }
    }

    Image {
        id: userButton

        width: 18
        height: 18

        anchors.left: notificationButton.right
        anchors.leftMargin: imageButtonLeftMargin
        anchors.top: parent.top
        anchors.topMargin: topMargin

        sourceSize: "18x18"
        source: userButtonMouseArea.pressed ? "qrc:/Assets/Icon/Header/user_press.svg" :
                                              userButtonMouseArea.containsMouse ? "qrc:/Assets/Icon/Header/user_hover.svg" :
                                                                                  "qrc:/Assets/Icon/Header/user_default.svg"
        MouseArea
        {
            id: userButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                if(menuViewModel.LoginStatus)
                    utilityView.openLogoutMenu()
            }
        }
    }

    Image {
        id: manualButton

        width: 18
        height: 18

        anchors.left: userButton.right
        anchors.leftMargin: imageButtonLeftMargin
        anchors.top: parent.top
        anchors.topMargin: topMargin

        sourceSize: "18x18"
        source: manualButtonMouseArea.pressed ? "qrc:/Assets/Icon/Header/manual_press.svg" :
                                                manualButtonMouseArea.containsMouse ? "qrc:/Assets/Icon/Header/manual_hover.svg" :
                                                                                      "qrc:/Assets/Icon/Header/manual_default.svg"
        MouseArea
        {
            id: manualButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: mainViewModel.openManual()
        }
    }

    Image {
        id: separator

        width : 1
        height : 12

        anchors.left: manualButton.right
        anchors.leftMargin: imageButtonLeftMargin + 11
        anchors.top: parent.top
        anchors.topMargin: 14

        sourceSize: "1x12"
        source: "qrc:/Assets/Icon/Header/separator.svg"
    }

}

