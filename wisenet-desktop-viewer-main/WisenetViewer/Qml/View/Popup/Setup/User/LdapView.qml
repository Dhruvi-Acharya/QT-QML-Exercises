import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0

import WisenetStyle 1.0
import WisenetLanguage 1.0
import Wisenet.Define 1.0
import Wisenet.Setup 1.0
import "qrc:/"

Page {
    id: root
    property int xMargin : 15
    property int yMargin: 20
    visible: true

    property color backgroundColor: WisenetGui.color_setup_Background
    property int textColumnWidth: 150
    property int lineSpace: 32
    property bool rightVisible: false

    background: Rectangle{
        color: WisenetGui.color_setup_Background
    }

    function allUncheckedDisplay() {
        ldapUserTable.allCheckedStatus = false
    }

    function checkTextFields() {
        console.log("checkTextFields()")
        applyButton.allTextFieldFilled = false
        if(serverUrlTextField.textField.text.length === 0) {
            messageDialog.message = WisenetLinguist.pleaseEnterServerUrl
            return
        }
        else if(adminDNTextField.textField.text.length === 0) {
            messageDialog.message = WisenetLinguist.pleaseEnterAdminDn
            return
        }
        else if(passwordTextField.textField.text.length === 0) {
            messageDialog.message = WisenetLinguist.passwordWarningEmpty
            return
        }
        else if(searchBaseTextField.textField.text.length === 0) {
            messageDialog.message = WisenetLinguist.pleaseEnterSearchBase
            return
        }
        applyButton.allTextFieldFilled = true
    }

    onVisibleChanged: {
        ldapUserViewModel.loadLdapSetting()

        if (root.visible) {
            groupCombo.model = ldapUserViewModel.getUserGroupList()

            serverUrlTextField.textField.text = ldapUserViewModel.serverUrl
            adminDNTextField.textField.text = ldapUserViewModel.adminDN
            passwordTextField.textField.text = ""
            searchBaseTextField.textField.text = ldapUserViewModel.searchBase
            searchFilterTextField.textField.text = ldapUserViewModel.searchFilter
        }
        else {
            root.rightVisible = false
            root.allUncheckedDisplay()
            ldapUserViewModel.clear()
        }
    }

    component BaseText: Text{
        color: WisenetGui.contrast_04_light_grey
        height: 14
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    WisenetDivider {
        id : divider
        anchors{top: parent.top; topMargin: 20; left: parent.left; leftMargin: 15}
        label: "LDAP"
    }

    WisenetSetupApplyResultMessage{
        id: applyResultMessage
        anchors {
            right : divider.right
            rightMargin: 30
            verticalCenter: divider.bottom
        }
    }

    WisenetMessageDialog {
        id: messageDialog
        applyButtonVisible: false
        cancelButtonText: WisenetLinguist.cancel
        applyButtonText: WisenetLinguist.ok
        messageAlign: Text.AlignHCenter
        onApplyButtonClicked: {
            console.log("[LdapView] OK Button Clicked.")
            ldapUserViewModel.updateLdapSetting()
            messageDialog.close()
            root.rightVisible = true
        }
        onVisibleChanged: {
            if(!visible) {
                applyButtonVisible = false
            }
        }
    }

    WisenetMessageDialog {
        id: duplicateIDMessage
        message: WisenetLinguist.sameLDAPIdUser;
        applyButtonVisible: true
        cancelButtonText: WisenetLinguist.cancel
        applyButtonText: WisenetLinguist.ok
        onApplyButtonClicked: {
            console.log("[LdapView] duplicateIDMessage Clicked.")
            duplicateIDMessage.close()
            ldapUserViewModel.registerLdapUser(groupCombo.currentText)
        }
    }


    Rectangle {
        id: ldapSettingRect
        color: WisenetGui.transparent
        width: 400
        anchors.top: divider.bottom
        anchors.topMargin: 20
        anchors.left: parent.left

        Rectangle {
            id: textArea
            width: 200
            color: WisenetGui.transparent
            anchors{top: ldapSettingRect.top;
                    topMargin: 20;
                    left: ldapSettingRect.left;
                    leftMargin: 15}

            BaseText {
                id: serverUrlText
                text: WisenetLinguist.serverUrl
                width: textColumnWidth
                anchors{top: textArea.top; left: textArea.left}
            }

            BaseText {
                id: adminDNText
                text: WisenetLinguist.adminDn
                width: textColumnWidth
                wrapMode: Text.Wrap
                anchors{ top: serverUrlText.bottom; topMargin: root.lineSpace; left: textArea.left}
            }

            BaseText {
                id: passwordText
                text: WisenetLinguist.password
                width: textColumnWidth
                anchors{top: adminDNText.bottom; topMargin: root.lineSpace; left: textArea.left}
            }

            BaseText {
                id: searchBaseText
                text: WisenetLinguist.searchBase
                width: textColumnWidth
                wrapMode: Text.Wrap
                anchors{top: passwordText.bottom; topMargin: root.lineSpace; left: textArea.left}
            }

            BaseText {
                id: searchFilterText
                text: WisenetLinguist.searchFilter + " (" + WisenetLinguist.optional + ")"
                width: textColumnWidth
                wrapMode: Text.Wrap
                anchors{top: searchBaseText.bottom; topMargin: root.lineSpace; left: textArea.left}
            }
        }

        Rectangle {
            id: textFieldArea
            color: "transparent"
            height: 220
            anchors{top: textArea.top; left: textArea.right; right: ldapSettingRect.right}

            WisenetTextField {
                id: serverUrlTextField
                anchors{top: textFieldArea.top; topMargin: -1; right: textFieldArea.right}
                placeholderText: "ldap://"
            }

            WisenetTextField {
                id: adminDNTextField
                anchors{top: serverUrlTextField.top; topMargin: root.lineSpace + 13;
                        right: serverUrlTextField.right}
            }

            WisenetPasswordBox {
                id: passwordTextField
                anchors{top: adminDNTextField.top; topMargin: root.lineSpace + 13
                        right: adminDNTextField.right}
                isEnabled: true
            }

            WisenetTextField {
                id: searchBaseTextField
                anchors{top: passwordTextField.top; topMargin: root.lineSpace + 13
                        right: passwordTextField.right}
            }

            WisenetTextField {
                id: searchFilterTextField
                anchors{top: searchBaseTextField.top; topMargin: root.lineSpace + 13
                        right: searchBaseTextField.right}
            }
        }

        WisenetGrayButton {
            id: applyButton
            anchors{top: textFieldArea.bottom; topMargin: root.lineSpace;
                    horizontalCenter: parent.horizontalCenter}
            text: WisenetLinguist.apply
            property bool allTextFieldFilled: false
            onClicked: {
                console.log("[LdapView] Apply Button clicked")
                checkTextFields()
                ldapUserViewModel.clear()
                if(allTextFieldFilled) { // 모든 필드에 값이 다 채워졌을 경우
                    pulseView.open()
                    // set ViewModel's property to textfields' text
                    ldapUserViewModel.serverUrl = serverUrlTextField.textField.text
                    ldapUserViewModel.adminDN = adminDNTextField.textField.text
                    ldapUserViewModel.password = passwordTextField.textField.text
                    ldapUserViewModel.searchBase = searchBaseTextField.textField.text
                    ldapUserViewModel.searchFilter = searchFilterTextField.textField.text
                    ldapUserViewModel.checkLdapServerUrl(); // make a LDAP connection after URL check
                }
                else{ // 값이 다 채워지지 않았을 경우 경고 메세지 출력
                    messageDialog.show()
                    root.rightVisible = false
                    root.allUncheckedDisplay()
                }
            }
        }
    }

    Rectangle {
        id: areaSeparator
        width:1; height: parent.height
        color: WisenetGui.setupPageLine
        anchors{top: divider.bottom; topMargin: 32;
                bottom: parent.bottom; bottomMargin: 10;
                left: ldapSettingRect.right; leftMargin: 10}
    }

    LdapUserViewModel {
        id: ldapUserViewModel

        onConnCompleted: {
            pulseView.close()
            if(connSuccess) { // 연결 성공
                if(searchSuccess) { // 검색 성공
                    if(isServerChanged)
                        messageDialog.message = WisenetLinguist.ldapConnSuccess + "\n" + WisenetLinguist.idFounded + " : " + fetchedUserCnt + "\n\n" + WisenetLinguist.ldapServerChanged + "\n" + WisenetLinguist.loginNewLdapID
                    else
                        messageDialog.message = WisenetLinguist.ldapConnSuccess + "\n" + WisenetLinguist.idFounded + " : " + fetchedUserCnt
                    userFoundText.userCount = fetchedUserCnt
                    messageDialog.applyButtonVisible = true
                }
                else // 검색 실패
                {
                    messageDialog.message = WisenetLinguist.searchFailed + "\n" + String(error)
                    root.rightVisible = false
                }
            }
            else // 연결 실패
            {
                messageDialog.message = WisenetLinguist.ldapConnFailed + "\n" + String(error)
                root.rightVisible = false
            }
            messageDialog.show()
        }

        onResultMessage: applyResultMessage.showMessage(success? WisenetLinguist.success : WisenetLinguist.failed)

    }

    ListModel {
        id: ldapUserColumns
        property int columnWidth: 130
        Component.onCompleted: {
            append({"columnWidth": 40, "title": qsTr("Check"), "sort": 3, "splitter": 1})
            append({"columnWidth": columnWidth, "title": qsTr("ID"), "sort": 3, "splitter": 1})
            append({"columnWidth": columnWidth, "title": WisenetLinguist.name, "sort": 3, "splitter": 1})
            append({"columnWidth": columnWidth, "title": WisenetLinguist.emailPageSubject, "sort": 3, "splitter": 1})
            append({"columnWidth": columnWidth, "title": qsTr("DN"), "sort": 3, "splitter": 1})
        }
    }

    DelegateChooser {
        id: ldapUserChooser
        DelegateChoice {
            id: checkDel
            column: 0
            delegate: WisenetSetupTableCheckRectangle {
                checkedValue: display

                onContainsMouseChanged: ldapUserViewModel.setHoveredRow(row, containsMouse)
                onClicked:{
                    ldapUserViewModel.setCheckState(row, checkedValue)
                    ldapUserViewModel.setHighlightRow(row, modifier)
                    if (display === false)
                        root.allUncheckedDisplay()
                }
            }
        }

        DelegateChoice {
            id: idDel
            column: 1
            delegate: Rectangle{
                id: idRect

                property int defaultCellHeight: 36
                implicitHeight: defaultCellHeight
                color: highlightRole ?  WisenetGui.color_primary_press :hoveredRole ? WisenetGui.color_primary : "transparent"

                Text{
                    id : cellText
                    text: display + "  "
                    anchors.centerIn: parent

                    color: highlightRole ?  WisenetGui.contrast_00_white :hoveredRole ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                    elide: Text.ElideRight

                    Image{
                        id: image
                        visible: ldapUserViewModel.isDuplicateID(row)
                        width: 16; height: 16
                        sourceSize: "16x16"
                        anchors {left:parent.right;verticalCenter: parent.verticalCenter}
                        source:idRectMouseArea.containsMouse? WisenetImage.object_search_filter_info_hover : WisenetImage.info_normal
                    }
                }

                WisenetTableCellLine{ anchors.bottom: parent.bottom }

                MouseArea {
                    id: idRectMouseArea
                    anchors.fill : parent
                    hoverEnabled: true
                    onContainsMouseChanged: {
                        ldapUserViewModel.setHoveredRow(row, containsMouse);
                        if(containsMouse) {
                            if(ldapUserViewModel.isDuplicateID(row))
                                duplicateIdToolTip.visible=true

                        }
                        else
                            duplicateIdToolTip.visible=false
                    }
                    onPressed: ldapUserViewModel.setHighlightRow(row, mouse.modifier);
                }
                WisenetMediaToolTip{
                    id: duplicateIdToolTip
                    text: WisenetLinguist.idAlreadyExist
                    delay: 200
                }
            }
        }

        DelegateChoice {
            id: dnDel
            column: 4
            delegate: WisenetSetupTableDefaultRect{
                text: display
                onPressed: ldapUserViewModel.setHighlightRow(row, modifier);

                onContainsMouseChanged: {
                    ldapUserViewModel.setHoveredRow(row, containsMouse)
                    dnToolTip.visible = containsMouse
                }
                WisenetMediaToolTip{
                    id: dnToolTip
                    text: display
                    delay: 200
                }
            }
        }

        DelegateChoice {
            delegate: WisenetSetupTableDefaultRect{
                text: display
                onPressed:ldapUserViewModel.setHighlightRow(row, modifier);
                onContainsMouseChanged: ldapUserViewModel.setHoveredRow(row, containsMouse);
            }
        }
    }


    Rectangle {
        id: ldapUserRect
        anchors{top: areaSeparator.top; left: areaSeparator.right; leftMargin: 10;
                right: parent.right; rightMargin: 10; bottom: parent.bottom}
        color:  "transparent"

        Rectangle {
            id: tableRect
            anchors{top: ldapUserRect.top; left: ldapUserRect.left; right: ldapUserRect.right
                    bottom: registerButton.top; bottomMargin: 40}
            color: "transparent"

            WisenetTable {
                id: ldapUserTable
                anchors.fill: parent
                columns: ldapUserColumns
                cellDelegate: ldapUserChooser
                tableModel: ldapUserViewModel
                contentColor: "transparent"
                visible: true

                onAllChecked: {
                    console.log("[LdapView] All LDAP user checked", checked)
                    ldapUserViewModel.setAllCheckState(checked)
                }
            }
        }

        BaseText{
            id: userFoundText
            anchors{left: tableRect.left; verticalCenter: registerButton.verticalCenter}
            property string userCount : ""
            text: "Result : " + userCount + " user found"
            color: WisenetGui.contrast_04_light_grey
            visible: root.rightVisible
        }

        WisenetGrayButton {
            id: registerButton
            anchors{bottom: parent.bottom; bottomMargin: 20; right: parent.right}
            text: WisenetLinguist.register
            visible: true
            enabled: ldapUserViewModel.selectedRowCount > 0
            onClicked: {
                console.log("[LdapView] Register Button clicked ", groupCombo.currentText)
                if(!ldapUserViewModel.checkDuplicateUser())
                    ldapUserViewModel.registerLdapUser(groupCombo.currentText)
                else
                    duplicateIDMessage.show()
            }
        }

        WisenetComboBox {
            id: groupCombo
            width: 180
            anchors{bottom: parent.bottom; bottomMargin: 20
                    right: registerButton.left; rightMargin: 5}
            visible: true
            enabled: root.rightVisible
        }
    }

    Popup {
        id: pulseView

        width: 100; height: 100
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose

        background: Rectangle{
            color: WisenetGui.transparent
        }

        contentItem: WisenetMediaBusyIndicator {
            id: processingIndicator
            width: 100; height: 100
            sourceWidth: 100; sourceHeight: 100
            anchors.centerIn: parent
            visible: true
            running: true
        }
    }
}
