import QtQuick 2.15
import QtQuick.Controls 2.15

Button{
    id: stateImageButton

    property var defaultImageSource: undefined
    property var hoverImageSource: undefined
    property var selectImageSource: undefined
    property var disableImageSource: undefined

    hoverEnabled: true

    background: Rectangle{
        color:{
            return WisenetGui.transparent
        }
    }

    Image {
        id: calendarViewPopupButtonImage
        anchors.fill: parent
        sourceSize: Qt.size(width, height)
        source: enabled === true ? defaultImageSource : disableImageSource
    }

    onHoveredChanged: {
        if(!enabled){
            calendarViewPopupButtonImage.source = disableImageSource
        }

        if(hovered){
            calendarViewPopupButtonImage.source = hoverImageSource
        }else{
            calendarViewPopupButtonImage.source = defaultImageSource
        }
    }

    onPressed: {
        if(!enabled){
            calendarViewPopupButtonImage.source = disableImageSource
        }

        calendarViewPopupButtonImage.source = selectImageSource
    }
    onReleased: {
        if(!enabled){
            calendarViewPopupButtonImage.source = disableImageSource
        }

        if(hovered){
            calendarViewPopupButtonImage.source = hoverImageSource
        }else{
            calendarViewPopupButtonImage.source = defaultImageSource
        }
    }

}
