import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.impl 2.12
import QtQml.Models 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Wisenet.Setup 1.0
import "./Setup/Device"
import "./Setup/Event"
import "./Setup/System"
import "./Setup/User"
import "./Setup/Log"
import "qrc:/"

Item {
    id: setupHomeId
    property alias dictionary : searchBox.dictionary
    signal gotoSetup(var index);

    visible:false

    Rectangle{
        id: contentBG
        anchors.fill: parent
        color:WisenetGui.contrast_11_bg

        Rectangle{
            id: setupHomeTop
            anchors.horizontalCenter: parent.horizontalCenter
            //y:164 // GUI on Figma
            y: (164/995) * setupHomeId.height
            z:100
            height: 100
            width: 320
            color:WisenetGui.contrast_11_bg
            border.color: WisenetGui.transparent
            border.width: 1
            Rectangle{
                id:setupHomeLogo
                y:0
                width:300
                height:20
                anchors.horizontalCenter: parent.horizontalCenter
                color:WisenetGui.transparent
                Row{
                    height:parent.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10
                    Image{
                        id: wisenetviewerImage
                        x:0
                        height: 25
                        source: logoManager.useCustomLogo? "file:///" + logoManager.customLogoPath : WisenetImage.wisenet_viewer_logo
                        sourceSize: "185x19"
                        fillMode: Image.PreserveAspectFit
                        anchors.top: parent.top
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.bottomMargin: 0
                    }
                    Text{
                        id:setupText
                        height:parent.height
                        //horizontalAlignment: Text.AlignHCenter
                        //width:parent.width
                        //height:25
                        color: WisenetGui.contrast_00_white
                        text: WisenetLinguist.setup
                        font.pixelSize: 20
                        bottomPadding: 0
                        anchors.top: parent.top
                        anchors.verticalCenter: parent.verticalCenter
                        verticalAlignment: Text.AlignVCenter
                        //anchors.bottomMargin: 0
                    }
                }
            }

            WisenetSearchBoxAutofill{
                id:searchBox
                anchors.horizontalCenter: setupHomeTop.horizontalCenter
                visible:setupHomeId.visible
                y:35
                height:34
                width:320
                autofillList_x: 0
                onReturnData:{
                    gotoSetup( word_data )
                }
                searchboxBorderWidth: 1
                searchboxBorderColor: WisenetGui.contrast_08_dark_grey
                searchboxBgColor: WisenetGui.transparent
                onSearchTextChanged: {
                    if(text==="dizzyboysdoze")
                    {
                        itemDevice.setRot(true);
                        itemEvent.setRot(true);
                        itemSystem.setRot(true);
                        shortcutAddDevice.setRot(true);
                        shortcutAddUserGroup.setRot(true);
                    }
                    else if( text==="taketheredpill")
                    {
                        itemDevice.setRot(false);
                        itemEvent.setRot(false);
                        itemSystem.setRot(false);
                        shortcutAddDevice.setRot(false);
                        shortcutAddUserGroup.setRot(false);
                    }
                }
            }
        }

        Rectangle{
            id: setupHomeContent1
            anchors.horizontalCenter: parent.horizontalCenter
            //y: 322 // GUI on Figma
            y: (322/995) * setupHomeId.height
            //height: (contentBG.height-setupHomeContent1.y)-10
            height:200
            width : parent.width-10
            //border.color: WisenetGui.contrast_07_grey
            color:WisenetGui.contrast_11_bg
            //border.color: WisenetGui.contrast_07_grey
            //border.width: 1

            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                y : 0
                spacing: 24
                SetupHomeViewItem{
                    id:itemDevice
                    title: WisenetLinguist.device
                    subtitle:WisenetLinguist.deviceSetup
                    iconImage: WisenetImage.tree_side_device
                    numSubItems: 2
                    subItem1title:WisenetLinguist.deviceList
                    subItem1menuId: WisenetSetupTree.SubPageIndex.DeviceList
                    subItem2title:WisenetLinguist.maintenance
                    subItem2menuId: WisenetSetupTree.SubPageIndex.DeviceMainteance
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemUser
                    title: WisenetLinguist.user
                    subtitle:WisenetLinguist.userSetup
                    iconImage: WisenetImage.tree_side_user
                    numSubItems: 3
                    subItem1title:WisenetLinguist.userList
                    subItem1menuId: WisenetSetupTree.SubPageIndex.UserList
                    subItem2title:WisenetLinguist.userGroup
                    subItem2menuId: WisenetSetupTree.SubPageIndex.UserGroup
                    subItem3title: "LDAP"
                    subItem3menuId: WisenetSetupTree.SubPageIndex.Ldap
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemEvent
                    title: WisenetLinguist.event
                    subtitle:WisenetLinguist.eventSetup
                    iconImage: WisenetImage.tree_side_event
                    numSubItems: 3
                    subItem1title:WisenetLinguist.eventRules
                    subItem1menuId: WisenetSetupTree.SubPageIndex.EventRules
                    subItem2title:WisenetLinguist.eventSchedule
                    subItem2menuId: WisenetSetupTree.SubPageIndex.EventSchedule
                    subItem3title:WisenetLinguist.emailPageSubject
                    subItem3menuId: WisenetSetupTree.SubPageIndex.Email
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemLog
                    title: WisenetLinguist.log
                    subtitle:WisenetLinguist.logDetail
                    iconImage: WisenetImage.tree_side_log
                    numSubItems: 3
                    subItem1title:WisenetLinguist.eventLog
                    subItem1menuId: WisenetSetupTree.SubPageIndex.EventLog
                    subItem2title: WisenetLinguist.systemLog
                    subItem2menuId: WisenetSetupTree.SubPageIndex.SystemLog
                    subItem3title: WisenetLinguist.auditLog
                    subItem3menuId: WisenetSetupTree.SubPageIndex.AuditLog
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemSystem
                    title: WisenetLinguist.system
                    subtitle: WisenetLinguist.systemSetup
                    iconImage: WisenetImage.tree_side_system
                    numSubItems: 3
                    subItem1title:WisenetLinguist.localSettings
                    subItem1menuId: WisenetSetupTree.SubPageIndex.LocalSettings
                    subItem2title:WisenetLinguist.eventNotifications
                    subItem2menuId: WisenetSetupTree.SubPageIndex.EventNotifications
                    subItem3title:WisenetLinguist.maintenance
                    subItem3menuId: WisenetSetupTree.SubPageIndex.SystemMaintenance
                    onItemClicked:gotoSetup(index)
                }
            }//Row
        }//Content1

        Rectangle{
            id: setupHomeContent2
            anchors.horizontalCenter: parent.horizontalCenter
            y: (623/995) * setupHomeId.height
            height: 150
            width : parent.width-10
            color:WisenetGui.transparent
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                y : 0
                spacing: 30
                SetupHomeViewItem{
                    id:shortcutDDNS
                    state: "shortcut"
                    title: WisenetLinguist.ddnsAndP2pRegister
                    iconImage:WisenetImage.setup_add_DDNS_P2P
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.DdnsP2pRegister)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddDevice
                    state: "shortcut"
                    title: WisenetLinguist.addDevice
                    iconImage:WisenetImage.setup_add_device
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.DeviceListAddDevice)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddUser
                    state: "shortcut"
                    title: WisenetLinguist.addUser
                    iconImage:WisenetImage.setup_add_user
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.UserListAddUser)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddUserGroup
                    state: "shortcut"
                    title: WisenetLinguist.addUserGroup
                    iconImage:WisenetImage.setup_add_usergroup
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.UserGroupAddUserGroup)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddEventRule
                    state: "shortcut"
                    title: WisenetLinguist.addEventRule
                    iconImage:WisenetImage.setup_add_eventrule
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.EventRulesAddEventRule)
                }
            }
        }
    }
}
/*
Window {
    id: setupHomeId
    width: WisenetGui.popupWindowMinimumWidht
    height: WisenetGui.popupWindowMinimumHeight
    minimumWidth: WisenetGui.popupWindowMinimumWidht
    minimumHeight: WisenetGui.popupWindowMinimumHeight
    visible: false
    color: WisenetGui.contrast_11_bg
    flags: Qt.FramelessWindowHint | Qt.Window
    title: WisenetLinguist.setup

    signal gotoSetup(var index);
    // Declare properties that will store the position of the mouse cursor
    property int previousX
    property int previousY
    property string windowState: "normal"

    property int setupHomeTitleHeight: 40
    property int mainTreeItemHeight : 36
    //property alias deviceFirmwareUpdate : deviceMaintenanceView.deviceFirmwareUpdate

    // 최대화/복귀 함수
    function doMaximize()
    {
        console.log("doMaximize start state: " + windowState)
        if(windowState != "maximized")
        {
            setupHomeId.showMaximized()
            windowState = "maximized"
        }
        else
        {
            setupHomeId.showNormal()
            windowState = "normal"
        }
        console.log("doMaximize end state: " + windowState)
    }

    Rectangle{
        id: setupHomeBorder
        anchors.fill: parent
        //border.color: WisenetGui.contrast_07_grey
        //border.width: 1
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_07_grey
        border.width: 1


        Rectangle{
            id: setupHomeTitle
            x: 1
            y: 1
            width: parent.width - 2
            height: setupHomeTitleHeight
            color: "transparent"

            Rectangle{
                id: setupHomeText
                x: 30
                y: 13
                width: parent.width -x -1
                height: 14
                color: "transparent"
                Text{
                    text: WisenetLinguist.setup
                    anchors.fill: parent
                    color: WisenetGui.contrast_00_white
                    font.pixelSize: 12
                }
            }

            SetupControlView{
                id: setupHomeWindowsControlView
                z: 6

                anchors{
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                    rightMargin: 1
                }

                onCloseButtonClicked: {
                    setupHomeId.visible=false
                    setupHomeId.close()
                }

                onMinimizeButtonClicked: setupHomeId.showMinimized()
                onMaximizeButtonClicked:doMaximize()
            }

            //Move SetupHome Window
            MouseArea {
                height: 30
                anchors.fill: parent
                anchors.rightMargin: setupHomeWindowsControlView.width + 45

                onPressed: {
                    previousX = mouseX
                    previousY = mouseY
                }

                onMouseXChanged: {
                    if(windowState == "maximized")
                    {
                        return
                    }

                    var dx = mouseX - previousX
                    setupHomeId.setX(setupHomeId.x + dx)
                }

                onMouseYChanged: {
                    if(windowState == "maximized")
                    {
                        return
                    }

                    var dy = mouseY - previousY
                    setupHomeId.setY(setupHomeId.y + dy)
                }

                onDoubleClicked:
                {
                    doMaximize()
                    console.log("double clicked")
                }
            }
        }
    }

    Rectangle{
        id: contentBG
        anchors.horizontalCenter: parent.horizontalCenter
        y: setupHomeTitle.height+1
        width:SetupHomeView.width
        height:SetupHomeView.height-setupHomeTitle.height-border.width

        color:WisenetGui.contrast_11_bg
        border.color: WisenetGui.contrast_07_grey
        border.width: 1

        Rectangle{
            id: setupHomeTop
            anchors.horizontalCenter: parent.horizontalCenter
            //y:164 // GUI on Figma
            y: (164/995) * setupHomeId.height
            z:100
            height: 100
            width: 320
            color:WisenetGui.contrast_11_bg
            border.color: WisenetGui.transparent
            border.width: 1
            Rectangle{
                id:setupHomeLogo
                y:0
                width:300
                height:20
                anchors.horizontalCenter: parent.horizontalCenter
                color:WisenetGui.transparent
                Row{
                    height:parent.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 4
                    Image{
                        id: wisenetviewerImage
                        x:0
                        source: WisenetImage.wisenet_viewer_logo
                        sourceSize: "185x19"
                        anchors.bottomMargin: 0
                    }
                    Text{
                        id:setupText
                        height:parent.height
                        //horizontalAlignment: Text.AlignHCenter
                        //width:parent.width
                        //height:25
                        color: WisenetGui.contrast_00_white
                        text: WisenetLinguist.setup
                        font.pixelSize: 20
                        bottomPadding: 0
                        verticalAlignment: Text.AlignVCenter
                        //anchors.bottomMargin: 0
                    }
                }
            }

            WisenetSearchBoxAutofill{
                id:searchBox
                anchors.horizontalCenter: setupHomeTop.horizontalCenter
                visible:setupHomeId.visible
                y:35
                height:34
                width:320
                onReturnData:{
                        gotoSetup( word_data )
                }
                dictionary:setupDictionary.data
                searchboxBorderWidth: 1
                searchboxBorderColor: WisenetGui.contrast_08_dark_grey
                searchboxBgColor: WisenetGui.transparent
                onSearchTextChanged: {
                    if(text==="dizzyboysdoze")
                    {
                        itemDevice.setRot(true);
                        itemEvent.setRot(true);
                        itemSystem.setRot(true);
                        shortcutAddDevice.setRot(true);
                        shortcutAddUserGroup.setRot(true);
                    }
                    else if( text==="taketheredpill")
                    {
                        itemDevice.setRot(false);
                        itemEvent.setRot(false);
                        itemSystem.setRot(false);
                        shortcutAddDevice.setRot(false);
                        shortcutAddUserGroup.setRot(false);
                    }
                }
            }
        }

        Rectangle{
            id: setupHomeContent1
            anchors.horizontalCenter: parent.horizontalCenter
            //y: 322 // GUI on Figma
            y: (322/995) * setupHomeId.height
            //height: (contentBG.height-setupHomeContent1.y)-10
            height:200
            width : parent.width-10
            //border.color: WisenetGui.contrast_07_grey
            color:WisenetGui.contrast_11_bg
            //border.color: WisenetGui.contrast_07_grey
            //border.width: 1

            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                y : 0
                spacing: 24
                SetupHomeViewItem{
                    id:itemDevice
                    title: WisenetLinguist.device
                    subtitle:WisenetLinguist.deviceSetup
                    iconImage: WisenetImage.tree_side_device
                    numSubItems: 2
                    subItem1title:WisenetLinguist.deviceList
                    subItem1menuId: WisenetSetupTree.SubPageIndex.DeviceList
                    subItem2title:WisenetLinguist.maintenance
                    subItem2menuId: WisenetSetupTree.SubPageIndex.DeviceMainteance
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemUser
                    title: WisenetLinguist.user
                    subtitle:WisenetLinguist.userSetup
                    iconImage: WisenetImage.tree_side_user
                    numSubItems: 2
                    subItem1title:WisenetLinguist.userList
                    subItem1menuId: WisenetSetupTree.SubPageIndex.UserList
                    subItem2title:WisenetLinguist.userGroup
                    subItem2menuId: WisenetSetupTree.SubPageIndex.UserGroup
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemEvent
                    title: WisenetLinguist.event
                    subtitle:WisenetLinguist.eventSetup
                    iconImage: WisenetImage.tree_side_event
                    numSubItems: 1
                    subItem1title:WisenetLinguist.eventRules
                    subItem1menuId: WisenetSetupTree.SubPageIndex.EventRules
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemLog
                    title: WisenetLinguist.log
                    subtitle:WisenetLinguist.logDetail
                    iconImage: WisenetImage.tree_side_log
                    numSubItems: 3
                    subItem1title:WisenetLinguist.eventLog
                    subItem1menuId: WisenetSetupTree.SubPageIndex.EventLog
                    subItem2title: WisenetLinguist.systemLog
                    subItem2menuId: WisenetSetupTree.SubPageIndex.SystemLog
                    subItem3title: WisenetLinguist.auditLog
                    subItem3menuId: WisenetSetupTree.SubPageIndex.AuditLog
                    onItemClicked:gotoSetup(index)
                }
                SetupHomeViewItem{
                    id:itemSystem
                    title: WisenetLinguist.system
                    subtitle: WisenetLinguist.systemSetup
                    iconImage: WisenetImage.tree_side_system
                    numSubItems: 3
                    subItem1title:WisenetLinguist.localSetup
                    subItem1menuId: WisenetSetupTree.SubPageIndex.LocalSettings
                    subItem2title:WisenetLinguist.maintenance
                    subItem2menuId: WisenetSetupTree.SubPageIndex.SystemMaintenance
                    subItem3title:WisenetLinguist.eventNotifications
                    subItem3menuId: WisenetSetupTree.SubPageIndex.EventNotifications
                    onItemClicked:gotoSetup(index)
                }
            }//Row
        }//Content1

        Rectangle{
            id: setupHomeContent2
            anchors.horizontalCenter: parent.horizontalCenter
            y: (623/995) * setupHomeId.height
            height: 150
            width : parent.width-10
            color:WisenetGui.transparent
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                y : 0
                spacing: 30
                SetupHomeViewItem{
                    id:shortcutDDNS
                    state: "shortcut"
                    title: WisenetLinguist.ddnsAndP2pRegister
                    iconImage:WisenetImage.setup_add_DDNS_P2P
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.DdnsP2pRegister)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddDevice
                    state: "shortcut"
                    title: WisenetLinguist.addDevice
                    iconImage:WisenetImage.setup_add_device
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.DeviceListAddDevice)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddUser
                    state: "shortcut"
                    title: WisenetLinguist.addUser
                    iconImage:WisenetImage.setup_add_user
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.UserListAddUser)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddUserGroup
                    state: "shortcut"
                    title: WisenetLinguist.addUserGroup
                    iconImage:WisenetImage.setup_add_usergroup
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.UserGroupAddUserGroup)
                }
                Rectangle{
                    anchors.verticalCenter:  shortcutDDNS.verticalCenter
                    width:1
                    height:57
                    color:WisenetGui.contrast_08_dark_grey
                }
                SetupHomeViewItem{
                    id:shortcutAddEventRule
                    state: "shortcut"
                    title: WisenetLinguist.addEventRule
                    iconImage:WisenetImage.setup_add_eventrule
                    onItemClicked:gotoSetup(WisenetSetupTree.SubPageIndex.EventRulesAddEventRule)
                }
            }
        }
    }


    // 위쪽 크기 변경
    MouseArea {
        id: topBorder
        height: 3
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right

            // 모퉁이 비우기 위함.
            leftMargin: 3
            rightMargin: 3
        }

        // 커서 모양 변경
        // https://doc.qt.io/qt-5/qcursor.html
        cursorShape: windowState != "maximized" ? Qt.SizeVerCursor : Qt.ArrowCursor

        onPressed: {
            // We memorize the position along the Y axis
            previousY = mouseY
        }

        // When changing a position, we recalculate the position of the window, and its height
        onMouseYChanged: {
            // 최대화 모드일때 동작하지 않도록 수정
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupHomeId.height - dy >= setupHomeId.minimumHeight)
            {
                setupHomeId.setY(setupHomeId.y + dy)
                setupHomeId.setHeight(setupHomeId.height - dy)
            }
        }
    }

    // 아래쪽 크기 변경
    MouseArea {
        id: bottomBorder
        height: 3
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right

            leftMargin: 3
            rightMargin: 3
        }
        cursorShape: windowState != "maximized" ? Qt.SizeVerCursor : Qt.ArrowCursor

        onPressed: {
            previousY = mouseY
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupHomeId.height + dy >= setupHomeId.minimumHeight)
                setupHomeId.setHeight(setupHomeId.height + dy)
        }
    }

    // 왼쪽 크기 변경
    MouseArea {
        id: leftBorder
        width: 3
        anchors {
            top: topBorder.bottom
            bottom: bottomBorder.top
            left: parent.left

            topMargin: 3
            bottomMargin: 3
        }

        cursorShape: windowState != "maximized" ? Qt.SizeHorCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(setupHomeId.width - dx >= setupHomeId.minimumWidth)
            {
                setupHomeId.setX(setupHomeId.x + dx)
                setupHomeId.setWidth(setupHomeId.width - dx)
            }
        }
    }

    // 오른쪽 크기 변경
    MouseArea {
        id: rightBorder
        width: 3
        anchors {
            top: topBorder.bottom
            bottom: bottomBorder.top
            right: parent.right

            topMargin: 3
            bottomMargin: 3
        }

        cursorShape: windowState != "maximized" ? Qt.SizeHorCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(setupHomeId.width + dx >= setupHomeId.minimumWidth)
            {
                setupHomeId.setWidth(setupHomeId.width + dx)
            }
        }
    }

    // 좌상
    MouseArea {
        id: topLeftPoint
        height: 3
        width: 3
        anchors {
            top: parent.top
            left: parent.left
        }

        // 커서 모양 변경
        cursorShape: windowState != "maximized" ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(setupHomeId.width - dx >= setupHomeId.minimumWidth)
            {
                setupHomeId.setX(setupHomeId.x + dx)
                setupHomeId.setWidth(setupHomeId.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupHomeId.height - dy >= setupHomeId.minimumHeight)
            {
                setupHomeId.setY(setupHomeId.y + dy)
                setupHomeId.setHeight(setupHomeId.height - dy)
            }
        }
    }

    // 우상
    MouseArea {
        id: topRightPoint
        height: 3
        width: 3
        anchors {
            top: parent.top
            right: parent.right
        }

        // 커서 모양 변경
        cursorShape: windowState != "maximized" ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(setupHomeId.width + dx >= setupHomeId.minimumWidth)
                setupHomeId.setWidth(setupHomeId.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupHomeId.height - dy >= setupHomeId.minimumHeight)
            {
                setupHomeId.setY(setupHomeId.y + dy)
                setupHomeId.setHeight(setupHomeId.height - dy)
            }
        }
    }

    // 좌하
    MouseArea {
        id: bottomLeftPoint
        height: 3
        width: 3
        anchors {
            bottom: parent.bottom
            left: parent.left
        }

        // 커서 모양 변경
        cursorShape: windowState != "maximized" ? Qt.SizeBDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(setupHomeId.width - dx >= setupHomeId.minimumWidth)
            {
                setupHomeId.setX(setupHomeId.x + dx)
                setupHomeId.setWidth(setupHomeId.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupHomeId.height + dy >= setupHomeId.minimumHeight)
                setupHomeId.setHeight(setupHomeId.height + dy)
        }
    }

    // 우하
    MouseArea {
        id: bottomRightPoint
        height: 3
        width: 3
        anchors {
            bottom: parent.bottom
            right: parent.right
        }

        // 커서 모양 변경
        cursorShape: windowState != "maximized" ? Qt.SizeFDiagCursor : Qt.ArrowCursor

        onPressed: {
            previousX = mouseX
            previousY = mouseY
        }

        onMouseXChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dx = mouseX - previousX

            if(setupHomeId.width + dx >= setupHomeId.minimumWidth)
                setupHomeId.setWidth(setupHomeId.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupHomeId.height + dy >= setupHomeId.minimumHeight)
                setupHomeId.setHeight(setupHomeId.height + dy)
        }
    }

    WisenetSetupDictionary{
        id:setupDictionary
    }
}
*/

