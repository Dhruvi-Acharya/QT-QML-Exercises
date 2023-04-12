import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {

    id: explorerPage
    anchors.fill: parent
    header: TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Device")
            width: implicitWidth
        }
        TabButton {
            text: qsTr("Alarm")
            width: implicitWidth
        }
        TabButton {
            text: qsTr("Layout")
            width: implicitWidth
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: bar.currentIndex

        //DeviceTreeView {}
        MainTreeView {}
        AlarmTreeView {}
        SiteTreeView {}

    }
}

