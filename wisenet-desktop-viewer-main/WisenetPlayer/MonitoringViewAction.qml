import QtQuick 2.15
import QtQuick.Controls 2.5

Item {
    id: monitoringViewAction

    signal openMediaActionTriggered();
    signal autoFitActionTriggered();
    signal toggleFullScreenTriggred();

    // Actions for mediaControllerView
    Action {
        shortcut: "Space"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionPlayPause()
        }
    }

    Action {
        shortcut: "Ctrl+Left"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionBackward()
        }
    }

    Action {
        shortcut: "Ctrl+Right"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionForward()
        }
    }

    Action {
        shortcut: "Ctrl+Up"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionZoomIn()
        }
    }

    Action {
        shortcut: "+"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionZoomIn()
        }
    }

    Action {
        shortcut: "Ctrl+Down"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionZoomOut()
        }
    }

    Action {
        shortcut: "-"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionZoomOut()
        }
    }

    Action {
        shortcut: "z"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionPrevEvent()
        }
    }

    Action {
        shortcut: "x"
        onTriggered: {
            //console.log("MonitoringViewAction : ", shortcut)
            mediaControllerView.actionNextEvent()
        }
    }

    // 단축키 중복 시 예제 (현재 activeFocusItem을 포함하는 view로 명령 전달)
    /*
    Action {
        shortcut: "F1"
        onTriggered: {
            var item = activeFocusItem
            while(item != null) {
                if(item == leftView) {
                    console.log("Call some leftView function")
                    break
                }
                else if(item == rightView) {
                    console.log("Call some rightView function")
                    break
                }
                else if(item == viewingGridView) {
                    console.log("Call some viewingGridView function")
                    break
                }
                else if(item == mediaControllerView) {
                    console.log("Call some mediaControllerView function")
                    break
                }

                item = item.parent
            }
        }
    }
    */

    // 미디어 파일 열기
    Action {
        shortcut: "Ctrl+O"
        onTriggered: openMediaActionTriggered();
    }
    // AutoFit
    Action {
        shortcut: StandardKey.Refresh
        onTriggered: autoFitActionTriggered();
    }
    // 전체화면 모드
    Action {
        shortcut: StandardKey.FullScreen
        onTriggered: toggleFullScreenTriggred();
    }
}
