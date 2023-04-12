import QtQuick 2.15
import QtQuick.Controls 2.15

MenuItem{
    id: wisenetMenuItem

    height: {
        if(visible) return 26
        else return 0
    }
    text : "menu item"
    contentItem: Text{
        id: contentText
        text: wisenetMenuItem.text
        color: wisenetMenuItem.enabled ? WisenetGui.white : WisenetGui.gray
        verticalAlignment: Qt.AlignVCenter
    }

    background: Rectangle{
        anchors.fill: parent
        color: wisenetMenuItem.hovered ? WisenetGui.menuItemHoveredColor : WisenetGui.menuItemColor
    }
}
