import QtQuick 2.12

Rectangle {
//    color: gcolor
    color: "lightblue"

    Gradient{
        id: gcolor
        GradientStop {position: 0; color: "green"}
        GradientStop {position: 0.5; color: "blue"}
    }
}
