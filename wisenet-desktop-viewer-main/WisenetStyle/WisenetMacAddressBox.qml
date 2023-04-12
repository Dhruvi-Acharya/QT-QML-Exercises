import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle{
    id: root
    color: "transparent"
    height: 16
    width: 159

    property bool readOnly : false

    readonly property string macAddress: mac1st.text + mac2nd.text + mac3rd.text + mac4th.text + mac5th.text + mac6th.text

    component MacAddressTextBox: WisenetTextBox{
        id : macAddressTextBox

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        width: 24
        text: ""
        maximumLength: 2

        capitalization: Font.AllUppercase
        horizontalAlignment: Text.AlignHCenter
        validator: RegExpValidator { regExp: /[0-9A-Fa-f]+/ }
        bottomBorderVisible: false
    }

    component MacSeparator: Text{

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4
        width: 3
        height: 17
        text: ":"
        font.pixelSize: 12
        color: enabled ? WisenetGui.contrast_02_light_grey : WisenetGui.contrast_07_grey

        horizontalAlignment: Text.AlignHCenter
    }

    function clearText(){
        mac1st.text = "";
        mac2nd.text = "";
        mac3rd.text = "";
        mac4th.text = "";
        mac5th.text = "";
        mac6th.text = "";
    }

    function setText(str){
        var upper = str.toUpperCase()
        mac1st.text = upper.substr(0,2)
        mac2nd.text = upper.substr(2,2)
        mac3rd.text = upper.substr(4,2)
        mac4th.text = upper.substr(6,2)
        mac5th.text = upper.substr(8,2)
        mac6th.text = upper.substr(10,2)
    }

    MacAddressTextBox{
        id : mac1st

        anchors.left: parent.left

        onTextChanged: {
            if(mac1st.text.length == 2)
            {
                mac2nd.textField.forceActiveFocus()
            }
        }
    }

    MacSeparator{
        id: separator1st

        anchors.left: mac1st.right
    }

    MacAddressTextBox{
        id : mac2nd

        anchors.left: separator1st.right
        onTextChanged: {
            if(mac2nd.text.length == 2)
            {
                mac3rd.textField.forceActiveFocus()
            }
        }

        onBackward:{
            mac1st.textField.forceActiveFocus()
        }
    }

    MacSeparator{
        id: separator2nd

        anchors.left: mac2nd.right
    }

    MacAddressTextBox{
        id : mac3rd

        anchors.left: separator2nd.right
        onTextChanged: {
            if(mac3rd.text.length == 2)
            {
                mac4th.textField.forceActiveFocus()
            }
        }

        onBackward:{
            mac2nd.textField.forceActiveFocus()
        }
    }

    MacSeparator{
        id: separator3rd

        anchors.left: mac3rd.right
    }

    MacAddressTextBox{
        id : mac4th

        anchors.left: separator3rd.right
        onTextChanged: {
            if(mac4th.text.length == 2)
            {
                mac5th.textField.forceActiveFocus()
            }
        }

        onBackward:{
            mac3rd.textField.forceActiveFocus()
        }
    }

    MacSeparator{
        id: separator4th

        anchors.left: mac4th.right
    }

    MacAddressTextBox{
        id : mac5th

        anchors.left: separator4th.right
        onTextChanged: {
            if(mac5th.text.length == 2)
            {
                mac6th.textField.forceActiveFocus()
            }
        }

        onBackward:{
            mac4th.textField.forceActiveFocus()
        }
    }

    MacSeparator{
        id: separator5th

        anchors.left: mac5th.right
    }

    MacAddressTextBox{
        id : mac6th

        anchors.left: separator5th.right
        onBackward:{
            mac5th.textField.forceActiveFocus()
        }
    }


    Rectangle{
        id: bottomBorder
        enabled: root.enabled
        color: root.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_07_grey
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
    }
}
