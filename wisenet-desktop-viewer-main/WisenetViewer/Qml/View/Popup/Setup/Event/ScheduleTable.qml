import QtQuick 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

Rectangle {
    id: root
    anchors.fill: parent
    color: "transparent"

    property int boxWidth : 26
    property int boxHeight : 36
    property int wideBoxWidth : 76
    property int wideBoxHeight : 58

    //view가 처음 실행될 때 제일 처음으로 불리는 곳, timeButton의 모델의 Element를 24개 추가
    Component.onCompleted: {
        for(var i =0; i<24; i++) {
            timeButton.model.append({timeChecked:true})
        }

        for (i=1; i<8; i++) {
            dayButton.model.append({name: localeManager.getDayName(i), dayChecked: true})
        }
        dayButton.model.append({name: "Holiday", dayChecked: true})
    }

    function applySchedule(row){ // 현재 체크되어 있는 박스들의 정보를 스트링으로 이어서 반환
        //console.log("ScheduleTable.qml applySchedule ", row)

        var currentInfo = ""

        // 현재 박스 정보를 스트링으로
        for(var i=0; i<8*24; i++){
            if (boxButton.itemAtIndex(i).checked === true)
            {
                boxButton.itemAtIndex(i).scheduleOn = true
                currentInfo += '1'
            }
            else
            {
                boxButton.itemAtIndex(i).scheduleOn = false
                currentInfo += '0'
            }
        }

        eventScheduleViewModel.editSchedule(row, currentInfo)
    }

    function showSchedule(scheduleTableInfo) {

        //console.log("[ScheduleTable] showSchedule ", scheduleTableInfo)

        for(var k=0; k<8*24; k++) {
            if (scheduleTableInfo[k] === '1'){
                boxButton.itemAtIndex(k).checked = true
                boxButton.itemAtIndex(k).scheduleOn = true
            }
            else{
                boxButton.itemAtIndex(k).checked = false
                boxButton.itemAtIndex(k).scheduleOn = false
            }
        }
    }

    Item {
        id: guide
        width: offGuide.width*2 + 11; height: 24
        anchors.top: root.top
        anchors.topMargin: 15
        anchors.left: root.left
        anchors.leftMargin: 16

        property bool checkMode : false

        Rectangle {
            id: offGuide
            width: 96; height: guide.height
            anchors.left: guide.left
            color: "transparent"
            border.color: parent.checkMode ? WisenetGui.contrast_07_grey: WisenetGui.contrast_02_light_grey
            border.width: 1

            Rectangle {
                id: offBox
                width: 10; height: 10
                color: WisenetGui.contrast_08_dark_grey
                anchors.left: offGuide.left
                anchors.leftMargin: 31
                anchors.verticalCenter: parent.verticalCenter
                border.color: WisenetGui.contrast_07_grey
                border.width: 1
            }

            Text {
                id: offTxt
                text: "Off"
                anchors.left: offBox.right
                anchors.leftMargin: 7
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                color: WisenetGui.contrast_00_white
            }

            MouseArea {
                id: offMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    guide.checkMode = false
                }
            }
        }

        Rectangle {
            id: onGuide
            width: 96; height: guide.height
            anchors.left: offGuide.right
            anchors.leftMargin: 11
            color: "transparent"
            border.color: parent.checkMode ? WisenetGui.contrast_02_light_grey : WisenetGui.contrast_07_grey
            border.width: 1

            Rectangle {
                id: onBox
                width: 10; height: 10
                anchors.left: onGuide.left
                anchors.leftMargin: 31
                anchors.verticalCenter: parent.verticalCenter
                color: WisenetGui.color_primary
            }

            Text {
                id: onTxt
                text: "On"
                anchors.left: onBox.right
                anchors.leftMargin: 7
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                color: WisenetGui.contrast_00_white
            }

            MouseArea {
                id: onMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    guide.checkMode = true
                }
            }
        }
    }

    Rectangle {
        id: scheduler
        color: "transparent"

        width: allButton.width + timeButton.width; height: allButton.height + dayButton.height
        anchors.top: guide.bottom
        anchors.topMargin: 24
        anchors.left: root.left
        anchors.leftMargin: 17

        Box {
            id: allButton
            width: root.wideBoxWidth; height: root.wideBoxHeight;
            text : WisenetLinguist.all
            textLabel.anchors.top: allButton.top
            textLabel.anchors.topMargin: 13
            textLabel.anchors.left: allButton.left
            textLabel.anchors.leftMargin: 13
            textLabel.anchors.right: allButton.right
            textLabel.anchors.rightMargin: 3

            topLineWidth: 1
            leftLineWidth: 1

            property bool allChecked: true

            Image {
                id: diagonalArrowImage
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 2
                width: 16
                height: 16
                source : WisenetImage.eventSchedule_arrowtip_diagonal
            }

            MouseArea {
                id: allButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                        for(var i=0; i<8*24; i++) {
                            boxButton.itemAtIndex(i).checked = !parent.allChecked
                            boxButton.itemAtIndex(i).scheduleOn = !parent.allChecked
                        }
                        for(i=0; i<8; i++)
                            days.get(i).dayChecked = !parent.allChecked

                        for(i=0; i<24; i++)
                            times.get(i).timeChecked = !parent.allChecked

                        parent.allChecked = !parent.allChecked
                }
            }
        }

        ListView {
            id: dayButton
            width: root.wideBoxWidth; height: root.boxHeight*7
            anchors.top: allButton.bottom
            anchors.left: parent.left

            clip: true
            model : days

            delegate: dayDelegate
            interactive: false
        }

        ListView {
            id: timeButton
            width: root.boxWidth * 24; height: root.wideBoxHeight
            anchors.left: dayButton.right

            clip: true
            model : times
            delegate: timeDelegate
            orientation: ListView.Horizontal

            interactive: false
        }

        ListModel {
            id: days
        }

        ListModel {
            id: times
        }

        Component {
            id: dayDelegate

            Box {
                id : dayBox
                width: root.wideBoxWidth; height: root.boxHeight;
                leftLineWidth: 1
                text: name
                textLabel.anchors.left: dayBox.left
                textLabel.anchors.leftMargin: 13
                textLabel.anchors.right: rightArrowImage.left
                textLabel.anchors.rightMargin: 4
                textLabel.anchors.verticalCenter: dayBox.verticalCenter
                visible: (name !== "Holiday")

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                            for (var i=index*24; i < (index+1)*24; i++) {
                                boxButton.itemAtIndex(i).checked = !dayChecked
                                boxButton.itemAtIndex(i).scheduleOn = !dayChecked
                            }
                            dayChecked = !dayChecked
                    }
                }
                Image {
                    id: rightArrowImage
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    width: 12; height: 12
                    source : WisenetImage.eventSchedule_arrowtip_right
                }
            }
        }

        Component {
            id: timeDelegate

            Box {
                id: timeBox
                width: root.boxWidth; height: allButton.height
                topLineWidth: 1
                text: index
                textLabel.anchors.top: timeBox.top
                textLabel.anchors.topMargin: 14
                textLabel.anchors.horizontalCenter: timeBox.horizontalCenter

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                            for(var i=index; i<8*24; i=i+24){
                                boxButton.itemAtIndex(i).checked = !timeChecked
                                boxButton.itemAtIndex(i).scheduleOn = !timeChecked
                            }
                            timeChecked = !timeChecked
                    }
                }

                Image {
                    id: downArrowImage
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom : parent.bottom
                    anchors.bottomMargin: 5
                    width: 12; height: 12
                    // sourceSize: Qt.size(18, 18)
                    source : WisenetImage.eventSchedule_arrowtip_down
                }
            }
        }

        Rectangle{
            width: 0.5; height: allButton.height + dayButton.height
            anchors.left: boxButton.right;
            anchors.top:timeButton.top
            color: WisenetGui.contrast_07_grey
        }

        Rectangle{
            width: allButton.width+timeButton.width; height: 0.5
            anchors.left: dayButton.left
            anchors.top: dayButton.bottom
            color: WisenetGui.contrast_07_grey
        }

        GridView {
            id: boxButton
            width: timeButton.width; height: dayButton.height
            cellWidth: root.boxWidth; cellHeight: root.boxHeight
            anchors.top : allButton.bottom
            anchors.left : allButton.right

            clip: true
            model: 8*24
            interactive: false

            delegate : Box {
                id: rect
                visible: (index < 7*24)
                width: root.boxWidth; height: root.boxHeight;

                topLineWidth: contains || pressed ? 2 : 0.5
                bottomLineWidth: contains || pressed ? 2 : 0.5
                leftLineWidth: contains || pressed ? 2 : 0.5
                rightLineWidth: contains || pressed ? 2 : 0.5

                property string dragColor: guide.checkMode ? WisenetGui.color_schedule_onDrag : WisenetGui.color_schedule_offDrag
                color: dragged ? dragColor : scheduleOn ? WisenetGui.color_primary_dark : WisenetGui.contrast_08_dark_grey
                borderColor: contains ? WisenetGui.color_yellow : pressed ? WisenetGui.contrast_02_light_grey : WisenetGui.contrast_07_grey

                property bool dragged: false // 마우스가 드래그한 영역인지
                property bool checked: false // 현재 이 박스가 체크된 상태인지
                property bool contains: false // 현재 이 박스 영역 안에 마우스가 위치하는지
                property bool pressed: false  // 이 박스 영역을 클릭했는지
                property bool scheduleOn: false // 이 박스의 스케줄이 on 되었는지

                MouseArea {
                    id: boxMouseArea
                    anchors.fill: parent
                    propagateComposedEvents: true
                    onClicked:{
                            rect.checked = guide.checkMode
                            rect.scheduleOn = guide.checkMode
                    }
                }
            }
        }

        MouseArea {
            id: boxArea

            width: boxButton.width; height: boxButton.height;

            anchors.bottom : parent.bottom
            anchors.right : parent.right

            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            propagateComposedEvents: true

            // 처음 클릭되었을 때의 마우스 좌표값을 저장하는 변수
            property int firstClickedX;
            property int firstClickedY;

            // 마우스 커서가 가리키는 박스의 index를 저장
            property int currentBoxX
            property int currentBoxY

            // 클릭되었을 때의 박스의 index를 저장하는 변수
            property int fx
            property int fy

            // release 전 가리키던 박스의 index를 저장하는 변수
            property int dx
            property int dy

            // 마우스를 누른 상태에서 마우스를 움직였을 때 바로 직전의 박스 index를 저장하는 변수
            property int pdx : 2
            property int pdy : 2

            // for문
            property int i
            property int j

            // 현재 드래그 영역 안에 있는 박스의 check 값을 수정
            function boxPropertyChange(fx, fy, dx, dy, task) {
                var by = Math.max(fy,dy)
                var bx = Math.max(fx,dx)

                for(j=Math.min(fy,dy); j<=by; j++) {
                    for(i=Math.min(fx,dx); i<=bx; i++) {
                        switch (task) {
                        case 1: // restore
                        {
                            boxButton.itemAtIndex(24*j +i).dragged = false
                            boxButton.itemAtIndex(24*j +i).checked = boxButton.itemAtIndex(24*j +i).scheduleOn
                            break
                        }
                        case 2: // check
                        {
                            boxButton.itemAtIndex(24*j +i).dragged = true
                            boxButton.itemAtIndex(24*j +i).checked = guide.checkMode
                            break
                        }
                        }
                    }
                }
            }

            onPositionChanged: {
                var xPoint = mouseX
                var yPoint = mouseY

                var mouseP = Qt.point(mouseX, mouseY)
                var lastClicked = boxButton.mapToItem(boxButton, mouseP)

                // 이전에 마우스를 contain했던 box의 contains를 false로 설정
                if (-1 < currentBoxX && currentBoxX < 24 && -1 < currentBoxY && currentBoxY < 8)
                    boxButton.itemAtIndex(24*currentBoxY +currentBoxX).contains = false

                // 박스 색 변경 코드
                currentBoxX = Math.floor(lastClicked.x/root.boxWidth)
                currentBoxY = Math.floor(lastClicked.y/root.boxHeight)

                // 마우스가 영역을 벗어났을 때 처리하는 코드
                if(mouseX < 0) {
                    xPoint = 0
                    lastClicked.x = 0
                }
                else if(boxArea.width < mouseX) {
                    xPoint = boxArea.width
                    lastClicked.x = boxArea.width - root.boxWidth
                }

                if(mouseY < 0) {
                    yPoint = 0
                    lastClicked.y = 0
                }
                else if (boxArea.height < mouseY) {
                    yPoint = boxArea.height
                    lastClicked.y = boxArea.height - root.boxHeight
                }

                // 마우스가 처음 눌린 박스의 인덱스 얻기
                fx  = Math.floor(firstClickedX/root.boxWidth)
                fy = Math.floor(firstClickedY/root.boxHeight)

                // 마우스가 눌러진 상태로 마우스 위치가 움직였을 때, 변경된 위치 값을 구해서 박스의 색깔을 변경함.
                if (pressed===true){
                    dx = Math.floor(lastClicked.x/root.boxWidth)
                    dy = Math.floor(lastClicked.y/root.boxHeight)

                    boxPropertyChange(fx, fy, pdx, pdy, 1) // restore
                    boxPropertyChange(fx, fy, dx, dy, 2)  // check

                    // positionChanged 함수 마지막에 pdx, pdy 값을 업데이트
                    pdx = dx
                    pdy = dy

                }

                // 마우스가 움직인 곳의 박스의 contain 값을 true로 변경
                if (-1 < currentBoxX && currentBoxX < 24 && -1 < currentBoxY && currentBoxY < 8) {
                    boxButton.itemAtIndex(24*currentBoxY +currentBoxX).contains = true
                    //currentBoxX = mouseX
                    //currentBoxY = mouseY
                }
            }

            onPressed: {
                var mouseP = Qt.point(mouse.x, mouse.y)
                var mapped = boxButton.mapToItem(boxButton, mouseP)

                firstClickedX = mapped.x
                firstClickedY = mapped.y

                // 처음으로 클릭되었던 박스의 색 변경을 위해 pressed를 true로 설정
                boxButton.itemAt(firstClickedX, firstClickedY).pressed = true
            }

            onReleased: {
                // 처음으로 클릭되었던 박스의 색깔 변경을 위해 pressed를 false로 설정
                boxButton.itemAt(firstClickedX, firstClickedY).pressed = false

                // when mouse is released, set pdx, pdy to 2
                pdx = 2
                pdy = 2

                for (var i=0; i<8*24; i++){
                    boxButton.itemAtIndex(i).dragged = false
                    boxButton.itemAtIndex(i).scheduleOn = boxButton.itemAtIndex(i).checked
                }
            }

            onExited: {
                // 마우스가 MouseArea를 벗어나면 마지막으로 마우스를 포함하고 있던 박스의 contains를 false로
                if (-1 < currentBoxX && currentBoxX < 24 && -1 < currentBoxY && currentBoxY < 8)
                    boxButton.itemAtIndex(24*currentBoxY +currentBoxX).contains = false
            }
        }
    }
}

