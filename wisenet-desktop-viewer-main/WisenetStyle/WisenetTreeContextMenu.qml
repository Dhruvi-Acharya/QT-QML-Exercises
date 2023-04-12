import QtQuick 2.15
import QtQuick.Controls 2.15

Menu {
    background: Rectangle {
        implicitWidth: 200
        color: "#606060"
        border.color: "#606060"
        radius: 2
    }

    property string itemUuid

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 100 }
    }
    /*
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 200 }
    }*/
}
