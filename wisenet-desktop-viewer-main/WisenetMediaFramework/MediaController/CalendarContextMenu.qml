import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
import "qrc:/"
import "qrc:/WisenetStyle/"

WisenetMediaMenu {
    id: contextMenu
    implicitWidth: {
        var textWidth = dummyEventLogText.contentWidth + 32
        if(textWidth < 180)
            textWidth = 180
        return textWidth
    }

    WisenetMediaMenuItem {
        id: eventSrarchMenuItem
        text: WisenetLinguist.eventSearch
        onTriggered: {
            showEventSearchView()
        }
    }

    WisenetMediaMenuItem {
        id: eventLogMenuItem
        text: WisenetLinguist.eventLog
        visible: userGroupModel && userGroupModel.isAdmin
        onTriggered: {
            showEventLogView()
        }

        Text {
            id: dummyEventLogText
            text: eventLogMenuItem.text
            color: WisenetGui.transparent
            font.pixelSize: 12
        }
    }
}
