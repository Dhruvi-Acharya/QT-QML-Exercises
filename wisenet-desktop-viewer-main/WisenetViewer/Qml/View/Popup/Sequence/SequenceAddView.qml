import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import WisenetStyle 1.0
import WisenetLanguage 1.0

Window{
    id: sequenceAddWindow
    width: 550
    height: 462
    minimumWidth: 550
    minimumHeight: 462
    flags: Qt.FramelessWindowHint | Qt.Window
    color: WisenetGui.backgroundColor
    title: WisenetLinguist.newSequence
//    modality : Qt.WindowModal

    property int sequenceMainTitleHeight: 40
    property bool isEditMode: false
    property string sequenceId: ""

    property int pressedX: 0
    property int pressedY: 0

    function addNewSequence(){
        sequenceAddWindow.isEditMode = false
        sequenceAddViewModel.sequenceName = ""
        sequenceNameEditBox.text = ""
        sequenceMainText.text = WisenetLinguist.newSequence
        sequenceAddViewModel.reloadLayout()
        sequenceAddWindow.visible = true
    }

    function editSequence(sequenceName, sequenceId){
        sequenceAddWindow.isEditMode = true
        sequenceAddViewModel.sequenceName = sequenceName
        sequenceNameEditBox.text = sequenceName
        sequenceAddWindow.sequenceId = sequenceId
        sequenceMainText.text = WisenetLinguist.editSequence
        sequenceAddViewModel.loadLayouts(sequenceName, sequenceId)
        sequenceAddWindow.visible = true
    }

    WisenetMessageDialog{
        id: errorDialog

        visible: false
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }

    Rectangle {
        id: sequenceMainBorder
        anchors.fill: parent
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_07_grey
        border.width: 1

        Rectangle{
            id: sequenceMainTitle

            height: sequenceMainTitleHeight

            anchors.left: parent.left
            anchors.right: parent.right

            color: "transparent"

            component WindowButton: Button{
                y: 11
                width: 16
                height: 16
                hoverEnabled: true
                background: Rectangle {
                    color: WisenetGui.transparent
                }
            }

            Rectangle{
                id: sequenceMainTextRect

                height: 28

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 21
                anchors.leftMargin: 36

                color: "transparent"
                Text{
                    id: sequenceMainText
                    text: sequenceAddWindow.isEditMode? WisenetLinguist.editSequence + " " + sequenceAddWindow.sequenceName : WisenetLinguist.newSequence
                    anchors.fill: parent
                    color: WisenetGui.contrast_00_white
                    font.pixelSize: 24
                }
            }

            MouseArea {
                anchors.fill: parent
                anchors.right: closeButton.left
                acceptedButtons: Qt.LeftButton
                onPressed: {
                    pressedX = mouseX
                    pressedY = mouseY
                }
                onPositionChanged: {
                    var newX = sequenceAddWindow.x + mouseX - pressedX
                    var newY = sequenceAddWindow.y + mouseY - pressedY

                    if(newX+10 > Screen.desktopAvailableWidth)
                        newX = Screen.desktopAvailableWidth - 10
                    if(newY+10 > Screen.desktopAvailableHeight)
                        newY = Screen.desktopAvailableHeight - 10

                    sequenceAddWindow.x = newX
                    sequenceAddWindow.y = newY
                }
            }

            WindowButton {
                id: closeButton
                anchors.right: parent.right
                anchors.rightMargin: 9

                Image {
                    anchors.fill: parent
                    sourceSize.width: 24
                    sourceSize.height: 24
                    source: parent.pressed ? WisenetImage.popup_close_press :
                                             parent.hovered ? WisenetImage.popup_close_hover : WisenetImage.popup_close_default
                }
                onClicked: {
                    sequenceAddWindow.close()
                }
            }

            Rectangle{
                anchors.top: sequenceMainTextRect.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 21
                anchors.leftMargin: 36
                anchors.rightMargin: 34
                width: 480
                height: 1
                color: WisenetGui.contrast_08_dark_grey
            }
        }

        Rectangle {
            id: sequenceEditContent
            anchors.top: sequenceMainTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            z: 1

            WisenetLabel{
                id: sequenceNameDescription
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 48
                anchors.leftMargin: 89
                text: WisenetLinguist.name
            }

            WisenetTextBox{
                id: sequenceNameEditBox
                placeholderText: WisenetLinguist.enterNameMessage
                text: sequenceAddViewModel.sequenceName
                Binding { target: sequenceAddViewModel; property: "sequenceName"; value: sequenceNameEditBox.text }
                anchors.top: parent.top
                anchors.left: sequenceNameDescription.right
                anchors.topMargin: 50
                anchors.leftMargin: 42
            }

            WisenetLabel{
                id: selectLayoutDescription
                text: WisenetLinguist.selectLayout
                anchors.top: sequenceNameDescription.top
                anchors.left: parent.left
                anchors.topMargin: 39
                anchors.leftMargin: 89
            }

            WisenetLabel{
                id: intervalDescription
                text: WisenetLinguist.interval
                anchors.top: sequenceNameDescription.top
                anchors.left: selectLayoutDescription.right
                anchors.topMargin: 39
                anchors.leftMargin: 140
            }


            ListView{
                id: sequenceListView
                property var dataCount: sequenceAddViewModel.sequenceListModel.dataCount

                anchors.top: selectLayoutDescription.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 39
                anchors.leftMargin: 42
                anchors.rightMargin: 32

                clip: true

                height: 250
                spacing: 3
                ScrollBar.vertical: WisenetScrollBar {
                    id: verticalScrollbar
                    anchors.right: sequenceListView.right
                }

                model: sequenceAddViewModel.sequenceListModel

                delegate: Item {
                    id: sequenceItem
                    width: sequenceListView.width
                    height: 40

                    property var itemModel: model
                    property var itemIndex: index

                    Rectangle {
                        id: itemLayout
                        anchors.fill: parent
                        color: "transparent"

                        Image {
                            id: addDeleteButtonImage
                            anchors.left: parent.left
                            width: 16

                            source: model.buttonDisplay === true ? WisenetImage.sequence_add : WisenetImage.sequence_remove

                            sourceSize.width: 16
                            sourceSize.height: 16

                            MouseArea{
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor

                                onClicked: {
                                    console.log("row:"+model.row+",ct:" + sequenceListView.count + "," + model.buttonDisplay);
                                    if(model.buttonDisplay) {
                                        sequenceAddViewModel.addSequenceItem()
                                        sequenceListView.contentY = sequenceListView.contentHeight
                                    }
                                    else {
                                        sequenceAddViewModel.removeSequenceItem(model.row);
                                    }
                                }

                            }
                        }

                        WisenetComboBox{
                            id: selectedLayoutComboBox
                            model: sequenceAddViewModel.layoutList
                            anchors.left: addDeleteButtonImage.right
                            anchors.leftMargin: 20
                            width: 228

                            currentIndex: sequenceItem.itemModel.selectedLayout

                            visible: !sequenceItem.itemModel.buttonDisplay

                            Binding { target: sequenceItem.itemModel; property: "selectedLayout"; value: selectedLayoutComboBox.currentIndex }
                        }

                        WisenetComboBox{
                            id: intervalComboBox
                            anchors.left: selectedLayoutComboBox.right
                            anchors.leftMargin: 22
                            width: 49
                            height: 20

                            model: [5, 10, 15, 20, 30, 40, 50, 60]

                            currentIndex:
                            {
                                console.log("SequenceAddView.intervalComboBox - sequenceItem.itemModel.interval=", sequenceItem.itemModel.interval, "indexOfValue(sequenceItem.itemModel.interval)=",indexOfValue(sequenceItem.itemModel.interval))
                                if(sequenceItem.itemModel.interval === 5) {
                                    return 0
                                }
                                if(sequenceItem.itemModel.interval === 10) {
                                    return 1
                                }
                                if(sequenceItem.itemModel.interval === 15) {
                                    return 2
                                }
                                if(sequenceItem.itemModel.interval === 20) {
                                    return 3
                                }
                                if(sequenceItem.itemModel.interval === 30) {
                                    return 4
                                }
                                if(sequenceItem.itemModel.interval === 40) {
                                    return 5
                                }
                                if(sequenceItem.itemModel.interval === 50) {
                                    return 6
                                }
                                if(sequenceItem.itemModel.interval === 60) {
                                    return 7
                                }
                                return 0
                            }

                            visible: !sequenceItem.itemModel.buttonDisplay

                            onActivated: {
                                sequenceItem.itemModel.interval = intervalComboBox.currentValue
                            }
                        }

//                        WisenetSpinBox{
//                            id: spinBox
//                            anchors.left: selectedLayoutComboBox.right
//                            anchors.leftMargin: 22
//                            width: 49
//                            height: 20
//                            font.pixelSize: 12
//                            from: 5
//                            to: 60

//                            value: model.interval

//                            onValueChanged: {
//                                console.log("onValueChanged : ",spinBox.value)
//                                if(spinBox.value < spinBox.from) {
//                                    spinBox.value = spinBox.from
//                                }

//                                if(spinBox.value > spinBox.to) {
//                                    spinBox.value = spinBox.to
//                                }
//                            }

//                            visible : !model.buttonDisplay

//                            Binding { target: model; property: "interval"; value: spinBox.value }
//                        }


                        WisenetLabel{
                            id: secondsRangeDescription
                            anchors.left: intervalComboBox.right
                            anchors.leftMargin: 3
                            width: 86
                            text: WisenetLinguist.sec + "(5~60)"
                            visible: intervalComboBox.visible //sequenceItem.itemIndex < sequenceListView.dataCount-1
                        }

                        Rectangle {
                            id: moveButtonGroupRect
                            anchors.left: secondsRangeDescription.right
                            anchors.leftMargin: 5
                            WisenetImageButton{
                                id: moveUpImage
                                anchors.left: parent.left

                                width:18
                                height:18

                                checkable: false
                                imageSelected: WisenetImage.move_up_normal
                                imageNormal: WisenetImage.move_up_normal
                                imageHover: WisenetImage.move_up_normal
                                imageDisabled: WisenetImage.move_up_dim

                                visible: (sequenceItem.itemIndex > 0) && (sequenceItem.itemIndex < sequenceListView.dataCount-1)

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        sequenceAddViewModel.swapSequenceItem(sequenceItem.itemIndex-1, sequenceItem.itemIndex)
                                    }
                                }
                            }

                            WisenetImageButton{
                                id: moveDownImage
                                anchors.left: moveUpImage.visible ? moveUpImage.right : parent.left

                                width:18
                                height:18

                                checkable: false
                                imageSelected: WisenetImage.move_down_normal
                                imageNormal: WisenetImage.move_down_normal
                                imageHover: WisenetImage.move_down_normal
                                imageDisabled: WisenetImage.move_down_dim

                                visible: sequenceItem.itemIndex < sequenceListView.dataCount-2

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        sequenceAddViewModel.swapSequenceItem(sequenceItem.itemIndex, sequenceItem.itemIndex+1)
                                    }
                                }
                            }
                        }
                    }
                }


            }


            Rectangle {
                id: sequenceConfirmButtonGroupRect
                anchors.top: sequenceListView.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 4
                anchors.bottomMargin: 24

                WisenetOrangeButton{
                    id: sequenceEditOkButton
                    anchors.left: parent.left
                    anchors.leftMargin: 149
                    text: WisenetLinguist.ok

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if(sequenceNameEditBox.text.trim() === ""){
                                errorDialog.message = WisenetLinguist.enterNameMessage
                                errorDialog.visible = true
                                return;
                            }

                            if(isEditMode) {
                                console.log("update mode-- OK button clicked")
                                sequenceAddViewModel.updateLayout(sequenceAddWindow.sequenceId) // 갱신된 정보를 sequenceAddViewModel를 통해 DB에 갱신

                                sequenceAddWindow.close()
                            }
                            else {
                                sequenceAddViewModel.saveLayout()
                                sequenceAddWindow.close()
                            }
                        }
                    }
                }

                WisenetGrayButton{
                    anchors.left: sequenceEditOkButton.right
                    anchors.leftMargin: 10
                    text: WisenetLinguist.cancel

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            sequenceAddWindow.close()
                        }
                    }
                }

            }
        }
    }
}
