import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebEngine 1.10
import QtGraphicalEffects 1.15
GridItemBase {
    id: webItem
    property alias  webUrl : webEngineView.url
    property bool   useDigestAuth: false
    property string userId
    property string userPw

    property alias  webView: webEngineView

    signal webContextMenuRequested(Item item);
    NumberAnimation{
        id:closeAnim
        target: webItem;
        properties:"opacity";
        to:0
        duration:150
        onStopped: {
            console.log("webItem destroy()");
            webItem.destroy();
        }
    }
    NumberAnimation{
        id: openAnim
        target: webItem;
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
        console.debug("close webItem!!");
    }

    function closeWithoutAnimation()
    {
        webItem.destroy();
        console.debug("close webItem!!");
    }

    onMouseOverChanged: {
        /* ViewingGrid의 mouseOver 이벤트는 웹아이템에서는 무시한다. */
    }

    WebEngineView {
        id: webEngineView
        anchors.margins: 5
        anchors.top: osdTopControl.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right : parent.right
        //zoomFactor: 0.8
        url: itemModel.webPageUrl

        property bool resetAuth: true
        property bool useAuth: itemModel.webPageUseAuth;
        property string userName: itemModel.webPageUser;
        property string userPassword: itemModel.webPagePassword;

        Connections{
            target: itemModel

            // id, password, use auth가 변경되면 session 정보 clear
            onWebpageClear:{
                console.log("webItem.onWebpageClear()");
                webEngineView.profile.clearHttpCache();
                webEngineView.resetAuth = true
            }
        }

        onUseAuthChanged: {
            console.log('webItem.onUseAuthChanged()', useAuth);
            webEngineView.profile.clearHttpCache();
            resetAuth = true;
        }

        onAuthenticationDialogRequested: {
            console.log("webItem.onAuthenticationDialogRequested()", resetAuth, useAuth);

            // https://doc.qt.io/qt-5/qml-qtwebengine-webengineview.html#authenticationDialogRequested-signal
            // Signal handlers need to call request.accepted = true to prevent a default dialog from showing up

            // auth를 사용할 경우에만 추가 팝업을 안띄우게 함. 실패하여도 그냥 실패로 끝나도록 함.
            if (useAuth) {
                request.accepted = true
                request.dialogAccept(webEngineView.userName, webEngineView.userPassword)
            }

            resetAuth = false;
        }

        onJavaScriptDialogRequested: {
            console.log("webItem.onJavaScriptDialogRequested()", resetAuth, useAuth);
        }

        onZoomFactorChanged: {
            console.log("webItem.onZoomFactorChanged()", zoomFactor);
        }

        onContentsSizeChanged: {
            console.log("webItem.onContentsSizeChanged()", contentsSize, zoomFactor);
            zoomFactor = zoomFactor;
        }

        onContextMenuRequested: {
            console.log("webItem.onContextMenuRequested()");
            request.accepted = true;
            webItem.webContextMenuRequested(webItem);
        }
        onCertificateError: {
            console.log("webItem.onCertificateError()");
            error.ignoreCertificateError()
            return true
        }
    }



    WebTopControl {
        id: osdTopControl
        displayName: itemModel.webPageName
        viewZoomFactor: webEngineView.zoomFactor
        minHeight: 1
        onItemClosed: {
            webItem.itemClose(webItem);
        }
        onReloaded: {
            console.log("osdTopControl.onReloaded()");
            webEngineView.reload();
        }
        onZoomIn: {
            var factor = webEngineView.zoomFactor + 0.1;
            if (factor > 5.0)
                factor = 5.0
            //console.log("zoomin:", factor, webEngineView.zoomFactor);
            webEngineView.zoomFactor = factor;
            webEngineView.zoomFactor = factor;
        }
        onZoomOut: {
            var factor = webEngineView.zoomFactor - 0.1;
            if (factor < 0.3)
                factor = 0.3
            //console.log("zoomout:", factor, webEngineView.zoomFactor);
            webEngineView.zoomFactor = factor;
            webEngineView.zoomFactor = factor;
        }
    }

    Component.onCompleted: {
        //console.log('WebItem::', Screen.devicePixelRatio, Screen.pixelDensity);
    }
}
