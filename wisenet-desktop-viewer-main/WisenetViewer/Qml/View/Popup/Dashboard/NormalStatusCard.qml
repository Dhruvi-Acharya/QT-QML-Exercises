import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import WisenetStyle 1.0

Rectangle {
    id: normalStatusCard

    width: 260
    height: 430
    color: WisenetGui.contrast_10_dark
    radius: 4

    border.width: 1
    border.color: WisenetGui.contrast_08_dark_grey

    property string objectType : WisenetLinguist.device
    property bool isCamera : false
    property int objectCount : 0

    Image{
        id: icon

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 18

        width: 36
        height: 36

        sourceSize.width: 36
        sourceSize.height: 36

        source: isCamera ? "qrc:/Assets/Icon/Dashboard/title_camera.svg" : "qrc:/Assets/Icon/Dashboard/title_recorder.svg"
    }

    Text{
        id: objectTypeText

        height:22
        anchors.top: parent.top
        anchors.left: icon.right
        anchors.topMargin: 17
        anchors.leftMargin: 9

        text: normalStatusCard.objectType

        color: WisenetGui.contrast_02_light_grey
        font.pixelSize: 16

        verticalAlignment: Text.AlignVCenter
    }

    Text{
        id: objectCountText

        height:26
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 15
        anchors.rightMargin: 21

        text: objectCount

        color: WisenetGui.contrast_02_light_grey
        font.pixelSize: 20

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
    }

    Rectangle{
        id: separator

        height: 1
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 56

        color: WisenetGui.contrast_08_dark_grey
    }

    Rectangle{
        id: imageRect

        anchors.top: separator.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: "transparent"

        Image{
            id: normalStatusImage

            width: 100
            height: 100
            sourceSize.width: 100
            sourceSize.height: 100

            anchors.centerIn: parent

            source: "qrc:/Assets/Icon/Dashboard/normal_status.svg"
        }

        Text{
            id: goodText

            width: 150
            height: 25

            anchors.top: normalStatusImage.bottom
            anchors.topMargin: 9
            anchors.horizontalCenter: parent.horizontalCenter

            color: WisenetGui.contrast_06_grey
            text: WisenetLinguist.good

            font.pixelSize: 16

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
