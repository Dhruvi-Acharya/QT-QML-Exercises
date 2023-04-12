import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

WisenetMediaDialog {
    title: WisenetLinguist.addPreset
    width: 450
    height: 256
    applyAndClose: false
    property int maxPresets: 0
    signal presetAdded(int no, string name);

    function showDialog(item) {
        var presetList = item.player.ptzPresets;
        presetComboList.clear();
        presetListForCommand.clear();
        var firstIndex = maxPresets;
        for (var i = 0 ; i < maxPresets ; i++) {
            var presetNo = i+1;
            var presetName = getPresetName(presetNo, presetList);
            if (presetName === "") {
                if (firstIndex > i) {
                    firstIndex = i;
                }
            }
            presetComboList.append({
                                  "text" : presetNo+":"+presetName
                              });
            presetListForCommand.append({
                                  "pno" : presetNo,
                                  "pname" : presetName
                              });
        }
        presetCombo.currentIndex = firstIndex;
        open();
    }

    function getPresetName(presetNo, presetList) {
        if (presetList) {
            for (var i  = 0 ; i < presetList.length ; i++) {
                if (presetList[i].presetNo === presetNo)
                    return presetList[i].presetName;
            }
        }
        return "";
    }
    onClosed: {
        presetComboList.clear();
        presetListForCommand.clear();
        presetCombo.currentIndex = -1;
    }

    onAccepted: {
        if (presetCombo.currentIndex > -1) {
            var currentIndex = presetCombo.currentIndex;
            var number = presetListForCommand.get(currentIndex).pno;
            var beforeName = presetListForCommand.get(currentIndex).pname;
            var newName = presetTextField.text

            if (newName.length > 0) {
                console.log("preset name=", newName, ", number=", number);
                presetAdded(number, newName);
            }
        }
        close();
    }

    ListModel {
        id: presetListForCommand
    }
    ListModel {
        id: presetComboList
    }
    contentItem: Item {
        Row {
            id: numberRow
            anchors {
                top: parent.top
                topMargin: 28
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 45
            WisenetMediaTextLabel {
                id: numberLabel
                text: WisenetLinguist.number
                width: 80
                height: 20
            }

            WisenetMediaComboBox {
                id: presetCombo
                width: 228
                model: presetComboList
                onCurrentIndexChanged: {
                    console.log("currentIndex=", currentIndex)
                }
            }
        }
        Row {
            id: nameRow
            anchors {
                top: numberRow.bottom
                topMargin: 28
                horizontalCenter: parent.horizontalCenter
            }

            spacing: 45
            WisenetMediaTextLabel {
                id: nameLabel
                text: WisenetLinguist.name
                width: 80
                height: 20
            }

            WisenetMediaTextField {
                id: presetTextField
                maximumLength: 32
                width: 228
                height: 20
            }
        }
    }
}

