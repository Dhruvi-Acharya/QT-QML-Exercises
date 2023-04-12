import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

GridItemBase {
    id: imageItem
    property alias imageSource : image.source
    property bool isBackground: itemModel.imageBackground

    backgroundColor: 'transparent'

    Rectangle {
        id: backgroundRect
        anchors.fill: parent
        anchors.margins: contentPadding
        color: backgroundColor
    }

    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        asynchronous: true
        property string imageName

        onSourceChanged: {
            if (source) {
                if (itemModel.isSVG(source)) {
                    console.log("SVG image, set sourceSize Property 1024w for scale, url=", source);
                    sourceSize.width = 1024;
                }
                else {
                    console.log("No svg image, set sourceSize Property, url=", source);
                }
                var sourceString = source.toString()
                imageName = (sourceString.slice(sourceString.lastIndexOf("/")+1))
                imageName = decodeURIComponent(imageName)   // 특수문자 깨지는 문제 수정
            }
        }

        onStatusChanged: {
            //console.log("imageItem status changed::", image.status);
            if (image.status === Image.Ready) {
                console.log('Image Loaded: sourceSize ==', source, implicitWidth, implicitHeight);
            }
            else if (image.status === Image.Loading) {

            }
            else if (image.status === Image.Error) {

            }
        }
        mipmap: true
    }

    NumberAnimation{
        id:closeAnim
        target: imageItem;
        properties:"opacity";
        to:0
        duration:150
        onStopped: {
            console.log("imageItem destroy()");
            imageItem.destroy();
        }
    }
    NumberAnimation{
        id: openAnim
        target: imageItem;
        properties:"opacity";
        to:1.0
        duration:150
    }

    function open()
    {
        openAnim.start();
    }

    function close()
    {
        closeAnim.start();
        console.debug("close imageItem!!");
    }

    function closeWithoutAnimation()
    {
        imageItem.destroy();
        console.debug("close imageItem!!");
    }

    onMouseOverChanged: {
        osdControl.controlVisible = (mouseOver && !selected) ? true : false
    }

    Item {
        id: osdControl
        visible: opacity > 0
        opacity: osdControl.controlVisible || imageItem.itemNameAlwaysVisible ? 1.0 : 0.0
        x:0
        y:0
        // CHECK POINT
        // 아이템간 바인딩이 많은 경우 성능 부하 발생하여 visible 상태가 아닌경우에는 binding을 풀어줌.
        width:visible?imageItem.width:1
        height:visible?imageItem.height:1

        property bool controlVisible: false

        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }

        TopControl {
            id: osdTopControl
            noDisplayName : isBackground
            displayName: image.imageName
            onItemClosed: {
                imageItem.itemClose(imageItem);
            }
        }
    }

}
