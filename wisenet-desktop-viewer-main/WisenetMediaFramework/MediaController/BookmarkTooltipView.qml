import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Item {
    id: bookmarkTooltipItem
    height: baseRect.height + arrowRect.height
    visible: false

    property string name: "name"
    property string description: "description"
    property int maxTextWidth: 250

    Rectangle {
        id: baseRect
        width: parent.width
        height: descriptionText.text != "" ? nameText.height + descriptionText.height + 12
                                           : nameText.height + 8
        color: WisenetGui.contrast_02_light_grey
        radius: 2

        Text {
            id: nameText
            anchors.margins: 4
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            text: name
            color: WisenetGui.contrast_08_dark_grey
            font.bold: true
            font.pixelSize: 11
            clip: true
        }

        Text {
            id: descriptionText
            anchors.margins: 4
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: nameText.bottom
            text: description
            color: WisenetGui.contrast_08_dark_grey
            font.pixelSize: 11
            clip: true
        }
    }

    Rectangle {
        id: arrowRect
        width: 6
        height: 5
        anchors.top: baseRect.bottom
        color: WisenetGui.transparent

        Canvas {
            anchors.fill: parent
            contextType: "2d"
            onPaint: {
                var context = getContext("2d")
                context.reset();
                context.moveTo(0, 0);
                context.lineTo(width, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = WisenetGui.contrast_02_light_grey
                context.fill();
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onContainsMouseChanged: {
            if(containsMouse) {
                parent.visible = true
            }
            else {
                parent.visible = false
            }
        }
    }

    function showBookmarkTooltip(centerX, name, description) {
        // 이름 설명 업데이트
        bookmarkTooltipItem.name = name
        bookmarkTooltipItem.description = description

        // Text 길이에 따른 Width 계산
        nameText.wrapMode = Text.NoWrap
        descriptionText.wrapMode = Text.NoWrap

        var textWidth = nameText.contentWidth
        if(descriptionText.contentWidth > textWidth)
            textWidth = descriptionText.contentWidth

        if(textWidth + 8 > maxTextWidth) {
            bookmarkTooltipItem.width = maxTextWidth
            nameText.wrapMode = Text.Wrap
            descriptionText.wrapMode = Text.Wrap
        }
        else {
            bookmarkTooltipItem.width = textWidth + 8
        }

        // 툴팁 위치 계산
        var posX = centerX - bookmarkTooltipItem.width / 2
        var arrowX = bookmarkTooltipItem.width / 2 - 3

        if(posX < 0) {
            posX = 0
            arrowX = centerX - 3
        }
        else if(posX + bookmarkTooltipItem.width > graphWidth) {
            posX = graphWidth - bookmarkTooltipItem.width
            arrowX = centerX - 3 - posX
        }

        bookmarkTooltipItem.x = posX
        arrowRect.x = arrowX

        bookmarkTooltipItem.visible = true
    }
}
