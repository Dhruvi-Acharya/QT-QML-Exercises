import QtQuick 2.15
import "Popup/HealthMonitoring"

Item {

    Connections{
        target: healthMonitoringService

        onHealthMonitoringView_SetVisible:{
            if(visible)
                healthMonitoringViewModel.initialize()
            healthMonitoringView.visible = visible
        }

    }

    HealthMonitoringView{
        id: healthMonitoringView

        visible: false
    }
}
