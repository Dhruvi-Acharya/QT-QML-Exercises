import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetPlayer 1.0
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

Rectangle {
    id: headerView
    color: WisenetGui.layoutTabColor

    property int logoRectWidth : logoRect.width
    property int layoutNavigationViewWidth : layoutNavigationView.width
    property int timeTextWidth : digitalClock.width
    property int windowsControlViewWidth : windowsControlRect.width

    property alias layoutNavigationView : layoutNavigationView
    /*
    property alias layoutList : layoutNavigationView.layoutList
    property alias currentLayout : layoutNavigationView.currentLayout
    property alias isFocusedWindow : layoutNavigationView.isFocusedWindow
    */

    signal closeButtonClicked()

    signal tabChanged(var itemType, var id, var name)
    signal tabClosedWithoutSave(var closedLayouts)

    function makeLayoutWithItems(itemType, parentId, channelNumber){
        layoutNavigationView.makeNewLayoutwithItems(itemType, parentId, channelNumber)
    }

    function checkEmptyNavigator(){
        layoutNavigationView.checkEmptyNavigator()
    }

    function openInNewTab(items){
        layoutNavigationView.openInNewTab(items)
    }

    function openNavigationBarContext(x, y){
        navigationBarMenu.popup()
    }

    Rectangle {
        id: logoRect
        width: 240
        height: parent.height
        z: 2
        color: WisenetGui.layoutTabColor

        Image{
            id: mainMenuOpenButton
            width: 80
            height: 20
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: logoManager.useCustomLogo ? "file:///"+ logoManager.customLogoPath : "qrc:/images/wisenet_logo.svg"
            sourceSize: "80x20"
        }
    }

    //LayoutNavigationView{
    Rectangle {
        z: 1
        id: layoutNavigationView
        anchors.left: logoRect.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        /*
        menuButtonWidth : menuPopupView.width
        clockWidth : digitalClock.width
        utilityButtonsWidth : utilityView.width
        windowButtonsWidth : windowsControlView.width

        onTabChanged: headerView.tabChanged(itemType, id, name)
        onTabClosedWithoutSave: headerView.tabClosedWithoutSave(closedLayouts)
        */
    }

    Rectangle{
        id: digitalClock
        width: 117
        height: parent.height
        anchors.right: separator.left
        z: 2
        color: WisenetGui.contrast_09_dark

        Text{
            id: digitalClockText

            anchors.fill: parent
            anchors.rightMargin: 19

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter

            color: WisenetGui.white
            font.pixelSize: 12
            text: localeManager.getClock()

            // Refresh Timer
            Timer {
                property int gcCount: 0
                interval: 1000; running: true; repeat: true;
                onTriggered: {
                    gcCount++;
                    // gc() call per 60 seconds
                    if (gcCount >= 60) {
                        gcCount = 0;
                        //console.log("Call GC()!!", localeManager.getClock());
                        gc();
                    }
                    digitalClockText.timeChanged()
                }
            }

            // Update time text
            function timeChanged() {
                digitalClockText.text = localeManager.getClock()
            }
        }
    }

    Image {
        id: separator
        width: 1
        height: 12
        z: 2
        anchors.right: windowsControlRect.left
        anchors.rightMargin: 19
        anchors.top: parent.top
        anchors.topMargin: 14
        sourceSize: "1x12"
        source: "qrc:/images/separator.svg"
    }

    Rectangle{
        id: windowsControlRect
        width: 69
        height: parent.height
        z: 2
        anchors.right: parent.right
        anchors.rightMargin: 2
        color: WisenetGui.layoutTabColor

        Image {
            id: minimizeButton
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.right: maximizeButton.left
            anchors.rightMargin: 6
            sourceSize: "16x16"
            source: minimizeButtonMouseArea.containsPress ? "qrc:/images/minimize_button_press.svg" :
                    minimizeButtonMouseArea.containsMouse ? "qrc:/images/minimize_button_hover.svg" :
                                                            "qrc:/images/minimize_button_default.svg"
            MouseArea
            {
                id: minimizeButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    mainWindow.lastViewerMode = mainWindow.viewerMode
                    mainWindow.viewerMode = MainViewModel.Minimized
                }
            }
        }

        Image {
            id: maximizeButton
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.right: closeButton.left
            anchors.rightMargin: 6
            sourceSize: "16x16"

            property bool isMaximized : mainWindow.viewerMode === MainViewModel.Maximized
            source: {
                if(maximizeButtonMouseArea.containsPress) {
                    return isMaximized ? "qrc:/images/normalize_button_press.svg"
                                       : "qrc:/images/maximize_button_press.svg"
                }
                else if(maximizeButtonMouseArea.containsMouse) {
                    return isMaximized ? "qrc:/images/normalize_button_hover.svg"
                                       : "qrc:/images/maximize_button_hover.svg"
                }
                else {
                    return isMaximized ? "qrc:/images/normalize_button_default.svg"
                                       : "qrc:/images/maximize_button_default.svg"
                }
            }

            MouseArea
            {
                id: maximizeButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    if (mainWindow.viewerMode === MainViewModel.Windowed)
                        mainWindow.viewerMode = MainViewModel.Maximized
                    else if (mainWindow.viewerMode === MainViewModel.Maximized)
                        mainWindow.viewerMode = MainViewModel.Windowed
                }
            }
        }

        Image {
            id: closeButton
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.right: parent.right
            anchors.rightMargin: 9
            sourceSize: "16x16"
            source: closeButtonMouseArea.containsPress ? "qrc:/images/x_button_press.svg" :
                    closeButtonMouseArea.containsMouse ? "qrc:/images/x_button_hover.svg" :
                                                         "qrc:/images/x_button_default.svg"
            MouseArea
            {
                id: closeButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: closeButtonClicked()
            }
        }
    }
}
