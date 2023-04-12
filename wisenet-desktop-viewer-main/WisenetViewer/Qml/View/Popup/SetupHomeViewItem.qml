import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.impl 2.12
import QtQml.Models 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick 2.0

import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Wisenet.Setup 1.0
import "./Setup/Device"
import "./Setup/Event"
import "./Setup/System"
import "./Setup/User"
import "./Setup/Log"

Item {
    id:setupHomeItemId
    width: itemBG.width
    height: itemBG.height
    property bool timerTrigger : false;
    property var iconImage
    property string title : ""
    property string subtitle : ""
    property int numSubItems
    property string subItem1title : ""
    property string subItem2title : ""
    property string subItem3title : ""
    property int subItem1menuId
    property int subItem2menuId
    property int subItem3menuId

    signal itemClicked(var index);

    //property color itemRectBGColor: WisenetGui.transparent
    //property color itemRectBorderColor: WisenetGui.transparent
    property color itemRectBGColor: WisenetGui.contrast_10_dark
    property color itemRectBorderColor: WisenetGui.contrast_08_dark_grey

    states: [
        State {
            name: "shortcut"
            PropertyChanges {
                target: itemBG
                width:152
                height:132
                border.width: 0
                color:itemBG.color=WisenetGui.transparent;
            }
            PropertyChanges {
                target: topArea
                width:itemBG.width
                height:itemBG.height
                x:0
                y:0
            }
            PropertyChanges {
                target: topAreaMouseArea
                onEntered:{ shortcutFocusOnOff(true)}
                onExited:{shortcutFocusOnOff(false)}
            }
            PropertyChanges {
                target: iconArea
                anchors.horizontalCenter: topArea.horizontalCenter
                y:35
                width:36
                height:36
            }
            PropertyChanges {
                target: titleArea
                anchors.horizontalCenter: topArea.horizontalCenter
                y:81
                width:topArea.width
                height:40
            }
            PropertyChanges {
                target: title_id
                font.pixelSize: 12
                minimumPixelSize: 10
                horizontalAlignment: Text.AlignHCenter
                wrapMode:Text.Wrap
            }
            PropertyChanges {
                target: separation
                visible:false
            }
            PropertyChanges {
                target: bottomArea
                visible:false
            }
            PropertyChanges {
                target: subtitleArea
                visible:false
            }
        }
    ]

    function itemFocusOnOff( isOn ){
        if( isOn )
        {
            itemBG.color=itemRectBGColor;
            itemBG.border.color=WisenetGui.color_primary;
        }
        else
        {
            itemBG.color=itemRectBGColor;
            itemBG.border.color=itemRectBorderColor;
        }
    }
    function shortcutFocusOnOff( isOn ){
        if( isOn )
        {
            itemBG.color=itemRectBGColor;
            title_id.color=WisenetGui.color_primary
        }
        else
        {
            itemBG.color=WisenetGui.transparent;
            title_id.color=WisenetGui.contrast_00_white
        }
    }
    function setRot( isOn ){
        if( isOn ){
            timerTrigger = true;
        }
        else{
            timerTrigger = false;
            itemBG.rotation = 0;
        }
    }

    Rectangle{
        id:itemBG
        width:207
        height:190
        //color:"grey"
        border.width: 1
        color: itemRectBGColor
        border.color : itemRectBorderColor
        MouseArea{
            width:itemBG.width
            height:itemBG.height
            x:0
            y:0
            hoverEnabled: true
            onEntered:{ itemFocusOnOff(true)}
            onExited:{ itemFocusOnOff(false)}
            propagateComposedEvents: true
        }
        Rectangle{
            id:topArea
            width:(parent.width-x)-20 // "-20" to make smaller than parent, otherwise, parent's onExited is not working
            height:60
            color: WisenetGui.transparent
            y:30
            x:35
            Rectangle{
                id:iconArea
                x:4
                y:4
                width:45
                height:45
                //color:"red"
                color: WisenetGui.transparent
                Image{
                    id:iconImg
                    anchors.fill: parent
                    sourceSize: Qt.size(iconArea.width-2,iconArea.height-2)
                    source:iconImage
                }
            }
            Rectangle{
                id:titleArea
                x:iconArea.x + iconArea.width + 15
                y:0
                width:parent.width-titleArea.x
                height:30
                color:WisenetGui.transparent
                Text {
                    id:title_id
                    x:0
                    y:0
                    width:titleArea.width
                    height:titleArea.height
                    text: title
                    font.pixelSize: 24
                    color:WisenetGui.contrast_00_white
                    fontSizeMode: Text.HorizontalFit
                }
            }
            Rectangle{
                id:subtitleArea
                width:titleArea.width
                y:titleArea.height
                x:titleArea.x
                height:parent.height-titleArea.height
                color: WisenetGui.transparent
                Text {
                    id:subtitle_id
                    width:parent.width
                    height:parent.height
                    x:0
                    y:0
                    wrapMode:Text.Wrap
                    text: subtitle
                    font.pixelSize: 12
                    minimumPixelSize: 8
                    color: WisenetGui.contrast_05_grey
                    fontSizeMode: Text.HorizontalFit
                }
            }
            MouseArea{
                id:topAreaMouseArea
                width:parent.width
                height:parent.height
                x:0
                y:0
                onReleased:{
                    setupHomeItemId.itemClicked(subItem1menuId)
                }
                hoverEnabled: true
                onEntered:{ itemFocusOnOff(true);title_id.color=WisenetGui.color_primary; subtitle_id.color=WisenetGui.color_primary;}
                onExited:{ itemFocusOnOff(false);title_id.color=WisenetGui.contrast_00_white; subtitle_id.color=WisenetGui.contrast_05_grey;}
            }
        }
        Rectangle{
            id:separation
            height: 1
            width : 139
            x:34
            y: 90 + 2 // +2 for word wrap of title text
            color : WisenetGui.contrast_08_dark_grey
        }

        Rectangle{
            id:bottomArea
            x:35
            y:103
            width:itemBG.width-bottomArea.x
            height:itemBG.height-topArea.height
            //color:"grey"
            color: WisenetGui.transparent
            Column{
                anchors.fill: parent
                anchors.top: bottomArea.top
                y : 5
                id:bottomColumn
                spacing:5
                Text{
                    text: subItem1title
                    width:parent.width
                    font.pixelSize: 12
                    minimumPixelSize: 6
                    fontSizeMode: Text.HorizontalFit
                    //color: "yellow"
                    color: WisenetGui.contrast_00_white
                    MouseArea{
                        anchors.fill: parent
                        onReleased:{
                            if( numSubItems > 0 )
                                setupHomeItemId.itemClicked(subItem1menuId)
                        }
                        hoverEnabled: true
                        onEntered:{ itemFocusOnOff(true);parent.color=WisenetGui.color_primary}
                        onExited: {itemFocusOnOff(false);parent.color= WisenetGui.contrast_00_white}
                    }
                }
                Text{
                    text: subItem2title
                    width:parent.width
                    font.pixelSize: 12
                    minimumPixelSize: 6
                    fontSizeMode: Text.HorizontalFit
                    //color: "yellow"
                    color: WisenetGui.contrast_00_white
                    MouseArea{
                        anchors.fill: parent
                        onReleased:{
                            if( numSubItems > 1 )
                                setupHomeItemId.itemClicked(subItem2menuId)
                        }
                        hoverEnabled: true
                        onEntered:{ itemFocusOnOff(true);parent.color=WisenetGui.color_primary}
                        onExited: { itemFocusOnOff(false);parent.color= WisenetGui.contrast_00_white}
                    }
                }
                Text{
                    text: subItem3title
                    width:parent.width
                    font.pixelSize: 12
                    minimumPixelSize: 6
                    fontSizeMode: Text.HorizontalFit
                    //color: "yellow"
                    color: WisenetGui.contrast_00_white
                    MouseArea{
                        anchors.fill: parent
                        onReleased:{
                            if( numSubItems > 2 )
                                setupHomeItemId.itemClicked(subItem3menuId)
                        }
                        hoverEnabled: true
                        onEntered:{ itemFocusOnOff(true);parent.color=WisenetGui.color_primary}
                        onExited: { itemFocusOnOff(false);parent.color= WisenetGui.contrast_00_white}
                    }
                }
            }
        }
        Timer{
            id: itemTimer
            interval: 50;
            running: timerTrigger;
            repeat:true;
            onTriggered: {
                itemBG.rotation += 5;
            }
        }
    }
}
