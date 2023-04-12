import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.5
import Qt.labs.platform 1.1
import WisenetStyle 1.0
import WisenetLanguage 1.0
import "qrc:/Qml/View/Popup/EventSearch"

Item{
    id: eventLogItem

    function search(){
        eventLogTitleAndSearchView.reset()
        var allDevice = true; var devices = [];var allEvent = true; var events = [];
        eventLogResultView.search(eventLogTitleAndSearchView.fromDate(),eventLogTitleAndSearchView.toDate(),allDevice,devices,allEvent,events)
    }

    Rectangle{
        id: eventLogBackgroud
        anchors.fill: parent
        color: WisenetGui.contrast_11_bg

        Rectangle{

            id: eventLogPage
            anchors.fill: parent
            anchors.leftMargin: 24
            anchors.topMargin: 20
            anchors.rightMargin: 24

            color: WisenetGui.transparent

            LogTitleAndSearchView{
                id: eventLogTitleAndSearchView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 94

                onSearchButonClicked: {
                    var allDevice = true; var devices = [];var allEvent = true; var events = [];
                    eventLogResultView.search(eventLogTitleAndSearchView.fromDate(),eventLogTitleAndSearchView.toDate(),allDevice,devices,allEvent,events)
                }
            }

            //List
            EventSearchResultView {
                id: eventLogResultView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: eventLogTitleAndSearchView.bottom
                anchors.bottom: parent.bottom
                backgroundColor: WisenetGui.contrast_11_bg
                tableBorderColor: WisenetGui.contrast_08_dark_grey
                tableLowBorderColor: WisenetGui.contrast_09_dark

                Component.onCompleted: {
                    useExportButton(true)
                }

                onExportButtonClicked: {
                    console.debug("Export Button clicked")
                    var file = "file:///EventLog_";
                    file += new Date().toLocaleString(Qt.locale(),"yyyy-MM-dd_h-mm-ss")
                    file += ".csv"
                    exportLogDialog.currentFile = file
                    exportLogDialog.open()
                }
            }
        }
    }

    FileDialog {
        id: exportLogDialog
        title: WisenetLinguist.exportButton
        acceptLabel: WisenetLinguist.exportButton
        rejectLabel: WisenetLinguist.cancel
        fileMode: FileDialog.SaveFile
        folder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
        //file: fileName
        nameFilters: [ "Comma-Separated Values (*.csv)" ]
        visible: false

        onAccepted: {
            //exportVideoModel.filePath = filePathConveter.getPathByUrl(folder)
            //exportVideoModel.fileName = filePathConveter.getFileNameByUrl(file)
            eventLogResultView.tableModel.exportCSV(filePathConveter.getPathByUrl(folder), filePathConveter.getFileNameByUrl(file))
        }
    }

    onVisibleChanged: {
        if(visible){
            search();
        }
    }

}


