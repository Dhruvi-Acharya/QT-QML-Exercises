import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

Popup{
    id: resourceUsageView
    topInset: 0
    leftInset: 0
    rightInset: 0
    bottomInset: 0
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    width: 333
    height: 76

    background: Rectangle {
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1
    }

    visible: false

    Rectangle{
        anchors.fill: parent
        anchors.margins: 1

        color: WisenetGui.contrast_08_dark_grey

        Text{
            id: cpuText

            width: 30
            height: 16

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 16
            anchors.leftMargin: 16

            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter

            text: "CPU"
        }

        Text{
            id: ramText

            width: 30
            height: 16

            anchors.top: cpuText.bottom
            anchors.left: parent.left
            anchors.topMargin: 12
            anchors.leftMargin: 16

            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter

            text: "RAM"
        }


        ProgressBar{
            id: cpuUsage
            anchors.top: parent.top
            anchors.left: cpuText.right
            anchors.topMargin: 16
            anchors.leftMargin: 12

            width: 126
            height: 16

            from: 0
            to: 100
            value: resourceUsageViewModel.cpuUsage

            background: Rectangle {
                anchors.fill: parent
                color: WisenetGui.contrast_07_grey
            }

            contentItem: Item {
                anchors.fill: parent
                Rectangle {
                    width: cpuUsage.visualPosition * parent.width
                    height: parent.height
                    color: (resourceUsageViewModel.cpuUsage >= 0 && resourceUsageViewModel.cpuUsage <=60) ? "#8DC63F" :
                                                                                                            (resourceUsageViewModel.cpuUsage > 60 && resourceUsageViewModel.cpuUsage < 90) ? WisenetGui.color_primary :
                                                                                                                                                                                             WisenetGui.color_accent
                }

                /*
                Text{
                    anchors.centerIn: parent
                    text: resourceUsageViewModel.cpuUsageString + " %"
                    color: WisenetGui.contrast_00_white

                    font.pixelSize: 12
                }*/
            }
        }

        ProgressBar{
            id: ramUsage
            anchors.top: cpuUsage.bottom
            anchors.left: ramText.right
            anchors.topMargin: 12
            anchors.leftMargin: 12

            width: 126
            height: 16

            from: 0
            to: 100
            value: resourceUsageViewModel.memoryUsage

            background: Rectangle {
                anchors.fill: parent
                color: WisenetGui.contrast_07_grey
            }

            contentItem: Item {
                anchors.fill: parent
                Rectangle {
                    width: ramUsage.visualPosition * parent.width
                    height: parent.height
                    color: (resourceUsageViewModel.memoryUsage >= 0 && resourceUsageViewModel.memoryUsage <=60) ? "#8DC63F" :
                                                                                                                  (resourceUsageViewModel.memoryUsage > 60 && resourceUsageViewModel.memoryUsage < 90) ? WisenetGui.color_primary :
                                                                                                                                                                                                         WisenetGui.color_accent
                }
            }
        }

        Text{
            id: cpuUsageText

            width: 40
            height: 16
            anchors.top: parent.top
            anchors.left: cpuUsage.right
            anchors.topMargin: 16
            anchors.leftMargin: 10

            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight

            text: resourceUsageViewModel.cpuUsageString + " %"
        }

        Text{
            id: memoryUsageText

            width: 40
            height: 16
            anchors.top: cpuUsageText.bottom
            anchors.left: ramUsage.right
            anchors.topMargin: 12
            anchors.leftMargin: 10

            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight

            text: resourceUsageViewModel.memoryUsageString + " %"
        }

        Text{
            id: memoryUsageDetailsText

            width: 40
            height: 16
            anchors.left: memoryUsageText.right
            anchors.bottom: memoryUsageText.bottom
            anchors.leftMargin: 5

            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 11
            verticalAlignment: Text.AlignVCenter

            text: "(" + resourceUsageViewModel.usedMemory + " / " + resourceUsageViewModel.totalMemory + "GB)"
        }

    }
}
