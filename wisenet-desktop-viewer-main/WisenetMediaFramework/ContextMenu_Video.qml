import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0

WisenetMediaMenu {
    id: videoMenu

    signal openInNewTabTriggered();
    signal openInNewWindowTriggered();
    signal deleteTriggered();
    signal setRotationTriggered(int rotation);
    signal fillModeUpdateTriggered(int fillMode);
    signal profileUpdateTriggered(int profile);
    signal videoStatusTriggered(bool isOn);
    signal bncSettingsOpen();
    signal cameraSetupOpen();
    signal nvrSetupOpen();
    signal eventRuleSetupOpen();
    signal ptzPresetOpen();
    signal cameraFocusSettingsOpen();

    signal fisheyeSetupOpen();
    signal editNameTriggered();
    signal fullScreenTriggered(bool isFullScreen);

    property bool layoutLocked: false
    property bool isFullScreen: false
    property bool isStandAlone: false
    property bool isUserAdmin: false
    property var videoItem

    property bool isCameraType: false
    property bool isMediaFileOnly: false

    function show(item)
    {
        setBinding(item);
        popup();
    }

    function setBinding(item)
    {
        videoItem = item;
        toggleFullScreenAction.checked = isFullScreen;
        isCameraType = item.isCamera
        cameraSetupAction.enabled = isUserAdmin
        eventRulesAction.enabled = isUserAdmin

        if (item.isCamera) {
            cameraSettingsSeperator.visible = true;
            profileAction.setMenuVisible(true);

            ptzPresetAction.visible = !isStandAlone && !videoItem.supportFisheyeDewarp;
            imageSettingsAction.visible = !isStandAlone;
            cameraSetupAction.visible = !isStandAlone;
            eventRulesAction.visible = !isStandAlone;

            ptzPresetAction.enabled = videoItem.supportPreset && !videoItem.mediaSeekable;
            imageSettingsAction.enabled = videoItem.supportFocusSetup && !videoItem.mediaSeekable;
        }
        else if (item.isLocalResource){
            profileAction.setMenuVisible(false);
            ptzPresetAction.visible = false;
            imageSettingsAction.visible = false;
            cameraSetupAction.visible = false;
            eventRulesAction.visible = false;
        }
        else if (item.isZoomTarget) {
            cameraSettingsSeperator.visible = false;
            profileAction.setMenuVisible(false)
            ptzPresetAction.visible = false;
            imageSettingsAction.visible = false;
            if (item.zoomTargetOfCamera) {
                cameraSettingsSeperator.visible = true;
                cameraSetupAction.visible = true;
                eventRulesAction.visible = true;
            }
            else {
                cameraSettingsSeperator.visible = false;
                cameraSetupAction.visible = false;
                eventRulesAction.visible = false;
            }
        }

        // zoomTarget은 회전, Status 설정은 표시안함.
        rotationAction.setMenuVisible(!item.isZoomTarget && !item.supportFisheyeDewarp)
        videoStatusAction.visible = !item.isZoomTarget
        fisheyeSetupAction.visible = item.isLocalResource;

        rotationAction.setBinding(item.itemModel.rotation);
        fillModeAction.setBinding(item.itemModel.itemFillMode);
        profileAction.setBinding(item.itemModel.itemProfile);
        videoStatusAction.checked = item.itemModel.videoStatus;

        bncAction.enabled = !layoutLocked;
        rotationAction.enabled = !layoutLocked;
        fillModeAction.enabled = !layoutLocked;
        profileAction.enabled = !layoutLocked;
        fisheyeSetupAction.enabled = !layoutLocked;
    }

    WisenetMediaMenuQuickButtons {
        id: quickButtons
        editVisible: !layoutLocked && cameraSetupAction.visible && cameraSetupAction.enabled
        deleteVisible: !layoutLocked
        openInNewTabVisibile : !isStandAlone && !isMediaFileOnly
        openInNewWindowVisible: !isStandAlone && !isMediaFileOnly
        settingsVisible: (cameraSetupAction.visible && cameraSetupAction.enabled) || fisheyeSetupAction.visible
        settingsTooltipText: fisheyeSetupAction.visible? WisenetLinguist.fisheyeSetup : WisenetLinguist.settings
        onSettingsClicked: {
            if (cameraSetupAction.visible && cameraSetupAction.enabled)
                videoMenu.cameraSetupOpen()
            else if (fisheyeSetupAction.visible)
                videoMenu.fisheyeSetupOpen();
        }
        onEditClicked: videoMenu.editNameTriggered();
        onOpenInNewTabClicked: videoMenu.openInNewTabTriggered()
        onOpenInNewWindowClicked: videoMenu.openInNewWindowTriggered()
        onDeleteClicked: videoMenu.deleteTriggered();
    }
    WisenetMediaMenuSeparator {}
    WisenetMediaMenuItem {
        id: toggleFullScreenAction
        text: WisenetLinguist.fullScreen;
        singleCheckable: true
        checkable: true
        displayShotcutOnly: StandardKey.FullScreen
        onTriggered: videoMenu.fullScreenTriggered(toggleFullScreenAction.checked);
        visible: !isStandAlone
    }

    WisenetMediaMenuItem {
        id: bncAction
        text: WisenetLinguist.brightnessContrast
        onTriggered: videoMenu.bncSettingsOpen();
    }

    WisenetMediaMenu {
        id: rotationAction
        title: WisenetLinguist.imageRotation

        function setBinding(rotation) {
            original.checked = rotation === 0;
            degree90.checked = rotation === 90;
            degree180.checked = rotation === 180;
            degree270.checked = rotation === 270;
        }

        Action {
            id: original
            text: qsTr("+0");
            checkable: true
            checked: false
            onTriggered: rotationAction.checkTrigger(0)
        }
        Action {
            id: degree90
            text: qsTr("+90");
            checkable: true
            checked: false
            onTriggered: rotationAction.checkTrigger(90)
        }
        Action {
            id: degree180
            text: qsTr("+180");
            checkable: true
            checked: false
            onTriggered: rotationAction.checkTrigger(180)
        }
        Action {
            id: degree270
            text: qsTr("+270");
            checkable: true
            checked: false
            onTriggered: rotationAction.checkTrigger(270)
        }
        function checkTrigger(rotate) {
            if (videoItem.itemModel.rotation !== rotate) {
                videoMenu.setRotationTriggered(rotate)
            }
        }
    }

    WisenetMediaMenu {
        id: fillModeAction
        title: WisenetLinguist.videoFillMode

        function setBinding(fillMode) {
            stretchAction.checked = (fillMode === MediaLayoutItemViewModel.Stretch);
            preserveAspectRatioAction.checked = (fillMode === MediaLayoutItemViewModel.PreserveAspectRatio);
        }

        Action {
            id: stretchAction
            text: WisenetLinguist.stretch
            checkable: true
            checked: false
            onTriggered: fillModeAction.checkTrigger(MediaLayoutItemViewModel.Stretch);

        }
        Action {
            id: preserveAspectRatioAction
            text: WisenetLinguist.preserveAspectioRatio
            checkable: true
            checked: false
            onTriggered: fillModeAction.checkTrigger(MediaLayoutItemViewModel.PreserveAspectRatio);

        }
        function checkTrigger(fillMode) {
            if (videoItem.itemModel.itemFillMode !== fillMode) {
                videoMenu.fillModeUpdateTriggered(fillMode);
            }
        }
    }

    WisenetMediaMenuItem {
        id: fisheyeSetupAction
        text: WisenetLinguist.fisheyeSetup
        onTriggered: videoMenu.fisheyeSetupOpen();
    }

    WisenetMediaMenuSeparator {id: cameraSettingsSeperator}
    WisenetMediaMenu {
        id: profileAction
        title: WisenetLinguist.videoProfile

        function setBinding(profileMode) {
            profileAuto.checked = (profileMode === MediaLayoutItemViewModel.Auto);
            profileHigh.checked = (profileMode === MediaLayoutItemViewModel.High);
            profileLow.checked = (profileMode === MediaLayoutItemViewModel.Low);
        }

        Action {
            id: profileAuto
            text: WisenetLinguist.auto
            checkable: true
            checked: false
            onTriggered: profileAction.checkTrigger(MediaLayoutItemViewModel.Auto);
        }
        Action {
            id: profileHigh
            text: WisenetLinguist.primaryProfile;
            checkable: true
            checked: false
            onTriggered: profileAction.checkTrigger(MediaLayoutItemViewModel.High);
        }
        Action {
            id: profileLow
            text: WisenetLinguist.secondaryProfile;
            checkable: true
            checked: false
            onTriggered: profileAction.checkTrigger(MediaLayoutItemViewModel.Low);
        }
        function checkTrigger(profile)
        {
            if (videoItem.itemModel.itemProfile !== profile) {
                videoMenu.profileUpdateTriggered(profile);
            }
        }
    }

    WisenetMediaMenuItem {
        id: ptzPresetAction
        text: WisenetLinguist.ptzPreset;
        onTriggered: videoMenu.ptzPresetOpen();
    }
    WisenetMediaMenuItem {
        id: imageSettingsAction
        text: WisenetLinguist.cameraFocusSettings
        onTriggered: videoMenu.cameraFocusSettingsOpen();
    }
    WisenetMediaMenuItem {
        id: cameraSetupAction
        text: WisenetLinguist.nvrSettings;
        onTriggered: {
            console.log("ContextMenu_Video cameraSetupAction onTriggered")
            videoMenu.nvrSetupOpen();
        }
    }
    WisenetMediaMenuItem {
        id: eventRulesAction
        text: WisenetLinguist.eventRules;
        onTriggered: videoMenu.eventRuleSetupOpen();
    }

    WisenetMediaMenuSeparator {}
    WisenetMediaMenuItem {
        id: videoStatusAction
        singleCheckable: true
        action: Action {
            text: WisenetLinguist.videoStatus;
            onTriggered: videoMenu.videoStatusTriggered(videoStatusAction.checked);
            checkable: true
        }
    }


}
