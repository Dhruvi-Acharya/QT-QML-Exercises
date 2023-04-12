import QtQuick 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0

Rectangle{
    id: deviceInformationList

    property var deviceListProxy
    property int verticalMargin : 10
    radius: 10
    color: "transparent"
    border.width: 1
    border.color: WisenetGui.contrast_08_dark_grey

    component DeviceDetailsTextSet: Rectangle{
        id: detailsTextSet

        property alias title: titleText.text
        property alias content: contentText.text
        property alias imageVisible: deviceDetailsIcon.visible

        height: 36
        width: (deviceDetailsIcon.visible === true ? deviceDetailsIcon.width+10 : 0) +
               (titleText.contentWidth >= contentText.contentWidth ? titleText.contentWidth : contentText.contentWidth)

        color: "transparent"


        Rectangle{
            id: deviceDetailsIcon
            height: 36
            width: visible ? 36 : 0

            color: "#D9D9D9"
        }

        Text{
            id: titleText

            anchors.top: parent.top
            anchors.left: deviceDetailsIcon.right
            anchors.topMargin: 1
            anchors.leftMargin: deviceDetailsIcon.visible ? 10 : 0

            color: WisenetGui.contrast_05_grey
            font.pixelSize: 12
        }

        Text{
            id: contentText

            anchors.top: titleText.bottom
            anchors.left: deviceDetailsIcon.right
            anchors.topMargin: 2
            anchors.leftMargin: deviceDetailsIcon.visible ? 10 : 0

            color: WisenetGui.contrast_01_light_grey
            font.pixelSize: 14
        }
    }

    component CameraDetailsTextSet: Rectangle{
        id: cameraDetailsTextSet

        property alias title: cameraTitleText.text
        property alias content: cameraContentText.text
        property alias imageVisible: cameraDetailsIcon.visible
        property alias imageColor: cameraDetailsIcon.color

        height: 30
        color: "transparent"
        width: (cameraDetailsIcon.visible === true ? cameraDetailsIcon.width+6 : 0) +
               (cameraTitleText.contentWidth >= cameraContentText.contentWidth ? cameraTitleText.contentWidth : cameraContentText.contentWidth)

        Rectangle{
            id: cameraDetailsIcon
            height: 30
            width: visible ? 30 : 0

            color: "#D9D9D9"
        }

        Text{
            id: cameraTitleText

            anchors.top: parent.top
            anchors.left: cameraDetailsIcon.right
            anchors.leftMargin: cameraDetailsIcon.visible ? 6 : 0

            color: WisenetGui.contrast_05_grey
            font.pixelSize: 12
        }

        Text{
            id: cameraContentText

            anchors.top: cameraTitleText.bottom
            anchors.left: cameraDetailsIcon.right
            anchors.leftMargin: cameraDetailsIcon.visible ? 6 : 0

            color: WisenetGui.contrast_01_light_grey
            font.pixelSize: 14
        }
    }

    component CameraSummary: Rectangle{
        id: cameraSummary

        property alias title : summaryTitle.text
        property alias count : channelCount.text

        radius: 10
        color: "transparent"
        border.width: 1
        height: 20
        width: summaryTitle.contentWidth + channelCount.contentWidth + 20 + 6

        Text{
            id: summaryTitle

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 2
            anchors.leftMargin: 10

            font.pixelSize: 12
            color: WisenetGui.contrast_05_grey
        }

        Text{
            id: channelCount

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 2
            anchors.rightMargin: 10

            font.pixelSize: 12
            color: WisenetGui.contrast_01_light_grey
        }
    }

    function getFirmwareUpdateString(enumValue){
        if(enumValue === 0)
            return "Latest"
        else if(enumValue === 1)
            return "Need to update"
        else
            return "Unknown"
    }

    function getConnectionType(connectionType){
        if(connectionType === 0)
            return "StaticIPv4"
        else if(connectionType === 1)
            return "WisenetDDNS"
        else if(connectionType === 2)
            return "URL"
        else if(connectionType === 3)
            return "P2P"
        else if(connectionType === 4)
            return "P2P_RELAY"
        else if(connectionType === 5)
            return "StaticIPv6"
        else if(connectionType === 6)
            return "S1 DDNS"
        else if(connectionType === 7)
            return "S1 SIP"
        else if(connectionType === 8)
            return "S1 SIP RELAY"

        return "-"
    }

    function getChartData(percent){
        var a, b

        if(percent === 100){
            a = 100
            b = 0.001
        }
        else{
            a = percent
            b = 100 - percent
        }

        return {
            datasets: [{
                    data: [
                        a,b
                    ],
                    backgroundColor: [
                        '#00ff00'
                    ],
                    borderWidth:0,
                    weight:0.2
                }]
        }
    }

    Text{
        id: deviceInformationTitle

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 15
        anchors.leftMargin: 15

        text: "Device information"
        color: WisenetGui.contrast_02_light_grey
    }

    Flickable{
        id: deviceList
        anchors.top: deviceInformationTitle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 15
        anchors.leftMargin: 15
        anchors.bottomMargin: 15

        clip:true

        contentHeight: deviceListContainer.height

        ScrollBar.vertical: WisenetScrollBar {
            id: verticalScrollBar

            parent: deviceList

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            stepSize: verticalScrollBar.visualSize/4
        }

        Column{
            id: deviceListContainer

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 10

            Repeater{
                model: deviceListProxy

                Rectangle{
                    id: deviceInformation

                    width: deviceListContainer.width - 15
                    height: 15 +
                            deviceIcon.height + 4 +
                            deviceDetailsRect.height + verticalMargin +
                            cameraListRect.height + verticalMargin

                    color: WisenetGui.contrast_09_dark
                    border.color: WisenetGui.contrast_08_dark_grey
                    border.width: 1
                    radius: 10

                    // Device name
                    Image{
                        id: deviceIcon

                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 7
                        anchors.leftMargin: 15

                        width: 36
                        height: 36
                        sourceSize.width: 36
                        sourceSize.height: 36

                        source: "qrc:/Assets/Icon/Dashboard/title_recorder.svg"
                    }

                    Text{
                        id: deviceModelNameText

                        anchors.top: parent.top
                        anchors.left: deviceIcon.right
                        anchors.topMargin: 15
                        anchors.leftMargin: 13

                        color: WisenetGui.contrast_05_grey
                        text: deviceModelName

                        font.pixelSize: 14
                    }

                    Text{
                        id: deviceNameText

                        anchors.top: parent.top
                        anchors.left: deviceModelNameText.right
                        anchors.topMargin: 15
                        anchors.leftMargin: 7

                        color: WisenetGui.contrast_01_light_grey
                        text: deviceName

                        font.pixelSize: 14
                    }

                    // Icons
                    Rectangle{
                        id: deviceDetailsRect

                        anchors.top: deviceIcon.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 4
                        anchors.leftMargin: 15
                        anchors.rightMargin: 15
                        height: deviceInfoFlow.height + 30

                        color: WisenetGui.contrast_10_dark
                        border.color: WisenetGui.contrast_08_dark_grey
                        border.width: 1
                        radius: 10

                        Flow{
                            id: deviceInfoFlow

                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.topMargin: 15
                            anchors.leftMargin: 15
                            anchors.rightMargin: 15
                            spacing: 15

                            Rectangle{
                                id: diskRect

                                height: 36
                                width: diskUsageChart.width + 10 + diskStatusText.width
                                color: "transparent"

                                WisenetChart{
                                    id: diskUsageChart

                                    width: 36
                                    height: 36

                                    anchors.top: parent.top
                                    anchors.left: parent.left

                                    animationDuration: 0
                                    chartType: 'doughnut'

                                    chartData: getChartData(diskUsagePercent)

                                    chartOptions: {return {
                                            maintainAspectRatio: false,
                                            responsive: true,
                                            cutoutPercentage: 80,
                                            legend:{
                                                display: false
                                            },
                                            tooltips:{
                                                enabled: false
                                            }
                                        }
                                    }
                                }

                                Text{
                                    id: diskUsageText

                                    anchors.centerIn: diskUsageChart

                                    text: diskUsagePercent + "%"

                                    color: WisenetGui.contrast_01_light_grey
                                    font.pixelSize: 10
                                }

                                Text{
                                    id: diskText
                                    anchors.top: parent.top
                                    anchors.left: diskUsageChart.right
                                    anchors.topMargin: 1
                                    anchors.leftMargin: 10

                                    text: "Disk"

                                    color: WisenetGui.contrast_05_grey
                                    font.pixelSize: 12
                                }

                                Text{
                                    id: diskStatusText
                                    anchors.top: diskText.bottom
                                    anchors.left: diskUsageChart.right
                                    anchors.topMargin: 2
                                    anchors.leftMargin: 10


                                    text: "Normal"

                                    color: WisenetGui.contrast_01_light_grey
                                    font.pixelSize: 12
                                }
                            }

                            DeviceDetailsTextSet{
                                id: firmwareSet

                                title: "Firmware"
                                content: getFirmwareUpdateString(firmwareVersion)
                            }

                            DeviceDetailsTextSet{
                                id: timezoneInfo

                                title: "Timezone"
                                content: timezone
                            }


                            Rectangle{
                                id: connectionInfoRow

                                height: 36
                                width: 260
                                color: "transparent"

                                DeviceDetailsTextSet{
                                    id: ipInfo

                                    anchors.top: parent.top
                                    anchors.left: parent.left

                                    title: "IP"
                                    content: ipAddress
                                }

                                DeviceDetailsTextSet{
                                    id: portInfo

                                    anchors.top: parent.top
                                    anchors.left: ipInfo.right
                                    anchors.leftMargin: 10

                                    imageVisible: false

                                    title: "Port"
                                    content: portNumber
                                }


                                DeviceDetailsTextSet{
                                    id: connectionTypeInfo

                                    anchors.top: parent.top
                                    anchors.left: portInfo.right
                                    anchors.leftMargin: 10

                                    imageVisible: false

                                    title: "Type"
                                    content: getConnectionType(connectionType)
                                }
                            }

                            DeviceDetailsTextSet{
                                id: macAddressInfo

                                title: "MAC address"
                                content: macAddress
                            }

                            Rectangle{
                                id:recordingInfoRow

                                height: 36
                                width: 396
                                color: "transparent"

                                DeviceDetailsTextSet{
                                    id: dualRecordingInfo

                                    anchors.top: parent.top
                                    anchors.left: parent.left

                                    title: "Dual recording"
                                    content: dualRecordingEnabled ? WisenetLinguist.on : WisenetLinguist.off
                                }

                                DeviceDetailsTextSet{
                                    id: recordingPeriodInfo

                                    anchors.top: parent.top
                                    anchors.left: dualRecordingInfo.right
                                    anchors.leftMargin: 10

                                    imageVisible: false

                                    title: "Recording"
                                    content: recordingPeriod
                                }
                            }
                        }
                    }

                    // Button
                    Rectangle{
                        id: cameraListRect

                        height: 36 + (cameraList.visible ? cameraList.height + 15 : 0)

                        anchors.top: deviceDetailsRect.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: verticalMargin
                        anchors.leftMargin: 15
                        anchors.rightMargin: 15

                        color: WisenetGui.contrast_10_dark
                        border.color: WisenetGui.contrast_08_dark_grey
                        border.width: 1
                        radius: 10

                        Image{
                            id: cameraImage

                            width: 30
                            height: 30

                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.topMargin: 3
                            anchors.leftMargin: 15

                            sourceSize.width: 30
                            sourceSize.height: 30
                            source: "qrc:/Assets/Icon/Dashboard/title_camera.svg"
                        }

                        Text{
                            id: channelsText

                            anchors.top: parent.top
                            anchors.left: cameraImage.right
                            anchors.topMargin: 9
                            anchors.leftMargin: 10

                            color: WisenetGui.contrast_01_light_grey
                            text: "Channels"
                            font.pixelSize: 14
                            verticalAlignment: Text.AlignVCenter
                        }

                        Row{
                            height: 20
                            anchors.top: parent.top
                            anchors.right: openButton.left
                            anchors.topMargin: 8
                            anchors.rightMargin: 13
                            spacing: 10

                            CameraSummary{
                                id: normalSummary
                                title: "Good"
                                border.color: WisenetGui.color_secondary
                                count: normalChannelCount
                            }

                            CameraSummary{
                                id: networkSummary
                                title: "Network"
                                border.color: WisenetGui.color_accent
                                count: networkErrorChannelCount
                            }

                            CameraSummary{
                                id: firmwareSummary
                                title: "Firmware"
                                border.color: WisenetGui.color_accent
                                count: firmwareUpdateChannelCount
                            }
                        }


                        Image{
                            id: openButton

                            width: 12
                            height: 12
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.topMargin: 12
                            anchors.rightMargin: 15

                            sourceSize.width: 12
                            sourceSize.height: 12
                            source: cameraList.visible ? WisenetImage.tree_arrow_top : WisenetImage.tree_arrow_bottom

                            MouseArea{
                                anchors.fill: parent
                                onClicked: cameraList.visible = !cameraList.visible
                            }
                        }

                        // Camera
                        Flickable{
                            id: cameraList

                            anchors.top: cameraImage.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.topMargin: 10
                            anchors.leftMargin: 15
                            anchors.rightMargin: 5

                            clip: true
                            contentHeight: cameraListContainer.height + 15

                            // 1개일 떄 꽉 채움
                            height: deviceListProxy.rowCount() > 1 ?
                                        450 : deviceInformationList.height -
                                        (15 + deviceInformationTitle.height + 15 +
                                         7 + deviceIcon.height +
                                         15 + deviceDetailsRect.height + deviceIcon.height + 4 +
                                         15 + 15 + 15)

                            visible: false

                            ScrollBar.vertical: WisenetScrollBar {
                                id: cameraListVerticalScrollBar

                                parent: cameraList

                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                anchors.right: parent.right
                            }

                            Grid{
                                id: cameraListContainer

                                columns: 2
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                spacing: 15

                                Repeater{
                                    model: cameraListData

                                    Rectangle{
                                        id: cameraInformation

                                        width: (cameraList.width - 30) / 2
                                        height: cameraDetailsFlow.height + 35
                                        radius: 10

                                        color: WisenetGui.contrast_08_dark_grey
                                        border.color: WisenetGui.contrast_07_grey
                                        border.width: 1

                                        Rectangle{
                                            id: disconnectedBadge

                                            anchors.top: parent.top
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.topMargin: 1
                                            anchors.leftMargin: 1
                                            anchors.rightMargin: 1
                                            height: 30
                                            radius: 10
                                            color: WisenetGui.color_accent_dark

                                            visible: !cameraStatus

                                            Text{
                                                anchors.top: parent.top
                                                anchors.horizontalCenter: parent.horizontalCenter

                                                color: WisenetGui.contrast_01_light_grey
                                                text: "Disconnected"
                                                font.pixelSize: 10
                                            }

                                            Rectangle{
                                                id: disconnectedBadgeBorder
                                                anchors.top: parent.top
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                anchors.topMargin: 16
                                                height: 1
                                                color: WisenetGui.contrast_07_grey
                                            }
                                        }

                                        Rectangle{
                                            id: halfRect

                                            anchors.top: parent.top
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.topMargin: 18
                                            anchors.leftMargin: 1
                                            anchors.rightMargin: 1
                                            color: WisenetGui.contrast_08_dark_grey

                                            height: 20
                                        }

                                        Flow{
                                            id: cameraDetailsFlow

                                            anchors.top: parent.top
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.topMargin: 18
                                            anchors.leftMargin: 15
                                            anchors.rightMargin: 15
                                            spacing: 10

                                            CameraDetailsTextSet{
                                                id: cameraModelInfo

                                                imageColor: cameraStatus ? "lightblue" : "red"

                                                title: cameraModelName
                                                content: cameraName
                                            }

                                            CameraDetailsTextSet{
                                                id: cameraFirmwareInfo

                                                title: "Firmware"
                                                content: getFirmwareUpdateString(cameraFirmwareVersion)
                                            }

                                            CameraDetailsTextSet{
                                                id: cameraIpInfo

                                                width: 165
                                                title: "IP"
                                                content: cameraIp
                                            }

                                            CameraDetailsTextSet{
                                                id: primaryProfileInfo

                                                imageVisible: false
                                                width: 165

                                                title: "Primary profile"
                                                content: cameraPrimaryProfile
                                            }

                                            CameraDetailsTextSet{
                                                id: secondaryProfileInfo

                                                width: 165
                                                imageVisible: false
                                                title: "Secondary profile"
                                                content: cameraSecondaryProfile
                                            }

                                            CameraDetailsTextSet{
                                                id: recordingProfileInfo

                                                width: 165
                                                imageVisible: false
                                                title: "Recording profile"
                                                content: cameraRecordingProfile
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
    }
}
