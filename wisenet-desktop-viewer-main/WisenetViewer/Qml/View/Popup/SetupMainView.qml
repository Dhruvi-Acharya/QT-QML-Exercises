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

ApplicationWindow {
    id: setupMainId
    width: WisenetGui.popupWindowMinimumWidht
    height: WisenetGui.popupWindowMinimumHeight
    minimumWidth: WisenetGui.popupWindowMinimumWidht
    minimumHeight: WisenetGui.popupWindowMinimumHeight
    visible: false
    color: WisenetGui.color_setup_Background
    flags: Qt.FramelessWindowHint | Qt.Window
    title: WisenetLinguist.setup

    signal openDdnsP2pSetup(int index);

    // Declare properties that will store the position of the mouse cursor
    property int imageSize: 12
    property int previousX
    property int previousY
    property string windowState: "normal"

    property int setupMainTitleHeight: 40
    property int mainTreeItemHeight : 30
    property alias deviceFirmwareUpdate : deviceMaintenanceView.deviceFirmwareUpdate

    property int viewX : 0
    property int viewWidth : width
    property int viewY : 0
    property int viewHeight : height

    property int previousTreeSplitterY
    property int previousLeftBarX
    readonly property int leftPanelMaxWidth: 600
    readonly property int leftPanelMinWidth: 240

    Connections{
        target: deviceFirmwareUpdate
        onCancelSetupQuit :{
            setupMainId.visible = false
            setupMainId.close()
        }
        onCancelSubTree:{
            content.currentIndex = index
        }
        onCancelSubTreeReject:{
            selectTree(WisenetSetupTree.SubPageIndex.DeviceMainteance)
        }
    }

    // 홈페이지 열기
    function gotoHome()
    {
        setVisible(true);
        setupHomeView.visible = true;
        raise();
    }

    // 세부설정페이지 열기
    function gotoSetup(index)
    {
        if( index === WisenetSetupTree.SubPageIndex.DdnsP2pRegister) {
            setVisible(false)
            openDdnsP2pSetup(index);
            return;
        }

        setVisible(true);
        setupHomeView.visible = false;
        raise();

        if( index === WisenetSetupTree.SubPageIndex.DeviceListAddDevice )
        {
            selectTree(WisenetSetupTree.SubPageIndex.DeviceList)
            selectAddDevice()
        }
        else if( index === WisenetSetupTree.SubPageIndex.UserListAddUser )
        {
            selectTree(WisenetSetupTree.SubPageIndex.UserList)
            selectAddUser()
        }
        else if( index === WisenetSetupTree.SubPageIndex.UserGroupAddUserGroup )
        {
            selectTree(WisenetSetupTree.SubPageIndex.UserGroup)
            selectAddUserGroup()
        }
        else if( index === WisenetSetupTree.SubPageIndex.EventRulesAddEventRule )
        {
            selectTree(WisenetSetupTree.SubPageIndex.EventRules)
            selectAddEventRule()
        }
        else if(index === WisenetSetupTree.SubPageIndex.DeviceMainteanceChangePassword)
        {
            selectTree(WisenetSetupTree.SubPageIndex.DeviceMainteance)
            selectDeviceMainteanceChangePassword()
        }
        else
        {
            selectTree(index)
        }
    }

    function showAndHideLeftView(show){
        if (!show)
        {
            setupLeftView.state = "HIDE"
            setupLeftView.visible = false
            content.anchors.left = setupLeftViewRow.left
        }
        else
        {
            setupLeftView.state = "OPEN"
            setupLeftView.visible = true
            content.anchors.left = setupLeftView.right
        }
    }

    // 최대화/복귀 함수
    function doMaximize()
    {
        if(windowState != "maximized")
        {
            setupMainId.showMaximized()
            windowState = "maximized"
        }
        else
        {
            setupMainId.showNormal()
            windowState = "normal"
        }
    }

    function expandAll() {
        for(var i=0; i < menuTree.model.rowCount(); i++) {
            var proxyIndex = menuTree.model.index(i,0)
            if(!menuTree.isExpanded(proxyIndex)) {
                menuTree.expand(proxyIndex)
            }
        }
    }

    function treeToContentIndex(index){
        var childCount = 0
        for(var i = 0; i< index.parent.row; i++){
            var mainIndex = menuTree.model.sourceModel.index(i,0)
            childCount += menuTree.model.sourceModel.rowCount(mainIndex)
        }
        return childCount + index.row
    }

    function contentToTreeIndex(parentIndex, index){
        var childCount = 0
        for(var i = 0; i< parentIndex.row; i++){
            var mainIndex = menuTree.model.sourceModel.index(i,0)
            childCount += menuTree.model.sourceModel.rowCount(mainIndex)
        }
        return index - childCount
    }

    function getParentIndex(subIndexRow){
        var childCount = 0
        var parent = menuTree.model.sourceModel.index(0,0)
        for(var i = 0; i< menuTree.model.sourceModel.rowCount(); i++){
            var mainIndex  = menuTree.model.sourceModel.index(i,0)
            childCount += menuTree.model.sourceModel.rowCount(mainIndex)
            if(childCount > subIndexRow)
                return mainIndex
        }
    }

    function selectTree(subIndexRow, guid){
        var parentIndex = getParentIndex(subIndexRow)
        content.currentIndex = subIndexRow
        var treeIndex = menuTree.model.sourceModel.index(contentToTreeIndex(parentIndex, subIndexRow), 0, parentIndex)
        var proxyIndex = menuTree.model.mapFromSource(treeIndex)
        menuTree.selection.setCurrentIndex(proxyIndex, ItemSelectionModel.ClearAndSelect)
        if(guid)
            console.log("selectTree:",guid)
    }
    function selectDeviceList(devices){
        deviceListView.selectDevices(devices)
    }
    function selectDeviceListChannel(channels){
        deviceListView.selectChannels(channels)
    }
    function selectAddDevice(){
        deviceListView.selectAddDevice()
    }
    function selectAddUser(){
        userListView.openAddUser()
    }
    function selectAddUserGroup(){
        userGroupView.openAddUserGroup()
    }
    function selectAddEventRule(){
        eventRuleView.openAddEventRule()
    }
    function selectDeviceMainteanceChangePassword(){
        deviceMaintenanceView.openPasswordChange()
    }

    function searchEventLog(){
        eventLogView.search();
    }

    function initializeTree(){
        deviceListView.initializeTree()
    }

    Rectangle{
        id: setupMainBorder
        anchors.fill: parent
        //border.color: WisenetGui.contrast_07_grey
        //border.width: 1
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_07_grey
        border.width: 1


        Rectangle{
            id: setupMainTitle
            x: 1
            y: 1
            width: parent.width - 2
            height: setupMainTitleHeight
            color: "transparent"

            Rectangle{
                id: setupMainText
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
                id: setupMainWindowsControlView
                z: 6

                anchors{
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                    rightMargin: 1
                }

                onCloseButtonClicked: {
                    deviceFirmwareUpdate.askCancelFirmwareUpdate(WisenetSetupTree.SubPageIndex.SetupQuit)
                }

                onMinimizeButtonClicked: {
                    console.log("SetupMainView minimizeButton clicked")
                    if(Qt.platform.os === "windows") {
                        setupMainId.showMinimized()
                    }
                    else if (Qt.platform.os === "linux") {
                        // 2023.01.04. 우분투에서 최소화 안되는 현상
                        setupMainId.visible = false;
                    }
                    else {
                        // mac에서는 FramelessWindowHint set시 정상동작하지 않으므로, flag값을 변경한다.
                        if(Qt.platform.os === "osx") {
                            flags = Qt.Window | Qt.CustomizeWindowHint | Qt.WindowMinMaxButtonsHint
                        }
                        setupMainId.visibility = Window.Windowed
                        setupMainId.visibility = Window.Minimized
                    }
                }
                onMaximizeButtonClicked:doMaximize()
            }

            //Move SetupMain Window
            MouseArea {
                height: 30
                anchors.fill: parent
                anchors.rightMargin: setupMainWindowsControlView.width + 45

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
                    setupMainId.setX(setupMainId.x + dx)
                }

                onMouseYChanged: {
                    if(windowState == "maximized")
                    {
                        return
                    }

                    var dy = mouseY - previousY
                    setupMainId.setY(setupMainId.y + dy)
                }

                onDoubleClicked:
                {
                    doMaximize()
                }
            }
        }


        SetupHomeView {
            id: setupHomeView
            x: setupMainTitle.x
            y: setupMainTitle.y + setupMainTitle.height
            visible:true
            width: parent.width - (parent.border.width*2)
            height: parent.height - setupMainTitle.height - (parent.border.width*2)
            dictionary: setupDictionary.data
            onGotoSetup: setupMainId.gotoSetup(index)
        }

        Rectangle{
            id: setupLeftViewRow

            anchors.fill: parent
            anchors.topMargin: setupMainTitleHeight
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            anchors.bottomMargin: 1

            visible: !setupHomeView.visible

            Rectangle{
                id: setupLeftView

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                width: 240

                state: "OPEN"

                Behavior on width  {
                    id: leftViewWidthBehavior
                    NumberAnimation {
                        duration: 300; easing.type: Easing.InOutQuad
                    }
                }

                Rectangle{
                    id : bgHomeAndSearch
                    x : 0
                    y : 0
                    z : 100
                    width: parent.width
                    height : 70
                    Rectangle{
                        id:searchButton
                        x:0
                        y:0
                        z:100
                        width:parent.width
                        height:bgHomeAndSearch.height-homeButton.height
                        WisenetSearchBoxAutofill{
                            x:0
                            y:0
                            z:100
                            width:parent.width
                            height:parent.height
                            visible: setupMainId.visible
                            dictionary: setupDictionary.data
                            autofillList_x: 0
                            searchboxIcon_x: 21
                            onReturnData: setupMainId.gotoSetup(word_data)
                        }
                    }
                    Rectangle{
                        id:homeButton
                        x:0
                        y:searchButton.height
                        z:99
                        width:parent.width
                        height:40
                        color: WisenetGui.color_setup_Background
                        Image {
                            id :homeIcon
                            anchors.verticalCenter: parent.verticalCenter
                            x:17
                            width: 16
                            height: 16
                            sourceSize: "16x16"
                            source: WisenetImage.tree_side_home
                        }
                        Text{
                            id:homeText
                            text:"Home"
                            color: WisenetGui.contrast_05_grey
                            font.pixelSize: 12
                            anchors.verticalCenter: parent.verticalCenter
                            x:homeIcon.x+homeIcon.width + 8
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                gotoHome();
                            }
                            hoverEnabled: true
                            onEntered: homeText.color=WisenetGui.contrast_00_white
                            onExited: homeText.color=WisenetGui.contrast_05_grey
                        }
                    }
                }

                TreeView{
                    id: menuTree
                    //anchors.fill: parent
                    x:0
                    y:bgHomeAndSearch.height
                    width:parent.width
                    height:parent.height-bgHomeAndSearch.height
                    /*
                    width: parent.width
                    height: parent.height
                    Layout.fillHeight: true
                    */headerVisible: false
                    backgroundVisible: true
                    frameVisible: false


                    selection: ItemSelectionModel{
                        id: treeSelection
                        model: treeProxyModel
                    }

                    model: treeProxyModel
                    Component.onCompleted: expandAll()

                    TableViewColumn {
                        id: tvColumn
                        role: "displayRole"
                    }

                    onClicked: {
                        var treeIndex = menuTree.model.mapToSource(index)
                        var item =  menuTree.model.sourceModel.data(treeIndex)

                        if(item.status === WisenetSetupTree.MenuType.Main)
                            menuTree.isExpanded(index) ? menuTree.collapse(index) : menuTree.expand(index)
                        else{
                            deviceFirmwareUpdate.askCancelFirmwareUpdate(treeToContentIndex(treeIndex))
                        }
                    }

                    style : TreeViewStyle{

                        backgroundColor: WisenetGui.color_setup_Background
                        indentation: 0
                        rowDelegate: Rectangle{
                            id: rowDelegate
                            height: setupMainId.mainTreeItemHeight
                            color: WisenetGui.color_setup_Background
                        }
                        branchDelegate: Rectangle{
                            color: "transparent"
                        }

                        itemDelegate:Rectangle{
                            id: itemBackground
                            width: parent.width
                            height: setupMainId.mainTreeItemHeight
                            color: WisenetGui.transparent


                            Rectangle{
                                width: parent.width
                                height: 1
                                visible: styleData.hasChildren
                                color: WisenetGui.contrast_09_dark
                                anchors.top : parent.top
                            }
                            Rectangle{
                                width: parent.width
                                height: 1
                                visible: styleData.hasChildren
                                color: WisenetGui.contrast_09_dark
                                anchors.bottom: parent.bottom
                            }


                            Row{
                                id : itemRow
                                anchors.fill: parent
                                spacing: 2
                                leftPadding: 16
                                Image {
                                    id : itemImage
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: 18
                                    height: 18
                                    sourceSize: "18x18"
                                    visible: styleData.hasChildren===undefined ? false : styleData.hasChildren
                                    source: styleData.value.icon===undefined ? "" : styleData.value.icon
                                }

                                Text{
                                    id: treeText
                                    text: ((styleData.hasChildren)?"":"       ") + styleData.value.display
                                    color: styleData.selected ? WisenetGui.contrast_00_white: WisenetGui.contrast_05_grey
                                    font.pixelSize: 12
                                    anchors.verticalCenter: parent.verticalCenter
                                    leftPadding: 5
                                }
                            }

                            Image {
                                id : itemImag2
                                anchors.right: parent.right
                                anchors.rightMargin: 16
                                width: 12
                                height: 12
                                sourceSize: "12x12"
                                anchors.verticalCenter: parent.verticalCenter
                                visible: styleData.hasChildren
                                source: (styleData.isExpanded ? WisenetImage.tree_unfold : WisenetImage.tree_fold)
                            }
                        }
                    }
                }
            }

            StackLayout {
                id : content

                anchors.left: setupLeftView.right
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                currentIndex: -1

                /*onCurrentIndexChanged: {
                    switch(currentIndex){
                    case WisenetSetupTree.SubPageIndex.DeviceList:
                        deviceListView.visible = true
                        break
                    case WisenetSetupTree.SubPageIndex.EventLog:
                        eventLogView.visible = true;
                        break;
                    case WisenetSetupTree.SubPageIndex.SystemLog:
                        systemLogView.visible = true;
                        break;
                    case WisenetSetupTree.SubPageIndex.AuditLog:
                        auditLogView.visible = true;
                        break;
                    default:
                        break
                    }
                }*/

                //Device
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    DeviceListView{
                        id : deviceListView

                        anchors.fill: parent

                    }
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    DeviceMaintenanceView{
                        id : deviceMaintenanceView
                        anchors.fill: parent
                    }
                }

                //User
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    UserListView{
                        id: userListView
                        anchors.fill: parent
                    }
                }
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    UserGroupView{
                        id: userGroupView
                        anchors.fill: parent
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    LdapView{
                        id: ldapView
                        anchors.fill: parent
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                //Event
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    EventRulesView{
                        id:eventRuleView
                    }
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    ScheduleView{
                        id:scheduleView
                    }
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    EventEmailView{
                        id:eventEmailView
                    }
                }

                //Log
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    EventLogView{
                        id: eventLogView
                        anchors.fill: parent
                    }
                }

                //System Log
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    SystemLogView{
                        id: systemLogView
                        anchors.fill: parent
                    }
                }

                //Audit Log
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    AuditLogView{
                        id: auditLogView
                        anchors.fill: parent
                    }
                }

                //System
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    SystemLocalSettingsView{
                    }
                }
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    EventNotificationsView{
                    }
                }
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    SystemMaintenanceView{
                    }
                }
            }

            Image{
                id: foldLeftImage
                width: 7
                height: 24
                anchors.left: content.left
                anchors.leftMargin: foldLeftImage.width/2
                anchors.verticalCenter: parent.verticalCenter
                sourceSize: "7x24"

                source: {
                    if(foldLeftButtonMouseArea.pressed)
                    {
                        if(setupLeftView.state === "OPEN")
                            return WisenetImage.setup_left_arrow_hover
                        else
                            return WisenetImage.setup_right_arrow_hover
                    }
                    else if(foldLeftButtonMouseArea.containsMouse)
                    {
                        if(setupLeftView.state === "OPEN")
                            return WisenetImage.setup_left_arrow_hover
                        else
                            return WisenetImage.setup_right_arrow_hover
                    }
                    else
                    {
                        if(setupLeftView.state === "OPEN")
                            return WisenetImage.setup_left_arrow_normal
                        else
                            return WisenetImage.setup_right_arrow_normal
                    }
                }

                MouseArea{
                    id: foldLeftButtonMouseArea

                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        console.log("foldLeftButton")

                        if (setupLeftView.state === "OPEN")
                        {
                            showAndHideLeftView(false)
                        }
                        else
                        {
                            showAndHideLeftView(true)
                        }
                    }
                }
            }
        }
    }

    TreeProxyModel{
        id: treeProxyModel
        sourceModel: treeModel
    }
    TreeModel{
        id: treeModel
        TreeElement{
            property string display: WisenetLinguist.device
            property int status: WisenetSetupTree.MenuType.Main
            property string icon: WisenetImage.tree_side_device

            TreeElement{property string display: WisenetLinguist.deviceList; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.maintenance; property int status: WisenetSetupTree.MenuType.Sub}
        }
        TreeElement{
            property string display: WisenetLinguist.user
            property int status: WisenetSetupTree.MenuType.Main
            property string icon: WisenetImage.tree_side_user

            TreeElement{property string display: WisenetLinguist.userList; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.userGroup; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: "LDAP"; property int status: WisenetSetupTree.MenuType.Sub}

        }
        TreeElement{
            property string display: WisenetLinguist.event
            property int status: WisenetSetupTree.MenuType.Main
            property string icon: WisenetImage.tree_side_event

            TreeElement{property string display: WisenetLinguist.eventRules; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.eventSchedule; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.emailPageSubject; property int status: WisenetSetupTree.MenuType.Sub}
        }
        TreeElement{
            property string display: WisenetLinguist.log
            property int status: WisenetSetupTree.MenuType.Main
            property string icon: WisenetImage.tree_side_log

            TreeElement{property string display: WisenetLinguist.eventLog; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.systemLog; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.auditLog; property int status: WisenetSetupTree.MenuType.Sub}
        }
        TreeElement{
            property string display: WisenetLinguist.system
            property int status: WisenetSetupTree.MenuType.Main
            property string icon: WisenetImage.tree_side_system

            TreeElement{property string display: WisenetLinguist.localSettings; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.eventNotifications; property int status: WisenetSetupTree.MenuType.Sub}
            TreeElement{property string display: WisenetLinguist.maintenance; property int status: WisenetSetupTree.MenuType.Sub}
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

            if(setupMainId.height - dy >= setupMainId.minimumHeight)
            {
                setupMainId.setY(setupMainId.y + dy)
                setupMainId.setHeight(setupMainId.height - dy)
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

            if(setupMainId.height + dy >= setupMainId.minimumHeight)
                setupMainId.setHeight(setupMainId.height + dy)
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

            if(setupMainId.width - dx >= setupMainId.minimumWidth)
            {
                setupMainId.setX(setupMainId.x + dx)
                setupMainId.setWidth(setupMainId.width - dx)
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

            if(setupMainId.width + dx >= setupMainId.minimumWidth)
            {
                setupMainId.setWidth(setupMainId.width + dx)
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

            if(setupMainId.width - dx >= setupMainId.minimumWidth)
            {
                setupMainId.setX(setupMainId.x + dx)
                setupMainId.setWidth(setupMainId.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupMainId.height - dy >= setupMainId.minimumHeight)
            {
                setupMainId.setY(setupMainId.y + dy)
                setupMainId.setHeight(setupMainId.height - dy)
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

            if(setupMainId.width + dx >= setupMainId.minimumWidth)
                setupMainId.setWidth(setupMainId.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupMainId.height - dy >= setupMainId.minimumHeight)
            {
                setupMainId.setY(setupMainId.y + dy)
                setupMainId.setHeight(setupMainId.height - dy)
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

            if(setupMainId.width - dx >= setupMainId.minimumWidth)
            {
                setupMainId.setX(setupMainId.x + dx)
                setupMainId.setWidth(setupMainId.width - dx)
            }
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupMainId.height + dy >= setupMainId.minimumHeight)
                setupMainId.setHeight(setupMainId.height + dy)
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

            if(setupMainId.width + dx >= setupMainId.minimumWidth)
                setupMainId.setWidth(setupMainId.width + dx)
        }

        onMouseYChanged: {
            if(windowState == "maximized")
            {
                return
            }

            var dy = mouseY - previousY

            if(setupMainId.height + dy >= setupMainId.minimumHeight)
                setupMainId.setHeight(setupMainId.height + dy)
        }
    }

    WisenetSetupDictionary{
        id:setupDictionary
    }
}
