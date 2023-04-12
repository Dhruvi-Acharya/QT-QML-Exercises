import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

MenuItem {
    id: menuItem

    property bool  singleCheckable : false
    property color hoverColor : WisenetGui.color_primary
    property color textColor : WisenetGui.contrast_02_light_grey
    property color textOverColor : WisenetGui.contrast_01_light_grey
    property alias displayShotcutOnly : dummyShortcut.sequence
    readonly property alias shortCutText: shortcutText.text

    leftPadding: 15
    rightPadding: 15
    topPadding: 6
    bottomPadding: 6

    spacing:6
    height: visible ? implicitHeight : 0
    font.pixelSize: 12

    readonly property int implicitTextWidth : {
        var calcWidth = titleText.implicitWidth + shortcutText.implicitWidth + leftPadding + rightPadding + 10;
        if (indicator.visible)
            calcWidth += indicator.width + leftPadding
        if (arrow.visible)
            calcWidth += arrow.width
        return calcWidth
    }

    Shortcut {
        id: dummyShortcut
        enabled: false
    }
    onActionChanged: {
        if (menuItem.action.shortcut)
            dummyShortcut.sequence = menuItem.action.shortcut
    }

    indicator: Image {
        x: menuItem.leftPadding
        y: menuItem.topPadding + (menuItem.availableHeight - height) / 2
        height: 12
        width: 12
        fillMode: Image.PreserveAspectFit

        visible: menuItem.checkable
        source: menuItem.checked ? (singleCheckable?"images/check_singleOn.svg":"images/check_on.svg") : (singleCheckable?"images/check_singleOff.svg":"images/check_off.svg")
        sourceSize: "12x12"
    }

    arrow : Image {
        x: menuItem.width - width - menuItem.rightPadding
        y: menuItem.topPadding + (menuItem.availableHeight - height) / 2

        visible: menuItem.subMenu
        width: 12
        height: 12
        sourceSize: "12x12"
        source: hovered? "images/arrow_hover.svg" :  "images/arrow_normal.svg";
    }

    contentItem: Item {
        readonly property real arrowPadding: menuItem.subMenu && menuItem.arrow ? menuItem.arrow.width + menuItem.spacing : 0
        readonly property real indicatorPadding: menuItem.checkable && menuItem.indicator ? menuItem.indicator.width + menuItem.spacing : 0

        Text {
            id: titleText
            anchors.fill: parent
            leftPadding: parent.indicatorPadding
            rightPadding: parent.arrowPadding + shotcutItem.width
            //text: dummyShortcut.nativeText ? menuItem.text + "  (" + dummyShortcut.nativeText + ")" : menuItem.text
            text: menuItem.text
            font: menuItem.font
            opacity: enabled ? 1.0 : 0.4
            color: menuItem.highlighted ? menuItem.textOverColor : menuItem.textColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        Item {
            id: shotcutItem
            visible: dummyShortcut.nativeText ? true : false
            //anchors.leftMargin: 2
            anchors.right: parent.right
            anchors.top:titleText.top
            anchors.bottom: titleText.bottom
            width : visible ? shortcutText.width : 0
            Text {
                id: shortcutText
                x: 0; y: 0;
                height: parent.height
                width: implicitWidth + 2
                text: dummyShortcut.nativeText
                font.pixelSize: 8
                color: menuItem.highlighted ? menuItem.textOverColor : menuItem.textColor
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
            }
        }


    }

    background: Rectangle {
        id: bg
        implicitWidth: 180
        implicitHeight: 28
        x: 1
        y: 1
        width: menuItem.width - 2
        height: menuItem.height - 2
        color: menuItem.highlighted ? menuItem.hoverColor : "transparent"
    }
}
