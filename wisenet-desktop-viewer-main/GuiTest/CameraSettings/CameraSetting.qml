import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./controls" as Res

ApplicationWindow {
    id: cameraSettingWindow

    Component.onCompleted:
    {
       title: qsTr("Camera Settings - ") + cameraSettingModel.name + " - Wisenet Viewer Client"
    }
    flags: Qt.Dialog
    visible: true
    width: 800
    height: 600
    minimumWidth: 800
    minimumHeight: 600
    color: "#383838"

    header: TabBar {
        id: bar
        width: parent.width

        Repeater {
            model:[qsTr("General"), qsTr("Profile"), qsTr("Fisheye"), qsTr("Web Page"), qsTr("Expert"), qsTr("Manual")]

            TabButton {
                text: modelData
                width: implicitWidth
                font.capitalization: Font.Capitalize
            }
        }
    }
    footer: DialogButtonBox{
        id : footerId
        background: Rectangle{
            color: "transparent"       
        }

        buttonLayout: DialogButtonBox.WinLayout

        Res.MyButton{
            text: "OK"
            implicitWidth: 80
            normalColor: "#ff6c00"
            hoverColor: "#ff7d10"
            pressColor: "#f06200"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Res.MyButton{
            text: "Apply"
            implicitWidth: 80
            DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
        }
        Res.MyButton{
            text: "Cancel"
            implicitWidth: 80
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }

        onAccepted: {
            cameraSettingWindow.close();
        }

        onApplied: {
        }

        onRejected: {
            cameraSettingWindow.close();
        }
    }
    StackLayout {
        anchors.fill: parent
        currentIndex: bar.currentIndex
        Layout.fillWidth: true
        Layout.fillHeight: true

        CameraSettingGeneralPage {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        CameraSettingProfilePage {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        CameraSettingFisheyePage {

        }
        CameraSettingWebPage {
            Layout.fillHeight: true
            Layout.fillWidth: true

        }
        AddDeviceManual {
            Layout.fillHeight: true
            Layout.fillWidth: true

        }
    }
}
