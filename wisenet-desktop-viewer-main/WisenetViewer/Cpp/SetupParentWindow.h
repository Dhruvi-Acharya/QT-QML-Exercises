#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "PopupParentWindow.h"

class SetupParentWindow : public QObject
{
    Q_OBJECT
public:
    explicit SetupParentWindow(QObject *parent, PopupParentWindow* popupParent);

signals:
    void setupMainView_SetSetupVisible(bool visible);
    void setupMainView_GotoHome();
    void setupMainView_GotoSetup(int menu = 0);
    void setupMainView_SelectDeviceList(QStringList channels);
    void setupMainView_SelectDeviceListChannel(QStringList channels);
    void setupMainView_SearchEventLog();
    void setupMainView_SelectAddDevice();

    void setupMainView_openP2p();

    void closeAll();
    void initializeTree();
private:
    QQmlApplicationEngine engine;
};
