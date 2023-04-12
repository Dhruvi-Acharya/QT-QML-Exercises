import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

Rectangle{
    id: cameraErrorDetailView

    property int flowWidth
    property var disconnectedCameraListModel
    property var connectedCameraListModel

    property var groupTreeProxyModel
    property var deviceTreeProxyModel
    color: "transparent"

    function expandGroupTree(){
        groupTree.expandAll()
    }

    function expandDeviceTree(){
        deviceTree.expandAll()
    }

    signal selectedGroupChanged(var groupId)
    signal selectedDeviceChanged(var deviceId)
    signal filterChanged(var filter)

    Rectangle{
        id: filterRect

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        height: 19

        state: "STATUS"

        onStateChanged: {
            console.log("cameraErrorDetailView onStateChanged ", state)
            cameraErrorDetailView.filterChanged(state)
        }

        color: "transparent"

        Row{
            anchors.fill: parent
            Text{
                id: byStatus
                text: WisenetLinguist.status

                color: filterRect.state === "STATUS" ? WisenetGui.color_primary : WisenetGui.contrast_04_light_grey

                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: filterRect.state = "STATUS"
                }
            }

            Text{
                text: "  |  "

                color: WisenetGui.contrast_04_light_grey

                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }

            Text{
                id: byGroup
                text: WisenetLinguist.group

                color: filterRect.state === "GROUP" ? WisenetGui.color_primary : WisenetGui.contrast_04_light_grey

                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: filterRect.state = "GROUP"
                }
            }

            Text{
                text: "  |  "

                color: WisenetGui.contrast_04_light_grey

                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }

            Text{
                id: byNvr
                text: WisenetLinguist.device

                color: filterRect.state === "NVR" ? WisenetGui.color_primary : WisenetGui.contrast_04_light_grey

                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: filterRect.state = "NVR"
                }
            }
        }
    }

    Rectangle{
        id: treeRect

        anchors.top: filterRect.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 220

        radius: 4
        visible: filterRect.state !== "STATUS"

        color: WisenetGui.contrast_08_dark_grey

        WisenetDeviceSelectionTree{
            id: groupTree
            anchors.fill: parent
            treeModel: groupTreeProxyModel
            state: "DashboardTree"
            bgColor: WisenetGui.contrast_08_dark_grey
            radius: 4

            visible: filterRect.state === "GROUP"

            onSelectedDeviceChanged: {
                //console.log("deviceTree.onSelectedDeviceChanged", selectedDeviceValue)
                var groupId;

                if(selectedDeviceValue){
                    selectedDeviceValue.forEach(function(item) {
                        groupId = item.uuid
                    });
                }

                //console.log("groupTree.onSelectedDeviceChanged", groupId)
                cameraErrorDetailView.selectedGroupChanged(groupId)
            }
        }

        WisenetDeviceSelectionTree{
            id: deviceTree
            anchors.fill: parent
            treeModel: deviceTreeProxyModel
            state: "DashboardTree"
            bgColor: WisenetGui.contrast_08_dark_grey
            radius: 4

            visible: filterRect.state === "NVR"

            onSelectedDeviceChanged: {
                //console.log("deviceTree.onSelectedDeviceChanged", selectedDeviceValue)
                var deviceId;

                if(selectedDeviceValue){
                    selectedDeviceValue.forEach(function(item) {
                        deviceId = item.uuid
                    });
                }

                //console.log("deviceTree.onSelectedDeviceChanged", deviceId)
                cameraErrorDetailView.selectedDeviceChanged(deviceId)
            }
        }
    }

    Flickable{
        id: scrollArea

        anchors.top: filterRect.bottom
        anchors.topMargin: 20
        anchors.left: filterRect.state !== "STATUS" ? treeRect.right : parent.left
        anchors.leftMargin: filterRect.state !== "STATUS" ? 12 : 0
        anchors.bottom: parent.bottom
        anchors.right: parent.right
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

            spacing: 10

            ObjectStatusList{
                id: disconnected

                errorColor: WisenetGui.color_dashboard_camera_disconnected_error
                title: WisenetLinguist.disconnected

                width: filterRect.state !== "STATUS" ? flowWidth - treeRect.width - 10 : flowWidth
                listModel: disconnectedCameraListModel
            }

            ObjectStatusList{
                id: connected

                errorColor: WisenetGui.contrast_02_light_grey
                title: WisenetLinguist.connected

                width: filterRect.state !== "STATUS" ? flowWidth - treeRect.width - 10 : flowWidth
                listModel: connectedCameraListModel
            }
        }
    }
}
