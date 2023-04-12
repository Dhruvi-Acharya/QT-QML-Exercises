#include "HealthMonitoringParentWindow.h"
#include "LogoManager.h"
#include "ViewModel/HealthMonitoring/HealthMonitoringViewModel.h"

HealthMonitoringParentWindow::HealthMonitoringParentWindow(QObject *parent)
    : QObject(parent)
{    
    engine.addImportPath("qrc:/");

    engine.rootContext()->setContextProperty("logoManager", LogoManager::getInstance());
    engine.rootContext()->setContextProperty("healthMonitoringViewModel", HealthMonitoringViewModel::getInstance());
    engine.rootContext()->setContextProperty("healthMonitoringService", this);

    engine.load(QUrl(QStringLiteral("qrc:/Qml/View/HealthMonitoringParent.qml")));
}
