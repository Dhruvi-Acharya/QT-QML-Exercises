import QtQuick 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0

Rectangle {
    id: alertLog

    radius: 10
    color: "transparent"
    border.color: WisenetGui.contrast_08_dark_grey
    border.width: 1

    component TabTitle: Rectangle{
        id: tabTitle

        property alias title: titleText.text

        signal clicked()

        width: titleText.contentWidth
        height: 16
        color: "transparent"


        Text{
            id: titleText

            anchors.fill: parent
            color: WisenetGui.contrast_01_light_grey

            verticalAlignment: Text.AlignVCenter

            font.pixelSize: 12
        }

        MouseArea{
            anchors.fill: parent

            onClicked: tabTitle.clicked()
        }
    }

    component TabSeperator: Rectangle{
        id: tabSeperator

        width: seperator.contentWidth + 20
        height: 16
        color: "transparent"


        Text{
            id: seperator

            text: "|"

            anchors.fill: parent
            color: WisenetGui.contrast_06_grey

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            font.pixelSize: 12
        }
    }

    Text{
        id: alertLogTitle

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 15
        anchors.leftMargin: 20

        text: "Alerts notification"
        color: WisenetGui.contrast_02_light_grey
        font.pixelSize: 16
    }

    Rectangle{
        id: alertLogMenus

        anchors.top: parent.top
        anchors.topMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 20

        width: 400
        height: 16

        color: "transparent"

        Row{
            id: tabs
            anchors.top: parent.top
            anchors.right: parent.right
            TabTitle{
                title: "All"
            }

            TabSeperator{}

            TabTitle{
                title: "Status(NVR)"
            }

            TabSeperator{}

            TabTitle{
                title: "Disk(NVR)"
            }

            TabSeperator{}

            TabTitle{
                title: "Network"
            }

            TabSeperator{}

            TabTitle{
                title: "Firmware"
            }
        }
    }

    WisenetSearchBox{
        id: searchTextBox

        anchors.top: alertLogMenus.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 20
        width: 200
        height: 31
        bgColor: WisenetGui.contrast_09_dark

        onSearchTextChanged:{
            console.log("Alert log list - onSearchTextChanged :" + text)
            //bookmarkListViewModel.bookmarkListSortFilterProxyModel.setFilterText(text)
        }
    }

    Rectangle{
        id: clearButton

        width: 20
        height: 20
        anchors.top: alertLogTitle.bottom
        anchors.right: sortButton.left
        anchors.topMargin: 12
        anchors.rightMargin: 8
        radius: 4
        color: "#D9D9D9"
    }

    Rectangle{
        id: sortButton

        width: 20
        height: 20
        anchors.top: alertLogTitle.bottom
        anchors.right: parent.right
        anchors.topMargin: 12
        anchors.rightMargin: 24
        radius: 4
        color: "#D9D9D9"
    }

    Rectangle{
        id: logContainer

        property int selectedMenu : 0

        anchors.top: sortButton.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.topMargin: 5
        anchors.leftMargin: 20
        anchors.rightMargin: 7
        anchors.bottomMargin: 20

        color: "transparent"

        ListView{
            id: logList

            anchors.fill: parent
            model: 30
            clip:true

            delegate: Rectangle {
                id: log

                width: logList.width
                height: 52
                color: "transparent"

                Text{
                    id: timeText
                    width: 47

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 18
                    color: WisenetGui.contrast_01_light_grey
                    font.pixelSize: 12

                    text: "12:00:00"
                }

                Rectangle{
                    id: errorIconRect

                    anchors.top: parent.top
                    anchors.left: timeText.right
                    anchors.topMargin: 13
                    anchors.leftMargin: 15
                    width: 63
                    height: 26

                    radius: 4
                    color: "transparent"
                    border.color: WisenetGui.contrast_08_dark_grey
                    border.width: 1

                    Text{
                        id: errorType

                        anchors.centerIn: parent
                        font.pixelSize: 12
                        color: WisenetGui.contrast_01_light_grey

                        text: "No disk"
                    }
                }

                Rectangle{
                    id: errorMessageRect
                    color: "transparent"

                    anchors.top: parent.top
                    anchors.left: errorIconRect.right
                    anchors.right: settingButton.left
                    anchors.topMargin: 10
                    anchors.leftMargin: 14

                    Text{
                        id: errorText

                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        color: WisenetGui.contrast_01_light_grey
                        verticalAlignment: Text.AlignVCenter

                        text: "No NVR's HDD connected"
                        font.pixelSize: 12
                    }

                    Text{
                        id: recommendedAction

                        anchors.top: errorText.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.topMargin: 1
                        color: WisenetGui.contrast_04_light_grey
                        verticalAlignment: Text.AlignVCenter

                        text: "Check the device"
                        font.pixelSize: 10
                    }
                }

                Rectangle{
                    id: settingButton

                    width: 20
                    height: 20
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 15
                    anchors.rightMargin: 17

                    color: WisenetGui.contrast_04_light_grey
                }

                Rectangle{
                    id: bottomBorder
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 1

                    color: WisenetGui.contrast_08_dark_grey
                }
            }
            ScrollBar.vertical: WisenetScrollBar {}
        }
    }
}
