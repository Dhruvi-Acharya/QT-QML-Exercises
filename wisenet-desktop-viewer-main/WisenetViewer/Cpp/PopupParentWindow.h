#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class PopupParentWindow : public QObject
{
    Q_OBJECT
public:
    explicit PopupParentWindow(QObject *parent = nullptr);

public slots:
    void ApplyLanguage();

signals:
    void setupMainView_SetSetupVisible(bool visible);
    void setupMainView_GotoHome();
    void setupMainView_GotoSetup(int menu = 0);
    void setupMainView_SelectDeviceList(QStringList channels);
    void setupMainView_SelectDeviceListChannel(QStringList channels);
    void setupMainView_SearchEventLog();
    void setupMainView_SelectAddDevice();

    void addNewSequence();
    void editSequence(QString displayName, QString itemUuid);
    void openSequenceAddCancelDialog();

    // Initialize device credential view
    void openInitializeDeviceCredentialView();

    // P2P register view
    void openP2pView();

    // Web page add view
    void webPageAddView_Open(QString title);

    // Export progress view
    void exportProgressView_Open();
    void exportProgressView_ForceClose();

    void closeAll();

private:
    QQmlApplicationEngine engine;
};
