import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./controls" as Res

Rectangle{
    id: root

    implicitWidth: width
    color: "#383838"
    anchors.margins: 15

    Rectangle{
        id : mainRect
        color: "transparent"
        anchors.fill: parent
        anchors.margins: 10

        Res.MySwitch{
            id : mySwitch
            text: "Fisheye Dewarping"
        }
        Rectangle{
            id : settingRect
            height : 120
            color: "transparent"
            border.color: "black"
            y : mainRect.height - height
            width: secondRect.width
            enabled: mySwitch.checked
            opacity: mySwitch.checked? 1 : 0.2

            RowLayout{
                id : colLayout
                anchors.fill:  parent

                Rectangle{
                    id : mountRect
                    width: 200
                    height: settingRect.height
                    color: "transparent"
                }

                Rectangle {
                    id: slideColumn
                    x: mountRect.width
                    y : 0
                    width: secondRect.width - mountRect.width
                    height: 104

                    color: "transparent"
                    Layout.fillWidth: true

                    Column{
                        Res.MySlider {
                            id: sizeSlide;
                            name: "Size"
                            width:  slideColumn.width
                        }
                        Res.MySlider {
                            id: xSlide;
                            name: "X Offset"
                            width:  slideColumn.width
                        }
                        Res.MySlider {
                            id: ySlide;
                            name: "Y Offset"
                            width:  slideColumn.width
                        }
                        Res.MySlider {
                            id: lineWidthCtrl;
                            name: "Ellipticity"
                            width:  slideColumn.width
                        }
                    }
                }
            }
        }
        Rectangle{
            id : secondRect
            width: mainRect.width
            height: mainRect.height - mySwitch.height - settingRect.height
            y : mySwitch.height
            x : 0
            enabled: mySwitch.checked
            color: "transparent"
            opacity: mySwitch.checked? 1 : 0.2
            border.color: "black"
            anchors.margins: 5

            Text {
                id: name
                text: qsTr("123456")
            }

            Canvas {
                id:canvas
                width: secondRect.width
                height: secondRect.height
                anchors.fill: secondRect

                property color strokeStyle:  Qt.darker(fillStyle, 1.4)
                property color fillStyle: "#80c4ff" // red
                property bool fill: true
                property bool stroke: true
                property real alpha: 0.25
                property real scale : 0.75 + sizeSlide.value / 25
                property real xPos : (xSlide.value -1) * (canvas.width/100)
                property real yPos : (ySlide.value -1) * (canvas.height/100)

                antialiasing: true

                onFillChanged:requestPaint();
                onStrokeChanged:requestPaint();
                onScaleChanged:requestPaint();
                onXPosChanged:requestPaint();
                onYPosChanged:requestPaint();

                onPaint: {
                    var ctx = canvas.getContext('2d');
                    var originX = xPos
                    var originY = yPos
                    var originWidth = canvas.width/4
                    var originHeight = canvas.height/4

                    ctx.save();
                    ctx.clearRect(0, 0, canvas.width, canvas.height);
                    ctx.translate(originX, originY);
                    ctx.globalAlpha = canvas.alpha;
                    ctx.strokeStyle = canvas.strokeStyle;
                    ctx.fillStyle = canvas.fillStyle;

                    ctx.translate(originX, originY)
                    ctx.scale(canvas.scale, canvas.scale);
                    ctx.rotate(canvas.rotate);
                    ctx.translate(-originX, -originY)

                    ctx.beginPath();

                    ctx.arc(originX, originY, originWidth, originHeight, Math.PI * 2, false);

                    ctx.closePath();

                    if (canvas.fill)
                        ctx.fill();
                    if (canvas.stroke)
                        ctx.stroke();
                    ctx.restore();
                }
            }
        }
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
