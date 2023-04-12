import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15
import WisenetMediaFramework 1.0
import "qrc:/WisenetStyle/"

Item {
    id: gridItemBase

    /* 뷰모델 바인딩 */
    readonly property int  column: layoutItemModel.column
    readonly property int  row: layoutItemModel.row
    readonly property int  columnCount: layoutItemModel.columnCount
    readonly property int  rowCount: layoutItemModel.rowCount
    readonly property int  itemType: layoutItemModel.itemType // 아이템 타입
    readonly property string  itemId: layoutItemModel.itemId
    readonly property bool isNull: layoutItemModel.itemType === MediaLayoutItemViewModel.NullItem
    readonly property bool isCamera: layoutItemModel.itemType === MediaLayoutItemViewModel.Camera
    readonly property bool isWebpage: layoutItemModel.itemType === MediaLayoutItemViewModel.Webpage
    readonly property bool isLocalResource: layoutItemModel.itemType === MediaLayoutItemViewModel.LocalResource
    readonly property bool isZoomTarget: layoutItemModel.itemType === MediaLayoutItemViewModel.ZoomTarget
    readonly property bool isImage: layoutItemModel.itemType === MediaLayoutItemViewModel.Image
    readonly property bool isBackground: layoutItemModel.imageBackground
    property bool   isVideoItem: false

    property bool   hasPlayer: false
    property bool   mediaSeekable: false
    property var    mediaPlayPosition: 0

    /* 그리드뷰에서 바인딩되는 프로퍼티들 */
    property real   gridX : 0
    property real   gridY : 0
    property real   gridCellWidth: 0
    property real   gridCellHeight: 0
    property real   gridViewX: 0
    property real   gridViewY: 0
    property real   gridViewWidth: 0
    property real   gridViewHeight: 0

    // 상단 OSD 항시 표시 ON/OFF 옵션
    property bool   itemNameAlwaysVisible: false

    // editable 여부 (잠금 및 공유가 아닌 경우 true, 뷰잉그리드에서 바인딩)
    property bool   isEditable: true

    property bool   isFocusedWindow: false // 현재 뷰잉그리드가 포커스된 윈도우에 있는지
    property bool   focusedTarget: false // 현재 포커스된 채널인지 (grid상에 유일함)
    property bool   selected: false // 현재 선택된 채널인지 (이동을 위해서)
    property bool   positionChanging: false
    property bool   fakeUpdaing: false
    property real   tempX: 0
    property real   tempY: 0
    property real   tempW: 0
    property real   tempH: 0

    property real   cellX: (column*gridCellWidth) + gridX
    property real   cellY: (row*gridCellHeight) + gridY
    property real   cellW: gridCellWidth*columnCount
    property real   cellH: gridCellHeight*rowCount

    property bool   mouseOver: false


    // 이동, 크기 조정시 타겟 영역을 그려주는 부분(공통)
    property int  targetColumn : 0
    property int  targetRow : 0
    property int  targetColumnCount : 1
    property int  targetRowCount : 1

    property alias  itemModel: layoutItemModel
    property real   posAnimDuration: 150
    property real   contentPadding : 1

    // Sequence item 여부
    property bool isSequenceItem : false


    width: positionChanging ? tempW : cellW
    height: positionChanging ? tempH : cellH
    x: positionChanging ? tempX : cellX
    y: positionChanging ? tempY : cellY
    z: positionChanging ? 2 : (isBackground ? 0 : 1)
    //z: (positionChanging||mouseOver) ? 2 : 1
    opacity: 0.0

    property color backgroundColor : WisenetGui.contrast_10_dark

//    scale: mouseOver ? 1.1 : 1.0
//    Behavior on scale {
//        NumberAnimation {duration: 60}
//    }

    signal itemClose(Item item);
    signal itemMouseMoved();


    // defualtItemViewModel
    MediaLayoutItemViewModel{
        id: layoutItemModel
        itemType: MediaLayoutItemViewModel.NullItem
    }

    // 선택 사각형
    Rectangle {
        id: selectedRect
        anchors.fill: parent
        visible: parent.selected
        color: WisenetGui.color_primary_dark
        opacity: 0.2
        z:3
    }

    // 이동시 타겟 사각형
    Rectangle {
        id: targetRect
        color: WisenetGui.color_primary_dark
        opacity: 0.3
        visible: false
        width: 0
        height: 0
        x: 0
        y: 0
        z: 0
    }

    onFocusedTargetChanged: {
        if (!focusedTarget) {
            focusedRect.highlight = false;
        }
        else {
            focusedRect.highlight = true;
        }
    }

    function updateFocusedRect(show)
    {
        if (focusedTarget) {
            focusedRect.highlight = show;
        }
    }

    // 포커스 사각형
    Rectangle {
        id: focusedRect
        anchors.fill: parent
        visible: gridItemBase.focusedTarget
        color: "transparent"
        border.color: highlight ? WisenetGui.color_primary : WisenetGui.contrast_07_grey
        border.width: 2
        z:4
        property bool highlight: true
    }


    ParallelAnimation {
        id: posAnim
        NumberAnimation {
            target: gridItemBase
            properties: "tempX"
            to: gridItemBase.cellX
            duration: gridItemBase.posAnimDuration
        }
        NumberAnimation {
            target: gridItemBase
            properties: "tempY"
            to: gridItemBase.cellY
            duration: gridItemBase.posAnimDuration
        }
        NumberAnimation {
            target: gridItemBase
            properties: "tempW"
            to: gridItemBase.cellW
            duration: gridItemBase.posAnimDuration
        }
        NumberAnimation {
            target: gridItemBase
            properties: "tempH"
            to: gridItemBase.cellH
            duration: gridItemBase.posAnimDuration
        }
        onStopped: {
            gridItemBase.positionChanging = false;
        }
    }

    function setItem(itemType, column, row, columnCount, rowCount)
    {
        layoutItemModel.itemType = itemType;
        layoutItemModel.column = column;
        layoutItemModel.row = row;
        layoutItemModel.columnCount = columnCount;
        layoutItemModel.rowCount = rowCount;
    }

    function setFillMode(mode)
    {
        layoutItemModel.itemFillMode = mode;
    }

    function setProfile(profile)
    {
        layoutItemModel.itemProfile = profile;
    }

    function setVideoStatus(vstatus)
    {
        layoutItemModel.videoStatus = vstatus;
    }

    function setCamearaParam(deviceId, channelId)
    {
        layoutItemModel.deviceId = deviceId;
        layoutItemModel.channelId = channelId;
    }
    function setLocalResourceParam(path)
    {
        layoutItemModel.localResource = path;
    }
    function setWebpageParam(webPageId)
    {
        layoutItemModel.webPageId = webPageId;
    }

    function setFakeUpdating(fakeUpdate)
    {
        if (fakeUpdate !== fakeUpdaing) {
            fakeUpdaing = fakeUpdate;
            if (!fakeUpdaing && positionChanging) {
                opacity = 0.5;
                targetRect.visible = true;
            }
        }
    }

    function setPositionChanging(enabled)
    {
        if (enabled !== positionChanging) {
            if (enabled) {
                tempX = cellX;
                tempY = cellY;
                tempW = cellW;
                tempH = cellH;

                targetColumn = column;
                targetRow = row;
                targetColumnCount = columnCount;
                targetRowCount = rowCount;

                if (!fakeUpdaing) {
                    opacity = 0.5;
                    targetRect.visible = true;
                }

                positionChanging = enabled;
                return true;
            }
            // 종료하는 경우 tempX와 realX사이에 애니메이션 동작을 수행한후 값을 변경한다.
            else {
                var isChanged =
                        (layoutItemModel.column !== targetColumn) ||
                        (layoutItemModel.row !== targetRow) ||
                        (layoutItemModel.columnCount !== targetColumnCount) ||
                        (layoutItemModel.rowCount !== targetRowCount);

                layoutItemModel.column = targetColumn;
                layoutItemModel.row = targetRow;
                layoutItemModel.columnCount = targetColumnCount;
                layoutItemModel.rowCount = targetRowCount;
                opacity = 1.0;
                targetRect.visible = false;
                fakeUpdaing = false;

                posAnim.restart();
                return isChanged;
            }
        }
        return false;
    }


    // 아이템이 다른 곳으로 이동해야 할 때 호출 (중첩될 경우 후 이동동작)
    function movePosition(tColumn, tColumnCount, tRow, tRowCount)
    {
        tempX = cellX;
        tempY = cellY;
        tempW = cellW;
        tempH = cellH;
        layoutItemModel.column = tColumn;
        layoutItemModel.row = tRow;
        layoutItemModel.columnCount = tColumnCount;
        layoutItemModel.rowCount = tRowCount;
        targetRect.visible = false;
        opacity = 1.0
        positionChanging = true;
        posAnim.restart();
    }

    function mapToTargetItemPos()
    {
        if (positionChanging) {
            targetRect.x = (targetColumn * gridCellWidth) + gridX - x;
            targetRect.y = (targetRow * gridCellHeight) + gridY - y;
            targetRect.width = targetColumnCount * gridCellWidth;
            targetRect.height = targetRowCount * gridCellHeight;
        }
    }
}
