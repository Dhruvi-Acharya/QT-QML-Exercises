import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import WisenetMediaFramework 1.0
import WisenetStyle 1.0
import WisenetLanguage 1.0

WisenetPopupWindowBase {
    title: WisenetLinguist.setup
    width: 530
    height: 422
    resizeEnabled: false
    minmaxButtonVisible: false
    applyButtonVisible: false
    cancelButtonText: WisenetLinguist.close

    component CommonLabel: WisenetLabel {
        x: 50
        width: 175
        height: 14
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.Wrap
    }

    CommonLabel {
        id: languageLabel
        anchors.top: parent.top
        anchors.topMargin: 27
        text: WisenetLinguist.language
    }

    CommonLabel {
        id: useHWDecodingLabel
        visible: decoderManager.supportsHardwareDecoding()
        anchors.top: languageLabel.bottom
        anchors.topMargin: 32
        text: WisenetLinguist.useHardwareDecoding
    }

    CommonLabel {
        id: versionLabel
        anchors.top: useHWDecodingLabel.visible ? useHWDecodingLabel.bottom : languageLabel.bottom
        anchors.topMargin: 32
        text: WisenetLinguist.version
    }

    CommonLabel {
        id: licenseLabel
        anchors.top: versionLabel.bottom
        anchors.topMargin: 32
        text: WisenetLinguist.opensourceLicense
    }

    WisenetComboBox{
        id : languageCombo
        x: 230
        anchors.verticalCenter: languageLabel.verticalCenter
        popup.height: 242

        model: [
            WisenetLinguist.korean,
            WisenetLinguist.english + "-USA",
            WisenetLinguist.english + "-UK",
            WisenetLinguist.chinese,
            WisenetLinguist.croatian,
            WisenetLinguist.czech,
            WisenetLinguist.danish,
            WisenetLinguist.dutch,
            WisenetLinguist.finnish,
            WisenetLinguist.french,
            WisenetLinguist.german,
            WisenetLinguist.greek,
            WisenetLinguist.hungarian,
            WisenetLinguist.italian,
            WisenetLinguist.japanese,
            WisenetLinguist.norwegian,
            WisenetLinguist.polish,
            WisenetLinguist.portuguese,
            WisenetLinguist.romanian,
            WisenetLinguist.russian,
            WisenetLinguist.serbian,
            WisenetLinguist.spanish,
            WisenetLinguist.taiwanese,
            WisenetLinguist.turkish
        ]

        currentIndex: languageManager.language
        onActivated: {
            languageManager.language = currentIndex
        }
    }

    WisenetCheckBox{
        id: useHwDecodingCheckBox
        visible: decoderManager.supportsHardwareDecoding()
        anchors.left: languageCombo.left
        anchors.verticalCenter: useHWDecodingLabel.verticalCenter

        onCheckedChanged: {
            mainViewModel.setUseHwDecoding(checked)
        }
    }

    CommonLabel {
        id: versionValue
        anchors.left: languageCombo.left
        anchors.verticalCenter: versionLabel.verticalCenter
        text: "v" + Qt.application.version + ", " + mainViewModel.buildDate
    }

    WisenetColorLabel {
        id: opensourceButton
        anchors.left: languageCombo.left
        anchors.verticalCenter: licenseLabel.verticalCenter
        text: WisenetLinguist.view
        onClicked: mainViewModel.openSourceLicense()
    }
}
