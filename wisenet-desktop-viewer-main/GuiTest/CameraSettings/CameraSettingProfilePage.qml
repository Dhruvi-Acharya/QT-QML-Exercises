import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./controls" as Res

Rectangle{
    id : root

    property int marginX: 30
    property int marginY: 12
    property int headHeight: 30
    property int lineHeight: 1

    x : 0
    width: parent.width
    Layout.fillWidth: true
    Layout.fillHeight: true
    implicitWidth: width
    color: "#383838"

    ColumnLayout {
        id : leftColumn
        spacing: 2
        Layout.leftMargin: marginX
        Layout.rightMargin: marginX
        x : marginX
        y : marginY
        width: (root.width)/2 - marginX - marginX /2
        Rectangle{
            color: "transparent"
            Layout.preferredHeight:headHeight
            Layout.preferredWidth: leftColumn.width
            Res.MyRowLayoutHeadText {text: "Primary Stream"}
        }
        Rectangle{
            Layout.preferredHeight:lineHeight
            Layout.preferredWidth: leftColumn.width
            Res.MyRowLayoutLine{}
        }

        Rectangle{
            Layout.preferredWidth: leftColumn.width
            CameraSettingProfile{
                currentModel: primaryModel
            }
        }
    }
    ColumnLayout {
        id : rightColumn
        spacing: 2
        Layout.leftMargin: marginX
        Layout.rightMargin: marginX
        x : (root.width)/2
        y : marginY
        width: (root.width)/2 - marginX - marginX /2
        Rectangle{
            color: "transparent"
            Layout.preferredHeight:headHeight
            Layout.preferredWidth: rightColumn.width
            Res.MyRowLayoutHeadText {text: "Secondary Stream"}
        }
        Rectangle{
            Layout.preferredHeight:lineHeight
            Layout.preferredWidth: rightColumn.width
            Res.MyRowLayoutLine{ }
        }

        Rectangle{
            Layout.preferredWidth: rightColumn.width
            CameraSettingProfile{
                currentModel: secondaryModel
            }
        }
    }
}

