import QtQuick 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0

Popup{
    id: contentSelectPopup

    width: 242
    height: 32 * searchMenuCount

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property int searchMenuCount : 3

    signal currentIndexChanged(var idx)

    Rectangle{
        anchors.fill: parent

        border.color: WisenetGui.contrast_07_grey
        border.width: 1
        color: WisenetGui.contrast_09_dark

        Rectangle{
            id: eventSearchMenu

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: (parent.height - searchMenuCount + 1) / searchMenuCount
            color: eventSearchMenuMouseArea.containsMouse ? WisenetGui.color_primary : "transparent"

            Text{
                anchors.fill: parent
                anchors.leftMargin: 20
                verticalAlignment: Text.AlignVCenter
                color: WisenetGui.contrast_00_white
                text: WisenetLinguist.eventSearch

                wrapMode: Text.Wrap
            }

            MouseArea{
                id: eventSearchMenuMouseArea

                hoverEnabled: true
                anchors.fill: parent

                onClicked: {
                    contentSelectPopup.currentIndexChanged(0)
                    contentSelectPopup.close()
                }
            }
        }

        Rectangle{
            id: menuSeparator

            anchors.top: eventSearchMenu.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color:  WisenetGui.contrast_07_grey
        }

        Rectangle{
            id: objectSearchMenu

            anchors.top: menuSeparator.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: (parent.height - searchMenuCount + 1) / searchMenuCount
            color: objectSearchMenuMouseArea.containsMouse ? WisenetGui.color_primary : "transparent"

            Text{
                anchors.fill: parent
                anchors.leftMargin: 20
                verticalAlignment: Text.AlignVCenter
                color: WisenetGui.contrast_00_white
                text: WisenetLinguist.objectSearch

                wrapMode: Text.Wrap
            }

            MouseArea{
                id: objectSearchMenuMouseArea

                hoverEnabled: true
                anchors.fill: parent

                onClicked: {
                    contentSelectPopup.currentIndexChanged(1)
                    contentSelectPopup.close()
                }
            }
        }

        Rectangle{
            id: smartSearchMenuSeparator
            anchors.top: objectSearchMenu.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color:  WisenetGui.contrast_07_grey
        }

        Rectangle{
            id: smartSearchMenu

            anchors.top: smartSearchMenuSeparator.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: (parent.height - searchMenuCount + 1) / searchMenuCount
            color: smartSearchMenuMouseArea.containsMouse ? WisenetGui.color_primary : "transparent"

            Text{
                anchors.fill: parent
                anchors.leftMargin: 20
                verticalAlignment: Text.AlignVCenter
                color: WisenetGui.contrast_00_white
                text: WisenetLinguist.smartSearch

                wrapMode: Text.Wrap
            }

            MouseArea{
                id: smartSearchMenuMouseArea

                hoverEnabled: true
                anchors.fill: parent

                onClicked: {
                    contentSelectPopup.currentIndexChanged(2)
                    contentSelectPopup.close()
                }
            }
        }

//        Rectangle{
//            id: textSearchMenuSeparator
//            anchors.top: smartSearchMenu.bottom
//            anchors.left: parent.left
//            anchors.right: parent.right
//            height: 1
//            color:  WisenetGui.contrast_07_grey
//        }

//        Rectangle{
//            id: textSearchMenu

//            anchors.top: textSearchMenuSeparator.bottom
//            anchors.left: parent.left
//            anchors.right: parent.right
//            height: (parent.height - 3) / 4
//            color: textSearchMenuMouseArea.containsMouse ? WisenetGui.color_primary : "transparent"

//            Text{
//                anchors.fill: parent
//                anchors.leftMargin: 20
//                verticalAlignment: Text.AlignVCenter
//                color: WisenetGui.contrast_00_white
//                text: WisenetLinguist.textSearch

//                wrapMode: Text.Wrap
//            }

//            MouseArea{
//                id: textSearchMenuMouseArea

//                hoverEnabled: true
//                anchors.fill: parent

//                onClicked: {
//                    contentSelectPopup.currentIndexChanged(3)
//                    contentSelectPopup.close()
//                }
//            }
//        }
    }
}
