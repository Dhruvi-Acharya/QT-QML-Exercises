import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetStyle 1.0
import QtWebEngine 1.10
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Wisenet.Setup 1.0


Rectangle{
    id : deviceListChannelViewId
    property int margin: 5
    property bool singleChannelVisible : true
    property var profileList : []
    property int highIndex
    property int lowIndex

    property int cellPadding : 5
    property int subColumnSpacing : 4

    color: WisenetGui.transparent
    anchors.fill: parent
    visible: false
    onVisibleChanged: {
        if(visible)
            readChannel()
        else{
            console.log("DeviceListChannelView onVisibleChanged ")
            channelListViewModel.reset()
            liveView.close()
        }
    }

    function setProfileInfo(){
        profileList = []
        highIndex = 0
        lowIndex = 0

        var tempHigh = Number(channelListViewModel.highProfile);
        var tempLow = Number(channelListViewModel.lowProfile);

        for(var i =0; i < channelListViewModel.rowCount; i++){
            profileList.push(channelListViewModel.get(i).profileName)
            if(channelListViewModel.get(i).profileNumber === tempHigh)
                highIndex = i

            if(channelListViewModel.get(i).profileNumber === tempLow)
                lowIndex = i
        }
    }



    function readChannel(){
        console.log("DeviceListChannelView readChannel " + channelListViewModel.fisheyeMount)//, deviceListViewModel.deviceName, deviceListViewModel.deviceGuid, deviceListViewModel.host)
        singleChannelVisible = (channelListViewModel.channelCount > 1)?false:true

        channelNameId.text = channelListViewModel.channelName
        guidId.content = channelListViewModel.channelId
        useChannelId.checked = channelListViewModel.useChannel
        fisheyeDewarpingId.checked = channelListViewModel.useDewarping
        if(fisheyeDewarpingId.checked){

            var  mount = WisenetLinguist.ceiling
            if(channelListViewModel.fisheyeMount === ChannelListViewModel.L_Ceiling)
                mount = WisenetLinguist.ceiling
            else if(channelListViewModel.fisheyeMount === ChannelListViewModel.L_Wall)
                mount = WisenetLinguist.wall
            else if(channelListViewModel.fisheyeMount === ChannelListViewModel.L_Ground)
                mount = WisenetLinguist.ground

            mountId.currentIndex = mountId.indexOfValue(mount)

            console.log("DeviceListChannelView readChannel " + mount + " ----" + channelListViewModel.fisheyeLensType)
        }
        else{
            mountId.currentIndex = 0

        }
        if(channelListViewModel.fisheyeLensType !== "")
            lensTypeId.currentIndex = lensTypeId.indexOfValue(channelListViewModel.fisheyeLensType)
        else
            lensTypeId.currentIndex = 0

        if(singleChannelVisible)
            liveView.open(channelListViewModel.deviceId, channelListViewModel.channelId)
        else
            liveView.close()
        console.log("DeviceListChannelView readChannel singleChannelVisible=", singleChannelVisible)
    }

    function highProfileUpdate(index){
        console.log("DeviceListChannelView highProfileUpdate index=", index)
        primaryCodecId.content = channelListViewModel.get(index).codec?channelListViewModel.get(index).codec:"";
        primaryResolutionId.content = channelListViewModel.get(index).resolution?channelListViewModel.get(index).resolution:"";
        primaryFramerateId.content = channelListViewModel.get(index).framerate?(channelListViewModel.get(index).framerate + " fps"):"";
        primaryBitrateId.content = channelListViewModel.get(index).bitrate?(channelListViewModel.get(index).bitrate + " kbps"):"";
    }
    function lowProfileUpdate(index){
        console.log("DeviceListChannelView lowProfileUpdate index=", index)
        secondaryCodecId.content = channelListViewModel.get(index).codec?channelListViewModel.get(index).codec:"";
        secondaryResolutionId.content = channelListViewModel.get(index).resolution?channelListViewModel.get(index).resolution:"";
        secondaryFramerateId.content = channelListViewModel.get(index).framerate?(channelListViewModel.get(index).framerate + " fps"):"";
        secondaryBitrateId.content = channelListViewModel.get(index).bitrate?(channelListViewModel.get(index).bitrate + " kbps"):"";
    }

    Connections{
        target: channelListViewModel
        function onHighProfileChanged(high)
        {
            console.log("profileSettingPage onHighProfileChanged " + high)
            setProfileInfo()
            primaryId.model = profileList
            primaryId.currentIndex = highIndex
            highProfileUpdate(highIndex)

        }
        function onLowProfileChanged(low)
        {
            console.log("profileSettingPage onLowProfileChanged " + low)
            setProfileInfo()
            secondaryId.model = profileList
            secondaryId.currentIndex = lowIndex
            lowProfileUpdate(lowIndex)
        }
    }

    Row{
        leftPadding: 10
        topPadding: 20
        rightPadding: 10
        Rectangle{
            width: 410
            color: "transparent"
            height: 300

            Column{
                anchors.fill: parent
                Layout.leftMargin: 10
                Layout.topMargin: 10
                spacing: 11

                DeviceListRowControl{
                    WisenetLabel {
                        text: WisenetLinguist.channelName
                        width: 120
                        wrapMode: Text.Wrap
                    }

                    WisenetTextBox {
                        id : channelNameId
                        fontSize: 18
                        width: 228
                        height: 23
                        text : WisenetLinguist.channelName
                        enabled: singleChannelVisible
                    }
                }

                WisenetCell{
                    id : guidId
                    title : WisenetLinguist.channelNumber
                    visible: singleChannelVisible
                }

                DeviceListRowControl{
                    WisenetLabel {
                        text: WisenetLinguist.useChannel
                        width: 120
                        wrapMode: Text.Wrap
                    }
                    WisenetCheckBox2{
                        id : useChannelId
                        width: 250
                        text: WisenetLinguist.enable
                    }
                }

                DeviceListRowControl{
                    WisenetLabel {
                        text: WisenetLinguist.fisheyeDewarping
                        width: 120
                        wrapMode: Text.Wrap
                    }
                    WisenetCheckBox2{
                        id : fisheyeDewarpingId
                        width: 250
                        text: WisenetLinguist.enable
                    }
                }

                DeviceListRowControl{
                     WisenetLabel {
                        text: WisenetLinguist.mount
                        width: 120
                        wrapMode: Text.Wrap
                    }
                    WisenetComboBox{
                        id : mountId
                        enabled: fisheyeDewarpingId.checked
                        model: [ WisenetLinguist.ceiling, WisenetLinguist.wall, WisenetLinguist.ground]
                    }
                }
                DeviceListRowControl{
                     WisenetLabel {
                        text: WisenetLinguist.fisheyeCameraType
                        font.pixelSize: 11
                        width: 120
                        wrapMode: Text.Wrap
                    }
                    WisenetComboBox{
                        id : lensTypeId
                        enabled: fisheyeDewarpingId.checked
                        model: ["SNF-8010", "PNF-9010", "XNF-8010", "HCF-8010", "QNF-8010", "QNF-9010", "XNF-9010", "TNF-9010", "XNF-9013"]
                    }
                }
                DeviceListRowControl{
                    WisenetLabel {
                        text: WisenetLinguist.primaryProfile
                        width: 120
                        wrapMode: Text.Wrap
                    }
                    WisenetComboBox{
                        id : primaryId

                        onCurrentIndexChanged:{
                            highProfileUpdate(currentIndex)
                        }
                    }
                }
                Column{
                    spacing: subColumnSpacing

                    WisenetCell{
                        id : primaryCodecId
                        title : WisenetLinguist.codec
                        visible: singleChannelVisible
                        isSub: true
                    }
                    WisenetCell{
                        id : primaryResolutionId
                        title : WisenetLinguist.resolution
                        visible: singleChannelVisible
                        isSub: true
                    }
                    WisenetCell{
                        id : primaryFramerateId
                        title : WisenetLinguist.framerate
                        visible: singleChannelVisible
                        isSub: true
                    }
                    WisenetCell{
                        id : primaryBitrateId
                        title : WisenetLinguist.bitrate
                        visible: singleChannelVisible
                        isSub: true
                    }
                }
                DeviceListRowControl{
                    WisenetLabel {
                        text: WisenetLinguist.secondaryProfile
                        width: 120
                        wrapMode: Text.Wrap
                    }

                    WisenetComboBox{
                        id : secondaryId

                        onCurrentIndexChanged:{
                            lowProfileUpdate(currentIndex)
                        }
                    }
                }
                Column{
                    spacing: subColumnSpacing

                    WisenetCell{
                        id : secondaryCodecId
                        title : WisenetLinguist.codec
                        visible: singleChannelVisible
                        isSub: true
                    }
                    WisenetCell{
                        id : secondaryResolutionId
                        title : WisenetLinguist.resolution
                        visible: singleChannelVisible
                        isSub: true
                    }
                    WisenetCell{
                        id : secondaryFramerateId
                        title : WisenetLinguist.framerate
                        visible: singleChannelVisible
                        isSub: true
                    }
                    WisenetCell{
                        id : secondaryBitrateId
                        title : WisenetLinguist.bitrate
                        visible: singleChannelVisible
                        isSub: true
                    }
                }
            }
        }
        Rectangle{
            visible: singleChannelVisible
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.leftMargin: 10
            Layout.topMargin: 10
            width: deviceListChannelViewId.width - 430
            height: (width*0.5625 > deviceListChannelViewId.height -60)? (deviceListChannelViewId.height - 60) : width*0.5625
            color: WisenetGui.contrast_12_black
            //border.width: 1
            //border.color: WisenetGui.contrast_02_light_grey

            DeviceVideoItem {
                id: liveView
                anchors.fill: parent
                anchors.margins: 1
            }
        }
    }

    RowLayout{
        width: deviceListChannelViewId.width
        height: dialogButtonBox.height
        y : parent.height - 50
        id: applyCancelButtons

        WisenetDialogButtonBox{
            id: dialogButtonBox
            onAccepted: {
                var highP = channelListViewModel.getProfileId(primaryId.currentText)
                var lowP = channelListViewModel.getProfileId(secondaryId.currentText)

                var  mount = ChannelListViewModel.L_Ceiling
                if(mountId.currentText ===  WisenetLinguist.ceiling)
                    mount =ChannelListViewModel.L_Ceiling
                else if(mountId.currentText === WisenetLinguist.wall)
                    mount = ChannelListViewModel.L_Wall
                else if(mountId.currentText === WisenetLinguist.ground)
                    mount = ChannelListViewModel.L_Ground

                console.log("DeviceListChannelView changeChannelInfo " + mount  + "----- " + mountId.currentText + "-------" + ChannelListViewModel.L_Ceiling)


                channelListViewModel.changeChannelInfo(useChannelId.checked, fisheyeDewarpingId.checked,
                                                       mount, lensTypeId.currentText, highP, lowP, channelNameId.text)
            }
        }
    }
}
