import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import WisenetStyle 1.0

Rectangle{
    id: recorderErrorDetailView

    property int flowWidth
    //width: dashboardMain.width - 200 - recorderAbnormalStatus.width - 60

    property var powerErrorListModel
    property var diskErrorListModel
    property var overloadErrorListModel
    property var connectionErrorListModel
    property var fanErrorListModel

    property alias powerErrorVisible : powerError.visible
    property alias diskErrorVisible : diskError.visible
    property alias overloadErrorVisible : overloadError.visible
    property alias connectionErrorVisible : connectionError.visible
    property alias fanErrorVisible : fanError.visible

    color: "transparent"

    Flickable{
        id: scrollArea

        anchors.fill: parent
        clip: true

        contentHeight: deviceStatus.height

        ScrollBar.vertical: WisenetScrollBar {
            id: verticalScrollBar

            parent: scrollArea

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            stepSize: verticalScrollBar.visualSize/4
        }

        Column{
            id: deviceStatus
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 12

            DeviceStatusList{
                id: powerError

                errorColor: WisenetGui.color_dashboard_power_error
                title: WisenetLinguist.powerError

                width: flowWidth
                listModel: powerErrorListModel
            }

            DeviceStatusList{
                id: diskError

                errorColor: WisenetGui.color_dashboard_recording_error
                title:  WisenetLinguist.recordingError

                width: flowWidth
                listModel: diskErrorListModel
            }

            DeviceStatusList{
                id: overloadError

                errorColor: WisenetGui.color_dashboard_overload_error
                title:  WisenetLinguist.overload

                width: flowWidth
                listModel: overloadErrorListModel
            }

            DeviceStatusList{
                id: connectionError

                errorColor: WisenetGui.color_dashboard_connection_error
                title:  WisenetLinguist.connectionError

                width: flowWidth
                listModel: connectionErrorListModel
            }

            DeviceStatusList{
                id: fanError

                errorColor: WisenetGui.color_dashboard_fan_error
                title:  WisenetLinguist.fanError

                width: flowWidth
                listModel: fanErrorListModel
            }
        }
    }
}
