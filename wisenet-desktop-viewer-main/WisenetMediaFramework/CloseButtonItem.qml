import QtQuick 2.15
import "qrc:/WisenetStyle/"

WisenetMediaButtonItem {
    id:closeButton
    imgSourceSize: "24x24"
    radius: width*0.5
    bgOpacity: 0.7
    hoverColor: WisenetGui.contrast_08_dark_grey
    pressColor: WisenetGui.contrast_10_dark
    normalSource: "images/close_Default.svg"
    pressSource: "images/close_Press.svg"
    hoverSource: "images/close_Hover.svg"
}
