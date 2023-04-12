import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../Popup"
import WisenetStyle 1.0

Rectangle {
    id: menuPopupView
    width: 240
    height: 40
    color: WisenetGui.layoutTabColor

    signal changePage(string qmlPath)

    Image{
        id: mainMenuOpenButton

        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        //verticalAlignment: Image.AlignVCenter
        //horizontalAlignment: Image.AlignHCenter

        width: 80
        height: 20
        source: logoManager.useCustomLogo? "file:///" + logoManager.customLogoPath : "qrc:/Assets/Icon/Header/wisenet_logo.svg"
        sourceSize: "80x20"
    }
}
