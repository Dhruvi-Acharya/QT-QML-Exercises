import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0
import WisenetStyle 1.0
import WisenetLanguage 1.0

Item {
    id: resourceItem

    property alias textColor : resourceText.color
    property alias borderColor : treeItem.border.color
    property alias borderWidth : treeItem.border.width
    property alias backColor : treeItem.color

    property string uuid : (styleData.value && styleData.value.uuid) ? styleData.value.uuid : ""
    property string deviceUuid : (styleData.value && styleData.value.deviceUuid) ? styleData.value.deviceUuid : ""
    property string resourceName : (styleData.value && styleData.value.displayName) ? styleData.value.displayName : ""
    property int resourceType : (styleData.value && styleData.value.itemType) ? styleData.value.itemType : 0
    property int resourceStatus : (styleData.value && styleData.value.itemStatus) ? styleData.value.itemStatus : 0
    property int rowNumber : styleData.row ? styleData.row : 0

    property int channelNumber : (styleData.value && styleData.value.channelNumber) ? styleData.value.channelNumber : 0
    property string itemId : (styleData.value && styleData.value.itemId) ? styleData.value.itemId : ""// layout

    property bool canDrag : (styleData.value && styleData.value.itemType) ? dragAvailable(styleData.value.itemType) : false
    property bool saveStatus : (styleData.value && styleData.value.saveStatus) ? styleData.value.saveStatus : false
    property var layoutViewModel : (styleData.value && styleData.value.layoutViewModel) ? styleData.value.layoutViewModel : null
    property string userId : ""

    property bool brightIcon : false

    property int itemWidth: 18 + resourceText.width

    function getResourceIcon(type, status, name){
        if(brightIcon)
        {
            switch(type){
            case ItemType.MainRoot:
                break
            case ItemType.ResourceRoot:
                return WisenetImage.root_hover
            case ItemType.Group:
                return WisenetImage.group_hover
            case ItemType.Device:
                if(status === ItemStatus.Normal)
                    return WisenetImage.device_hover
                else
                    return WisenetImage.device_disconnect_hover
            case ItemType.Camera:
            case ItemType.Channel:{
                if(status === ItemStatus.Normal)
                    return WisenetImage.camera_hover
                else if(status === ItemStatus.Unused)
                    return WisenetImage.camera_unused_hover
                else
                    return WisenetImage.camera_disconnect_hover
            }
            case ItemType.LayoutRoot:
                return WisenetImage.layout_hover
            case ItemType.Layout:
            {
                if(layoutViewModel.locked)
                    return WisenetImage.layout_locked_hover
                else if(layoutViewModel.ownerId !== userId)
                    return WisenetImage.shared_layout_hover
                else
                    return WisenetImage.layout_hover
            }
            case ItemType.SequenceRoot:
                return WisenetImage.sequence_main_icon_hover
            case ItemType.Sequence:
                return WisenetImage.sequence_main_icon_hover
            case ItemType.SequenceItem:
                return WisenetImage.layout

            case ItemType.WebPageRoot:
                return WisenetImage.webpage_hover
            case ItemType.WebPage:
                return WisenetImage.webpage_hover

            case ItemType.LocalFileRoot:
                return WisenetImage.mediafile_root_hover
            case ItemType.LocalDirectory:
                return WisenetImage.mediafile_root_hover
            case ItemType.LocalFile:
            {
                if(isImageFile(name))
                    return WisenetImage.image_file_hover
                else
                {
                    if(status === ItemStatus.NotSigned)
                        return WisenetImage.verify_no_hover
                    else if(status === ItemStatus.Manipulated)
                        return WisenetImage.verify_fail_hover
                    else if(status === ItemStatus.Authenticated)
                        return WisenetImage.verify_success_hover
                    else if(status === ItemStatus.Verifying)
                        return WisenetImage.verifying_hover
                    else
                        return WisenetImage.video_file_hover
                }
            }
            default:
                return WisenetImage.root_hover
            }
        }
        else
        {
            switch(type){
            case ItemType.MainRoot:
                break
            case ItemType.ResourceRoot:
                return WisenetImage.root_normal
            case ItemType.Group:
                return WisenetImage.group_normal
            case ItemType.Device:
                if(status === ItemStatus.Normal)
                    return WisenetImage.device_normal
                else
                    return WisenetImage.device_disconnect
            case ItemType.Camera:
            case ItemType.Channel:{
                if(status === ItemStatus.Normal)
                    return WisenetImage.camera_normal
                else if(status === ItemStatus.Unused)
                    return WisenetImage.camera_unused
                else
                    return WisenetImage.camera_disconnect
            }
            case ItemType.LayoutRoot:
                return WisenetImage.layout
            case ItemType.Layout:
            {
                if(layoutViewModel.locked)
                    return WisenetImage.layout_locked
                else if(layoutViewModel.ownerId !== userId)
                    return WisenetImage.shared_layout
                else
                    return WisenetImage.layout
            }
            case ItemType.SequenceRoot:
                return WisenetImage.sequence_main_icon_default
            case ItemType.Sequence:
                return WisenetImage.sequence_main_icon_default
            case ItemType.SequenceItem:
                return WisenetImage.layout

            case ItemType.WebPageRoot:
                return WisenetImage.webpage
            case ItemType.WebPage:
                return WisenetImage.webpage

            case ItemType.LocalFileRoot:
                return WisenetImage.mediafile_root_normal
            case ItemType.LocalDirectory:
                return WisenetImage.mediafile_root_normal
            case ItemType.LocalFile:
            {
                if(isImageFile(name))
                    return WisenetImage.image_file_normal
                else
                {
                    if(status === ItemStatus.NotSigned)
                        return WisenetImage.verify_no_default
                    else if(status === ItemStatus.Manipulated)
                        return WisenetImage.verify_fail_default
                    else if(status === ItemStatus.Authenticated)
                        return WisenetImage.verify_success_default
                    else if(status === ItemStatus.Verifying)
                        return WisenetImage.verifying_default
                    else
                        return WisenetImage.video_file_normal
                }
            }
            default:
                return WisenetImage.root_normal
            }
        }
    }


    function dragAvailable(type){
        switch(type){
        case ItemType.MainRoot:
            return false;
        case ItemType.ResourceRoot:
            return false;
        case ItemType.Group:
            return true;
        case ItemType.Device:
            return true;
        case ItemType.Camera:
            return true;
        case ItemType.Channel:
            return true;

        case ItemType.LayoutRoot:
            return false;
        case ItemType.Layout:
            return true;

        case ItemType.SequenceRoot:
            return false;
        case ItemType.Sequence:
            return true;

        case ItemType.WebPageRoot:
        case ItemType.WebPage:
            return true;

        case ItemType.UserRoot:
        case ItemType.Role:
        case ItemType.User:
            return false;

        case ItemType.LocalFileRoot:
            return false;
        case ItemType.LocalFile:
            return true;
        default:
            return false;
        }
    }

    function isImageFile(displayName){
        var splitText = displayName.split('.')

        var extension = splitText[splitText.length-1].toLowerCase();
        if(extension === "jpg" || extension === "jpeg" || extension === "png" || extension === "bmp" )
            return true
        else
            return false

    }

    function getStatusColor(value) {
        switch(value) {
        case ItemStatus.Loading:
            return WisenetGui.treeTextColorLoading
        case ItemStatus.Normal:
            return WisenetGui.treeTextColorNormal
        case ItemStatus.Abnormal:
            return WisenetGui.treeTextColorAbnormal
        default:
            return WisenetGui.treeTextColorNormal
        }
    }

    function saveLayout(){
        console.log("ResourceItem::saveLayout()")
        styleData.value.saveLayout()
    }

    function getDisplayName(type, name){
        switch(type){
        case ItemType.ResourceRoot:
            return WisenetLinguist.root
        case ItemType.LayoutRoot:
            return WisenetLinguist.layouts
        case ItemType.SequenceRoot:
            return WisenetLinguist.sequences
        case ItemType.WebPageRoot:
            return WisenetLinguist.webPages
        case ItemType.LocalFileRoot:
            return WisenetLinguist.mediaFiles
        default:
            return name;
        }
    }

    function isRootItem(){
        if(styleData.value.itemType === ItemType.MainRoot ||
                styleData.value.itemType === ItemType.ResourceRoot ||
                styleData.value.itemType === ItemType.LayoutRoot ||
                styleData.value.itemType === ItemType.SequenceRoot ||
                styleData.value.itemType === ItemType.WebPageRoot ||
                styleData.value.itemType === ItemType.UserRoot ||
                styleData.value.itemType === ItemType.LocalFileRoot)
            return true
        else
            return false
    }

    Rectangle {
        id: treeItem
        anchors.fill: parent

        border.width: 0
        border.color: WisenetGui.color_primary_dark
        //focus: true

        Row {
            anchors.fill: parent
            spacing: 4
            Image {
                anchors.verticalCenter: parent.verticalCenter
                width: 18
                height: 18
                sourceSize: "18x18"
                source: styleData.value && styleData.value.itemType && styleData.value.itemStatus && styleData.value.displayName ? getResourceIcon(styleData.value.itemType, styleData.value.itemStatus, styleData.value.displayName) : ""
            }

            ResourceText{
                text: "*"
                anchors.verticalCenter: parent.verticalCenter
                visible: !saveStatus && resourceType === ItemType.Layout
                color: WisenetGui.white
            }

            ResourceText{
                id: resourceText
                anchors.verticalCenter: parent.verticalCenter
                text: styleData.value && styleData.value.itemType && styleData.value.displayName ? getDisplayName(styleData.value.itemType, styleData.value.displayName) : ""
                //color: WisenetGui.contrast_01_light_grey //getStatusColor(styleData.value.itemStatus)
                visible: true
            }

            ResourceAddressText{
                text: (styleData.value && styleData.value.deviceAddress) ? "(" + styleData.value.deviceAddress + ")" : ""
                anchors.verticalCenter: parent.verticalCenter
                visible: styleData.value && styleData.value.itemType && styleData.value.itemType === ItemType.Device ? true : false
            }

            /*
            BusyIndicator {
                running: styleData.value.itemStatus === 0
                anchors.verticalCenter: parent.verticalCenter
                implicitHeight: 30
            }
            */
        }
    }
}

