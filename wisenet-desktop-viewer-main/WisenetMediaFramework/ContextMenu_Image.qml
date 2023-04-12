import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0

WisenetMediaMenu {
    id: imageItemMenu

    signal openInNewTabTriggered();
    signal openInNewWindowTriggered();
    signal deleteTriggered();
    signal setBackgroundTriggered(bool setOn);
    signal fullScreenTriggered(bool isFullScreen);

    property var imageItem
    property bool layoutLocked: false
    property bool isFullScreen: false

    property bool isStandAlone: false
    property bool isMediaFileOnly: false

    function show(item)
    {
        setBinding(item);
        popup();
    }

    function setBinding(item)
    {
        imageItem = item;
        if (setBackgroundAction.implicitTextWidth > width) {
            width = setBackgroundAction.implicitTextWidth;
        }
        toggleFullScreenAction.checked = isFullScreen;
        setBackgroundAction.setBinding(item.itemModel.imageBackground)
        setBackgroundAction.enabled = !layoutLocked
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons
        editVisible: false
        settingsVisible: false
        deleteVisible: !layoutLocked
        openInNewTabVisibile : !isStandAlone && !isMediaFileOnly
        openInNewWindowVisible: !isStandAlone && !isMediaFileOnly
        onOpenInNewTabClicked: imageItemMenu.openInNewTabTriggered()
        onOpenInNewWindowClicked: imageItemMenu.openInNewWindowTriggered()
        onDeleteClicked: imageItemMenu.deleteTriggered();
    }

    WisenetMediaMenuSeparator {}
    WisenetMediaMenuItem {
        id: toggleFullScreenAction
        text: WisenetLinguist.fullScreen;
        singleCheckable: true
        checkable: true
        displayShotcutOnly: StandardKey.FullScreen
        onTriggered: imageItemMenu.fullScreenTriggered(toggleFullScreenAction.checked);
        visible: !isStandAlone
    }


    WisenetMediaMenuItem {
        id: setBackgroundAction
        text: WisenetLinguist.setBackgroundImage;
        singleCheckable: true

        function setBinding(bgEnabled) {
            checked = bgEnabled;
        }

        checkable: true
        checked: false
        onTriggered: {
            if (imageItem.itemModel.imageBackground !== checked) {
                imageItemMenu.setBackgroundTriggered(checked);
            }
        }
    }
}
