import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1

Window {
    width: 480
    height: 430
    visible: true
    title: qsTr("Language converter")

    property int textMargin : 15

    Rectangle{
        anchors.fill: parent
        color: "#121212"

        TextBox{
            id: folderText

            width : 250
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
            anchors.leftMargin: 50

            width: 100
            height: 30

            text: "Browse"
            font.pixelSize: 12

            onClicked: {
                folderDialog.visible = true
            }
        }

        Button{
            id: tsToXlsx

            anchors.top: openFolder.bottom
            anchors.left: parent.left
            anchors.topMargin: 50
            anchors.leftMargin: 100

            width: 100
            height: 30

            text: ".ts to .xlsx"
            font.pixelSize: 12

            onClicked: {
                mainViewModel.tsToXlsx()
            }
        }

        Button{
            id: xlsxToTs

            anchors.top: openFolder.bottom
            anchors.left: tsToXlsx.right
            anchors.topMargin: 50
            anchors.leftMargin: 50

            width: 100
            height: 30

            text: ".xlsx to .ts"
            font.pixelSize: 12

            onClicked: {
                mainViewModel.xlsxToTs()
            }
        }

        Text{
            id: text0

            anchors.top: tsToXlsx.bottom
            anchors.left: parent.left
            anchors.topMargin: 30
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "0. WisenetLanguage 폴더에서 ts파일 삭제, make_ts.bat 실행"
            color: "white"
        }

        Text{
            id: text1

            anchors.top: text0.bottom
            anchors.left: parent.left
            anchors.topMargin: textMargin
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "1. lang_*.ts파일을 위에서 지정한 폴더에 놓는다."
            color: "white"
        }

        Text{
            id: text2

            anchors.top: text1.bottom
            anchors.left: parent.left
            anchors.topMargin: textMargin
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "2. .to to xlsx 버튼을 누르면 language.xlsx가 생성된다."
            color: "white"
        }

        Text{
            id: text3

            anchors.top: text2.bottom
            anchors.left: parent.left
            anchors.topMargin: textMargin
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "3. language.xlsx를 열어 번역본을 복사한다."
            color: "white"
        }

        Text{
            id: text4

            anchors.top: text3.bottom
            anchors.left: parent.left
            anchors.topMargin: textMargin
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "4. .xlsx to .ts 버튼을 누르면 output 폴더에 lang_*.ts 파일이 생성된다."
            color: "white"
        }

        Text{
            id: text5

            anchors.top: text4.bottom
            anchors.left: parent.left
            anchors.topMargin: textMargin
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "5. 생성된 lang_*.ts파일을 WisenetLanguage 폴더에 복사한다."
            color: "white"
        }

        Text{
            id: text6

            anchors.top: text5.bottom
            anchors.left: parent.left
            anchors.topMargin: textMargin
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "6. Linguist에서 lang_*.ts파일을 열고(드래그) Ctrl+S 를 누른다."
            color: "white"
        }

        Text{
            id: text7

            anchors.top: text6.bottom
            anchors.left: parent.left
            anchors.topMargin: textMargin
            anchors.leftMargin: 20

            font.pixelSize: 14
            text: "7. 메뉴에서 파일-모두 배포한다."
            color: "white"
        }
    }

    FolderDialog {
        id: folderDialog
        visible: false
        flags: FolderDialog.ShowDirsOnly
        folder: mainViewModel.path
        onAccepted: {
            folderText.text = filePathConveter.getPathByUrl(currentFolder)
        }
    }
}
