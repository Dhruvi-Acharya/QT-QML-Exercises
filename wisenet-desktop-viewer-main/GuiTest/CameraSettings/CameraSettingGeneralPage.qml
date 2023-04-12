import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import QtQuick.Extras 1.4
import "./controls" as Res

Rectangle{
    id: cameraSettingGeneralPage

    property int copyButtonBase : 6
    property int marginX: 20

    color: "#383838"
    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        id: mainLayout;

        Component.onCompleted:
        {
            generalModelId.append({ elementName: "Vendor",elementValue:cameraSettingModel.vendor})
            generalModelId.append({ elementName: "Model",elementValue:cameraSettingModel.model})
            generalModelId.append({ elementName: "Firmware",elementValue:cameraSettingModel.firmware})
            generalModelId.append({ elementName: "IP Address",elementValue:cameraSettingModel.ipAddress})
            generalModelId.append({
                            elementName: "Web Page",
                            textFormat: Text.RichText,
                            elementValue: cameraSettingModel.webPage
                            //onLinkActivated: Qt.openUrlExternally(cameraSetting.webPage)
                        })
            generalModelId.append({ elementName: "MAC Address",elementValue:cameraSettingModel.macAddress})
            generalModelId.append({ elementName: "Camera ID",elementValue:cameraSettingModel.cameraID})
            generalModelId.append({ elementName: "Primary Stream",elementValue:cameraSettingModel.primaryStream})
            generalModelId.append({ elementName: "Secondary Stream",elementValue:cameraSettingModel.secondaryStream})
        }

        anchors.margins: 5; x : 20; y : 10
        Layout.fillWidth: true

        RowLayout {
            id: modelName
            Layout.fillWidth: true
            TextField {
                id : deviceName
                //text : cameraSettingModel.name
                color: "white"
                background: Rectangle{color: "#383838"}
                font.bold: true
                font.pixelSize: 20
            }
            Image{ source: "controls/images/hmfile_hash_f0f01292.png"; fillMode: Image.Tile}
            Component.onCompleted:
            {
                deviceName.text = cameraSettingModel.name
            }

        }
        ListModel{
            id : generalModelId
        }

        GridLayout
        {
            id : grid
            columns: 3
            rowSpacing: 5
            columnSpacing: 20
            Repeater {
                model : generalModelId
                Layout.fillWidth: true
                Res.MyCellItemTitle{
                    text : elementName;
                    Layout.column: 0
                    Layout.row: index
                }
            }

            Repeater {
                model : generalModelId
                Layout.fillWidth: true
                Res.MyCellItem{
                    text : elementValue;
                    Layout.column: 1
                    Layout.row: index
                }
            }
            TextEdit{
                   id: myClipBoard
                   visible: false
               }
            Repeater{
                model : [copyButtonBase,copyButtonBase+1,copyButtonBase+2]
                Res.MyCopyButton{
                    Layout.column: 2
                    Layout.row: modelData

                    onRelease: {
                        myClipBoard.text = generalModelId.get(index + copyButtonBase).elementValue
                        myClipBoard.selectAll();
                        myClipBoard.copy();
                        console.debug("line number:" + index);
                    }
                }
            }
        }
    }
    Rectangle{
        id : secondRect

        property int marginX: 20
        property int marginY: 12

        x : 0
        y : 260
        width: parent.width
        height: parent.height - 100
        color : "#303030"
        Layout.fillHeight: true
        Layout.fillWidth: true

        Rectangle{
            id : imageControlId
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: secondRect.marginX
            Layout.rightMargin: secondRect.marginX
            width : (secondRect.width)/2 - secondRect.marginX - secondRect.marginX /2
            height: 150
            x : secondRect.marginX
            y : secondRect.marginY
            color: "transparent"
            ColumnLayout{
                id : iamgeControlLayout
                anchors.fill: imageControlId
                Layout.fillWidth: true
                Res.MyRowLayoutHeadText {text: "Image Control"}
                Res.MyRowLayoutLine{}
                GridLayout{
                    columns: 2
                    rowSpacing: 1
                    Layout.fillWidth: true
                    Res.MyCellItemTitle{text : "Aspect Ratio"}
                    Res.MyCombobox{
                        Layout.fillWidth: true
                        model : ["Auto", "4:3", "16:9", "1:1"]
                    }
                    Res.MyCellItemTitle{text : "Default rotation"}
                    Res.MyCombobox{
                        Layout.fillWidth: true
                        model : ["0 degrees", "90 degrees", "180 degrees", "270 degrees"]
                   }
                }
                Res.MyPaddingRect{}
            }
        }
        Rectangle{
            id: audioControl
            Layout.fillHeight: true
            Layout.fillWidth: true
            x: (secondRect.width / 2) + (marginX /2)
            y : secondRect.marginY
            Layout.leftMargin: secondRect.marginX
            Layout.rightMargin: secondRect.marginX

            width : (secondRect.width)/2 - (secondRect.marginX * 1.5)
            height: 150
            color: "transparent"
            ColumnLayout{
                id : audioControlLayout
                anchors.fill: audioControl
                Res.MyRowLayoutHeadText{text : "Audio"}
                Res.MyRowLayoutLine{}

                RowLayout{
                    Res.MyCheckBox{
                            id : enableAudioId
                            text: "Enable Audio"
                            checked: true
                    }
                }
               Res.MyPaddingRect{}
            }
        }
        Rectangle{
            id: authentication
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: secondRect.marginX
            Layout.rightMargin: secondRect.marginX
            x : secondRect.marginX
            y : 150
            width : (secondRect.width)/2 - secondRect.marginX - secondRect.marginX /2
            height: 150
            color: "transparent"
            ColumnLayout{
                anchors.fill: parent
                Res.MyRowLayoutHeadText{text : "Authentication"}
                Res.MyRowLayoutLine{}
                RowLayout{
                    Res.MyButton{
                         text: "Edit Credentials"
                         onClicked: {
                             modalWindow.id = cameraSettingModel.id
                             modalWindow.password = cameraSettingModel.password
                             console.debug(cameraSettingModel.id + " " + cameraSettingModel.password)
                             console.debug(modalWindow.id + " " + modalWindow.password)
                             modalWindow.show()
                         }
                    }
                }
                Res.MyPaddingRect{}
            }
        }
    }
    ApplicationWindow{
        id : modalWindow
        flags: Qt.Dialog
        width: 400
        height: 150
        modality: Qt.ApplicationModal
        color : "#303030"
        title: "Authentication credentials - Wisenet Viewer"
        minimumWidth: width
        maximumWidth: width

        minimumHeight: height
        maximumHeight: height

        property var id : ""
        property var password : ""

        Rectangle{
            Layout.topMargin: 10
            Layout.fillWidth: true
            GridLayout{
                y : 10
                x : 10
                Layout.margins: 10
                columns: 2
                rowSpacing: 10
                columnSpacing: 10
                Layout.fillWidth: true
                Res.MyCellItemTitle{text : "Login"}
                RowLayout {
                    Layout.preferredWidth: 315
                    Layout.fillWidth: true
                    TextField {
                        id : loginField
                        text : modalWindow.id
                        color: "white"
                        Layout.fillWidth: true
                        background: Rectangle{color: "#383838"}
                        font.bold: true
                        font.pixelSize: 15
                    }                    
                }
                Res.MyCellItemTitle{text : "Password"}
                RowLayout {
                    Layout.preferredWidth: 315
                    Layout.fillWidth: true
                    TextField {
                        id : passwordField
                        text : modalWindow.password
                        color: "white"
                        Layout.fillWidth: true
                        background: Rectangle{color: "#383838"}
                        font.bold: true
                        font.pixelSize: 15
                        echoMode: TextInput.Password
                    }
                }
            }

        }

        footer: DialogButtonBox{
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
                text: "Cancel"
                implicitWidth: 80
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }

            onAccepted: {
                modalWindow.close();
            }

            onRejected: {
                modalWindow.close();
            }
        }
    }
}

