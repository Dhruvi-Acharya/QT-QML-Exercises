import QtQuick 2.15
import QtQuick.Controls 2.15
import "setup"

Item {
    width: 55
    height: 24

    Image{
        id: mainMenuOpenButton
        anchors.fill: parent

        source: {
            if(mainMenu.opened === true)
                return "icon/main_menu_p.png"

            if(mainMenuMouseArea.containsPress)
                return "icon/main_menu_p.png"
            else if(mainMenuMouseArea.containsMouse)
                return "icon/main_menu_o.png"
            else
                return "icon/main_menu_n.png"
        }

        MouseArea{
            id: mainMenuMouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked:{
                mainMenu.popup(mainMenuMouseArea, 0, 24)
            }
        }
    }

    Menu{
        id: mainMenu

        background: Rectangle {
            implicitWidth: 200
            color: "#606060"
            border.color: "#606060"
            radius: 2
        }

        MenuItem{
            id: logout
            height: 26
            text : "Disconnect from Server"
            contentItem: Text{
                text: logout.text
                color: "white"
            }

            background: Rectangle{
                anchors.fill: parent
                color: logout.hovered ? "#F06200" : "#606060"
            }

            onClicked : {
                navigate("login/LoginView")
                _mainTreeModel.refreshRootItem()
                loginModel.logout()
            }
        }

        MenuSeparator {
            leftPadding: 10
            rightPadding: 10
            contentItem: Rectangle {
                implicitWidth: 200
                implicitHeight: 1
                color: "#505050"
            }
        }

        MenuItem{
            id: addDeviceMenuItem
            height: 26
            text : "Add Device"
            contentItem: Text{
                text: addDeviceMenuItem.text
                color: "white"
            }

            background: Rectangle{
                anchors.fill: parent
                color: addDeviceMenuItem.hovered ? "#F06200" : "#606060"
            }

            onClicked: addDeviceWindow.visible = true
        }

        MenuItem{
            id: addTabMenuItem
            height: 26
            text : "Add tab"

            contentItem: Text{
                text: addTabMenuItem.text
                color: "white"
            }

            background: Rectangle{
                anchors.fill: parent
                color: addTabMenuItem.hovered ? "#F06200" : "#606060"
            }

            onClicked: {
                layoutNavigator.addLayout("")
            }
        }

        MenuItem{
            id: addWindow
            height: 26
            text : "Add window"

            contentItem: Text{
                text: addWindow.text
                color: "white"
            }

            background: Rectangle{
                anchors.fill: parent
                color: addWindow.hovered ? "#F06200" : "#606060"
            }

            onClicked: {
                windowHanlder.instatiateNewWindow()
            }
        }
    }

}
