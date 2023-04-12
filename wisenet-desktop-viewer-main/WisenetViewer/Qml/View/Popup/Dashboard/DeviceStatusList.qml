import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "qrc:/"

Rectangle {
    id: deviceStatusList

    // 높이 가변
    height: titleText.height + separator.height + itemList.height + titleTextTopMargin + separatorTopMargin + itemListTopMargin + itemListBottomMargin

    property alias errorColor : errorRect.color
    property alias title : titleText.text
    property var listModel

    property int titleTextTopMargin: 12
    property int separatorTopMargin: 44
    property int itemListTopMargin: 15
    property int itemListBottomMargin: 15

    color: WisenetGui.contrast_08_dark_grey
    radius: 4

    Rectangle{
        id: errorRect

        width: 14
        height: 14
        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 15
    }

    Text{
        id: titleText

        height: 20
        anchors.top: parent.top
        anchors.topMargin: titleTextTopMargin
        anchors.left: errorRect.right
        anchors.leftMargin: 10

        font.pixelSize: 16

        color: WisenetGui.contrast_02_light_grey

        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        id: separator
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: separatorTopMargin
        height: 1
        color: WisenetGui.contrast_10_dark
    }

    function getRandomInt(min, max) {
        min = Math.ceil(min);
        max = Math.floor(max);
        return Math.floor(Math.random() * (max - min)) + min; //최댓값은 제외, 최솟값은 포함
    }

    function getSubTitleText(name){
        if(name === "SystemEvent.DualSMPSFail")
            return WisenetLinguist.dualSmpsFail;
        else if(name === "SystemEvent.FanError")
            return WisenetLinguist.fanError;
        else if(name === "SystemEvent.CPUFanError")
            return WisenetLinguist.cpuFanError;
        else if(name === "SystemEvent.FrameFanError")
            return WisenetLinguist.frameFanError;
        else if(name === "SystemEvent.LeftFanError")
            return WisenetLinguist.leftFanError;
        else if(name === "SystemEvent.RightFanError")
            return WisenetLinguist.rightFanError;
        else if(name === "SystemEvent.HDDFull")
            return WisenetLinguist.hddFull;
        else if(name === "SystemEvent.SDFull")
            return WisenetLinguist.sdFull;
        else if(name === "SystemEvent.NASFull")
            return WisenetLinguist.nasFull;
        else if(name === "SystemEvent.HDDNone")
            return WisenetLinguist.hddNone;
        else if(name === "SystemEvent.HDDFail")
            return WisenetLinguist.hddFail;
        else if(name === "SystemEvent.SDFail")
            return WisenetLinguist.sdFail;
        else if(name === "SystemEvent.NASFail")
            return WisenetLinguist.nasFail;
        else if(name === "SystemEvent.HDDError")
            return WisenetLinguist.hddError;
        else if(name === "SystemEvent.RAIDDegrade")
            return WisenetLinguist.raidDegrade;
        else if(name === "SystemEvent.RAIDRebuildStart")
            return WisenetLinguist.raidRebuildStart;
        else if(name === "SystemEvent.RAIDFail")
            return WisenetLinguist.raidFail;
        else if(name === "SystemEvent.iSCSIDisconnect")
            return WisenetLinguist.iScsiDisconnect;
        else if(name === "SystemEvent.NASDisconnect")
            return WisenetLinguist.nasDisconnect;
        else if(name === "SystemEvent.RecordFiltering")
            return WisenetLinguist.recordFiltering;
        else if(name === "SystemEvent.RecordingError")
            return WisenetLinguist.recordingError;
        else if(name === "SystemEvent.CpuOverload")
            return WisenetLinguist.cpuOverload;
        else if(name === "SystemEvent.NetTxTrafficOverflow")
            return WisenetLinguist.netTxTrafficOverflow;
        else if(name === "SystemEvent.NetCamTrafficOverFlow")
            return WisenetLinguist.netCamTrafficOverFlow;
        else if(name === "SystemEvent.VPUError")
            return WisenetLinguist.vpuError;
        else if(name === "SystemEvent.MemoryError")
            return WisenetLinguist.memoryError;
        else if(name === "SystemEvent.LowFps")
            return WisenetLinguist.lowFps;
        else if(name === "SystemEvent.CoreService.DeviceDisconnectedByService")
            return WisenetLinguist.systemEventDeviceDisconnectedByService
    }

    Flow{
        id: itemList
        anchors.top: separator.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: itemListTopMargin
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 5

        Repeater{
            id: errorModel
            model: listModel
            //model: getRandomInt(5,8)//listModel

            Rectangle{
                width: 220
                height: subTitleText.height + cameraListModel.height + 10
                color: WisenetGui.contrast_08_dark_grey

                visible: cameraList.count > 0

                Text{
                    id: subTitleText

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    text: getSubTitleText(subTitle)

                    elide: Text.ElideRight

                    color: WisenetGui.contrast_02_light_grey
                    font.pixelSize: 14
                }

                Column{
                    id: cameraListModel

                    anchors.top: subTitleText.bottom
                    anchors.topMargin: 10
                    anchors.left: parent.left

                    spacing: 5

                    Repeater{
                        id: cameraList
                        model: listData

                        Rectangle{
                            width: 220
                            height: 20

                            color: "transparent"

                            //radius: 8

                            Image{
                                id: itemIcon
                                anchors.top: parent.top
                                anchors.topMargin: 1
                                anchors.left: parent.left

                                width: 18
                                height: 18
                                source: objectType === 0 ? WisenetImage.tree_nvr_normal : WisenetImage.camera_normal
                            }

                            Text{
                                id: nameText
                                anchors.left: itemIcon.right
                                anchors.top: parent.top
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                anchors.leftMargin: 4

                                text: model.objectName

                                color: WisenetGui.contrast_04_light_grey

                                elide: Text.ElideRight

                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 12

                                MouseArea{
                                    id: nameTextMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                }

                                WisenetMediaToolTip {
                                    text: parent.text
                                    visible: nameText.truncated && nameTextMouseArea.containsMouse
                                    delay: 200
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
