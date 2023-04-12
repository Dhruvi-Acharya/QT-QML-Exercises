import QtQuick 2.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "./controls" as Res

RowLayout{
    id : root

    property bool h264Visibility : false
    property bool isEncodingPriorityVisibility : false
    property var currentModel: primaryModel

    Layout.fillWidth: true
    Layout.fillHeight: false

    GridLayout{
        id : grid

        property alias gridStates : codecCombo.state

        y : 0
        Layout.fillWidth: true
        rowSpacing: 5
        columns: 2

        Res.MyCellItemTitle{ text : "Codec" ;}
        Res.MyCombobox{
            id : codecCombo
            Layout.fillWidth: true
            Component.onCompleted:{
                model = currentModel.codecList
                currentIndex = indexOfValue(currentModel.codec)
            }
            onCurrentValueChanged: {
                if(displayText == "MJPEG")
                    root.h264Visibility = false
                else
                    root.h264Visibility = true
            }
        }
        Res.MyCellItemTitle{ text : "Resolution"}
        Res.MyCombobox{
            Layout.fillWidth: true
            Component.onCompleted:{
                model = currentModel.resolutionList
                currentIndex = indexOfValue(currentModel.resolution)
            }
        }

        Res.MyCellItemTitle{
            visible: root.h264Visibility
            text : "Bitrate Control Type"}
        Res.MyCombobox{
            visible: root.h264Visibility
            Layout.fillWidth: true
            Component.onCompleted:{
                model = ["CBR", "VBR"]
                currentIndex = indexOfValue(currentModel.bitrateControlType)
            }
            onCurrentValueChanged: {
                if(displayText == "CBR")
                    root.isEncodingPriorityVisibility = true
                else
                    root.isEncodingPriorityVisibility = false
            }
        }

        Res.MyCellItemTitle{
            visible: root.h264Visibility & root.isEncodingPriorityVisibility
            text : "Encoding Priority"}
        Res.MyCombobox{
            visible: root.h264Visibility & root.isEncodingPriorityVisibility
            Layout.fillWidth: true
            Component.onCompleted:{
                model = ["FrameRate", "Compression Level"]
                currentIndex = indexOfValue(currentModel.encodigPriority)
            }
        }

        Res.MyCellItemTitle{
            visible: root.h264Visibility
            text : "Gov Length"}
        SpinBox{
            visible: root.h264Visibility
            from : 0
            editable: true
            Component.onCompleted:{
                value : currentModel.govLength
                to : currentModel.maxGov
            }
        }

        Res.MyCellItemTitle{
            visible: root.h264Visibility
            text : "Profile"
        }
        Res.MyCombobox{
            visible: root.h264Visibility
            Layout.fillWidth: true
            Component.onCompleted:{
                model = ["BaseLine", "Main", "High"]
                currentIndex = indexOfValue(currentModel.h264Profile)
            }
        }
        Res.MyCellItemTitle{
            visible: root.h264Visibility
            text : "Entropy Coding"
        }
        Res.MyCombobox{
            visible: root.h264Visibility
            Layout.fillWidth: true
            Component.onCompleted:{
                model = ["CAVLC", "CABAC"]
                currentIndex = indexOfValue(currentModel.entropyCoding)
            }
        }
        Res.MyCellItemTitle{
            text : "                        "
        }
        Res.MyCellItemTitle{
            text : "                                  "
            Layout.fillWidth: true
        }
    }
}
