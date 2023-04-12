import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0

Item {
    id: searchBox
    signal searchTextChanged(var text)

    property string lastDisplayText: ""

    property alias textFieldfocus: input.focus
    property alias bgColor: wrapper.color
    property color borderColor : WisenetGui.transparent
    property int borderWidth : 0
    property string placeholderText : WisenetLinguist.search
    property bool setByProgam: false
    width: parent.width
    height: 30

    signal downKeyPressed();
    signal upKeyPressed();
    signal escapePressed();
    signal accepted();

    function reset(){
        if (input.text != "") {
            input.text = ""
        }
        searchTextChanged(input.displayText)
    }

    function setText(text) {
        //console.log("setText!", text, input.text, input.displayText)
        if (input.text !== text) {
            setByProgam = true;
            input.text = text;
        }
    }
    function undoText() {
        if (input.text != lastDisplayText) {
            setByProgam = true
            input.text = lastDisplayText
        }
    }

    function resetPlaceholderText() {
        input.placeholderText = ""
        input.placeholderText = searchBox.placeholderText
    }

    property int icon_x : 13

    Rectangle{
        id: wrapper
        anchors.fill: parent
        color: WisenetGui.contrast_08_dark_grey
        border.color: borderColor
        border.width: borderWidth

        Image {
            id: searchImage
            x: icon_x
            anchors.verticalCenter: parent.verticalCenter
            width: 11
            height: 11
            sourceSize: Qt.size(width, height)
            source: mouseArea.containsMouse ? "qrc:/WisenetStyle/Icon/search-hover.svg" : "qrc:/WisenetStyle/Icon/search-normal.svg"
        }

        Timer{
            id: searchTextChangedTimer
            interval: 200; running: false; repeat: false
            onTriggered : {
                //console.debug("searchTextChangedTimer triggered!")
                if(lastDisplayText !== input.displayText){
                    lastDisplayText = input.displayText
                    searchTextChanged(input.displayText)

                }
            }
        }

        TextField {
            id: input
            //focus: true//글씨를 바로 써도 입력되게끔
            anchors.top: parent.top
            anchors.left: searchImage.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 10
            anchors.rightMargin: 10

            placeholderText: WisenetLinguist.search // searchBox.placeholderText // 다국어 실시간 반영되도록 수정 (by younginn.yun, 20220607)
            placeholderTextColor: mouseArea.containsMouse ? WisenetGui.contrast_00_white : WisenetGui.contrast_06_grey

            font.pixelSize: 12
            color: WisenetGui.contrast_02_light_grey

            background: Rectangle {
                color: WisenetGui.transparent
            }

            validator: RegExpValidator{
                regExp: /[^`]+/
            }
            Keys.onDownPressed: {
                searchBox.downKeyPressed();
            }
            Keys.onUpPressed: {
                searchBox.upKeyPressed();
            }
            Keys.onEscapePressed: {
                searchBox.escapePressed();
            }

            onAccepted: {
                searchBox.accepted();
            }
//            onTextEdited: {
//                searchTextChangedTimer.restart()
//            }
            onDisplayTextChanged: {
//                console.log("onDisplayTextChanged() triggered",
//                            setByProgam, searchTextChangedTimer.running);
                if (setByProgam) {
                    setByProgam = false;
                    searchTextChangedTimer.stop()
                }
                else {
                    searchTextChangedTimer.restart()
                }
            }

        }

        MouseArea{
            id: mouseArea

            anchors.fill: parent
            hoverEnabled: true

            cursorShape: Qt.IBeamCursor

            onPressed: {
                mouse.accepted = false
            }
        }
    }
}

