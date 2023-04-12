#include "MultiWindow.h"
#include "LanguageManager.h"
#include "WisenetMediaPlayer.h"
#include "AudioOutputManager.h"
#include "AudioInputManager.h"
#include "ControllerManager.h"
#include "FilePathConveter.h"
#include "Utility/PasswordStrengthChecker.h"
#include "LogoManager.h"
#include "MediaController/ExportVideoProgressModel.h"
#include "ViewModel/Notification/NotificationViewModel.h"
#include "DecoderManager.h"
#include "ViewModel/Setup/System/SystemMaintenanceViewModel.h"
#include "Model/UserGroupModel.h"
#include "VersionManager.h"

#include "ViewModel/Event/EventListViewModel.h"

#include "ViewModel/InitialPasswordSettingViewModel.h"
#include "ViewModel/Webpage/WebpageAddViewModel.h"

#include "ViewModel/ResourceUsage/ResourceUsageViewModel.h"

//#include "ViewModel/TextSearch/TextSearchViewModel.h"
#include "ViewModel/Setup/Device/AutoDiscoveryViewModel.h"
#include "ViewModel/Setup/Device/ManualDiscoveryViewModel.h"
#include "ViewModel/Sequence/SequenceAddViewModel.h"
#include "Model/ShortcutBoxListModel.h"

MultiWindow::MultiWindow(QObject *parent) :
    QObject(parent)
{
}

MultiWindow::MultiWindow(QString key, MultiWindowHandler* handler, MainViewModel::ViewerMode mode, QObject *parent) :
    QObject(parent)
{
    QString buildDate = __DATE__;
    m_mainViewModel.setViewerMode(mode);
    m_mainViewModel.setBuildDate(buildDate);

    connect(LoginViewModel::getInstance(), &LoginViewModel::SendRequest, this, &MultiWindow::Request);
    connect(InitialPasswordSettingViewModel::getInstance(), &InitialPasswordSettingViewModel::SendRequest, this, &MultiWindow::Request);
    //connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered, this, &MultiWindow::CoreServiceEventTriggered, Qt::QueuedConnection);

    SetLoginStatus(handler->GetLoginStatus());
    engine.addImportPath("qrc:/");
    //engine.addImportPath("qrc:/WisenetStyle");

    engine.rootContext()->setContextProperty("gAudioManager", AudioOutputManager::Instance());
    engine.rootContext()->setContextProperty("gAudioInputManager", AudioInputManager::Instance());
    engine.rootContext()->setContextProperty("gControllerManager", ControllerManager::Instance());
    engine.rootContext()->setContextProperty("passwordStrengthChecker", PasswordStrengthChecker::getInstance());

    engine.rootContext()->setContextProperty("filePathConveter", FilePathConveter::getInstance());
    engine.rootContext()->setContextProperty("localSettingViewModel", LocalSettingViewModel::getInstance());
    engine.rootContext()->setContextProperty("exportVideoProgressModel", ExportVideoProgressModel::getInstance());
    engine.rootContext()->setContextProperty("notificationViewModel", NotificationViewModel::getInstance());

    engine.rootContext()->setContextProperty("languageManager", LanguageManager::getInstance());
    engine.rootContext()->setContextProperty("localeManager", QLocaleManager::Instance());
    engine.rootContext()->setContextProperty("versionManager", VersionManager::Instance());

    connect(LanguageManager::getInstance(), &LanguageManager::languageChanged, this, &MultiWindow::ApplyLanguage);

    engine.rootContext()->setContextProperty("windowHandler", handler);
    engine.rootContext()->setContextProperty("windowGuid", key);

    engine.rootContext()->setContextProperty("initialPasswordSettingViewModel", InitialPasswordSettingViewModel::getInstance());
    engine.rootContext()->setContextProperty("mainViewModel", &m_mainViewModel);
    engine.rootContext()->setContextProperty("menuViewModel", &m_menuViewModel);
    engine.rootContext()->setContextProperty("loginViewModel", LoginViewModel::getInstance());
    engine.rootContext()->setContextProperty("resourceViewModel", &m_resourceViewModel);
    engine.rootContext()->setContextProperty("sequenceAddViewModel", SequenceAddViewModel::getInstance());
    engine.rootContext()->setContextProperty("webpageAddViewModel", WebpageAddViewModel::getInstance());

    //engine.rootContext()->setContextProperty("textSearchViewModel", TextSearchViewModel::getInstance());

    engine.rootContext()->setContextProperty("eventListViewModel", EventListViewModel::getInstance());
    engine.rootContext()->setContextProperty("bookmarkListViewModel", &m_bookmarkListViewModel);

    //engine.rootContext()->setContextProperty("dashboardViewModel", DashboardViewModel::getInstance());

    engine.rootContext()->setContextProperty("layoutNavigationViewModel", &m_layoutNavigationViewModel);

    engine.rootContext()->setContextProperty("manualDiscoveryViewModel", ManualDiscoveryViewModel::getInstance());
    engine.rootContext()->setContextProperty("autoDiscoveryViewModel", AutoDiscoveryViewModel::getInstance());
    engine.rootContext()->setContextProperty("systemMaintenanceViewModel", SystemMaintenanceViewModel::getInstance());
    engine.rootContext()->setContextProperty("userGroupModel", UserGroupModel::getInstance());
    engine.rootContext()->setContextProperty("decoderManager", DecoderManager::getInstance());

    engine.rootContext()->setContextProperty("resourceUsageViewModel", ResourceUsageViewModel::getInstance());
    engine.rootContext()->setContextProperty("logoManager", LogoManager::getInstance());
    engine.rootContext()->setContextProperty("shortcutBoxListModel", ShortcutBoxListModel::getInstance());
    engine.rootContext()->setContextProperty("windowService", this);

    engine.load(QUrl(QStringLiteral("qrc:/Qml/View/WisenetViewer.qml")));
}

MultiWindow::~MultiWindow()
{
    qDebug() << "~MultiWindow()";
}

void MultiWindow::Request(const ViewerRequestSharedPtr& request)
{
    qDebug() << "MultiWindow::Request()";
    switch(request->requestType)
    {
    case RequestType::ShowPasswordSettingPage:
    case RequestType::ShowLoginPage:
    case RequestType::ShowMonitoringPage:
    case RequestType::Logout:
        emit RequestToAllWindows(request);
        break;
    }
}

void MultiWindow::ShowPasswordChangeView()
{
    emit m_mainViewModel.showPasswordChangeView();
}

void MultiWindow::ShowLoginView()
{
    emit m_mainViewModel.showLoginView();
}

void MultiWindow::ShowMonitoringView()
{
    emit m_mainViewModel.showMonitoringView();
}

void MultiWindow::FocusWisenetViewer(const bool onOff)
{
    emit m_mainViewModel.focusWisenetViewer(onOff);
}

void MultiWindow::clear()
{
    m_layoutNavigationViewModel.closeAll();
    m_resourceViewModel.clear();
    //m_eventListViewModel.clear();
    m_bookmarkListViewModel.clear();
}

void MultiWindow::SetLoginStatus(const bool status)
{
    m_mainViewModel.SetLoginStatus(status);
    m_menuViewModel.SetLoginStatus(status);
}

void MultiWindow::ApplyLanguage()
{
    engine.retranslate();
}

void MultiWindow::initializeObjects()
{
    m_resourceViewModel.initializeTree();
    m_bookmarkListViewModel.initializeList();
}

void MultiWindow::mediaOpen(int type, QString itemId, QString channelNumber)
{
    qDebug() << "[MultiWindow] mediaOpen" << type << itemId << channelNumber;
    emit m_layoutNavigationViewModel.mediaOpen(type, itemId, channelNumber);
}

void MultiWindow::mediaOpen(DragItemListModel* dragItemListModel)
{
    qDebug() << "[MultiWindow] mediaOpen DragItemListModel" << dragItemListModel;
    emit m_layoutNavigationViewModel.listMediaOpen(dragItemListModel);
}

void MultiWindow::mediaOpen(QVariantList items)
{
    qDebug() << "[MultiWindow] mediaOpen QVariantList" << items;
    emit m_layoutNavigationViewModel.gridItemsMediaOpen(items);
}

void MultiWindow::layoutChange(QString layoutID)
{
    LayoutTreeItemInfo* layoutTreeItemInfo = (LayoutTreeItemInfo*)m_resourceViewModel.layoutTreeModel()->getLayoutItemInfo(layoutID);

    if(layoutTreeItemInfo != nullptr)
    {
        qDebug() << "[MultiWindow] layoutOpen signal" << layoutID;
        emit m_layoutNavigationViewModel.layoutOpen(layoutTreeItemInfo->uuid(), layoutTreeItemInfo->displayName(), layoutTreeItemInfo->layoutViewModel());
    }
}

void MultiWindow::layoutOpen(QStringList layoutList)
{
    qDebug() << "[MultiWindow] layoutOpen" << layoutList;
    m_layoutNavigationViewModel.closeAll();

    foreach(auto& layoutId, layoutList)
    {
        LayoutTreeItemInfo* layoutTreeItemInfo = (LayoutTreeItemInfo*)m_resourceViewModel.layoutTreeModel()->getLayoutItemInfo(layoutId);

        if(layoutTreeItemInfo != nullptr)
        {
            qDebug() << "[MultiWindow] layoutOpen signal" << layoutId;
            emit m_layoutNavigationViewModel.layoutOpen(layoutTreeItemInfo->uuid(), layoutTreeItemInfo->displayName(), layoutTreeItemInfo->layoutViewModel());
        }
        else
        {
            qDebug() << "[MultiWindow] layoutOpen layoutTreeItemInfo == nullptr" << layoutId;
        }
    }

    if(m_layoutNavigationViewModel.layoutTabModel()->count() == 0)
        m_layoutNavigationViewModel.makeNewTab();
}

void MultiWindow::sequenceOpen(QString sequenceId, QString displayName)
{
    qDebug() << "[MultiWindow] sequenceOpen" << sequenceId<<displayName;
    m_layoutNavigationViewModel.closeAll();

    emit m_layoutNavigationViewModel.sequenceOpen(sequenceId, displayName);

    if(m_layoutNavigationViewModel.layoutTabModel()->count() == 0)
        m_layoutNavigationViewModel.makeNewTab();
}

void MultiWindow::close()
{
    emit m_mainViewModel.close();
}

QList<QString> MultiWindow::getSavedLayoutList()
{
    return m_layoutNavigationViewModel.getSavedLayoutList();
}

void MultiWindow::refreshWindowPosition()
{
    emit m_mainViewModel.refreshWindowPosition();
}

QList<QString> MultiWindow::screenInfo()
{
    emit m_mainViewModel.refreshScreen();
    return m_mainViewModel.getScreenInfo();
}

QString MultiWindow::positionInfo()
{
    return m_mainViewModel.getPositionInfo();
}

void MultiWindow::setPosition(int x, int y, int width, int height)
{
    emit m_mainViewModel.setPosition(x, y, width, height);
}

void MultiWindow::setViewerMode(int mode)
{
    m_mainViewModel.setViewerMode((MainViewModel::ViewerMode)mode);
}

int MultiWindow::viewerMode()
{
    return m_mainViewModel.viewerMode();
}

void MultiWindow::setDisplayOsd(bool osd)
{
     m_mainViewModel.setDisplayOsd(osd);
}

bool MultiWindow::displayOsd()
{
    return m_mainViewModel.displayOsd();
}

void MultiWindow::setDisplayVideoStatus(bool videoStatus)
{
     m_mainViewModel.setDisplayVideoStatus(videoStatus);
}

bool MultiWindow::displayVideoStatus()
{
    return m_mainViewModel.displayVideoStatus();
}

void MultiWindow::setPanelFlipList(QList<bool> panelFlipList)
{
    m_mainViewModel.setPanelFlipList(panelFlipList);
}

QList<bool> MultiWindow::panelFlipList()
{
    return m_mainViewModel.panelFlipList();
}

bool MultiWindow::isLinkedMode()
{
    return m_mainViewModel.isLinkedMode();
}

bool MultiWindow::isThumbnailMode()
{
    return m_mainViewModel.isThumbnailMode();
}

bool MultiWindow::isMultiTimelineMode()
{
    return m_mainViewModel.isMultiTimelineMode();
}

float MultiWindow::timelineHeight()
{
    return m_mainViewModel.timelineHeight();
}

void MultiWindow::setIsLinkedMode(bool linkedMode)
{
    m_mainViewModel.setIsLinkedMode(linkedMode);
}

void MultiWindow::setIsThumbnailMode(bool thumbnailMode)
{
    m_mainViewModel.setIsThumbnailMode(thumbnailMode);
}

void MultiWindow::setIsMultiTimelineMode(bool multiTimelineMode)
{
    m_mainViewModel.setIsMultiTimelineMode(multiTimelineMode);
}

void MultiWindow::setTimelineHeight(float height)
{
    m_mainViewModel.setTimelineHeight(height);
}
