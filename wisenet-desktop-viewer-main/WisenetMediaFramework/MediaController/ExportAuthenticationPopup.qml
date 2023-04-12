import QtQuick 2.15
import "qrc:/WisenetStyle/"

WisenetMessageDialog {
    width: 449
    height: 257
    applyButtonText: WisenetLinguist.ok
    applyClose: false

    property bool logout: loginViewModel.logout

    onLogoutChanged: {
        if(logout) {
            // 로그아웃 되면 lockTimer 초기화
            lockTimer.stop()
            lockTimer.failCount = 0
            lockTimer.repeatCount = 0
        }
    }

    onVisibleChanged: {
        if(visible) {
            // 팝업 표시될 때 기존 입력한 값 초기화
            passwordBox.text = ""
            dontAskAgainCheckBox.checked = false
        }
    }

    onApplyButtonClicked: {
        // 패스워드 체크
        if(lockTimer.running) {
            // lock 상태이면 남은 시간 표시
            errorDialog.message = errorDialog.message = WisenetLinguist.loginLocked.arg(30 - lockTimer.repeatCount)
            errorDialog.show()
            return
        }

        var password = loginViewModel.hashPassword ? getHashPassword(loginViewModel.userGuid , passwordBox.text) : passwordBox.text
        if(password === loginViewModel.password) {
            // 패스워드가 맞았을 때
            if(dontAskAgainCheckBox.checked) {
                loginViewModel.exportAuthenticated = true   // 다시 보지 않기 체크상태면 인증 상태 저장
            }

            // lockTimer 초기화
            lockTimer.stop()
            lockTimer.failCount = 0
            lockTimer.repeatCount = 0

            close()
            exportPopupView.show()  // 내보내기 팝업 표시
        }
        else {
            // 패스워드가 틀렸을 때
            lockTimer.failCount++
            if(!lockTimer.running && lockTimer.failCount >= 5)
                lockTimer.start()   // 5회 이상 실패 시 잠금 처리

            errorDialog.message = errorDialog.message = WisenetLinguist.invalidPassword
            errorDialog.show()  // 패스워드 오류 팝업
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        color: WisenetGui.contrast_09_dark

        Keys.onPressed: {
            if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                applyButtonClicked()
            }
        }

        Text {
            id: idText
            y: 20
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 90
            anchors.rightMargin: 90
            font.pixelSize: 12
            leftPadding: 6
            color: WisenetGui.contrast_01_light_grey
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            text: WisenetLinguist.id_ + " : " + loginViewModel.loginId
        }

        WisenetPasswordBox {
            id: passwordBox
            anchors.top: idText.bottom
            anchors.topMargin: 25
            anchors.left: idText.left
            anchors.right: idText.right
            placeholderText: WisenetLinguist.password
            showPasswordButtonVisible: true
        }

        WisenetCheckBox2{
            id: dontAskAgainCheckBox
            anchors.top: passwordBox.bottom
            anchors.topMargin: 25
            anchors.left: idText.left
            anchors.right: idText.right
            text: WisenetLinguist.doNotAskAgain
        }
    }

    Timer {
        id: lockTimer
        interval: 1000; running: false; repeat: true

        property int failCount: 0
        property int repeatCount: 0

        onTriggered: {
            repeatCount++
            if(repeatCount >= 30) {
                // 30초 잠금 후 해제
                repeatCount = 0
                failCount = 4   // 한 번 잠긴 사용자는 한 번만 더 틀려도 다시 잠김
                stop()
            }
        }
    }

    WisenetMessageDialog {
        id: errorDialog
        width: 440
        height: 180
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.close
    }
}
