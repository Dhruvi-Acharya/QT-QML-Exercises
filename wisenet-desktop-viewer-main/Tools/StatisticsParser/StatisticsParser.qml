import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

Window {
    width: 600
    height: 200
    visible: true
    title: qsTr("Statistics json parser")

    property int textMargin : 15

    Connections{
        target: mainViewModel

        function onGeneratedSuccessfully(){
            completeDialog.open()
        }
    }

    Rectangle{
        anchors.fill: parent
        color: "#121212"

        TextBox{
            id: folderText

            width : 400
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 50
            anchors.leftMargin: 50

            placeholderText: "Folder"
            readOnly: true

            text: ""

            onTextChanged: {
                mainViewModel.path = text
            }
        }

        Button{
            id: openFolder

            anchors.top: parent.top
            anchors.left: folderText.right
            anchors.topMargin: 40
            anchors.leftMargin: 20

            width: 100
            height: 30

            text: "Browse"
            font.pixelSize: 12

            onClicked: {
                folderDialog.visible = true
            }
        }

        Button{
            id: generateXlsx

            anchors.top: openFolder.bottom
            anchors.left: parent.left
            anchors.topMargin: 50
            anchors.leftMargin: 100

            width: 100
            height: 30

            text: "Generate .xlsx"
            font.pixelSize: 12

            onClicked: {
                if(folderText.text == "")
                {
                    alertDialog.open()
                    return;
                }

                mainViewModel.loadJson()
            }
        }
    }

    Dialog {
        id: alertDialog
        title: "Alert"
        standardButtons: Dialog.Ok

        Text{
            text: "Set the file path."
        }
    }

    Dialog {
        id: completeDialog
        title: "Alert"
        standardButtons: Dialog.Ok

        Text{
            text: "The result.xlsx file has been generated successfully."
        }

        onAccepted: {
            mainViewModel.openFolder()
        }
    }

    FileDialog {
        id: folderDialog
        visible: false
        nameFilters: ["Json files(*.json)"]
        selectMultiple: false
        folder: mainViewModel.path
        onAccepted: {
            folderText.text = filePathConveter.getPathByUrl(fileUrl)
        }
    }
}
