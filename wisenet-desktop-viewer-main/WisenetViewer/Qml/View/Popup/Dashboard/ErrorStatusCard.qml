import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import WisenetStyle 1.0

Rectangle {
    id: errorStatusCard

    width: 260
    height: 430
    color: "transparent"
    radius: 4

    property string objectType : WisenetLinguist.device
    property bool isCamera : false

    property int powerErrorCount : 0
    property int diskErrorCount : 0
    property int overloadErrorCount : 0
    property int connectionErrorCount : 0
    property int fanErrorCount : 0

    property int disconnectedCount : 0
    property int connectedCount : 0

    property int recorderCount : 0

    component ErrorItem: Rectangle{

        property alias errorName: errorText.text
        property alias iconColor : errorIcon.color
        property int errorCount : 1

        width: parent.width
        height: 40
        color: "transparent"

        visible: errorCount > 0

        Rectangle{
            id: errorIcon

            width: 10
            height: 10
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 15
        }

        Text{
            id: errorText

            anchors.left: errorIcon.right
            anchors.leftMargin: 10

            anchors.verticalCenter: parent.verticalCenter

            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 14
        }

        Text{
            id: errorCountText

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            color: WisenetGui.contrast_01_light_grey
            font.pixelSize: 16
            font.bold: true
            text : errorCount
        }
    }

    Image{
        id: icon

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 18

        width: 36
        height: 36

        sourceSize.width: 36
        sourceSize.height: 36

        source: isCamera ? "qrc:/Assets/Icon/Dashboard/title_camera.svg" : "qrc:/Assets/Icon/Dashboard/title_recorder.svg"
    }

    Text{
        id: objectTypeText

        height:22
        anchors.top: parent.top
        anchors.left: icon.right
        anchors.topMargin: 17
        anchors.leftMargin: 9

        text: errorStatusCard.objectType

        color: WisenetGui.contrast_02_light_grey
        font.pixelSize: 16

        verticalAlignment: Text.AlignVCenter
    }

    Text{
        id: objectCountText

        height:26
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 15
        anchors.rightMargin: 21

        text: isCamera ? disconnectedCount + connectedCount : recorderCount

        color: WisenetGui.contrast_02_light_grey
        font.pixelSize: 20

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
    }

    Rectangle{
        id: separator

        height: 1
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 56

        color: WisenetGui.contrast_08_dark_grey
    }

    Text{
        id: errorTitle

        height: 19

        anchors.top: separator.bottom
        anchors.topMargin: 23
        anchors.left: parent.left
        anchors.leftMargin: 20

        color: WisenetGui.contrast_06_grey
        text: WisenetLinguist.error

        font.pixelSize: 14
        verticalAlignment: Text.AlignVCenter
    }

    Column{
        id: recorderError

        visible: !isCamera

        anchors.top: errorTitle.bottom
        anchors.topMargin: 9
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.right: parent.right
        anchors.rightMargin: 24
        anchors.bottom: parent.bottom

        ErrorItem{
            id: powerError
            errorName: WisenetLinguist.powerError
            iconColor: WisenetGui.color_dashboard_power_error
            errorCount: powerErrorCount
        }

        ErrorItem{
            id: recordingError
            errorName: WisenetLinguist.recordingError
            iconColor: WisenetGui.color_dashboard_recording_error
            errorCount: diskErrorCount
        }

        ErrorItem{
            id: overloadError
            errorName: WisenetLinguist.overload
            iconColor: WisenetGui.color_dashboard_overload_error
            errorCount: overloadErrorCount
        }

        ErrorItem{
            id: connectionError
            errorName: WisenetLinguist.connectionError
            iconColor: WisenetGui.color_dashboard_connection_error
            errorCount: connectionErrorCount
        }

        ErrorItem{
            id: fanError
            errorName: WisenetLinguist.fanError
            iconColor: WisenetGui.color_dashboard_fan_error
            errorCount: fanErrorCount
        }
    }

    Column{
        id: cameraError

        visible: isCamera

        anchors.top: errorTitle.bottom
        anchors.topMargin: 9
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.right: parent.right
        anchors.rightMargin: 24
        anchors.bottom: parent.bottom

        ErrorItem{
            id: disconnectedError
            errorName: WisenetLinguist.disconnected
            iconColor: WisenetGui.color_dashboard_camera_disconnected_error
            errorCount: disconnectedCount
        }

        /*
        ErrorItem{
            id: videolossError
            errorName: WisenetLinguist.connected
            iconColor: "green"
            errorCount: connectedCount
        }*/
    }

    /*
    Rectangle{
        id: separator2

        height: 1

        color: WisenetGui.contrast_08_dark_grey
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.right: parent.right
        anchors.rightMargin: 24
        anchors.bottom: errorTotalText.top
        anchors.bottomMargin: 16
    }

    Text{
        id: errorTotalText

        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 23

        text: WisenetLinguist.errorTotal

        color: WisenetGui.contrast_06_grey
        font.pixelSize: 16
    }

    Text{
        id: totalErrorCountText

        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        text: isCamera === false ? powerErrorCount + diskErrorCount + overloadErrorCount + connectionErrorCount + fanErrorCount : disconnectedCount

        horizontalAlignment: Text.AlignRight

        color: WisenetGui.contrast_01_light_grey
        font.pixelSize: 20
    }*/
}
