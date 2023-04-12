import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import "qrc:/WisenetStyle/"

Dialog {
    id: control

    modal:true
    closePolicy: Popup.CloseOnEscape//Popup.NoAutoClose
    //dim:false

    property bool onlyCloseType: false
    property bool rejectAndClose: true
    property bool applyAndClose: true
    property alias applyText: applyButton.text
    property alias cancelText: cancelButton.text
    property alias closeText: closeButton.text
    property alias applyEnabled: applyButton.enabled

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding,
                            implicitHeaderWidth,
                            implicitFooterWidth)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding
                             + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                             + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))
    topInset:0
    leftInset: 0
    bottomInset: 0
    rightInset: 0
    topPadding: 0
    bottomPadding: 0
    leftPadding: 34
    rightPadding: 34

    property url popup_close_press:   "images/PopupWindow/close_Press.svg"
    property url popup_close_hover:   "images/PopupWindow/close_Hover.svg"
    property url popup_close_default: "images/PopupWindow/close_Default.svg"


    //anchors.centerIn: Overlay.overlay
    parent: Overlay.overlay
    property int parentWidth : parent ? parent.width : width
    property int parentHeight : parent ? parent.height : height
    property int headerHeight : title ? 70 : 34
    x: Math.round((parentWidth - width) / 2)
    y: Math.round((parentHeight - height) / 2)

    onOpened: {
        x = Math.round((parentWidth - width) / 2)
        y = Math.round((parentHeight - height) / 2)
    }

    background: Rectangle {
        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1
    }

    header: Item{
        height: control.title ? 70 : 34

        /* header content */
        Item {
            anchors {
                fill: parent
                leftMargin: control.leftPadding
                rightMargin: control.rightPadding
            }
            Text {
                visible: control.title
                text: control.title
                font.pixelSize: 24
                elide: Label.ElideRight
                color: WisenetGui.contrast_00_white

                anchors {
                    fill:parent
                    verticalCenter: parent.verticalCenter
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }
            Rectangle {
                visible: control.title
                color: WisenetGui.contrast_08_dark_grey
                height: 1
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
            }
        }

        /* mouse area for drag */
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            property int pressedX: 0
            property int pressedY: 0
            property int maxMovableX : control.parentWidth-control.width
            property int maxMovableY : control.parentHeight-control.height
            onPressed: {
                pressedX = mouseX
                pressedY = mouseY
            }
            onPositionChanged: {
                var newX = control.x + mouseX - pressedX
                var newY = control.y + mouseY - pressedY

                if(newX > maxMovableX)
                    newX = maxMovableX
                if(newY > maxMovableY)
                    newY = maxMovableY

                control.x = newX
                control.y = newY
            }
        }

        /* x button */
        Button {
            width: 24
            height: 24
            hoverEnabled: true
            background: Rectangle {
                color: WisenetGui.transparent
            }
            anchors {
                right: parent.right
                top: parent.top

                topMargin: 10
                rightMargin: 10
            }
            Image {
                anchors.fill: parent
                sourceSize: Qt.size(width, height)
                source: parent.pressed ? popup_close_press :
                        parent.hovered ? popup_close_hover : popup_close_default
            }
            onClicked: {
                control.reject()
            }
        }
    }

    footer: Rectangle {
        color:"transparent"
        height: 70
        Row {
            id: controlButtonsRow
            anchors {
                centerIn: parent
            }
            spacing: 10
            WisenetMediaTextButtonItem {
                id: applyButton
                visible: !control.onlyCloseType
                isOrangeType: true
                text: WisenetLinguist.apply
                onClicked: {
                    if (control.applyAndClose)
                        control.accept()
                    else
                        control.accepted()
                }
            }
            WisenetMediaTextButtonItem {
                id: cancelButton
                visible: !control.onlyCloseType
                text: WisenetLinguist.cancel
                onClicked: {
                    if (control.rejectAndClose)
                        control.reject();
                    else
                        control.rejected();
                }
            }
            WisenetMediaTextButtonItem {
                id: closeButton
                visible: control.onlyCloseType
                text: WisenetLinguist.close
                onClicked: {
                    if (control.rejectAndClose)
                        control.reject();
                    else
                        control.rejected();
                }
            }
        }
    }
    Overlay.modal: Rectangle {
        color:  Color.transparent(WisenetGui.contrast_11_bg, 0.5)
    }

    Overlay.modeless: Rectangle {
        color: "transparent"
    }
}
