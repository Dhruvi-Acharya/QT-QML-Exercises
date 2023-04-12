import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import WisenetStyle 1.0
import QtQml.Models 2.15
import "qrc:/"
import WisenetMediaFramework 1.0
import Wisenet.Define 1.0

Rectangle{
    id: windowsControlView

    color: WisenetGui.layoutTabColor

    width: 69
    height: 40

    signal closeButtonClicked();

    Image {
        id: minimizeButton

        width: 16
        height: 16

        anchors.top: parent.top
        anchors.topMargin: 12
        anchors.right: maximizeButton.left
        anchors.rightMargin: 6

        sourceSize: "16x16"
        source: minimizeButtonMouseArea.containsMouse ? "qrc:/Assets/Icon/Header/minimize_button_hover.svg" :
                                                        minimizeButtonMouseArea.containsPress ? "qrc:/Assets/Icon/Header/minimize_button_press.svg" :
                                                                                                "qrc:/Assets/Icon/Header/minimize_button_default.svg"
        MouseArea
        {
            id: minimizeButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                console.log("minimizeButton clicked")
                wisenetViewerMainWindow.lastViewerMode = wisenetViewerMainWindow.viewerMode
                wisenetViewerMainWindow.viewerMode = MainViewModel.Minimized
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

        property bool isMaximized : wisenetViewerMainWindow.viewerMode === MainViewModel.Maximized
        source: {
            if(maximizeButtonMouseArea.containsMouse)
            {
                return isMaximized ? "qrc:/Assets/Icon/Header/normalize_button_hover.svg" : "qrc:/Assets/Icon/Header/maximize_button_hover.svg"
            }
            else if(maximizeButtonMouseArea.containsPress)
            {
                return isMaximized ? "qrc:/Assets/Icon/Header/normalize_button_press.svg" : "qrc:/Assets/Icon/Header/maximize_button_press.svg"
            }
            else
            {
                return isMaximized ? "qrc:/Assets/Icon/Header/normalize_button_default.svg" : "qrc:/Assets/Icon/Header/maximize_button_default.svg"
            }
        }

        MouseArea
        {
            id: maximizeButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                if (wisenetViewerMainWindow.viewerMode === MainViewModel.Windowed) {
//                    console.log("maximizeButton clicked() to MainViewModel.Maximized");
                    wisenetViewerMainWindow.viewerMode = MainViewModel.Maximized
                }
                else if (wisenetViewerMainWindow.viewerMode === MainViewModel.Maximized) {
//                    console.log("maximizeButton clicked() to MainViewModel.Windowed");
                    wisenetViewerMainWindow.viewerMode = MainViewModel.Windowed
                }
                console.log("maximizeButton clicked()", wisenetViewerMainWindow.viewerMode)
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

        source: closeButtonMouseArea.containsMouse ? "qrc:/Assets/Icon/Header/x_button_hover.svg" :
                                                     closeButtonMouseArea.containsPress ? "qrc:/Assets/Icon/Header/x_button_press.svg" :
                                                                                          "qrc:/Assets/Icon/Header/x_button_default.svg"

        MouseArea
        {
            id: closeButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: windowsControlView.closeButtonClicked()
        }
    }

}

