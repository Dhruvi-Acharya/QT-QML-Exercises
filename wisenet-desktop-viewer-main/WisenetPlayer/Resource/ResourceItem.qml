import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0
import WisenetLanguage 1.0
import "qrc:/WisenetStyle"

Item {
    id: resourceItem

    property alias textColor : resourceText.color
    property alias borderColor : treeItem.border.color
    property alias borderWidth : treeItem.border.width
    property alias backColor : treeItem.color

    property string uuid : styleData.value.uuid ? styleData.value.uuid : ""
    property string resourceName : styleData.value.displayName ? styleData.value.displayName : ""
    property int resourceType : styleData.value.itemType ? styleData.value.itemType :  -1
    property int resourceStatus : styleData.value.itemStatus ? styleData.value.itemStatus : -1
    property int rowNumber : styleData.row
    property bool canDrag : dragAvailable(styleData.value.itemType)
    property bool brightIcon : false

    implicitWidth: treeItem.implicitWidth

    function getResourceIcon(type, status, name){
        if(brightIcon)
        {
            switch(type){
            case ItemType.MainRoot:
                break
            case ItemType.LocalFileRoot:
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
            case ItemType.LocalFileRoot:
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


    function dragAvailable(type) {
        if(type === ItemType.LocalFile)
            return true

        return false
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

    function getDisplayName(type, name){
        if(type === ItemType.LocalFileRoot)
            return WisenetLinguist.mediaFiles

        return decodeURIComponent(name)   // 특수문자 깨지는 문제 수정name
    }

    function isRootItem() {
        if(styleData.value.itemType === ItemType.MainRoot || styleData.value.itemType === ItemType.LocalFileRoot)
            return true
        else
            return false
    }

    Rectangle {
        id: treeItem
        anchors.fill: parent
        color: "transparent"
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
                source: getResourceIcon(resourceType, resourceStatus, resourceName)
            }

            Text {
                id: resourceText
                color: WisenetGui.treeTextColorNormal
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                font.pixelSize: 12
                elide: Qt.ElideRight
                anchors.verticalCenter: parent.verticalCenter
                text: getDisplayName(resourceType, resourceName)
                visible: true
            }
        }
    }
}

