import QtQuick 2.0
import QtQuick.Controls 2.15
import QtWebEngine 1.10

Rectangle{
    id: root
    width: 800
    height: 600
    implicitWidth: width
    color: "#383838"

    WebEngineView{
        anchors.fill: parent
        //url : "http://192.168.80.11"

        onAuthenticationDialogRequested: {
            console.log("[WebEngineView] onAuthenticationDialogRequested")

        }
        Component.onCompleted:{
            console.log("[WebEngineView] Component.onCompleted")
        }
    }
}

