import QtQuick 2.0
import WisenetStyle 1.0
import WisenetLanguage 1.0


Item{
    id : root
    anchors.fill: parent
    //spacing: 10

    property bool isEnable: false
    property int duration : 3
    property bool isAlarmDurationView : false

    property alias sliderFrom : durationSlider.from
    property alias sliderTo : durationSlider.to

    property alias comboBoxModel : combo.model

    function valueToIndex(durationParam){
        var ret = 0

        if(!isAlarmDurationView){
            if(durationParam === 3)
                ret = 0
            else if(durationParam === 4)
                ret = 1
            else
                ret = 2
        }
        else{
            ret = durationParam
        }

        console.log("[AddEventRuleDuration] valueToIndex durationParam:" + durationParam + " ret:" + ret)
        return ret
    }

    function indexToValue(index){
        var ret = 3
        if(!isAlarmDurationView){
            if(index === 0)
                ret = 3
            else if(index === 1)
                ret = 4
            else
                ret = 5
        }
        else
            ret = index
        console.log("[AddEventRuleDuration] indexToValue index:" + index + " ret:" + ret)
        return ret
    }

    WisenetText {
        id : durationText
        text: WisenetLinguist.duration
        width: 73
        enabled: isEnable
        opacity: enabled?1:0.3
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter:  parent.verticalCenter
    }
    WisenetSlider{
        id : durationSlider
        enabled: durationText.enabled
        opacity: enabled?1:0.3
        width: 120
        anchors.left: durationText.right
        anchors.verticalCenter:  parent.verticalCenter

        property int duration: root.duration
        from : 3
        to : 5
        onDurationChanged: {
            value = duration
            console.log("[AddEventRuleDuration] slider onDurationChanged root.duration:" + root.duration)
        }
        onValueChanged: {
            if(pressed){
                root.duration = value
                console.log("[AddEventRuleDuration] slider onValueChanged root.duration:" + root.duration)
            }
        }
    }
    WisenetComboBox{
        id : combo
        enabled: durationSlider.enabled
        opacity: enabled?1:0.3
        model: ["3 " + WisenetLinguist.sec,
            "4 " + WisenetLinguist.sec,
            "5 " + WisenetLinguist.sec]
        width: 108
        anchors.verticalCenter:  parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10

        property int duration: root.duration

        onDurationChanged: {
            currentIndex =  root.valueToIndex(duration)
            console.log("[AddEventRuleDuration] combo onDurationChanged root.duration:" + root.duration + "currentIndex:" + currentIndex)
        }
        onActivated: {
            root.duration = root.indexToValue(currentIndex)
            console.log("[AddEventRuleDuration] combo onActivated root.duration:" + root.duration + "currentIndex:" + currentIndex)
        }

    }
}

