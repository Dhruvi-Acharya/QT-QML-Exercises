#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class HealthMonitoringParentWindow : public QObject
{
    Q_OBJECT
public:
    explicit HealthMonitoringParentWindow(QObject *parent = nullptr);

signals:
    void healthMonitoringView_SetVisible(bool visible);

private:
    QQmlApplicationEngine engine;
};
