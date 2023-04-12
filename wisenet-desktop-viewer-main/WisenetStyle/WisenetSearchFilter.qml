import QtQuick 2.0
import QtQuick.Controls 2.15
import WisenetLanguage 1.0

Item {
    signal searchFilterTextChanged(var text)

    property string lastDisplayText: ""

    property alias textFieldfocus: input.focus
    property alias bgColor: wrapper.color
    width: parent.width
    height: 30

    function reset(){
        input.text = ""
    }

    Rectangle{
        id: wrapper
        anchors.fill: parent
        color: WisenetGui.contrast_08_dark_grey
        border.width: 1
        border.color: WisenetGui.contrast_07_grey

        TextField {
            id: input
            //focus: true//글씨를 바로 써도 입력되게끔
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: searchImage.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 20
            anchors.rightMargin: 12

            placeholderText: WisenetLinguist.search
            placeholderTextColor: WisenetGui.contrast_06_grey

            font.pixelSize: 12
            color: WisenetGui.contrast_02_light_grey

            background: Rectangle {
                color: WisenetGui.transparent
            }

            validator: RegExpValidator{
                regExp: /[^`]+/
            }

            Keys.onPressed: {
                if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter){
                    console.debug("WiseneSearchFilter Return/Enter - searchTextChanged: " + input.displayText)
                    lastDisplayText = input.displayText
                    searchFilterTextChanged(input.displayText)
                }
            }
        }

        WisenetImageButton{
            id: searchImage
            x: parent.width - 35
            anchors.verticalCenter: parent.verticalCenter

            width:11
            height:11

            //visible: notificationItemMouseArea.containsMouse

            checkable: false
            imageSelected: "/WisenetStyle/Icon/search-press.svg"
            imageNormal: "qrc:/WisenetStyle/Icon/search-normal.svg"
            imageHover: "/WisenetStyle/Icon/search-hover.svg"

            onClicked: {
                console.debug("WiseneSearchFilter onClicked - searchTextChanged: " + input.displayText)
                lastDisplayText = input.displayText
                searchFilterTextChanged(input.displayText)
            }
        }

    }
}
