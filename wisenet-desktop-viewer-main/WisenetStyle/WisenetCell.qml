import QtQuick 2.3
import QtQuick.Layouts 1.15

Row{
    id : root
    property string title: ""

    property alias  content: contentId.text
    property int pointSize : 12
    property bool isSub : false

    Layout.preferredWidth: 380
    property alias leftPadding: titleId.leftPadding
    //Layout.fillWidth: true
    spacing: 40

    WisenetLabel{
        visible: root.isSub
        width: 120

        wrapMode: Text.Wrap
    }
    Row{
        spacing: (root.isSub?10 : 40)
        WisenetLabel{
            id: titleId
            text: root.title
            width: 120
            wrapMode: Text.Wrap
            leftPadding: 0
        }


        TextEdit {
            id : contentId
            Layout.alignment: Qt.AlignLeft
            text: " "
            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: pointSize
            readOnly: true
            selectByMouse: true
            textMargin: 1
        }
    }
}
