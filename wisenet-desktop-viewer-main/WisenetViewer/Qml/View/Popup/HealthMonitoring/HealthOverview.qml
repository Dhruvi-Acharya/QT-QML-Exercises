import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

Rectangle{
    id: healthOverview

    component OverviewItem: Rectangle{
        id: overviewItem

        property alias title: titleText.text
        property alias errorCountText: errorCount.text
        property bool status
        width: 179
        height: 52
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_08_dark_grey
        border.width: 1
        radius: 10

        Rectangle{
            id: statusIcon

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 7
            anchors.leftMargin: 10

            width: 38
            height: 38

            color: "transparent"
            border.color: status ? WisenetGui.color_secondary : WisenetGui.color_accent
            border.width: 2
            radius: 19
        }

        Text{
            id: errorCount

            anchors.fill: statusIcon
            color: WisenetGui.color_accent
            font.pixelSize: 18

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            visible: !status
        }

        Text{
            id: titleText

            anchors.top: parent.top
            anchors.left: statusIcon.right
            anchors.topMargin: 7
            anchors.leftMargin: 10
            color: WisenetGui.contrast_06_grey

            font.pixelSize: 12
        }

        Text{
            id: statusText

            anchors.top: titleText.bottom
            anchors.left: statusIcon.right
            anchors.topMargin: 1
            anchors.leftMargin: 10

            text: status ? "Good" : "Error"
            color: WisenetGui.contrast_01_light_grey

            font.pixelSize: 18
        }

    }

    radius: 10
    color: "transparent"
    border.color: WisenetGui.contrast_08_dark_grey
    border.width: 1

    Text{
        id: healthOverviewTitle

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 15
        anchors.leftMargin: 20

        text: "Health overview"
        color: WisenetGui.contrast_01_light_grey
        font.pixelSize: 16
    }

    Flow{
        id: overviewFlow

        anchors.top: healthOverviewTitle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 15
        anchors.bottomMargin: 20

        spacing: 17

        OverviewItem{
            id: deviceOverview

            title: "Device"
        }

        OverviewItem{
            id: diskOverview

            title: "Disk"
        }

        OverviewItem{
            id: nvrNetworkOverview

            title: "Network(NVR)"
            status: healthMonitoringViewModel.deviceNetworkErrorCount === 0
            errorCountText: healthMonitoringViewModel.deviceNetworkErrorCount
        }

        OverviewItem{
            id: cameraNetworkOverview

            title: "Network(Camera)"
            status: healthMonitoringViewModel.cameraNetworkErrorCount === 0
            errorCountText: healthMonitoringViewModel.cameraNetworkErrorCount
        }

        OverviewItem{
            id: nvrUpdateOverview

            title: "Update(NVR)"
            status: healthMonitoringViewModel.deviceFirmwareUpdateCount === 0
            errorCountText: healthMonitoringViewModel.deviceFirmwareUpdateCount
        }

        OverviewItem{
            id: cameraUpdateOverview

            title: "Update(Camera)"
            status: healthMonitoringViewModel.cameraFirmwareUpdateCount === 0
            errorCountText: healthMonitoringViewModel.cameraFirmwareUpdateCount
        }
    }
}
