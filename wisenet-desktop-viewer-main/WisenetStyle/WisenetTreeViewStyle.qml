import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TreeViewStyle {
    id: root

    property int styleWidth: 6

    indentation: 18
    incrementControl: Item {
        visible:false
    }
    decrementControl: Item {
        visible:false
    }

    frame: Item {
        visible: false
    }
    scrollBarBackground: Item {
        implicitWidth: root.styleWidth
        implicitHeight: root.styleWidth
        clip: true
        opacity: 1.0
    }
    handle: Item {
        implicitWidth: root.styleWidth
        implicitHeight: root.styleWidth
        Rectangle {
            radius: width * 0.5
            opacity: styleData.pressed ? 1 : styleData.hovered ? 1 : 0.75
            anchors.top: !styleData.horizontal ? parent.top : undefined
            anchors.margins: 0
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: styleData.horizontal ? parent.left : undefined

            color: styleData.pressed ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
            width: !styleData.horizontal ? root.styleWidth : parent.width
            height: styleData.horizontal ? root.styleWidth : parent.height
        }
    }
    branchDelegate: Item {
        width: indentation
        height: 18
        Image {
            visible: styleData.column === 0 && styleData.hasChildren
            anchors.centerIn: parent
            sourceSize: Qt.size(14, 14)
            source: styleData.isExpanded ? WisenetImage.resource_tree_branch_expanded : WisenetImage.resource_tree_branch_collapsed
        }
    }
}
