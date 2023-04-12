import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.5
import Qt.labs.platform 1.1
import WisenetStyle 1.0
import WisenetLanguage 1.0

Item {
    id: auditLogItem


    Rectangle{
        id: auditLogBackgroud
        anchors.fill: parent
        color: WisenetGui.contrast_11_bg

        Rectangle{

            id: auditLogPage
            anchors.fill: parent
            anchors.leftMargin: 24
            anchors.topMargin: 20
            anchors.rightMargin: 24

            color: WisenetGui.transparent

            LogTitleAndSearchView{
                id: auditLogTitleAndSearchView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 94

                Component.onCompleted: {
                    changeTitle(WisenetLinguist.auditLog)
                }

                onSearchButonClicked: {
                    auditLogResultView.search(auditLogTitleAndSearchView.fromDate(),auditLogTitleAndSearchView.toDate())
                }
            }

            //List
            AuditLogResultView {
                id: auditLogResultView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: auditLogTitleAndSearchView.bottom
                anchors.bottom: parent.bottom
                backgroundColor: WisenetGui.contrast_11_bg
                tableBorderColor: WisenetGui.contrast_08_dark_grey
                tableHeaderBackgroundColor: WisenetGui.contrast_09_dark
                tableLowBorderColor: WisenetGui.contrast_09_dark

                Component.onCompleted: {
                    useExportButton(true)
                }

                onExportButtonClicked: {
                    console.debug("Export Button clicked")
                    var file = "file:///AuditLog_";
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
            auditLogResultView.tableModel.exportCSV(filePathConveter.getPathByUrl(folder), filePathConveter.getFileNameByUrl(file))
        }
    }

    onVisibleChanged: {
        if(visible){
            auditLogTitleAndSearchView.reset()
            auditLogResultView.search(auditLogTitleAndSearchView.fromDate(),auditLogTitleAndSearchView.toDate())
        }
    }
}
