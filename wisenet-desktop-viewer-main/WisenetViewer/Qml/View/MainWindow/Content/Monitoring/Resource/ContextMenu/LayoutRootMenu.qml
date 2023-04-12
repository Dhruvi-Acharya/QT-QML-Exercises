import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

WisenetMediaMenu{
    id: layoutRootMenu

    signal expandAll()
    signal collapseAll()

    Action{
        text : WisenetLinguist.newLayout
        onTriggered: resourceViewModel.layoutTreeModel.makeNewLayout()
    }

    WisenetMediaMenuSeparator{}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: layoutRootMenu.expandAll()
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: layoutRootMenu.collapseAll()
    }
}
