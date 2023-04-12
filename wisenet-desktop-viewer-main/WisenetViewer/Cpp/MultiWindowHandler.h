#pragma once

#include <memory>
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QUuid>
#include "MultiWindow.h"
#include "LanguageManager.h"
#include "../WisenetMediaFramework/ViewModel/DragItemListModel.h"
#include "QCoreServiceManager.h"
#include "PopupParentWindow.h"
#include "SearchParentWindow.h"
#include "SetupParentWindow.h"
#include "HealthMonitoringParentWindow.h"
#include "StatisticsClient.h"
#include "MaskingParentWindow.h"

class MultiWindow;

class MultiWindowHandler : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isSettingsOpened READ isSettingsOpened WRITE setIsSettingsOpened NOTIFY isSettingsOpenedChanged)
public:
    explicit MultiWindowHandler(QObject* parent = nullptr);
    virtual ~MultiWindowHandler();
    void NotifyLoginStatus(const bool arg);
    bool GetLoginStatus() { return m_loginStatus; }

    bool isSettingsOpened(){
        return m_isSettingsOpened;
    }

    void setIsSettingsOpened(bool isSettingsOpened){
        m_isSettingsOpened = isSettingsOpened;
        emit isSettingsOpenedChanged(isSettingsOpened);
    }

    Q_INVOKABLE int windowCount(){ return m_windowsKeyList.count(); }
    void loadLatestSetting(QList<QStringList>& layouts, QStringList& screens, QStringList& windowPositions,
                           QStringList& displayOsds, QStringList& displayVideoStatuses, QList<QStringList>& panelFlipList, QStringList& viewerModes,
                           QStringList& isLinkedMode, QStringList& isThumbnailMode, QStringList& isMultiTimelineMode, QStringList& timelineHeight);
    void saveLatestSetting();

    void makeSetupParent();

private:
    void connectSignals(MultiWindow* multiWindow);
public slots:
    // QML
    void initializeNewWindow(bool isFirst = true);
    void initializeNewWindowWithMediaOpen(int type, QString itemId, QString channelNumber = "");
    void initializeNewWindowWithItems(DragItemListModel* dragItemListModel);
    void initializeNewWindowWithGridItems(QVariantList items);
    void initializeNewWindowWithLayouts(QStringList layoutIds);
    void initializeNewWindowWithSequence(QString sequenceId, QString displayName);
    void destroyWindow(QString key);
    void exitProgram(bool restart = false);
    void switchWindow(QString key, bool toLeft);


    void coreServiceEventTriggered(QCoreServiceEventPtr event);

    // C++
    void Request(const WisenetViewerDefine::ViewerRequestSharedPtr&);

    // Setup
    void setupMainView_SetSetupVisible(bool visible);
    void setupMainView_GotoHome();
    void setupMainView_GotoSetup(int menu = 0);
    void setupMainView_SelectDeviceList(QStringList channels);
    void setupMainView_SelectDeviceListChannel(QStringList channels);
    void setupMainView_SearchEventLog();
    void setupMainView_SelectAddDevice();

    // Event
    void eventSearchView_SetVisible(bool visible);
    void eventSearchView_SetSearchMenu(int menu);
    void eventSearchView_ResetFilter();
    void eventSearchView_SetChannelFilter(QStringList channelGuids);
    void eventSearchView_SetDateFilter(QVariant start, QVariant end);
    void eventSearchView_SearchByFilter();
    void eventSearchPopupOpenShortcutClicked(int menu);

    // Health monitoring
    void healthMonitoringView_SetVisible(bool visible);

    // Masking
    void maskingView_Open();

signals:
    // C++
    void SetLogin(const bool login);
    void isSettingsOpenedChanged(bool opened);
    void setCloseByButton(bool close);

private:    
    QMap<QString, MultiWindow*> m_windows;
    PopupParentWindow* m_popupParent = nullptr;
    SearchParentWindow* m_searchParent = nullptr;
    SetupParentWindow* m_setupParent = nullptr;
    HealthMonitoringParentWindow* m_healthMonitoringParent = nullptr;
    MaskingParentWindow* m_maskingParent = nullptr;
    QList<QString> m_windowsKeyList;
    StatisticsClient m_firebaseWebClient;
    bool m_loginStatus = false;
    bool m_isSettingsOpened = false;
};
