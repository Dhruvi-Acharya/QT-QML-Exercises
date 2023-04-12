import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    width: 24
    height: 24
    hoverEnabled: true
    background: Rectangle{
        color: {
            return WisenetGui.transparent
        }
    }

    Image{
        id: closeButtonImage
        x: 0
        y: 0
        width: 24
        height: 24
        source: "qrc:/WisenetStyle/Icon/popup-close-normal.png"
    }

    onPressed: {
        closeButtonImage.source = "qrc:/WisenetStyle/Icon/popup-close-press.png"
    }
    onHoveredChanged: {
        if(hovered){
            closeButtonImage.source = "qrc:/WisenetStyle/Icon/popup-close-hover.png"
        }else {
            closeButtonImage.source = "qrc:/WisenetStyle/Icon/popup-close-normal.png"
        }
    }
}
