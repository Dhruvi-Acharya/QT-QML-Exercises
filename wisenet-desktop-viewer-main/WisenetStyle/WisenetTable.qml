import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0

Item{
    id: mainItem
    property bool popupDepth : false
    property color borderColor: WisenetGui.contrast_07_grey
    property color headerBackgroundColor: WisenetGui.contrast_09_dark

    property alias columnWidthProvider: tableView.columnWidthProvider

    property var cellDelegate: null
    property ListModel columns : ListModel{}
    property var tableModel : null
    property int headerHeight : 36;
    property alias verticalScrollPosition : verticalScrollbar.position
    property var tableFooterY: tableView.contentHeight < tableView.height ?
                                   tableView.contentHeight+ tableView.y + headerHeight
                                   : tableView.height + tableView.y + headerHeight
    property int tableWidth: tableView.width
    property int tableContentWidth : tableView.contentWidth
    property int tableContentY : tableView.contentY
    property var lastSortIndex : 0
    property bool allCheckedStatus: false


    function initializeSortState(sortIndex){

        lastSortIndex = sortIndex

        for(var i = 0; i < headingsRepeater.count; i++){

            if(sortIndex === i){
                headingsRepeater.itemAt(i).sortIndicator = 1
            }else if(headingsRepeater.itemAt(i).sortIndicator !== 3){
                headingsRepeater.itemAt(i).sortIndicator = 0
            }
        }
    }


    function tableForceLayout(){ tableView.forceLayout();}

    signal tableCompleted()
    signal filterImageClicked(var index, var x, var y)
    signal splitterDragged()

    property var tableContainer: tableContainer
    property var headingsRepeater: headingsRepeater
    property var tableViewY: tableView.y

    property alias contentColor: tableContainer.color
    signal allChecked(bool checked)

    Rectangle {
        id: tableContainer
        color : WisenetGui.transparent
        anchors.fill: parent

        Component.onCompleted: tableCompleted()

        Rectangle{
            id: headingsFlickTopBorder
            anchors.left: tableContainer.left
            anchors.top: tableContainer.top
            width: tableContainer.width
            height: 1
            color: borderColor
        }

        Rectangle{
            id:headingsFlckBackground
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headingsFlickTopBorder.bottom
            anchors.bottom: headingsFlickBottomBorder.top
            color: headerBackgroundColor
        }

        Flickable {
            id: headingsFlick
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: headerHeight

            anchors.topMargin: 1
            //anchors.leftMargin: 1
            //anchors.rightMargin: 1

            contentWidth: tableView.contentWidth
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Row{
                id: headings
                //spacing: 1

                Repeater {
                    id: headingsRepeater
                    model: columns.count

                    Rectangle {
                        id: columnRect
                        width: splitter.x + splitter.width
                        height: headingsFlick.height
                        color: headerBackgroundColor


                        property int filterIndicator : columns.get(index).filter ? columns.get(index).filter : 0;
                        property int sortIndicator: columns.get(index).sort;

                        WisenetCheckBox2{
                            id: columnCheckBox
                            z:1
                            anchors.centerIn: parent
                            visible: columns.get(index).title === "Check" ? true : false
                            checked: mainItem.allCheckedStatus
                            onClicked: {
                                 console.log("click tableview check", checked)

                                mainItem.allCheckedStatus = !mainItem.allCheckedStatus
                                console.log("click tableview all check after ", checked, mainItem.allCheckedStatus)
                                mainItem.allChecked(checked)
                            }
                        }

                        MouseArea {
                            id: selectArea
                            anchors.fill: parent
                            anchors.rightMargin: 5
                            hoverEnabled: true

                            acceptedButtons: Qt.LeftButton
                            onClicked: {
                                if(parent.sortIndicator !== 3) {

                                    //console.debug("lastSortIndex is ", parent.parent.lastSortIndex);
                                    //console.debug("sort is ", parent.sortIndicator );

                                    if(mainItem.lastSortIndex !== index) {
                                        headingsRepeater.itemAt(mainItem.lastSortIndex).sortIndicator = 0
                                    }

                                    if(parent.sortIndicator === 1){
                                        parent.sortIndicator = 2;
                                        verticalScrollbar.position = 0;
                                        tableModel.sort(index,Qt.AscendingOrder);
                                    } else {
                                        parent.sortIndicator = 1;
                                        verticalScrollbar.position = 0;
                                        tableModel.sort(index,Qt.DescendingOrder);
                                    }

                                    mainItem.lastSortIndex = index;
                                }
                            }
                            onEntered: {
                                columnRect.color = (mainItem.popupDepth? WisenetGui.contrast_07_grey: WisenetGui.contrast_08_dark_grey)
                            }
                            onExited: {
                               columnRect.color = (mainItem.popupDepth? WisenetGui.contrast_08_dark_grey: WisenetGui.contrast_09_dark)
                            }
                        }

                        Rectangle{
                            id: columnContentRectangle
                            width: splitter.x
                            height: parent.height
                            visible: ((columns.get(index).title === "Check" ? false : true) && columns.get(index).columnWidth > 0)
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: WisenetGui.transparent
                            clip: true

                            Row{
                                anchors.centerIn: parent
                                spacing: 1

                            Button{
                                id: filterButton
                                width: 11
                                height: 12
                                visible: (columns.get(index).title !== "Check" && (columnRect.filterIndicator === 1 || columnRect.filterIndicator === 2) && (columns.get(index).columnWidth > 0))
                                anchors.verticalCenter: parent.verticalCenter
                                hoverEnabled: true

                                background: Rectangle{
                                    color: WisenetGui.transparent
                                }
                                Image{
                                    anchors.fill: parent
                                    sourceSize: Qt.size(width, height)
                                    source:{
                                        if(columnRect.filterIndicator === 1){
                                            if(filterButton.hovered){
                                                return "qrc:/WisenetStyle/Icon/Table/table-filter-hover.svg"
                                            }else if(filterButton.visible){
                                                return "qrc:/WisenetStyle/Icon/Table/table-filter-normal.svg"
                                            }else{
                                                return "qrc:/WisenetStyle/Icon/Table/table-filter-press.svg"
                                            }
                                        }else if(columnRect.filterIndicator === 2){
                                            if(filterButton.hovered){
                                                return "qrc:/WisenetStyle/Icon/Table/table-search-hover.svg"
                                            }else if(filterButton.visible){
                                                return "qrc:/WisenetStyle/Icon/Table/table-search-normal.svg"
                                            }else{
                                                return "qrc:/WisenetStyle/Icon/Table/table-search-press.svg"
                                            }
                                        }

                                        return ""
                                    }
                                }
                                onClicked: {
                                    filterImageClicked(index,columnRect.x, (columnRect.y + columnRect.height))
                                }
                            }

                            Text{
                                id: columnNameText
                                visible: ((columns.get(index).title === "Check" ? false : true) && columns.get(index).columnWidth > 0)
                                color: WisenetGui.contrast_02_light_grey
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: 12
                                text: {
                                    if(columnRect.sortIndicator === 1){
                                        return columns.get(index).title + " ↓"
                                    }else if(columnRect.sortIndicator === 2){
                                        return columns.get(index).title + " ↑"
                                    }else{
                                        return columns.get(index).title
                                    }
                                }
                                elide: Qt.ElideRight
                            }
                            }
                        }


                        Item {
                            id: splitter
                            x: columns.get(index).columnWidth - 5
                            width: 5
                            height: parent.height
                            visible: columns.get(index).splitter === 1 ? true : false

                            onXChanged: {
                                if(splitter.visible && x < 10){
                                    x = 10;
                                }
                            }

                            Rectangle{

                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter

                                width: 1
                                height: 14
                                color: WisenetGui.contrast_07_grey
                            }

                            HoverHandler{
                                cursorShape: Qt.SplitHCursor
                            }
                            DragHandler {
                                cursorShape: Qt.SplitHCursor
                                xAxis.enabled: true; yAxis.enabled: false
                                onActiveChanged:{
                                    if(active) splitterTimer.start()
                                    else splitterTimer.stop()
                                    splitterDragged()
                                }
                            }
                            Timer {
                                id: splitterTimer
                                interval: 16; repeat: true
                                onTriggered: {
                                    var width = 0;
                                    for(var i = 0; i < columns.count ; i++){
                                        width += headingsRepeater.itemAt(i).width;
                                    }
                                    tableView.contentWidth = width;
                                    tableView.forceLayout()
                                }
                            }
                        }
                    }
                }
            }

            onContentXChanged: {
                if(contentX >= 0 && (tableView.contentX !== contentX))
                    tableView.contentX = contentX
            }
        }

        Rectangle{
            id: headingsFlickBottomBorder
            anchors.left: tableContainer.left
            anchors.top: headingsFlick.bottom
            width: tableContainer.width
            height: 1
            color: borderColor
        }

        TableView {
            id: tableView
            columnSpacing: 0
            rowSpacing: 0
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: headingsFlickBottomBorder.bottom
            anchors.bottom: parent.bottom
            anchors.leftMargin: 1
            anchors.rightMargin: 1

            columnWidthProvider: function (column) {return headingsRepeater.itemAt(column).width}

            model : tableModel
            reuseItems : true

            ScrollBar.vertical: WisenetScrollBar {
                id: verticalScrollbar
            }

            ScrollBar.horizontal: WisenetScrollBar {
                id: horizontalScrollbar
                orientation: Qt.Horizontal
            }

            delegate: cellDelegate

            onContentXChanged: {
                if(contentX >= 0 && (headingsFlick.contentX !== contentX))
                    headingsFlick.contentX = contentX
            }
        }

        Rectangle{
            id: tableViewBottomBorder
            anchors.left: tableContainer.left
            anchors.top: tableView.bottom
            width: tableContainer.width
            height: 1
            color: borderColor
            visible: (mainItem.height > tableView.contentHeight)?false:true
        }

        //onWidthChanged: {
        //    var totalWidth = 0
        //
        //    if(headingsFlick.contentWidth < width){
        //        headingsRepeater.itemAt(headingsRepeater.count-1).width += (width - headingsFlick.contentWidth)
        //        tableForceLayout()
        //    }
        //}
    }
}
