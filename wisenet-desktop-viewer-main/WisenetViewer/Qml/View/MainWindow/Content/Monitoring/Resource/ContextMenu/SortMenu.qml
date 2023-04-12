import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

WisenetMediaMenu{
    id: sortMenu

    property bool isResourceTree
    signal expandAll(var isResourceTree)
    signal collapseAll(var isResourceTree)

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: sortMenu.expandAll(isResourceTree)
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: sortMenu.collapseAll(isResourceTree)
    }
}
