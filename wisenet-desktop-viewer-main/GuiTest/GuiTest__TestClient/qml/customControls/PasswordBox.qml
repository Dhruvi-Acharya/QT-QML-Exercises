import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle{
    color: "#181818"
    height: 30
    width: parent.width
    radius: 3
    property alias text: textField.text
    property bool isEnabled : false


    RowLayout{
        anchors.fill: parent

        TextField{
            id: textField
            Layout.fillWidth: true
            Layout.fillHeight: true
            echoMode: TextInput.Password

            style: TextFieldStyle{
                placeholderTextColor: "gray"
                textColor: "white"
                background: Rectangle{
                    anchors.fill: parent
                    //color를  지정하면 cusor가 뒤로 가면서 앞에서 보이지 않음
                    color: "transparent"
                }
            }

            onTextChanged: {
                // : 숫자, 특문 각 1회 이상, 영문은 2개 이상 사용하여 8자리 이상 입력
                var passReg = /(?=.*\d{1,50})(?=.*[~`!@#$%\^&*()-+=]{1,50})(?=.*[a-zA-Z]{2,50}).{8,50}$/;
                if(text.length < 8){
                    image.source = "icon/icon-short.PNG"
                    isEnabled = false;
                }
                else if(passReg.test(text)){
                    image.source = "icon/icon-good.PNG"
                    isEnabled = true;
                }
                else{
                    image.source = "icon/icon-weak.PNG"
                    isEnabled = false;
                }
            }
        }
        Image {
            id: image
            source: ""
        }
    }
}
