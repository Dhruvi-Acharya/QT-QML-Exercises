import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtWebEngine 1.10

Window {
    id : root
    width: 1024
    height: 750
    visible: false
    signal notFound()
    onVisibleChanged: {
        console.log("[DeviceListWebPage] onVisibleChanged", visible)
        if (!visible) {
            //webEngineView.stop()

            console.log("[DeviceListWebPage] about:blank for next time", visible)
            webEngineView.clear();
        }
    }

    function reload(p_url, p_id, p_password, p_supportWebSocket)
    {
        console.log("[DeviceListWebPage] reload() start", webEngineView.url, p_url)
        //webEngineView.profile.clearHttpCache();
        webEngineView.userId = p_id
        webEngineView.password = p_password
        webEngineView.supportDigest = p_supportWebSocket
        webEngineView.resetAuth = true;
        webEngineView.url = p_url

        console.log("[DeviceListWebPage] reload() finisned")
        console.log("")

    }

    function clearCredential()
    {
        webEngineView.profile.clearHttpCache()
    }

    WebEngineView {
        id: webEngineView
        property bool resetAuth: true
        property string userId: ""
        property string password: ""
        property bool supportDigest: false
        //url : "about:blank"
        anchors.fill: parent
        profile.httpCacheType: WebEngineProfile.NoCache
        profile.persistentCookiesPolicy: WebEngineProfile.NoPersistentCookies

        function clear()
        {
            //profile.clearHttpCache();
            webEngineView.url = "about:blank"
            console.log("[DeviceListWebPage]  clear()");
        }

        onNewViewRequested:{
            Qt.openUrlExternally(request.requestedUrl)
        }


        onAuthenticationDialogRequested: {
            console.log("[DeviceListWebPage]  onAuthenticationDialogRequested start");
            request.accepted = true;
            if (resetAuth) {
                console.log("[DeviceListWebPage]  onAuthenticationDialogRequested accept!");
                request.accepted = true;
                request.dialogAccept(userId, password)
            }
            resetAuth = false;
        }
        onCertificateError: {
            console.log("[DeviceListWebPage]  onCertificateError()");
            error.ignoreCertificateError()
            return true
        }
        onLoadingChanged: {
            console.log("[DeviceListWebPage] onLoadingChanged:", url, loadRequest.errorCode, loadRequest.status)
            if(loadRequest.errorCode == 404 && supportDigest)
                root.notFound()


        }
        onWindowCloseRequested: {
            console.log("[DeviceListWebPage] onWindowCloseRequested")
        }
    }
}
