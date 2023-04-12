#include "MultiWindowHandler.h"
#include "ViewModel/Notification/NotificationViewModel.h"
#include "ViewModel/Event/EventListViewModel.h"
#include "ViewModel/Setup/System/SystemMaintenanceViewModel.h"
#include "LogSettings.h"

MultiWindowHandler::MultiWindowHandler(QObject *parent) :
    QObject(parent)
{
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &MultiWindowHandler::coreServiceEventTriggered, Qt::QueuedConnection);

    connect(SystemMaintenanceViewModel::getInstance(), &SystemMaintenanceViewModel::exitProgram, this, &MultiWindowHandler::exitProgram);

    m_popupParent = new PopupParentWindow(this);
    m_firebaseWebClient.setLogger([](const QString& message){
        SPDLOG_INFO("[STATISTICS] {}", message.toStdString());
    });
}

MultiWindowHandler::~MultiWindowHandler()
{
    if(m_popupParent != nullptr)
    {
        delete m_popupParent;
        m_popupParent = nullptr;
    }

    if(m_popupParent != nullptr)
    {
        delete m_searchParent;
        m_searchParent = nullptr;
    }
}

void MultiWindowHandler::connectSignals(MultiWindow* multiWindow)
{
    connect(multiWindow, &MultiWindow::RequestToAllWindows, this, &MultiWindowHandler::Request);
    connect(this, &MultiWindowHandler::SetLogin, multiWindow, &MultiWindow::SetLoginStatus);

    // Setup
    connect(multiWindow, &MultiWindow::setupMainView_SetSetupVisible, this, &MultiWindowHandler::setupMainView_SetSetupVisible);
    connect(multiWindow, &MultiWindow::setupMainView_GotoHome, this, &MultiWindowHandler::setupMainView_GotoHome);
    connect(multiWindow, &MultiWindow::setupMainView_GotoSetup, this, &MultiWindowHandler::setupMainView_GotoSetup);
    connect(multiWindow, &MultiWindow::setupMainView_SelectDeviceList, this, &MultiWindowHandler::setupMainView_SelectDeviceList);
    connect(multiWindow, &MultiWindow::setupMainView_SelectDeviceListChannel, this, &MultiWindowHandler::setupMainView_SelectDeviceListChannel);
    connect(multiWindow, &MultiWindow::setupMainView_SearchEventLog, this, &MultiWindowHandler::setupMainView_SearchEventLog);
    connect(multiWindow, &MultiWindow::setupMainView_SelectAddDevice, this, &MultiWindowHandler::setupMainView_SelectAddDevice);

    // Event
    connect(multiWindow, &MultiWindow::eventSearchView_SetVisible, this, &MultiWindowHandler::eventSearchView_SetVisible);
    connect(multiWindow, &MultiWindow::eventSearchView_SetSearchMenu, this, &MultiWindowHandler::eventSearchView_SetSearchMenu);
    connect(multiWindow, &MultiWindow::eventSearchView_ResetFilter, this, &MultiWindowHandler::eventSearchView_ResetFilter);
    connect(multiWindow, &MultiWindow::eventSearchView_SetChannelFilter, this, &MultiWindowHandler::eventSearchView_SetChannelFilter);
    connect(multiWindow, &MultiWindow::eventSearchView_SetDateFilter, this, &MultiWindowHandler::eventSearchView_SetDateFilter);
    connect(multiWindow, &MultiWindow::eventSearchView_SearchByFilter, this, &MultiWindowHandler::eventSearchView_SearchByFilter);
    connect(multiWindow, &MultiWindow::eventSearchPopupOpenShortcutClicked, this, &MultiWindowHandler::eventSearchPopupOpenShortcutClicked);

    // Sequence
    connect(multiWindow, &MultiWindow::addNewSequence, m_popupParent, &PopupParentWindow::addNewSequence);
    connect(multiWindow, &MultiWindow::editSequence, m_popupParent, &PopupParentWindow::editSequence);
    connect(multiWindow, &MultiWindow::openSequenceAddCancelDialog, m_popupParent, &PopupParentWindow::openSequenceAddCancelDialog);

    // P2P
    connect(multiWindow, &MultiWindow::openInitializeDeviceCredentialView, m_popupParent, &PopupParentWindow::openInitializeDeviceCredentialView);
    connect(multiWindow, &MultiWindow::openP2pView, m_popupParent, &PopupParentWindow::openP2pView);

    connect(multiWindow, &MultiWindow::webPageAddView_Open, m_popupParent, &PopupParentWindow::webPageAddView_Open);

    // Export progress
    connect(multiWindow, &MultiWindow::exportProgressView_Open, m_popupParent, &PopupParentWindow::exportProgressView_Open);
    connect(multiWindow, &MultiWindow::exportProgressView_ForceClose, m_popupParent, &PopupParentWindow::exportProgressView_ForceClose);

    // Health monitoring
    connect(multiWindow, &MultiWindow::healthMonitoringView_SetVisible, this, &MultiWindowHandler::healthMonitoringView_SetVisible);

    // Masking
    connect(multiWindow, &MultiWindow::maskingView_Open, this, &MultiWindowHandler::maskingView_Open);
}

void MultiWindowHandler::setupMainView_SetSetupVisible(bool visible)
{
    if(!visible)
        return;
    makeSetupParent();

    m_setupParent->setupMainView_SetSetupVisible(visible);
}

void MultiWindowHandler::makeSetupParent()
{
    if (m_setupParent == nullptr)
        m_setupParent = new SetupParentWindow(this, m_popupParent);
}

void MultiWindowHandler::setupMainView_GotoHome()
{
    makeSetupParent();

    m_setupParent->setupMainView_GotoHome();
}

void MultiWindowHandler::setupMainView_GotoSetup(int menu)
{
    makeSetupParent();
    m_setupParent->setupMainView_GotoSetup(menu);
}

void MultiWindowHandler::setupMainView_SelectDeviceList(QStringList channels)
{
    makeSetupParent();

    m_setupParent->setupMainView_SelectDeviceList(channels);
}

void MultiWindowHandler::setupMainView_SelectDeviceListChannel(QStringList channels)
{
    makeSetupParent();

    m_setupParent->setupMainView_SelectDeviceListChannel(channels);
}

void MultiWindowHandler::setupMainView_SearchEventLog()
{
    makeSetupParent();

    m_setupParent->setupMainView_SearchEventLog();
}

void MultiWindowHandler::setupMainView_SelectAddDevice()
{
    makeSetupParent();

    m_setupParent->setupMainView_SelectAddDevice();
}

void MultiWindowHandler::eventSearchView_SetVisible(bool visible)
{    
    if(m_searchParent == nullptr)
    {
        if(!visible)
            return;

        m_searchParent = new SearchParentWindow(this, m_popupParent);
    }

    m_searchParent->eventSearchView_SetVisible(visible);
}

void MultiWindowHandler::eventSearchView_SetSearchMenu(int menu)
{
    if (m_searchParent == nullptr)
        m_searchParent = new SearchParentWindow(this, m_popupParent);

    m_searchParent->eventSearchView_SetSearchMenu(menu);
}

void MultiWindowHandler::eventSearchView_ResetFilter()
{
    if (m_searchParent == nullptr)
        m_searchParent = new SearchParentWindow(this, m_popupParent);

    m_searchParent->eventSearchView_ResetFilter();
}

void MultiWindowHandler::eventSearchView_SetChannelFilter(QStringList channelGuids)
{
    if (m_searchParent == nullptr)
        m_searchParent = new SearchParentWindow(this, m_popupParent);

    m_searchParent->eventSearchView_SetChannelFilter(channelGuids);
}

void MultiWindowHandler::eventSearchView_SetDateFilter(QVariant start, QVariant end)
{
    if (m_searchParent == nullptr)
        m_searchParent = new SearchParentWindow(this, m_popupParent);

    m_searchParent->eventSearchView_SetDateFilter(start, end);
}

void MultiWindowHandler::eventSearchView_SearchByFilter()
{
    if (m_searchParent == nullptr)
        m_searchParent = new SearchParentWindow(this, m_popupParent);

    m_searchParent->eventSearchView_SearchByFilter();
}

void MultiWindowHandler::eventSearchPopupOpenShortcutClicked(int menu)
{
    if(!m_searchParent)
        m_searchParent = new SearchParentWindow(this, m_popupParent);

    m_searchParent->eventSearchPopupOpenShortcutClicked(menu);
}

void MultiWindowHandler::healthMonitoringView_SetVisible(bool visible)
{
    if(!m_healthMonitoringParent)
        m_healthMonitoringParent = new HealthMonitoringParentWindow(this);

    m_healthMonitoringParent->healthMonitoringView_SetVisible(visible);
}

void MultiWindowHandler::maskingView_Open()
{
    if(!m_maskingParent)
        m_maskingParent = new MaskingParentWindow(this);

    emit m_maskingParent->maskingView_Open();
}

void MultiWindowHandler::initializeNewWindow(bool isFirst)
{
    QString key = QUuid::createUuid().toString();
    qDebug() << "[MultiWindowHandler] initializeNewWindow()--1-- " << key << isFirst;

    //MainViewModel::ViewerMode mode = isFirst ? MainViewModel::Maximized : MainViewModel::Windowed;
    MultiWindow* multiWindow = new MultiWindow(key, this, MainViewModel::Windowed);
    m_windows.insert(key, multiWindow);
    m_windowsKeyList.append(key);

    if(!isFirst) {
        multiWindow->initializeObjects();
    }

    connectSignals(multiWindow);

    qDebug() << "[MultiWindowHandler] initializeNewWindow()--2-- " << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
}

void MultiWindowHandler::switchWindow(QString key, bool toLeft)
{
    int nextIndex = m_windowsKeyList.indexOf(key);
    QString nextKey;

    qDebug() << "[MultiWindowHandler] switchWindow()--1-- index:" << nextIndex << "key:"<< key <<"toLeft:"<< toLeft;

    if(toLeft) {
        if(nextIndex > 0) {
            nextIndex -= 1;
        }
        else {
            nextIndex = m_windowsKeyList.size() - 1;
        }
    }
    else {
        if(nextIndex < m_windowsKeyList.size() - 1) {
            nextIndex += 1;
        }
        else {
            nextIndex = 0;
        }
    }

    nextKey = m_windowsKeyList[nextIndex];


    MultiWindow * multiWindow = m_windows[key];
    multiWindow->FocusWisenetViewer(false);
    qDebug() << "[MultiWindowHandler] switchWindow()--2-- key:" << key << ", multiWindow:"<<multiWindow;
    multiWindow = m_windows[nextKey];
    multiWindow->FocusWisenetViewer(true);

    qDebug() << "[MultiWindowHandler] switchWindow()--3-- nextKey:" << nextKey << ", nextIndex:" << nextIndex << ", multiWindow:"<<multiWindow<< ", Windows:" << m_windowsKeyList;
}

void MultiWindowHandler::initializeNewWindowWithMediaOpen(int type, QString itemId, QString channelNumber)
{
    QString key = QUuid::createUuid().toString();
    qDebug() << "[MultiWindowHandler] initializeNewWindowWithMediaOpen()--1-- " << key;

    MultiWindow* multiWindow = new MultiWindow(key, this, MainViewModel::Windowed);
    m_windows.insert(key, multiWindow);
    m_windowsKeyList.append(key);

    multiWindow->initializeObjects();

    connectSignals(multiWindow);

    multiWindow->mediaOpen(type, itemId, channelNumber);

    qDebug() << "[MultiWindowHandler] initializeNewWindowWithMediaOpen()--2-- " << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
    qDebug() << "[MultiWindowHandler] initializeNewWindowWithMediaOpen() " << type << itemId << channelNumber;
}

void MultiWindowHandler::initializeNewWindowWithItems(DragItemListModel* dragItemListModel)
{
    QString key = QUuid::createUuid().toString();
    qDebug() << "[MultiWindowHandler] initializeNewWindowWithItems()--1-- " << key;

    MultiWindow* multiWindow = new MultiWindow(key, this, MainViewModel::Windowed);
    m_windows.insert(key, multiWindow);
    m_windowsKeyList.append(key);

    multiWindow->initializeObjects();

    connectSignals(multiWindow);

    multiWindow->mediaOpen(dragItemListModel);
    qDebug() << "[MultiWindowHandler] initializeNewWindowWithItems()--2-- " << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
}

void MultiWindowHandler::initializeNewWindowWithGridItems(QVariantList items)
{
    QString key = QUuid::createUuid().toString();
    qDebug() << "[MultiWindowHandler] initializeNewWindowWithGridItems()--1-- " << key;

    MultiWindow* multiWindow = new MultiWindow(key, this, MainViewModel::Windowed);
    m_windows.insert(key, multiWindow);
    m_windowsKeyList.append(key);

    multiWindow->initializeObjects();

    connectSignals(multiWindow);

    multiWindow->mediaOpen(items);

    qDebug() << "[MultiWindowHandler] initializeNewWindowWithGridItems()--2-- " << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
}

void MultiWindowHandler::initializeNewWindowWithLayouts(QStringList layoutIds)
{
    QString key = QUuid::createUuid().toString();
    qDebug() << "[MultiWindowHandler] initializeNewWindowWithLayouts()--1-- " << key;

    MultiWindow* multiWindow = new MultiWindow(key, this, MainViewModel::Windowed);
    m_windows.insert(key, multiWindow);
    m_windowsKeyList.append(key);

    multiWindow->initializeObjects();

    connectSignals(multiWindow);

    multiWindow->layoutOpen(layoutIds);

    qDebug() << "[MultiWindowHandler] initializeNewWindowWithLayouts()--2-- " << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
}

void MultiWindowHandler::initializeNewWindowWithSequence(QString sequenceId, QString displayName)
{
    QString key = QUuid::createUuid().toString();
    qDebug() << "[MultiWindowHandler] initializeNewWindowWithSequence()--1-- " << key;

    MultiWindow* multiWindow = new MultiWindow(key, this, MainViewModel::Windowed);
    m_windows.insert(key, multiWindow);
    m_windowsKeyList.append(key);

    multiWindow->initializeObjects();

    connectSignals(multiWindow);

    //    multiWindow->layoutOpen(layoutIds);
    multiWindow->sequenceOpen(sequenceId, displayName);

    qDebug() << "[MultiWindowHandler] initializeNewWindowWithSequence()--2-- " << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
}

void MultiWindowHandler::destroyWindow(QString key)
{
    qDebug() << "[MultiWindowHandler] destroyWindow() --1--" << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
    if(m_windows.contains(key))
    {
        MultiWindow * multiWindow = m_windows.take(key);

        int indexToRemoved = -1;
        for(int i=0; i<m_windowsKeyList.length(); i++)
        {
            if(m_windowsKeyList[i] == key)
            {
                indexToRemoved = i;
                break;
            }
        }

        if(indexToRemoved >= 0)
            m_windowsKeyList.removeAt(indexToRemoved);

        multiWindow->deleteLater();
    }
    qDebug() << "[MultiWindowHandler] destroyWindow() --2--" << key << ", count:" << m_windows.count() << ", Windows:" << m_windowsKeyList;
}

void MultiWindowHandler::Request(const WisenetViewerDefine::ViewerRequestSharedPtr& request)
{
    qDebug() << "[MultiWindowHandler] Request() " << (int)request->requestType;

    if(request->requestType == RequestType::ShowPasswordSettingPage || request->requestType == RequestType::ShowLoginPage)
        NotifyLoginStatus(false);
    if(request->requestType == RequestType::ShowMonitoringPage)
        NotifyLoginStatus(true);

    if(request->requestType == RequestType::Logout)
    {
        saveLatestSetting();

        qDebug() << "[MultiWindowHandler] Request() Logout --1--  Windows:" << m_windowsKeyList;
        for(int i=m_windowsKeyList.count()-1; i >= 1; i--)
        {
            QString key = m_windowsKeyList[i];
            MultiWindow* window = m_windows[key];
            window->clear();
            window->close();

            destroyWindow(key);
        }
        qDebug() << "[MultiWindowHandler] Request() Logout --2--  Windows:" << m_windowsKeyList;

        MultiWindow* lastWindow = m_windows.first();
        NotificationViewModel::getInstance()->clear();
        EventListViewModel::getInstance()->clear();

        lastWindow->clear();
    }

    for (auto& window : m_windows)
    {
        switch(request->requestType)
        {
        case RequestType::ShowPasswordSettingPage:
            window->ShowPasswordChangeView();
            break;
        case RequestType::ShowLoginPage:
            window->ShowLoginView();
            break;
        case RequestType::ShowMonitoringPage:
            window->ShowMonitoringView();
            break;
        }
    }
}

void MultiWindowHandler::NotifyLoginStatus(const bool arg)
{
    m_loginStatus = arg;
    SetLogin(arg);
}

void MultiWindowHandler::exitProgram(bool restart)
{
    SPDLOG_INFO("MultiWindowHandler::exitProgram() restart: {}", restart);
    qDebug() << "MultiWindowHandler::exitProgram() restart:" << restart;


    auto settings = QCoreServiceManager::Instance().Settings();

    if(settings != nullptr)
    {
        if(settings->collectingAgreed())
        {
            QString machineKey;
            QCoreServiceManager::Instance().updateStatistics();
            QByteArray payload = QCoreServiceManager::Instance().generateStatistics(machineKey);

            QString appVersion = QCoreApplication::applicationVersion().remove(".");

            bool test = settings->statisticsTest();
            m_firebaseWebClient.setDefaultKey(machineKey, appVersion, test);

            if(payload != nullptr)
                m_firebaseWebClient.webRequest(payload);

            SPDLOG_INFO("MultiWindowHandler::exitProgram() send statistics: app version: {}, test: {}", appVersion.toStdString(), test);
            qDebug() << "MultiWindowHandler::exitProgram() send statistics: app version:" << appVersion << ", test:" << test;
        }
        else
        {
            qDebug() << "MultiWindowHandler::exitProgram() settings->collectingAgreed()" << settings->collectingAgreed();
        }
    }

    if(m_loginStatus)
        saveLatestSetting();

    emit setCloseByButton(true);

    for(int i=m_windowsKeyList.count()-1; i >= 0; i--)
    {
        QString key = m_windowsKeyList[i];

        MultiWindow* window = m_windows[key];

        window->clear();
        window->close();

        destroyWindow(key);
    }

    m_popupParent->closeAll();

    if(m_setupParent != nullptr)
        m_setupParent->closeAll();

    if(m_searchParent != nullptr)
        m_searchParent->closeAll();

    if(m_maskingParent != nullptr)
        m_maskingParent->closeAll();

    if(restart)
    {
        qApp->quit();

        QStringList argvs = { "--restart" };
        QProcess::startDetached(qApp->arguments()[0], argvs);
    }
}

void MultiWindowHandler::loadLatestSetting(QList<QStringList>& layouts, QStringList& screens, QStringList& windowPositions,
                                           QStringList& displayOsds, QStringList& displayVideoStatuses, QList<QStringList>& panelFlipList, QStringList& viewerModes,
                                           QStringList& isLinkedMode, QStringList& isThumbnailMode, QStringList& isMultiTimelineMode, QStringList& timelineHeight)
{
    qDebug() << "[MultiWindowHandler] loadLatestSetting()";

    auto setting =  QCoreServiceManager::Instance().Settings();
    if (setting == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    QString id = QString("");
    if (setting != nullptr) {
        id = setting->loginId();
        setting->loadLayouts(id, layouts);
        setting->loadScreen(id, screens);
        setting->loadWindowPosition(id, windowPositions);
        setting->loadDisplayOsd(id, displayOsds);
        setting->loadDisplayVideoStatus(id, displayVideoStatuses);
        setting->loadPanelFlipList(id, panelFlipList);
        setting->loadViewerModes(id, viewerModes);
        setting->loadIsLinkedMode(id, isLinkedMode);
        setting->loadIsThumbnailMode(id, isThumbnailMode);
        setting->loadIsMultiTimelineMode(id, isMultiTimelineMode);
        setting->loadTimelineHeight(id, timelineHeight);

        qDebug() << "[MultiWindowHandler] loadLatestSetting()" << id << layouts << screens << windowPositions << displayOsds << panelFlipList << viewerModes;
    }
}

void MultiWindowHandler::saveLatestSetting()
{
    QList<QStringList> layouts;
    QStringList screens;
    QStringList windowPositions;
    QStringList displayOsds;
    QStringList displayVideoStatuses;
    QList<QStringList> panelFlipList; // left, right, bottom
    QStringList viewerModes;
    QStringList isLinkedMode;           // 동기화 재생 모드
    QStringList isThumbnailMode;        // 타임라인 썸네일 표시 모드
    QStringList isMultiTimelineMode;    // 멀티 타임라인 표시 모드
    QStringList timelineHeight;         // showHide 및 isThumbnailMode, isMultiTimelineMode on/off에 따라 복귀 할 컨트롤의 height

    for(int i=0; i<m_windowsKeyList.count(); i++)
    {
        QString windowKey = m_windowsKeyList[i];
        MultiWindow* window = m_windows[windowKey];

        QList<QString> layoutList = window->getSavedLayoutList();

        layouts.append(layoutList);

        qDebug() << "[MultiWindowHandler] saveLatestSetting() windowKey" << windowKey << ", tabList" << layoutList;


        if(i == 0)
        {
            screens = window->screenInfo();
        }

        window->refreshWindowPosition();
        QString position = window->positionInfo();

        qDebug() << "[MultiWindowHandler] saveLatestSetting() position" << position;

        windowPositions.append(position);

        QString osd = window->displayOsd()?"true":"false";
        displayOsds.append(osd);

        QString videoStatus = window->displayVideoStatus()?"true":"false";
        displayVideoStatuses.append(videoStatus);

        QStringList panelFlip;
        for(bool pf : window->panelFlipList())
        {
            panelFlip.append(pf?"true":"false");
        }
        panelFlipList.append(panelFlip);
        qDebug() << "[MultiWindowHandler] saveLatestSetting()" << i <<  "panelFlip :" << panelFlip;

        QString viewerMode = QString::number(window->viewerMode());
        viewerModes.append(viewerMode);
        qDebug() << "[MultiWindowHandler] saveLatestSetting()" << i <<  "viewerMode :" << viewerMode;

        QString linkedMode = window->isLinkedMode() ? "true" : "false";
        isLinkedMode.append(linkedMode);

        QString thumbnailMode = window->isThumbnailMode() ? "true" : "false";
        isThumbnailMode.append(thumbnailMode);

        QString multiTimelineMode = window->isMultiTimelineMode() ? "true" : "false";
        isMultiTimelineMode.append(multiTimelineMode);

        QString height = QString::number(window->timelineHeight());
        timelineHeight.append(height);
    }

    auto setting =  QCoreServiceManager::Instance().Settings();
    if (setting == nullptr) {
        return;
    }

    QString id = setting->loginId();
    setting->saveLayouts(id, layouts);
    setting->saveScreen(id, screens);
    setting->saveWindowPosition(id, windowPositions);
    setting->saveDisplayOsd(id, displayOsds);
    setting->saveDisplayVideoStatus(id, displayVideoStatuses);
    setting->savePanelFlipList(id, panelFlipList);
    setting->saveViewerModes(id, viewerModes);
    setting->saveIsLinkedMode(id, isLinkedMode);
    setting->saveIsThumbnailMode(id, isThumbnailMode);
    setting->saveIsMultiTimelineMode(id, isMultiTimelineMode);
    setting->saveTimelineHeight(id, timelineHeight);

    qDebug() << "[MultiWindowHandler] saveLatestSetting()" << id << layouts << panelFlipList << viewerModes;
}

void MultiWindowHandler::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    if(serviceEvent->EventTypeId() == Wisenet::Core::OpenLayoutEventType )
    {
        auto layoutOpenEvent = std::static_pointer_cast<Wisenet::Core::OpenLayoutEvent>(event->eventDataPtr);

        MultiWindow* firstWindow = m_windows[m_windowsKeyList.first()];

        if(firstWindow != nullptr)
            firstWindow->layoutChange( QString(layoutOpenEvent->layoutID.c_str()));
    }

    if(serviceEvent->EventTypeId() == Wisenet::Core::LoadLatestSettingsEventType)
    {
        QList<QStringList> layouts;
        QStringList screens;
        QStringList windowPositions;
        QStringList displayOsds;
        QStringList displayVideoStatuses;
        QList<QStringList> panelFlipList; // left, right, bottom
        QStringList viewerModes;
        QStringList isLinkedMode;
        QStringList isThumbnailMode;
        QStringList isMultiTimelineMode;
        QStringList timelineHeight;

        QStringList currentScreenInfo;

        // Load latest settings
        qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() --1-- loadLatestSetting";
        loadLatestSetting(layouts, screens, windowPositions, displayOsds, displayVideoStatuses, panelFlipList, viewerModes,
                          isLinkedMode, isThumbnailMode, isMultiTimelineMode, timelineHeight);

        qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() --2-- Open windows";
        // Open windows (첫번째 윈도우는 이미 열림 두번째부터)
        if(layouts.count() > 0)
        {
            // first window
            if(m_windows.count() >= 1)
            {
                MultiWindow* firstWindow = m_windows.first();
                currentScreenInfo = firstWindow->screenInfo();

                if(layouts[0].count())
                {
                    firstWindow->layoutOpen(layouts[0]);
                }
            }

            // other windows
            if(layouts.count() >= 2)
            {
                for(int i=1; i<layouts.count(); i++)
                {
                    if(layouts[i].count() > 0)
                        initializeNewWindowWithLayouts(layouts[i]);
                    else
                        initializeNewWindow(false);
                }
            }
        }

        // Screen 갯수가 같은지
        qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() --3-- Move windows screens" << screens << ", current" << currentScreenInfo;
        if(screens.size() == currentScreenInfo.size())
        {
            bool screenChanged = false;

            // 각 screen에 변경이 없는지 비교
            for(int i=0; i<screens.size(); i++)
            {
                if(screens[i] != currentScreenInfo[i])
                {
                    screenChanged = true;
                    break;
                }
            }

            // Screen 정보가 변경되지 않았으면 Window position 이동
            qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() --3--1 screenChanged" << screenChanged << ", windowPositions" << windowPositions << ", m_windowsKeyList" << m_windowsKeyList;
            if(!screenChanged && (windowPositions.size() == m_windowsKeyList.size()))
            {
                for(int i=0; i < m_windowsKeyList.size(); i++)
                {
                    QString windowKey = m_windowsKeyList[i];
                    MultiWindow* window = m_windows[windowKey];

                    QString position = windowPositions[i];
                    QStringList splitStr = position.split(':');


                    qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() --3--2"<<splitStr[0] << splitStr[1] << splitStr[2];
                    if(splitStr.size() == 5)
                    {
                        qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() --3--3 Width,height" << splitStr[3] << splitStr[4];
                        window->setPosition(splitStr[0].toInt(), splitStr[1].toInt(), splitStr[3].toInt(), splitStr[4].toInt());
                    }
                    else
                        window->setPosition(splitStr[0].toInt(), splitStr[1].toInt());
                }
            }
        }

        qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() " << displayOsds;

        if(displayOsds.size() <= m_windowsKeyList.size())
        {
            for(int i=0; i < displayOsds.size(); i++)
            {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];

                QString osd = displayOsds[i];

                qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() " << i <<  "osd : " << osd;

                if(osd == "true")
                    window->setDisplayOsd(true);
                else
                    window->setDisplayOsd(false);
            }
        }
        if(displayVideoStatuses.size() <= m_windowsKeyList.size())
        {
            for(int i=0; i < displayVideoStatuses.size(); i++)
            {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];
                QString videoStatus = displayVideoStatuses[i];
                if(videoStatus == "true")
                    window->setDisplayVideoStatus(true);
                else
                    window->setDisplayVideoStatus(false);
            }
        }

        if(panelFlipList.count() > 0)
        {
            for(int i=0; i<panelFlipList.count(); i++)
            {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];

                QList<bool> panelFlip;
                for(QString pf : panelFlipList[i])
                {
                    panelFlip.append(pf == "true" ? true : false);
                }

                if(panelFlip.size() >= 3)
                {
                    window->setPanelFlipList(panelFlip);
                }
                qDebug() << "[MultiWindowHandler] coreServiceEventTriggered()" << i <<  "panelFlip :" << panelFlip;
            }
        }

        if(viewerModes.size() <= m_windowsKeyList.size())
        {
            for(int i=0; i < viewerModes.size(); i++)
            {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];

                QString viewerMode = viewerModes[i];

                qDebug() << "[MultiWindowHandler] coreServiceEventTriggered() " << i <<  "viewerMode :" << viewerMode.toInt();

                window->setViewerMode(viewerMode.toInt());
            }
        }

        if(isLinkedMode.size() <= m_windowsKeyList.size()) {
            for(int i=0; i < isLinkedMode.size(); i++) {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];
                window->setIsLinkedMode(isLinkedMode[i] == "true");
            }
        }

        if(isThumbnailMode.size() <= m_windowsKeyList.size()) {
            for(int i=0; i < isThumbnailMode.size(); i++) {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];
                window->setIsThumbnailMode(isThumbnailMode[i] == "true");
            }
        }

        if(isMultiTimelineMode.size() <= m_windowsKeyList.size()) {
            for(int i=0; i < isMultiTimelineMode.size(); i++) {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];
                window->setIsMultiTimelineMode(isMultiTimelineMode[i] == "true");
            }
        }

        if(timelineHeight.size() <= m_windowsKeyList.size()) {
            for(int i=0; i < timelineHeight.size(); i++) {
                QString windowKey = m_windowsKeyList[i];
                MultiWindow* window = m_windows[windowKey];
                window->setTimelineHeight(timelineHeight[i].toFloat());
            }
        }
    }
}
