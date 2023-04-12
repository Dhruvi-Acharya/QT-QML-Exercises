import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: alarmPage
    anchors.fill: parent
    header: TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Notification")
        }
        TabButton {
            text: qsTr("Event")
        }
        TabButton {
            text: qsTr("Bookmarks")
        }
    }

    Behavior on visible  {
        NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: bar.currentIndex

        NotificationView{}
        AlarmView {}
        BookmarkView {}
    }
}

