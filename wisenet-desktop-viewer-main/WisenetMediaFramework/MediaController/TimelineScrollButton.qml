import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/WisenetStyle/"

Button{
    width: 20
    height: 12
    hoverEnabled: true

    property bool isLeftButton: true
    property bool isHighlighted: false

    background: Rectangle {
        color: !enabled ? WisenetGui.contrast_10_dark :
                pressed ? WisenetGui.contrast_07_grey :
                isHighlighted ? WisenetGui.color_primary : WisenetGui.contrast_08_dark_grey
    }

    Image {
        width: 9
        height: 8
        anchors.centerIn: parent
        sourceSize: Qt.size(width, height)
        source: {
            if(isLeftButton) {
                if(!parent.enabled)
                    return WisenetImage.mediaController_scroll_left_dim
                else if(parent.pressed || isHighlighted)
                    return WisenetImage.mediaController_scroll_left_press
                else
                    return WisenetImage.mediaController_scroll_left
            }
            else {
                if(!parent.enabled)
                    return WisenetImage.mediaController_scroll_right_dim
                else if(parent.pressed || isHighlighted)
                    return WisenetImage.mediaController_scroll_right_press
                else
                    return WisenetImage.mediaController_scroll_right
            }
        }
    }

    onClicked: {
        timelineControlView.animationDuration = 500;
        if(isLeftButton) {
            mediaControlModel.setVisibleTimeRange(-mediaControlModel.visibleTimeRange, 0)
        }
        else {
            mediaControlModel.setVisibleTimeRange(mediaControlModel.visibleTimeRange, 0)
        }
    }

    function updatesHighlighted() {
        if(!focusedViewingGridItem || !focusedViewingGridItem.hasPlayer
                || focusedViewingGridItem.mediaParam.isLive || focusedMediaPlayPosition === 0) {
            isHighlighted = false
            return;
        }

        if((isLeftButton && focusedMediaPlayPosition < mediaControlModel.visibleStartTime)
            || (!isLeftButton && focusedMediaPlayPosition > mediaControlModel.visibleEndTime))
            isHighlighted = true
        else
            isHighlighted = false
    }
}
