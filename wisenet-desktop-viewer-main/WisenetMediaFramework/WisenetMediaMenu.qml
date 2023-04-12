import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Menu {
    id: menu
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    margins: 0
    overlap: 1
    verticalPadding: 0
    bottomMargin: 10
    rightMargin: 10

    property color backgroundColor : WisenetGui.contrast_09_dark
    property color borderColor : WisenetGui.contrast_06_grey

    readonly property bool menuVisible: parent.visible
    function setMenuVisible(isVisible) {
        if (isVisible) {
            parent.visible = true;
            parent.height = parent.implicitHeight;
        }
        else {
            parent.visible = false;
            parent.height = 0;
        }
    }

    delegate: WisenetMediaMenuItem{}
    background: Rectangle {
        implicitWidth: 180
        implicitHeight: 28
        color: menu.backgroundColor
        border.color: menu.borderColor
    }

    Overlay.modal: Rectangle {
        color: Color.transparent(menu.palette.shadow, 0.5)
    }
    Overlay.modeless: Rectangle {
        color: Color.transparent(menu.palette.shadow, 0.12)
    }
}
