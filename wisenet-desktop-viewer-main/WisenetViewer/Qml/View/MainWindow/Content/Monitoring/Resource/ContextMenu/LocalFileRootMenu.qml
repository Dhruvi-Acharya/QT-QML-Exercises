import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0

WisenetMediaMenu{
    id: localfileRootMenu

    //signal openMediaFileDialog()
    signal expandAll()
    signal collapseAll()

    /*
    Action{
        text : ""
        onTriggered: localfileRootMenu.openMediaFileDialog()
    }

    WisenetMediaMenuSeparator{}*/

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: localfileRootMenu.expandAll()
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: localfileRootMenu.collapseAll()
    }
}
