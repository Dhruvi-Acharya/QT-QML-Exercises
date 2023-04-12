import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: cellDelegate

    function getStatusColor(value) {
        switch(value) {
        case 0: return "lightgray"
        case 1: return "white"
        case 2: return "hotpink"
        default: return "white";

        }
    }


    function checkIpVisible(value) {
        if (value >= 5 && value <=7)
            return true;
        return false;
    }

    function getType(value) {
        switch(value) {
        case 1:
            return Fa.fa_server;
        case 2:
            return Fa.fa_th;
        case 3:
            return Fa.fa_object_group;
        case 4:
            return Fa.fa_users_cog;
        case 6:
            return Fa.fa_archive;
        case 7:
            return Fa.fa_camera;
        case 8:
            return Fa.fa_video;
        case 10:
            return Fa.fa_user;
        default:
            return "";
        }
    }

    Rectangle {
        color: "transparent"
        RowLayout{
            FontImg {
                text: getType(styleData.value.itemType)
                color: getStatusColor(styleData.value.itemStatus)
            }

            ItemText{
                text: styleData.value.userDefineName+""
                color: getStatusColor(styleData.value.itemStatus)
            }

            ItemAddress{
                visible: checkIpVisible(styleData.value.itemType)
                text: styleData.value.deviceAddress+""
            }

            BusyIndicator {
                running: styleData.value.itemStatus === 0
                implicitHeight: 30
            }
        }

    }
}
