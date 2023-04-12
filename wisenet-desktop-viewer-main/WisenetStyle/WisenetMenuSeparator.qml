import QtQuick 2.15
import QtQuick.Controls 2.15

MenuSeparator {
    height: {
        if(visible) return 13
        else return 0
    }
    //leftPadding: 10
    //rightPadding: 10
    contentItem: Rectangle {
        implicitWidth: 200
        implicitHeight: 1
        color: WisenetGui.contrast_07_grey
    }
}
