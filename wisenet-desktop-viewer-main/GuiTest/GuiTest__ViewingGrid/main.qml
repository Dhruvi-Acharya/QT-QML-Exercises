import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Extras 1.4
import "modelHelper.js" as DeviceModel
import WisenetMediaFramework 1.0

Window {
    id: mainWindow
    width: 1280
    height: 800
    visible: true
    title: qsTr("Viewing Grid Test")

    //onActiveFocusItemChanged: console.log("activeFocusItem", activeFocusItem)

    property bool isFullscreen: false
    function toggleFullScreen(onOff)
    {
        if (onOff) {
            mainWindow.visibility = 5;
            bottomPanel.height = 0;
            rightPanel.width = 0;
            leftPanel.width = 0;
        }
        else {
            mainWindow.visibility = 2;
            bottomPanel.height = 50;
            rightPanel.width = 200;
            leftPanel.width = 250;
        }
    }

    Connections {
        target: serviceModel
        function onNewDeviceRegistered(info) {
            console.log("onNewDeviceResgistered!!", info.name, info.ip, info.deviceId, info.channelId);
            DeviceModel.addDevice(info.name, info.ip, info.deviceId, info.channelId, info.status);
        }

        function onDeviceUpdated(info) {
            console.log("onDeviceUpdated!!", info.deviceId, info.channelId, info.status);
            DeviceModel.updateDevice(info.deviceId, info.channelId, info.status);
        }

        function onNewWebpageAdded(info) {
            console.log("onNewWebpageAdded!!", info.webpageId, info.name, info.url);
            DeviceModel.addWebpage(info.name, info.url, info.webpageId);
        }

        function onLayoutChanged() {
            console.log("onLayoutChanged!!");
            viewingGrid.setLayout(serviceModel.layout)
        }
    }

    Connections {
        target: serviceModel?serviceModel.layout:null
        function onSaveStatusChanged() {
            console.log('layout changed!! status=', serviceModel.layout.saveStatus);
            DeviceModel.addEvent("layout changed!!");
        }
    }

    Dialog {
        id: addWebDialog
        title: "Add new webpage"
        anchors.centerIn: parent
        width: 400
        height: 400
        focus: true
        modal: true

        Column {
            anchors.fill: parent
            spacing: 5
            TextField{
                id: name
                width:parent.width
                placeholderText: "Name"
                text: "PRN-6410 Webpage"
                selectByMouse: true
            }
            TextField{
                id: url
                width:parent.width
                placeholderText: "URL"
                //text:"http://192.168.255.13"
                text:"http://naver.com"
                selectByMouse: true
            }
            CheckBox {
                id: useAuth
                checked: false
                text: "Digest Authentication"
            }

            TextField{
                id: userID
                width:parent.width
                placeholderText: "Account"
                text: "admin"
            }
            TextField{
                id: userPassword
                width:parent.width
                placeholderText: "Password"
                text: "5rlarudejr!"
            }
        }



        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            serviceModel.addWebpage(name.text, url.text, useAuth.checked, userID.text, userPassword.text);
        }
    }

/*
    MediaLayoutViewModel {
        id: testViewModel
        cellRatioW:160
        cellRatioH:90
        locked:false
    }
*/
    // 뷰잉 그리드 메인
    ViewingGrid {
        id: viewingGrid
        anchors.fill: parent
        color: '#101010'
        viewX: viewRegion.x
        viewY: viewRegion.y
        viewWidth: viewRegion.width
        viewHeight: viewRegion.height
        isFocusedWindow: true

        Component.onCompleted: {
            console.log("set layout!!");
            viewingGrid.setLayout(serviceModel.layout)
            console.log("GraphicsINFO::api=", GraphicsInfo.api, ",version=", GraphicsInfo.majorVersion, ".", GraphicsInfo.minorVersion,
                        ",profile=", GraphicsInfo.profile, ",renderableType=", GraphicsInfo.renderableType,
                        ",shaderType=", GraphicsInfo.shaderType);
        }

        // focused item 변경
        onFocusedItemChanged: {
            console.log("Window::onFocusedItemChanged!!", focusedItem, focusedItem.hasPlayer);
        }

        // selected items 변경
        onSelectedItemsChanged: {
            console.log("Window::onSelectedItemsChanged!!", selectedItems, selectedItems.length);
        }

        onGridItemsChanged: {
            console.log("Window::onGridItemsChanged!!", gridItems);
        }

        onItemCountChanged: {
            console.log("Window::onItemCountChanged!!", itemCount);
        }

        // 미디어 요청 관련
        onMediaOpenResult: {
            console.log("Window::onMediaOpenResult!!", item.mediaParam.sourceType, error);
            var targetDesc = "";
            if (item.mediaParam.sourceType === WisenetMediaParam.LocalResource) {
                targetDesc = item.mediaParam.localResource.toString();
            }
            else if (item.mediaParam.sourceType === WisenetMediaParam.Camera){
                targetDesc = item.mediaParam.deviceId + "," +item.mediaParam.channelId;
            }

            if (error === WisenetMediaPlayer.NoError) {
                if (item.mediaParam.sourceType === WisenetMediaParam.LocalResource) {
                    var path = item.mediaParam.localResource.toString(); // url to string
                    DeviceModel.addVideoFile(path);
                }

                DeviceModel.addEvent("success:"+targetDesc);
            }
            else {
                DeviceModel.addEvent("failed:"+targetDesc);
            }
        }
        onMediaOpenRequested: {
            console.log("Window::onMediaOpenRequested!!", items);
        }
        onMediaStatusChanged: {
            console.log("Window::onMediaStatusChanged!!", item, status);
        }

        // 레이아웃, 레이아웃 아이템 변경시 리소스에서 관리하는 레이아웃 모델 데이터 동기화
        // 아이템 추가
        onItemsAdded: {
            console.log("Window::onItemsAdded!!", items);
            serviceModel.layout.syncAddedItems(items);
        }
        // 아이템 삭제
        onItemsRemoved: {
            console.log("Window::onItemsRemoved!!", itemIds);
            serviceModel.layout.syncRemovedItems(itemIds);
        }
        // 아이템 변경(이동,리사이징,아이템 속성)
        onItemsUpdated: {
            console.log("Window::onItemsUpdated!!", items);
            serviceModel.layout.syncUpdatedItems(items);
        }
        // 레이아웃 설정값 변경 (셀종횡비, lock 설정 등)
        onLayoutPropertyUpdated: {
            console.log("Window::onLayoutPropertyUpdated!!", layoutModel);
            serviceModel.layout.syncPropertyUpdatdLayout(layoutModel);
        }

        // 전체화면 on/off
        onShowFullscreen: {
            console.log("Window::onShowFullscreen!!", onOff);
            mainWindow.toggleFullScreen(onOff)
        }

        // 새탭에서 열기
        onOpenInNewTabTriggered: {
            console.log("Window::onOpenInNewTabTriggered!!", items);
        }

        // 새창에서 열기
        onOpenInNewWindowTriggered: {
            console.log("Window::onOpenInNewWindowTriggered!!", items);
        }

        // 카메라 설정 페이지 열기
        onCameraSetupOpenTriggered: {
            console.log("Window::onOpenInNewWindowTriggered!!", items);
            for (var i = 0 ; i < items.length ; i++) {
                console.log("deviceId=", items[i].deviceId, ", channelId=", items[i].channelId);
            }
        }

        // 이벤트 룰 설정 페이지 열기
        onEventRulesSetupOpenTriggered: {
            console.log("Window::onEventRulesSetupOpenTriggered!!", items);
        }
    }

    Rectangle {
        id: leftPanel
        color: '#101010'
        opacity: 0.7
        width: 250
        anchors {
            left:parent.left
            top:parent.top
            bottom:parent.bottom
        }
        Row {
            id:info
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 10
            anchors.leftMargin: 10
            anchors.rightMargin: 5
            Text {
                id: itemHead
                text: "ITEM : "
                color: "white"
                font.bold: true
                font.pointSize: 15
            }
            Text {
                text: viewingGrid.itemCount
                color: "white"
                font.bold: true
                font.pointSize: 15
            }
        }
        Rectangle {
            id: deviceRect
            anchors {
                top: info.bottom
                topMargin: 30
                left: info.left
                leftMargin: 10
                right: leftPanel.right
                rightMargin: 10
            }

            height: 300
            color: "transparent"
            ListModel{
                id: devices
            }

            // 장치 리스트 표시 리스트뷰
            ListView {
                anchors.fill: parent
                id: deviceListView
                model:devices
                delegate: Component {
                    Item {                       
                        width: deviceRect.width
                        height: 15//status==="normal" ? 15 : 0
                        visible: true//status==="normal" ? true : false
                        //Drag.active:
                        Text {
                            id : deviceText
                            anchors.centerIn: parent
                            color:status==="normal"?"white":"red"
                            font.bold: true
                            font.pixelSize: 11
                            text: deviceName + "-" + channelID +"(" + deviceIP + ")"
                            Drag.active: dragArea.drag.active
                            Drag.dragType: Drag.Automatic
                            Drag.supportedActions: Qt.CopyAction
                            Drag.mimeData: {
                                "text/uri-list" : "wisenetviewer://dragsource/cameras"
                            }
                            Drag.source: devices
                        }
                        MouseArea {
                            id: dragArea
                            drag.target: deviceText

                            anchors.fill: parent
                            onClicked: {
                                deviceListView.currentIndex = index;
                            }
                            onDoubleClicked: {
                                var deviceId = devices.get(index).deviceID;
                                var channelId = devices.get(index).channelID;
                                console.log("double click device item", index, deviceId, channelId)

                                viewingGrid.addNewVideoChannel(deviceId, channelId);
                                //viewingGrid.addNewVideoChannel(deviceId, channelId, true, 1633424649000);
                                //viewingGrid.checkNewVideoChannel(deviceId, channelId);
                            }
                            onPressed: deviceText.grabToImage(function(result) {
                                            deviceText.Drag.imageSource = result.url;
                                       })
                        }
                    }
                }
                highlight: Rectangle {
                    color: 'orange'
                }
                //focus: true
            }
        }

        Rectangle {
            id: videoRect
            anchors {
                top: deviceRect.bottom
                topMargin: 20
                left: info.left
                leftMargin: 10
                right: leftPanel.right
                rightMargin: 10
            }
            color: "transparent"
            height: 120
            ListModel{
                id: videos
            }
            // 파일 리스트 표시 리스트뷰
            ListView {
                anchors.fill: parent
                id: videoFileListView
                model:videos
                delegate: Component {
                    Item {
                        width: parent.width
                        height: 15
                        Text {
                            id : videoText
                            anchors.centerIn: parent
                            color:"white"
                            font.bold: true
                            font.pixelSize: 11
                            text: filePath;
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                videoFileListView.currentIndex = index;
                            }
                            onDoubleClicked: {
                                var filePath = videos.get(index).filePath;
                                console.log("double click file item", index, filePath)
                                viewingGrid.addNewLocalFile(filePath);
                            }
                        }
                    }
                }
                highlight: Rectangle {
                    color: 'orange'
                }
                //focus: true
            }
        }
        Rectangle {
            anchors {
                top: videoRect.bottom
                topMargin: 20
                left: info.left
                leftMargin: 10
                right: leftPanel.right
                rightMargin: 10
                bottom: leftPanel.bottom
            }
            color: "transparent"
            ListModel{
                id: webpages
            }
            // 파일 리스트 표시 리스트뷰
            ListView {
                anchors.fill: parent
                id: webpageListView
                model:webpages
                delegate: Component {
                    Item {
                        width: parent.width
                        height: 15
                        Text {
                            id : webpageText
                            anchors.centerIn: parent
                            color:"white"
                            font.bold: true
                            font.pixelSize: 11
                            text: webpageName+"("+webpageUrl+")";
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                webpageListView.currentIndex = index;
                            }
                            onDoubleClicked: {
                                var webpageId = webpages.get(index).webpageID;
                                console.log("double click webpage item", index, webpageId)
                                viewingGrid.addNewWebpage(webpageId);
                            }
                        }
                    }
                }
                highlight: Rectangle {
                    color: 'orange'
                }
            }
        }
    }
    Rectangle {
        id: rightPanel
        color: '#101010'
        opacity: 0.7
        width: 200
        anchors {
            right:parent.right
            top:parent.top
            bottom:parent.bottom
        }
        ListModel{
            id: events
        }
        // 파일 리스트 표시 리스트뷰
        ListView {
            anchors.fill: parent
            id: eventsListView
            model:events
            delegate: Component {
                Item {
                    width: parent.width
                    height: 20
                    Text {
                        id : eventText
                        anchors.centerIn: parent
                        color:"white"
                        font.bold: true
                        font.pointSize: 8
                        text: description;
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            eventsListView.currentIndex = index;
                        }
                    }
                }
            }
            highlight: Rectangle {
                color: 'darkorange'
            }
            //focus: true
        }
    }
    Rectangle {
        id: topPanel
        color: '#101010'
        opacity: 0.7
        height: 80
        property int childHeight : height/2
        anchors {
            left: leftPanel.right
            right:rightPanel.left
            top:parent.top
        }

        Flow {
            id: btns
            anchors.fill:parent
            anchors.margins: 5

            spacing: 5
            z: 10

            TextField {
                id: ipText
                width: 110
                height:topPanel.childHeight
                font.pointSize: 8
                text: "192.168.255.201"
                //text: "192.168.255.21"
                //text: "192.168.255.13"
                //text: "192.168.10.100"
            }
            TextField {
                id: pwText
                width: 80
                height:topPanel.childHeight
                font.pointSize: 8
                text: "5rlarudejr!"
                //text: "000ppp[[["
            }
            Button {
                width: 80
                height:topPanel.childHeight
                font.pointSize: 8
                text:"Register"
                onClicked: {
                    console.log("Register", ipText.text, pwText.text);
                    serviceModel.registerDevice(ipText.text, pwText.text);
                }
            }
            Rectangle {
                width:20
                height:topPanel.childHeight
                color: "transparent"
            }

            // 테스트용 버튼
//            Button {
//                width: 80
//                height:topPanel.childHeight
//                font.pointSize: 8
//                text: "+1 Clone"
//                onClicked : {
//                    viewingGrid.cloneFocusedItemTest(1);
//                }
//            }
//            Button {
//                width: 80
//                height:topPanel.childHeight
//                font.pointSize: 8
//                text: "+10 Clone"
//                onClicked : {
//                    viewingGrid.cloneFocusedItemTest(10);
//                }
//            }
            Rectangle {
                width:20
                height:topPanel.childHeight
                color: "transparent"
            }
            Button {
                width: 60
                height:topPanel.childHeight
                font.pointSize: 8
                text: "Clear"
                onClicked : {
                    console.debug("clearButton clicked");
                    viewingGrid.cleanUp();
                }

            }
            Button {
                width: 60
                height:topPanel.childHeight
                font.pointSize: 8
                text: "AutoFit"
                onClicked : {
                    console.debug("AutoFit clicked");
                    viewingGrid.setAutoFit(true);
                }
            }

            Button {
                width: 60
                height:topPanel.childHeight
                font.pointSize: 8
                text: "WEB"
                onClicked : {
                    addWebDialog.open()
                }
            }
        }
    }
    Rectangle {
        id: bottomPanel
        color: '#101010'
        opacity: 0.7
        height: 50
        property int childHeight : 40
        anchors {
            left: leftPanel.right
            right:rightPanel.left
            bottom:parent.bottom
        }
        Flow {
            id: controlBtns
            anchors.fill:parent
            anchors.margins: 5
            spacing: 5
            z: 10

            Label {
                visible: viewingGrid.focusedItem.hasPlayer ? true : false
                id: playTimeLabel
                width: 200
                height:bottomPanel.childHeight
                font.pointSize: 11
                color: "orange"
                font.bold: true
                text: formatTime(viewingGrid.focusedItem.mediaPlayPosition)

                function formatTime(timeInMs)
                {
                    var d = new Date(0);
                    d.setUTCMilliseconds(timeInMs);
                    var str = Qt.formatDateTime(d, "yyyy-MM-dd hh:mm:ss.zzz");
                    //console.log("time", d, timeInMs, d.toString(), str);
                    return str;
                }
            }
            Label {
                visible: viewingGrid.focusedItem.mediaSeekable ? true : false
                id: playSpeed
                width: 50
                height:bottomPanel.childHeight
                font.pointSize: 10
                color: "orange"
                font.bold: true
                text: viewingGrid.focusedItem.hasPlayer?formatSpeed(viewingGrid.focusedItem.mediaPlaySpeed):1.0

                function formatSpeed(speed)
                {
                    var s = speed.toFixed(1);
                    return s;
                }
            }

            Button {
                width: 80
                height:bottomPanel.childHeight
                visible: (viewingGrid.focusedItem.mediaSeekable)
                font.pointSize: 8
                text:"LIVE"
                onClicked: {
                    if (viewingGrid.focusedItem.isCamera) {
                        viewingGrid.focusedItem.player.changeCameraStream(WisenetMediaParam.LiveAuto);
                    }
                }
            }
            Button {
                width: 80
                height:bottomPanel.childHeight
                visible: (!viewingGrid.focusedItem.mediaSeekable)
                font.pointSize: 8
                text: "PLAYBACK"
                onClicked : {
                    if (viewingGrid.focusedItem.isCamera) {
                        var hour = 3600*1000;
                        var pos = viewingGrid.focusedItem.mediaPlayPosition - hour;
                        console.log("request playback time=", pos, ",current=", viewingGrid.focusedItem.mediaPlayPosition)
                        viewingGrid.focusedItem.player.changeCameraStream(WisenetMediaParam.PlaybackAuto, "", pos, 0, 0);
                    }
                }
            }
            Button {
                width: 40
                height:bottomPanel.childHeight
                visible: (viewingGrid.focusedItem.mediaSeekable)
                font.pointSize: 8
                text: "-1H"
                onClicked : {
                    var hour = 3600*1000;
                    var pos = viewingGrid.focusedItem.mediaPlayPosition - hour;
                    viewingGrid.focusedItem.player.seek(pos);
                }
            }
            Button {
                width: 40
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "+1H"
                onClicked : {
                    var hour = 3600*1000;
                    var pos = viewingGrid.focusedItem.mediaPlayPosition + hour;
                    viewingGrid.focusedItem.player.seek(pos);
                }
            }/*
            Button {
                width: 40
                height:bottomPanel.childHeight
                visible: (viewingGrid.focusedItem.mediaSeekable)
                font.pointSize: 8
                text: "-1M"
                onClicked : {
                    var pos = viewingGrid.focusedItem.mediaPlayPosition - 60000;
                    viewingGrid.focusedItem.player.seek(pos);
                }
            }
            Button {
                width: 40
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "+1M"
                onClicked : {
                    var pos = viewingGrid.focusedItem.mediaPlayPosition + 60000;
                    viewingGrid.focusedItem.player.seek(pos);
                }
            }
            */
            Button {
                width: 50
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "-10s"
                onClicked : {
                    var pos = viewingGrid.focusedItem.mediaPlayPosition - 10000;
                    viewingGrid.focusedItem.player.seek(pos);
                }
            }
            Button {
                width: 50
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "+10s"
                onClicked : {
                    var min = 60*1000;
                    var pos = viewingGrid.focusedItem.mediaPlayPosition + 10000;
                    viewingGrid.focusedItem.player.seek(pos);
                }
            }

            Button {
                width: 30
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "|>"
                onClicked : {
                    viewingGrid.focusedItem.player.step(true);
                }
            }
            Button {
                width: 30
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "<|"
                onClicked : {
                    viewingGrid.focusedItem.player.step(false);
                }
            }
            /*
            Button {
                width: 30
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "1x"
                onClicked : {
                    viewingGrid.focusedItem.mediaPlaySpeed = 1.0;
                }
            }
            Button {
                width: 30
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "2x"
                onClicked : {
                    viewingGrid.focusedItem.mediaPlaySpeed = 2.0;
                }
            }
            */
            Button {
                width: 30
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "4x"
                onClicked : {
                    viewingGrid.focusedItem.mediaPlaySpeed = 4.0;
                }
            }
            Button {
                width: 40
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "-2x"
                onClicked : {
                    viewingGrid.focusedItem.mediaPlaySpeed = -2.0;

                }
            }
            Button {
                width: 60
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "PAUSE"
                onClicked : {
                    viewingGrid.focusedItem.player.pause();
                }
            }
            Button {
                width: 50
                visible: (viewingGrid.focusedItem.mediaSeekable)
                height:bottomPanel.childHeight
                font.pointSize: 8
                text: "PLAY"
                onClicked : {
                    viewingGrid.focusedItem.player.play();

                }
            }
        }
    }
    Rectangle {
        id: viewRegion
        visible:false
        anchors {
            left: leftPanel.right
            right: rightPanel.left
            top: topPanel.bottom
            bottom: bottomPanel.top
        }
    }


}
