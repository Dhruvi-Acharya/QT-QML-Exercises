import QtQuick 2.15
import QtQuick.Controls 2.12
//import QtQuick.Controls.impl 2.12

TabButton{
    id: control
    checkable: true

    property var imageNormal
    property var imageSelected
    property var imageHover
    property bool rightBarVisible : false
    font.pixelSize: 11

    /* revised by hh3.kim (2021-12-15)
    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon.source: control.checked? control.imageSelected : (control.hovered? control.imageHover : control.imageNormal)

        text: control.text
        font.pixelSize: control.fontPixelSize
        color: control.checked? WisenetGui.color_primary: (control.hovered? WisenetGui.contrast_00_white : WisenetGui.contrast_04_light_grey)
    }
    */

    Component.onCompleted: {
        if(buttonText.paintedWidth > 80)
            buttonText.width = 80
        else
            buttonText.width = buttonText.paintedWidth
    }


    contentItem: Item{
        anchors.fill: parent
        Rectangle {
            id: contentRect
            width: buttonImage.width + buttonText.width + 4; height: 14
            color: "transparent"
            anchors.centerIn: parent
            Image {
                id: buttonImage
                width: 14; height: 14
                sourceSize: "14x14"
                source: {
                    if(checked)
                        return control.imageSelected
                    else if(hovered)
                        return control.imageHover
                    return control.imageNormal
                }
            }
            Text {
                id: buttonText
                anchors.left: buttonImage.right
                anchors.leftMargin: 4
                anchors.verticalCenter: contentRect.verticalCenter
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                text: control.text
                font: control.font

                color: {
                    if(checked)
                        return WisenetGui.color_primary
                    else if(hovered)
                        return WisenetGui.contrast_00_white
                    return WisenetGui.contrast_04_light_grey
                }

            }
        }
    }

    background: Rectangle {
        id: bg
        implicitHeight: 42
        color: WisenetGui.contrast_09_dark

        Rectangle{
            id: rightBar

            width: 1
            color: WisenetGui.contrast_08_dark_grey

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: 14
            anchors.bottomMargin: 14

            visible: control.rightBarVisible
        }
    }
}

/*
TabButton {
    id : control
    text: "Tab Button"
    checkable: true

    width: 100
    height: 42


    property var imageNormal

    padding: 0
    spacing: 0

    contentItem: Rectangle{
        id: itemContent

        anchors.fill: parent
        color: "green"

        Item{
            anchors.centerIn: parent

            Image{
                id: buttonIcon
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width:14
                height:14
                source: imageNormal
            }

            Text {
                anchors.left: buttonIcon.right
                anchors.leftMargin: 4
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                text: control.text
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter

                color: (checked? WisenetGui.textBoxSelectColor: (control.hovered? WisenetGui.hoveredTextColor : WisenetGui.tabDefaultTextColor))

                elide: Text.ElideRight
            }
        }
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 42
        color: WisenetGui.contrast_11_bg
    }
}
*/
