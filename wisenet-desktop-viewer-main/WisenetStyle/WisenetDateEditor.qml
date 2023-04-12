import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 108
    height: 17

    property int year: Number(yearTextField.text)
    property int month: Number(monthTextField.text)
    property int day: Number(dayTextField.text)
    property bool controlFocus: focusScope.activeFocus
    property bool alignHCenter: false
    property int minimumYear: 1970
    property int maximumYear: new Date().getFullYear()
    property bool readOnly: false
    Component.onCompleted: {
        var dateFormat = localeManager.getDateFormat().toLowerCase()
        var currentOrder = 1
        var yearOrder = 0
        var monthOrder = 0
        var dayOrder = 0
        var separator = ""

        for(var i=0 ; i<dateFormat.length ; i++) {
            var c = dateFormat.charAt(i)

            if(separator == "" && c !== 'y' && c !== 'm' && c !== 'd' && c !== ' ')
                separator = c

            if(separator != "" && c === ' ' && separator.charAt(separator.length-1) != ' ')
                separator += ' '

            if(c === 'y' && yearOrder == 0)
                yearOrder = currentOrder++

            if(c === 'm' && monthOrder == 0)
                monthOrder = currentOrder++

            if(c === 'd' && dayOrder == 0)
                dayOrder = currentOrder++
        }

        //console.log("WisenetDateEditor::Component.onCompleted()", dateFormat, yearOrder, monthOrder, dayOrder, separator)

        splitText1.text = splitText2.text = separator

        if(yearOrder == 1) {
            // 년도가 맨 앞이면 yyyy.mm.dd 순서
            splitText1.anchors.left = yearTextField.right
            monthTextField.anchors.left = splitText1.right
            splitText2.anchors.left = monthTextField.right
            dayTextField.anchors.left = splitText2.right
        }
        else if(monthOrder == 1) {
            // 월이 맨 앞이면 mm.dd.yyyy 순서
            splitText1.anchors.left = monthTextField.right
            dayTextField.anchors.left = splitText1.right
            splitText2.anchors.left = dayTextField.right
            yearTextField.anchors.left = splitText2.right
        }
        else {
            // 일이 맨 앞이면 dd.mm.yyyy 순서
            splitText1.anchors.left = dayTextField.right
            monthTextField.anchors.left = splitText1.right
            splitText2.anchors.left = monthTextField.right
            yearTextField.anchors.left = splitText2.right
        }
    }

    function setDateText(yearVal, monthVal, dayVal) {
        yearTextField.text = yearVal
        monthTextField.text = monthVal < 10 ? "0" + monthVal : monthVal
        dayTextField.text = dayVal < 10 ? "0" + dayVal : dayVal
    }

    function getDateText() {
        if(year < minimumYear)
            yearTextField.text = minimumYear
        if(month < 10)
            monthTextField.text = "0" + month
        if(day < 10)
            dayTextField.text = "0" + day

        return yearTextField.text + "-" + monthTextField.text + "-" + dayTextField.text
    }

    function setDate(date) {
        var yearVal = date.getFullYear()
        var monthVal = date.getMonth() + 1
        var dayVal = date.getDate()

        setDateText(yearVal, monthVal, dayVal)
    }

    function getDate() {
        return new Date(year, month-1, day)
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter || event.key === Qt.Key_Escape)
            focusScope.focus = false
    }

    Rectangle {
        width: parent.width
        height: 1
        color: WisenetGui.contrast_06_grey
        anchors.bottom: parent.bottom
    }

    MouseArea {
        z: 2
        anchors.fill: parent
        enabled: readOnly
    }

    FocusScope {
        id: focusScope
        anchors.fill: parent

        Rectangle {
            id: textRow
            width: yearTextField.width + monthTextField.width*2 + splitText1.width*2
            height: 14
            //spacing: 0
            anchors.horizontalCenter: alignHCenter ? parent.horizontalCenter : undefined
            anchors.left: alignHCenter ? undefined : parent.left
            anchors.leftMargin: alignHCenter ? 0 : 3
            color: WisenetGui.transparent

            component BaseTextField: TextField {
                height: 14
                padding: 0
                leftPadding: 0
                background: Rectangle {
                    color: WisenetGui.transparent
                }
                font.pixelSize: 12
                color: !enabled ? WisenetGui.contrast_06_grey : hovered ? WisenetGui.contrast_00_white : WisenetGui.contrast_02_light_grey
                selectionColor: WisenetGui.color_primary_deep_dark
                selectedTextColor: WisenetGui.contrast_00_white
                verticalAlignment: TextInput.AlignVCenter
                horizontalAlignment: TextInput.AlignHCenter
                validator: RegExpValidator { regExp: /^[0-9]?[0-9]$/ }
                inputMethodHints: Qt.ImhDigitsOnly
                selectByMouse: true
                text: "01"
                readOnly: root.readOnly
                hoverEnabled: true
            }

            BaseTextField {
                id: yearTextField
                KeyNavigation.tab: monthTextField
                text: "1970"
                validator: RegExpValidator { regExp: /^[0-9]?[0-9]?[0-9]?[0-9]$/ }

                Keys.onPressed: {
                    var num = Number(text)

                    if(event.key === Qt.Key_Up) {
                        num += 1
                        text = num
                    }
                    else if(event.key === Qt.Key_Down && num > minimumYear) {
                        num -= 1
                        text = num
                    }
                }

                onTextChanged: {
                    var num = Number(text)

                    if(num > maximumYear)
                        text = maximumYear
                }

                onActiveFocusChanged: {
                    if(activeFocus) {
                        selectAll()
                    }
                    else {
                        var num = Number(text)
                        if(num < minimumYear)
                            text = minimumYear
                    }
                }
            }

            BaseTextField {
                id: splitText1
                text: "-"
                enabled: false
                //validator: undefined
                color: !root.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_02_light_grey
            }

            BaseTextField {
                id: monthTextField
                KeyNavigation.tab: dayTextField

                Keys.onPressed: {
                    var num = Number(text)

                    if(event.key === Qt.Key_Up) {
                        num += 1
                        text = num < 10 ? "0" + String(num) : String(num)
                    }
                    else if(event.key === Qt.Key_Down && num > 1) {
                        num -= 1
                        text = num < 10 ? "0" + String(num) : String(num)
                    }
                }

                onTextChanged: {
                    var time = Number(text)

                    if(time > 12)
                        text = "12"
                }

                onActiveFocusChanged: {
                    if(activeFocus) {
                        selectAll()
                    }
                    else {
                        var num = Number(text)

                        if(num < 1)
                            text = "01"
                        else if(num < 10)
                            text = "0" + num
                    }
                }
            }

            BaseTextField {
                id: splitText2
                text: "-"
                enabled: false
                //validator: undefined
                color: !root.enabled ? WisenetGui.contrast_06_grey : WisenetGui.contrast_02_light_grey
            }

            BaseTextField {
                id: dayTextField
                KeyNavigation.tab: yearTextField

                Keys.onPressed: {
                    var num = Number(text)

                    if(event.key === Qt.Key_Up) {
                        num += 1
                        text = num < 10 ? "0" + String(num) : String(num)
                    }
                    else if(event.key === Qt.Key_Down && num > 1) {
                        num -= 1
                        text = num < 10 ? "0" + String(num) : String(num)
                    }
                }

                onTextChanged: {
                    var num = Number(text)
                    var days = new Date(year, month, 0).getDate()

                    if(num > days)
                        text = days
                }

                onActiveFocusChanged: {
                    if(activeFocus) {
                        selectAll()
                    }
                    else {
                        var num = Number(text)

                        if(num < 1)
                            text = "01"
                        else if(num < 10)
                            text = "0" + num
                    }
                }
            }
        }
    }
}
