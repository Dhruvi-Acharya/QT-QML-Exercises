#pragma once

#include <memory>
#include <iostream>
#include <QDebug>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "WisenetViewerDefine.h"
#include "MultiWindowHandler.h"

#include "ViewModel/MainViewModel.h"
#include "ViewModel/MenuViewModel.h"
#include "ViewModel/LoginViewModel.h"
#include "ViewModel/ResourceViewModel.h"

#include "ViewModel/Bookmark/BookmarkListViewModel.h"

//#include "ViewModel/Dashboard/DashboardViewModel.h"
#include "ViewModel/Layout/LayoutNavigationViewModel.h"
#include "ViewModel/Setup/System/LocalSettingViewModel.h"

#include "../WisenetMediaFramework/ViewModel/DragItemListModel.h"

#include "QLocaleManager.h"


using namespace WisenetViewerDefine;

class MultiWindowHandler;
class MultiWindow : public QObject{
    Q_OBJECT
public:
    explicit MultiWindow(QObject *parent = nullptr);
    MultiWindow(QString key, MultiWindowHandler* handler,
                MainViewModel::ViewerMode mode,
                QObject *parent = nullptr);
    ~MultiWindow();

    void ShowPasswordChangeView();
    void ShowLoginView();
    void ShowMonitoringView();
    void FocusWisenetViewer(const bool onOff);
    void clear();
    void initializeObjects();
    void mediaOpen(int type, QString itemId, QString channelNumber = "");
    void mediaOpen(DragItemListModel* dragItemListModel);
    void mediaOpen(QVariantList items);
    void layoutOpen(QStringList layoutList);
    void sequenceOpen(QString sequenceId, QString displayName);
    void layoutChange(QString layoutID);
    void close();
    QList<QString> getSavedLayoutList();

    void refreshWindowPosition();
    QList<QString> screenInfo();
    QString positionInfo();
    void setPosition(int x, int y, int width=0, int height=0);
    void setViewerMode(int mode);
    int viewerMode();
    void setDisplayOsd(bool osd);
    bool displayOsd();
    void setDisplayVideoStatus(bool videoStatus);
    bool displayVideoStatus();
    void setPanelFlipList(QList<bool> panelFlipList);
    QList<bool> panelFlipList();
    void setFullScreen(bool fullScreen);
    bool fullScreen();
    bool isLinkedMode();           // 동기화 재생 모드
    bool isThumbnailMode();        // 타임라인 썸네일 표시 모드
    bool isMultiTimelineMode();    // 멀티 타임라인 표시 모드
    float timelineHeight();        // showHide 및 isThumbnailMode, isMultiTimelineMode on/off에 따라 복귀 할 컨트롤의 height
    void setIsLinkedMode(bool linkedMode);
    void setIsThumbnailMode(bool thumbnailMode);
    void setIsMultiTimelineMode(bool multiTimelineMode);
    void setTimelineHeight(float height);

public slots:
    // C++
    void Request(const WisenetViewerDefine::ViewerRequestSharedPtr&);
    void SetLoginStatus(const bool status);
    void ApplyLanguage();

signals:
    // C++
    void RequestToAllWindows(const WisenetViewerDefine::ViewerRequestSharedPtr&);

    // Setup view
    void setupMainView_SetSetupVisible(bool visible);
    void setupMainView_GotoHome();
    void setupMainView_GotoSetup(int menu = 0);
    void setupMainView_SelectDeviceList(QStringList channels);
    void setupMainView_SelectDeviceListChannel(QStringList channels);
    void setupMainView_SearchEventLog();
    void setupMainView_SelectAddDevice();

    // Event view
    void eventSearchView_SetVisible(bool visible);
    void eventSearchView_SetSearchMenu(int menu);
    void eventSearchView_ResetFilter();
    void eventSearchView_SetChannelFilter(QStringList channelGuids);
    void eventSearchView_SetDateFilter(QVariant start, QVariant end);
    void eventSearchView_SearchByFilter();
    void eventSearchPopupOpenShortcutClicked(int menu);

    // Sequence add view
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

    // Health monitoring
    void healthMonitoringView_SetVisible(bool visible);

    // Masking view
    void maskingView_Open();

private:
    QQmlApplicationEngine engine;

    // InitialPasswordSettingViewModel m_initialPasswordSettingViewModel;
    MainViewModel m_mainViewModel;
    MenuViewModel m_menuViewModel;
    ResourceViewModel m_resourceViewModel;
    BookmarkListViewModel m_bookmarkListViewModel;

    // Layout Tab
    LayoutNavigationViewModel m_layoutNavigationViewModel;

    //UserGroupModel m_userGroupModel;
};
