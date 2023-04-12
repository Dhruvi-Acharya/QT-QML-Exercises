import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0

WisenetMediaMenu {
    id: multipleItemsMenu

    signal openInNewTabTriggered();
    signal openInNewWindowTriggered();
    signal deleteTriggered();

    signal fillModeUpdateTriggered(int fillMode);
    signal profileUpdateTriggered(int profile);
    signal cameraSetupOpen();
    signal nvrSetupOpen();
    signal eventRuleSetupOpen();

    property var mitems
    property bool layoutLocked: false
    property bool isStandAlone: false
    property bool isUserAdmin: false
    property bool isMediaFileOnly: false

    function show(items)
    {
        setBinding(items)
        popup();
    }

    function setBinding(items)
    {
        mitems = items;

        var profileVisible = false;
        var fillModeVisible = false;
        var cameraSetupVisible = false;
        var eventRuleSetupVisible = false;

        for (var i = 0 ; i < items.length ; i++) {
            var item = items[i];
            if (item) {
                if (item.isCamera) {
                    if (!profileVisible)
                        profileVisible = true;
                    if (!fillModeVisible)
                        fillModeVisible = true;
                    if (!cameraSetupVisible)
                        cameraSetupVisible = true;
                    if (!eventRuleSetupVisible)
                        eventRuleSetupVisible = true;
                }
                else if (item.isLocalResource) {
                    if (!fillModeVisible)
                        fillModeVisible = true;
                }
                else if (item.isZoomTarget) {
                    if (!fillModeVisible)
                        fillModeVisible = true;
                }
            }
        }
        if (isStandAlone) {
            cameraSetupVisible = false;
            eventRuleSetupVisible = false;
        }

        profileAction.setMenuVisible(profileVisible);
        fillModeAction.setMenuVisible(fillModeVisible);
        cameraSetupAction.visible = cameraSetupVisible;
        eventRulesAction.visible = eventRuleSetupVisible;
        cameraSetupAction.enabled = isUserAdmin;
        eventRulesAction.enabled = isUserAdmin;

        quickButtons.settingsVisible = cameraSetupVisible && isUserAdmin;
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons
        editVisible: false
        deleteVisible: !layoutLocked
        openInNewTabVisibile : !isStandAlone && !isMediaFileOnly
        openInNewWindowVisible: !isStandAlone && !isMediaFileOnly
        settingsVisible: cameraSetupAction.visible && !isStandAlone
        onOpenInNewTabClicked: multipleItemsMenu.openInNewTabTriggered()
        onOpenInNewWindowClicked: multipleItemsMenu.openInNewWindowTriggered()
        onDeleteClicked: multipleItemsMenu.deleteTriggered();
        onSettingsClicked: multipleItemsMenu.cameraSetupOpen();
    }

    /* 퀵버튼과 중복인 메뉴 제거
    WisenetMediaMenuItem {
        id: openInNewTabAction
        text: WisenetLinguist.openInNewTab;
        onTriggered: multipleItemsMenu.openInNewTabTriggered()
        visible: !isStandAlone && !isMediaFileOnly
    }
    WisenetMediaMenuItem {
        id: openInNewWindowAction
        text: WisenetLinguist.openInNewWindow;
        onTriggered: multipleItemsMenu.openInNewWindowTriggered()
        visible: !isStandAlone && !isMediaFileOnly
    }
    Action {
        id: deleteAction
        text: WisenetLinguist.remove;
        onTriggered: multipleItemsMenu.deleteTriggered()
    }    
    WisenetMediaMenuSeparator {}
    */

    // 비디오 공통 아이템
    WisenetMediaMenu {
        id: fillModeAction
        title: WisenetLinguist.videoFillMode

        Action {
            id: stretchAction
            text: WisenetLinguist.stretch
            checkable: true
            checked: false
            onTriggered: multipleItemsMenu.fillModeUpdateTriggered(MediaLayoutItemViewModel.Stretch);

        }
        Action {
            id: preserveAspectRatioAction
            text: WisenetLinguist.preserveAspectioRatio
            checkable: true
            checked: false
            onTriggered: multipleItemsMenu.fillModeUpdateTriggered(MediaLayoutItemViewModel.PreserveAspectRatio);

        }
    }

    // 카메라 공통 메뉴아이템
    WisenetMediaMenu {
        id: profileAction
        title: WisenetLinguist.videoProfile

        Action {
            id: profileAuto
            text: WisenetLinguist.auto
            checkable: true
            checked: false
            onTriggered: multipleItemsMenu.profileUpdateTriggered(MediaLayoutItemViewModel.Auto);
        }
        Action {
            id: profileHigh
            text: WisenetLinguist.primaryProfile
            checkable: true
            checked: false
            onTriggered: multipleItemsMenu.profileUpdateTriggered(MediaLayoutItemViewModel.High);
        }
        Action {
            id: profileLow
            text: WisenetLinguist.secondaryProfile
            checkable: true
            checked: false
            onTriggered: multipleItemsMenu.profileUpdateTriggered(MediaLayoutItemViewModel.Low);
        }
    }
    WisenetMediaMenuItem {
        id: cameraSetupAction
        text: WisenetLinguist.nvrSettings
        onTriggered: {
            console.log("ContextMenu_MultipleItems cameraSetupAction onTriggered")
            multipleItemsMenu.nvrSetupOpen();
        }
    }
    WisenetMediaMenuItem {
        id: eventRulesAction
        text: WisenetLinguist.eventRules
        onTriggered: multipleItemsMenu.eventRuleSetupOpen();
    }
}
