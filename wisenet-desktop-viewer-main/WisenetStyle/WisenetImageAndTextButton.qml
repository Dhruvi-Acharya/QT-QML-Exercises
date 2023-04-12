import QtQuick 2.15
import WisenetLanguage 1.0

Item {
    id: control
    property url normalImage: "qrc:/WisenetStyle/Icon/log-export-normal.svg"
    property url hoverImage: "qrc:/WisenetStyle/Icon/log-export-hover.svg"
    property url pressImage: "qrc:/WisenetStyle/Icon/log-export-press.svg"

    property int imageWidth: 14
    property int imageHeight: 14

    property int textMargin: 6
    property int textFontSize: 12
    property color normalTextColor: WisenetGui.color_secondary
    property color hoverTextColor: WisenetGui.color_secondary_bright
    property color pressTextColor: WisenetGui.color_secondary_dark

    property alias txt: buttonText.text
    function changeText(text){
        buttonText.text = text
    }

    signal buttonClicked()

    Rectangle{
        id: imageAndTextButton
        anchors.fill: parent
        color: WisenetGui.transparent

        MouseArea {
          hoverEnabled: true
          anchors.fill: parent

          onContainsMouseChanged: {
              if(containsMouse){
                  if(pressed){
                      buttonImage.source = pressImage
                      buttonText.color = pressTextColor
                  }else{
                      buttonImage.source = hoverImage
                      buttonText.color = hoverTextColor
                  }
              }else{
                  buttonImage.source = normalImage
                  buttonText.color = normalTextColor
              }
          }

          onPressedChanged: {
              if(pressed){
                  buttonImage.source = pressImage
                  buttonText.color = pressTextColor
                  buttonClicked()
              }else{
                  if(containsMouse){
                      buttonImage.source = hoverImage
                      buttonText.color = hoverTextColor
                  }else{
                      buttonImage.source = normalImage
                      buttonText.color = normalTextColor
                  }
              }
          }
        }

        Image{
            id: buttonImage
            width: imageWidth
            height: imageHeight
            anchors.verticalCenter: parent.verticalCenter
            sourceSize: Qt.size(width,height)
            source: normalImage
        }

        Text{
            id: buttonText
            x: buttonImage.width + textMargin
            width: parent.width - buttonImage.width - textMargin
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            text: WisenetLinguist.exportButton
            color: normalTextColor
            font.pixelSize: textFontSize
            verticalAlignment: Text.AlignVCenter

            fontSizeMode: Text.HorizontalFit
            minimumPointSize: 1
            wrapMode: Text.Wrap

            onTruncatedChanged: {
                if(truncated){
                    while(truncated){
                        control.width += 1;
                        width += 1;
                    }
                }
            }
        }
    }
}
