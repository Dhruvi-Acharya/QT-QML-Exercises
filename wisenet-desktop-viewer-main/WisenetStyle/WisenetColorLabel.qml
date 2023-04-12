import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item{
    id : root
    width: 100
    height: 20
    text: ""
    property alias color: textEdit.color
    property alias text : textEdit.text
    property alias imageVisible : linkImage.visible
    property alias imageSource : linkImage.source
    signal clicked()

    Row{
        spacing: 4
        TextEdit {
            id : textEdit
            property int defaultPixelSize: 12
            //anchors.verticalCenter: parent.verticalCenter
          //  width: 100
            height: root.height
            text: ""
            verticalAlignment: Text.AlignVCenter
            color: mouseArea.containsMouse ? (mouseArea.pressed ? WisenetGui.color_secondary_dark : WisenetGui.color_secondary_bright)
                                           : WisenetGui.color_secondary
            font.pixelSize: defaultPixelSize
            readOnly: true
            textMargin: 1
        }
        Image {
            id: linkImage
            //x : textEdit.right
            width: 6
            height: 6
            visible: true
            sourceSize: "6x6"
            anchors.verticalCenter: parent.verticalCenter
            source: mouseArea.containsMouse ? (mouseArea.pressed ? WisenetImage.textLink_press : WisenetImage.textLink_hover)
                                            : WisenetImage.textLink
        }
    }
    MouseArea{
        id : mouseArea
        acceptedButtons: Qt.LeftButton
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
