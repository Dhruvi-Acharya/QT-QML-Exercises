import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import WisenetMediaFramework 1.0

WisenetMediaMenu {
    id: layoutMenu
    signal autoFitTriggered();
    signal saveTriggered();
    signal saveAsTriggered();
    signal openMediaFileTriggered();
    signal fullScreenTriggered(bool isFullScreen);
    signal lockTriggered(bool locked);
    signal cellRatioUpdateTriggered(int w, int h);
    signal fillModeUpdateTriggered(int fillMode);
    signal profileUpdateTriggered(int profile);
    signal videoStatusTriggered(bool isOn);

    property bool layoutLocked: false
    property bool layoutShared: false
    property bool editable: !layoutLocked && !layoutShared
    property bool isFullScreen: false
    readonly property alias fullscreenShortText: toggleFullScreenAction.shortCutText

    property bool isStandAlone: false
    property var layout

    property bool isMediaFileOnly: false
    onIsMediaFileOnlyChanged: {
        profileAction.setMenuVisible(!isMediaFileOnly)
    }

    property bool isMaskingMode: false
    onIsMaskingModeChanged: {
        fillModeAction.setMenuVisible(!isMaskingMode)
        profileAction.setMenuVisible(!isMaskingMode)
    }

    function show(item, fs)
    {
        setBinding(item, fs);
        popup();
    }

    function setBinding(item)
    {
        layout = item;
        if (saveAsAction.implicitTextWidth > width) {
            width = saveAsAction.implicitTextWidth;
        }

        saveAction.enabled = !item.saveStatus && editable;  // editable이 아니면 save disable
        toggleFullScreenAction.checked = isFullScreen;
        videoStatusAction.checked = item.videoStatus;
        fillModeAction.setBinding(item.layoutFillMode);
        profileAction.setBinding(item.layoutProfile);
        cellArAction.setBinding(item.cellRatioH);
        lockAction.checked = item.locked;

        lockAction.enabled = !layoutShared; // 공유레이아웃인 경우 lock을 풀수 없다.
        fillModeAction.enabled = editable;
        profileAction.enabled = editable;
        cellArAction.enabled = editable;
        openFilesAction.enabled = editable;
    }

    WisenetMediaMenuItem {
        id: zoomToFitAction
        text: WisenetLinguist.autoFit;
        displayShotcutOnly: isStandAlone ? "" : StandardKey.Refresh
        onTriggered: layoutMenu.autoFitTriggered();
        visible: !isMaskingMode
    }

    WisenetMediaMenuItem {
        id: toggleFullScreenAction
        text: WisenetLinguist.fullScreen;
        visible: !isStandAlone
        singleCheckable: true
        checkable: true
        displayShotcutOnly: StandardKey.FullScreen
        onTriggered: layoutMenu.fullScreenTriggered(toggleFullScreenAction.checked);
    }

    WisenetMediaMenuSeparator {
        visible: !isMediaFileOnly && !isMaskingMode
    }

    WisenetMediaMenuItem {
        id: saveAction
        text: WisenetLinguist.saveLayout;
        displayShotcutOnly: isStandAlone ? "" : "Ctrl+S"
        onTriggered: layoutMenu.saveTriggered();
        visible: !isMediaFileOnly && !isMaskingMode
    }

    WisenetMediaMenuItem {
        id: saveAsAction
        text: WisenetLinguist.saveLayoutAs;
        displayShotcutOnly: isStandAlone ? "" : (Qt.platform.os === "windows" ? "F12" : "Ctrl+Shift+S")
        onTriggered: layoutMenu.saveAsTriggered();
        visible: !isMediaFileOnly && !isMaskingMode
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem {
        id: openFilesAction
        text: WisenetLinguist.openMediaFiles;
        displayShotcutOnly: isStandAlone ? "" : "Ctrl+O"
        onTriggered: layoutMenu.openMediaFileTriggered();
    }

    WisenetMediaMenuSeparator {}

    WisenetMediaMenuItem {
        id: lockAction
        singleCheckable: true
        text: WisenetLinguist.lock
        onTriggered: {
            if (layout.locked !== lockAction.checked)
                layoutMenu.lockTriggered(lockAction.checked);
        }
        checkable: true
        visible: !standalone
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
        function checkTrigger(fillMode)  {
            if (layout.layoutFillMode !== fillMode) {
                layoutMenu.fillModeUpdateTriggered(fillMode)
            }
        }
    }

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
            text: WisenetLinguist.primaryProfile
            checkable: true
            checked: false
            onTriggered: profileAction.checkTrigger(MediaLayoutItemViewModel.High);

        }
        Action {
            id: profileLow
            text: WisenetLinguist.secondaryProfile
            checkable: true
            checked: false
            onTriggered: profileAction.checkTrigger(MediaLayoutItemViewModel.Low);

        }
        function checkTrigger(profile)
        {
            if (layout.layoutProfile !== profile) {
                layoutMenu.profileUpdateTriggered(profile);
            }
        }
    }

    WisenetMediaMenu {
        id: cellArAction
        title: WisenetLinguist.cellAspectRatio
        function setBinding(cellRatioH) {
            cellAr16_9Action.checked = (cellRatioH===90);
            cellAr4_3Action.checked = (cellRatioH===120);
        }
        Action {
            id: cellAr16_9Action
            text: qsTr("16:9");
            checkable: true
            checked: false
            onTriggered: cellArAction.checkTrigger(160, 90);
        }
        Action {
            id: cellAr4_3Action
            text: qsTr("4:3");
            checkable: true
            checked: false
            onTriggered: cellArAction.checkTrigger(160, 120);
        }
        function checkTrigger(w, h)
        {
            if (layout.cellRatioW !== w || layout.cellRatioH !== h) {
                layoutMenu.cellRatioUpdateTriggered(w,h);
            }
        }
    }

    WisenetMediaMenuItem {
        id: videoStatusAction
        singleCheckable: true
        action: Action {
            text: WisenetLinguist.videoStatus
            onTriggered: layoutMenu.videoStatusTriggered(videoStatusAction.checked);

            checkable: true
        }
    }

}
