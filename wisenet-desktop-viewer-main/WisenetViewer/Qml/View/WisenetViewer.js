var itemComponent = Qt.createComponent("qrc:/ViewingGrid.qml");

function createItem()
{
    if (itemComponent.status === Component.Ready) {
        var viewingGrid = itemComponent.createObject(wisenetViewerMainWindow,
                                                 {
                                                     "focus" : true,
                                                     "anchors.fill": Qt.binding(function(){return wisenetViewerMainWindow}),
                                                     "viewX" : Qt.binding(function(){return monitoringView.viewX}),
                                                     "viewY" : Qt.binding(function(){return monitoringView.viewY}),
                                                     "viewWidth" : Qt.binding(function(){return monitoringView.viewWidth}),
                                                     "viewHeight" : Qt.binding(function(){return monitoringView.viewHeight})
                                                 }
                                                 );
        return viewingGrid;
    }
    else if (itemComponent.status === Component.Error) {
        console.log("itemComponent status is not ready!!");
        return null;
    }
}
