
var viewingGrid;
var component = Qt.createComponent("videoItem0.qml");
var allItems = [];
var selectedItems = [];
var focusedItem = null;
var resizingItem = null;

function init(view)
{
    viewingGrid = view;
}

function cleanUp()
{
    allItems.forEach(function(item) {
        if (item != null)
            item.destroy();
    });
    allItems = [];
    selectedItems = [];
    focusedItem = null;
    resizingItem = null;
}

function addNewItem(itemView)
{
    var xIndex = allItems.length % 10;
    var yIndex = parseInt(allItems.length / 10);

    var viewItem = component.createObject(itemView,
                                          {
                                              gridX: xIndex,
                                              gridY: yIndex,
                                              gridW:1, gridH:1,
                                          }
                                          );

    if (allItems.length === 0)
        setFocusItem(viewItem);
    allItems.push(viewItem);
}

function setFocusItem(item)
{
    if (!item.focused) {
        if (focusedItem != null) {
            focusedItem.focused = false;
        }
        focusedItem = item;
        focusedItem.focused = true;
    }
}



function clearSelectedItems()
{
    selectedItems.forEach(function(item) {
        if (item != null)
            item.multipleSelected = false;
    });
    selectedItems = [];
}

function rectCollisionTest(r1, r2)
{
    if (r1.x < r2.x+r2.width && r1.x+r1.width > r2.x &&
        r1.y < r2.y+r2.height && r1.y+r1.height > r2.y)
        return true;
    return false;
}

function updateSelectedItems(sr)
{
    selectedItems = [];
    for (var i = 0 ; i < allItems.length ; i ++) {
        var item = allItems[i];
        if (rectCollisionTest(sr, item)) {
            item.multipleSelected = true;
            selectedItems.push(item);
        }
        else {
            item.multipleSelected = false;
        }
    }
}

function checkFocusItem(mx, my)
{
    for (var i = 0 ; i < allItems.length ; i ++) {
        if (allItems[i] != null) {
            if (allItems[i].isMouseIn(mx, my)) {
                setFocusItem(allItems[i]);
                return true;
            }
        }
    }
    return false;
}

function getGridX(mx)
{
    var ex = (mx - viewingGrid.gridPositionX) / viewingGrid.gridPixelWidth;
    if (ex < 0) ex = -1 + ex;
    return parseInt(ex);
}
function getGridY(my)
{
    var ey = (my - viewingGrid.gridPositionY) / viewingGrid.gridPixelHeight;
    if (ey < 0) ey = -1 + ey;
    return parseInt(ey);
}

var itemMovingX = 0
var itemMovingY = 0
var itemMoving = false
function readyToMoveFocusedItem(mx, my, targetRect)
{
    if (focusedItem != null) {
        itemMoving = true;
        itemMovingX = mx;
        itemMovingY = my;

        focusedItem.movingX = focusedItem.x;
        focusedItem.movingY = focusedItem.y;
        focusedItem.movingW = focusedItem.width;
        focusedItem.movingH = focusedItem.height;

        targetRect.gx = focusedItem.gridX;
        targetRect.gy = focusedItem.gridY;
        targetRect.gw = focusedItem.gridW;
        targetRect.gh = focusedItem.gridH;
    }
}

function getCenterX(lX)
{
    return (lX*viewingGrid.gridPixelWidth)+viewingGrid.gridPositionX + viewingGrid.gridPixelWidth/2;
}


function getCenterY(lY)
{
    return (lY*viewingGrid.gridPixelHeight)+viewingGrid.gridPositionY + viewingGrid.gridPixelHeight/2;
}

function startToMoveFocusedItem(mx, my, targetRect)
{
    if (itemMoving && focusedItem != null) {
        focusedItem.moving = true;
        focusedItem.movingX += (mx - itemMovingX);
        focusedItem.movingY += (my - itemMovingY);

        itemMovingX = mx;
        itemMovingY = my;

        var lX = getGridX(focusedItem.x);
        var rX = getGridX(focusedItem.x + focusedItem.width);
        var tY = getGridY(focusedItem.y);
        var bY = getGridY(focusedItem.y + focusedItem.height);

        var lxCenter = getCenterX(lX);
        var tyCenter = getCenterY(tY);

        var rxCenter = (lX === rX) ? lxCenter : getCenterX(rX);
        var byCenter = (tY === bY) ? tyCenter : getCenterY(bY);

        if (focusedItem.x < lxCenter)
            targetRect.gx = lX;
        else if (focusedItem.x+focusedItem.width > rxCenter)
            targetRect.gx = rX - targetRect.gw + 1;

        if (focusedItem.y < tyCenter)
            targetRect.gy = tY;
        else if (focusedItem.y+focusedItem.height > byCenter)
            targetRect.gy = bY - targetRect.gh + 1;

        //console.log('m=', lX, rX, tY, bY, targetRect.gx, targetRect.gy, focusedItem.x, focusedItem.y, lxCenter, rxCenter)
        targetRect.visible = true;
    }
}

function stopToMoveFocusedItem(mx, my, targetRect)
{
    if (itemMoving && focusedItem != null) {
        itemMovingX = 0;
        itemMovingY = 0;
        itemMoving = false;
        targetRect.visible = false;

        var unchanged = (focusedItem.gridX === targetRect.gx &&
                         focusedItem.gridY === targetRect.gy);

        if (!unchanged) {
            focusedItem.gridX = targetRect.gx;
            focusedItem.gridY = targetRect.gy;
        }

        focusedItem.moving = false;
        return !unchanged;
    }
    return false;
}

function getResizingTargetItem(mx, my, limit)
{
    for (var i = 0 ; i < allItems.length ; i ++) {
        if (allItems[i] != null) {
            if (allItems[i].isMouseResizable(mx, my, limit)) {
                resizingItem = allItems[i];
                return resizingItem;
            }
        }
    }
    return  null;
}

var itemResizingX = 0
var itemResizingY = 0
var itemResizing = false
function readyToResizeItem(mx, my, targetRect)
{
    if (resizingItem != null) {
        itemResizing = true;
        itemResizingX = mx;
        itemResizingY = my;

        resizingItem.movingX = resizingItem.x;
        resizingItem.movingY = resizingItem.y;
        resizingItem.movingW = resizingItem.width;
        resizingItem.movingH = resizingItem.height;
        resizingItem.moving = true;

        targetRect.gx = resizingItem.gridX;
        targetRect.gy = resizingItem.gridY;
        targetRect.gw = resizingItem.gridW;
        targetRect.gh = resizingItem.gridH;
    }
}

function startToResizeItem(mx, my, targetRect, xl, xr, yt, yb)
{
    if (itemResizing && resizingItem != null) {
        var diffX = mx - itemResizingX;
        var diffY = my - itemResizingY;

        if (xl) {
            resizingItem.movingX += diffX;
            resizingItem.movingW -= diffX;
        }
        if (yt) {
            resizingItem.movingY += diffY;
            resizingItem.movingH -= diffY;
        }
        if (xr) {
            resizingItem.movingW += diffX;
        }
        if (yb) {
            resizingItem.movingH += diffY;
        }

        itemResizingX = mx;
        itemResizingY = my;

        var limit = 2;
        var ltX = getGridX(resizingItem.x+limit);
        var ltY = getGridY(resizingItem.y+limit);
        var rbX = getGridX(resizingItem.x+resizingItem.width-limit);
        var rbY = getGridY(resizingItem.y+resizingItem.height-limit);

        targetRect.gx = ltX;
        targetRect.gy = ltY;
        targetRect.gw = rbX-ltX+1;
        targetRect.gh = rbY-ltY+1;
        targetRect.visible = true;
        //console.log('r=', ltX, ltY, rbX, rbY, targetRect.gx, targetRect.gy, targetRect.gw, targetRect.gh)
    }
}

function stopToResizeItem(mx, my, targetRect)
{
    if (itemResizing && resizingItem != null) {
        itemResizingX = 0;
        itemResizingY = 0;
        itemResizing = false;
        targetRect.visible = false;

        resizingItem.gridX = targetRect.gx
        resizingItem.gridY = targetRect.gy
        resizingItem.gridW = targetRect.gw
        resizingItem.gridH = targetRect.gh
        resizingItem.moving = false;
        resizingItem = null;
        return true;
    }
    return false;
}


function calculateFitInView(left, top, width, height)
{
    var lx = 0, rx = 1, ty = 0, by = 1;

    for (var i = 0 ; i < allItems.length ; i ++) {
        if (allItems[i] != null) {
            var item = allItems[i];
            if (item.gridX < lx)
                lx = item.gridX;
            if (item.gridX + item.gridW > rx)
                rx = item.gridX + item.gridW;
            if (item.gridY < ty)
                ty = item.gridY;
            if (item.gridY + item.gridH > by)
                by = item.gridY + item.gridH;
        }
    }

    console.log('f=', lx, rx, ty, by);
    var xCount = rx-lx;
    var yCount = by-ty;

    var calculatedCellWidth = 0;
    var calculatedCellHeight = 0;
    var dx = 0, dy = 0;
    var viewRatio = width / height;
    var cellRatio = (xCount*viewingGrid.gridCellSizeW) / (yCount*viewingGrid.gridCellSizeH);

    if (viewRatio <= cellRatio) {
        calculatedCellWidth = width;
        calculatedCellHeight = width / (xCount*viewingGrid.gridCellSizeW)  * (yCount*viewingGrid.gridCellSizeH);
        dy = (height - calculatedCellHeight) / 2;
    }
    else {
        calculatedCellWidth = height / (yCount*viewingGrid.gridCellSizeH) * (xCount*viewingGrid.gridCellSizeW);
        calculatedCellHeight = height;
        dx = (width - calculatedCellWidth) / 2;
    }

    var scale = calculatedCellWidth / (xCount*viewingGrid.gridCellSizeW);

    viewingGrid.gridZoomAnimationScale = scale;
    viewingGrid.gridZoomAnimationX = left+dx-(lx*scale*viewingGrid.gridCellSizeW);
    viewingGrid.gridZoomAnimationY = top+dy-(ty*scale*viewingGrid.gridCellSizeH);

}
