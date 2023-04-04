import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 150
    height: 500
    visible: true
    title: qsTr("ListView Example")

    Rectangle {
        id: rect
        height: 300
        anchors {left: parent.left; right: parent.right}
        color: "cyan"
    }

    ListView{
        id: lv
        clip: true
        focus: true
        property var collapsed: ({})
//        anchors.fill: parent
        anchors {left: parent.left; right: parent.right; top: rect.bottom; bottom: parent.bottom}
        model: NameModel {}
        delegate: NameDelegate{
            readonly property ListView __lv: ListView.view

            expanded: ListView.view.isSectionExpanded(model.team)

            MouseArea{
                anchors.fill: parent
                onClicked: __lv.currentIndex = index
            }
        }

        highlight: HighlightDelegate {
            width: parent.width
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        populate: Transition {
            NumberAnimation {properties: "x,y"; duration: 300}
        }

        section {
            property: "team"
            criteria: ViewSection.FullString

            delegate: SectionDelegate {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                text: section

                onClicked: ListView.view.toggleSection( section )
            }
        }

        function isSectionExpanded( section ) {
            return !(section in collapsed);
        }

        function showSection( section ) {
            delete collapsed[section]
            /*emit*/ collapsedChanged();
        }

        function hideSection( section ) {
            collapsed[section] = true
            /*emit*/ collapsedChanged();
        }

        function toggleSection( section ) {
            if ( isSectionExpanded( section ) ) {
                hideSection( section )
            } else {
                showSection( section )
            }
        }
    }
}
