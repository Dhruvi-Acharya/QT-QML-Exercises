import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0
//import QtWebEngine 1.10

WisenetMediaMenu {
    id: webpageMenu
    signal openInNewTabTriggered();
    signal openInNewWindowTriggered();
    signal deleteTriggered();
    signal editTriggered(var webpageId, var displayName);
    signal settingTriggered(var webpageId);
    signal backTriggered();
    signal reloadTriggered();
    signal forwardTriggered();
    signal fixedSizeTriggered(real zoomFactor);
    signal editNameTriggered();
    signal fullScreenTriggered(bool isFullScreen);
    signal showInBrowserTriggered();

    property var webpageItem
    property bool layoutLocked: false
    property bool isFullScreen: false
    property bool isStandAlone: false

    function show(item)
    {
        setBinding(item);
        popup();
    }

    function setBinding(item)
    {
        webpageItem = item;

        toggleFullScreenAction.checked = isFullScreen;
        backAction.enabled = webpageItem.webView.canGoBack;
        forwardAction.enabled = webpageItem.webView.canGoForward;
        reloadAction.enabled = true;
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons

        deleteVisible: !layoutLocked
        editVisible: !layoutLocked && !isStandAlone
        settingsVisible: !layoutLocked && !isStandAlone
        openInNewTabVisibile : !isStandAlone
        openInNewWindowVisible: !isStandAlone

        onOpenInNewTabClicked: webpageMenu.openInNewTabTriggered()
        onOpenInNewWindowClicked: webpageMenu.openInNewWindowTriggered()
        onEditClicked: webpageMenu.editNameTriggered(webpageItem)
        onDeleteClicked: webpageMenu.deleteTriggered();
        onSettingsClicked: webpageMenu.settingTriggered(webpageItem.itemModel.webPageId)
    }

    WisenetMediaMenuSeparator {}
    WisenetMediaMenuItem {
        id: toggleFullScreenAction
        text: WisenetLinguist.fullScreen;
        singleCheckable: true
        checkable: true
        displayShotcutOnly: StandardKey.FullScreen
        onTriggered: webpageMenu.fullScreenTriggered(toggleFullScreenAction.checked);
        visible: !isStandAlone
    }
    WisenetMediaMenuItem {
        id: showInBrowserAction
        text: WisenetLinguist.showInBrowser
        onTriggered: {
            webpageMenu.showInBrowserTriggered();
        }
    }
    WisenetMediaMenuItem {
        id: backAction
        text: WisenetLinguist.back
        onTriggered: {
            webpageMenu.backTriggered();
        }
    }
    WisenetMediaMenuItem {
        id: forwardAction
        text: WisenetLinguist.forward
        onTriggered: webpageMenu.forwardTriggered();
    }
    WisenetMediaMenuItem {
        id: reloadAction
        text: WisenetLinguist.reload
        onTriggered: webpageMenu.reloadTriggered();
    }
    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem {
        id: size200Action
        text: "200%"
        onTriggered: webpageMenu.fixedSizeTriggered(2.0)
    }

    WisenetMediaMenuItem {
        id: size100Action
        text: "100%"
        onTriggered: webpageMenu.fixedSizeTriggered(1.0)
    }
    WisenetMediaMenuItem {
        id: size75Action
        text: "75%"
        onTriggered: webpageMenu.fixedSizeTriggered(0.75)
    }
    WisenetMediaMenuItem {
        id: size50Action
        text: "50%"
        onTriggered: webpageMenu.fixedSizeTriggered(0.5)
    }
}
