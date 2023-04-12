import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

MenuItem {
    id: menuItem

    property alias openInNewTabVisibile: openInNewTabBtn.visible
    property alias openInNewWindowVisible: openInNewWindowBtn.visible
    property alias editVisible: editBtn.visible
    property alias deleteVisible: deleteBtn.visible
    property alias settingsVisible: settingsBtn.visible
    property alias settingsTooltipText: settingsTooltip.text

    property bool useOpenToolTip: false
    property bool isSeqeunceType: false
    signal openInNewTabClicked();
    signal openInNewWindowClicked();
    signal editClicked();
    signal deleteClicked();
    signal settingsClicked();

    padding:1
    spacing:1

    height: visible ? implicitHeight : 0;

    component WisenetMediaQuickButtonItem: WisenetMediaButtonItem {
        height: 36
        width: 36
        imgHeight: 18//hover ? 24 : 18
        imgWidth: imgHeight
        imgSourceSize : "18x18"
        normalColor: "transparent"
        pressColor: WisenetGui.contrast_08_dark_grey
        hoverColor: WisenetGui.contrast_08_dark_grey
        bgOpacity: 1.0
    }

    contentItem: Row {
        id: buttons
        spacing:0
        WisenetMediaQuickButtonItem {
            id: openInNewTabBtn
            normalSource: "images/half_Default.svg"
            pressSource: "images/half_Press.svg"
            hoverSource: "images/half_Hover.svg"
            onMouseClicked: {
                menuItem.openInNewTabClicked();
                triggered();
            }
            WisenetMediaToolTip {
                id: openTooltip
                visible: parent.hover
                text: useOpenToolTip ? WisenetLinguist.open : WisenetLinguist.openInNewTab
            }
        }
        WisenetMediaQuickButtonItem {
            id: openInNewWindowBtn
            normalSource: "images/full_Default.svg"
            pressSource: "images/full_Press.svg"
            hoverSource: "images/full_Hover.svg"
            onMouseClicked: {
                menuItem.openInNewWindowClicked();
                triggered();
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.openInNewWindow
            }
        }
        WisenetMediaQuickButtonItem {
            id: editBtn
            normalSource: "images/edit_Default.svg"
            pressSource: "images/edit_Press.svg"
            hoverSource: "images/edit_Hover.svg"
            onMouseClicked: {
                menuItem.editClicked();
                triggered();
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.edit
            }
        }
        WisenetMediaQuickButtonItem {
            id: deleteBtn
            normalSource: "images/delete_Default.svg"
            pressSource: "images/delete_Press.svg"
            hoverSource: "images/delete_Hover.svg"
            onMouseClicked: {
                menuItem.deleteClicked();
                triggered();
            }
            WisenetMediaToolTip {
                visible: parent.hover
                text: WisenetLinguist.remove
            }
        }
        WisenetMediaQuickButtonItem {
            id: settingsBtn
            normalSource: isSeqeunceType ? WisenetImage.sequence_edit_default : "images/settings_Default.svg"
            pressSource: isSeqeunceType ? WisenetImage.sequence_edit_dim : "images/settings_Press.svg"
            hoverSource: isSeqeunceType ? WisenetImage.sequence_edit_hover : "images/settings_Hover.svg"
            onMouseClicked: {
                menuItem.settingsClicked();
                triggered();
            }
            WisenetMediaToolTip {
                id: settingsTooltip
                visible: parent.hover
                text: WisenetLinguist.settings
            }
        }
    }

    background: Rectangle {
        id: bg
        implicitWidth: 180
        implicitHeight: 36
        x: 1
        y: 1
        width: menuItem.width - 2
        height: menuItem.height - 2
        color: WisenetGui.contrast_12_black
    }
}




