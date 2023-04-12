import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

WisenetMediaMenu{
    id: multiItemMenu

    property bool isResourceTree : true
    property alias verifyMenuVisible: verifyMenu.visible
    property alias deleteVisible : quickButtons.deleteVisible

    signal openItems()
    signal openItemsNewWindow()
    signal deleteItems(var remove)

    signal expandAll(var isResourceTree)
    signal collapseAll(var isResourceTree)
    signal verifyAll(var isResourceTree)

    WisenetMediaMenuQuickButtons {
        id: quickButtons
        settingsVisible: false
        useOpenToolTip: true

        onOpenInNewTabClicked: multiItemMenu.openItems()
        onOpenInNewWindowClicked: multiItemMenu.openItemsNewWindow()
        onDeleteClicked: multiItemMenu.deleteItems(false)

        editVisible: false
        deleteVisible: userGroupModel && userGroupModel.isAdmin
    }

    WisenetMediaMenuSeparator {}

    Action{
        text : WisenetLinguist.expandAll
        onTriggered: multiItemMenu.expandAll(isResourceTree)
    }

    Action{
        text : WisenetLinguist.collapseAll
        onTriggered: multiItemMenu.collapseAll(isResourceTree)
    }

    WisenetMediaMenuItem {
        id: verifyMenu
        text : WisenetLinguist.verifySignature
        visible: verifyMenuVisible
        onTriggered: multiItemMenu.verifyAll(isResourceTree)
    }
}
