#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtWebEngine>
#include <QTranslator>
#include <QFontDatabase>
#include <QCommandLineParser>
#include <QOperatingSystemVersion>
#include <QSysInfo>
#include <QSharedMemory>
#include "Cpp/WisenetViewer.h"
#include "QCoreServiceManager.h"
#include "DatabaseManager.h"
#include "LogManager.h"
#include "QLogSettings.h"
#include "WisenetMediaPlayer.h"
#include "AppLocalDataLocation.h"
#include "WisenetViewerDefine.h"
#include "LanguageManager.h"
#include "DecoderManager.h"
#include "VersionManager.h"
#include "./Cpp/ProcessModel.h"

#include "./Cpp/Model/TreeItemModel.h"
#include "./Cpp/Model/TreeProxyModel.h"
#include "../WisenetMediaFramework/ViewModel/DragItemListModel.h"
#include "MediaController/CalendarTimelineModel.h"
#include "MediaController/DateAreaModel.h"
#include "MediaController/GraphAreaModel.h"
#include "MediaController/MultiGraphAreaModel.h"
#include "MediaController/MediaControlModel.h"
#include "MediaController/TimeAreaModel.h"
#include "MediaController/BookmarkModel.h"
#include "MediaController/ExportVideoModel.h"
#include "MediaController/RecordTypeFilterViewModel.h"
#include "MediaController/PlaybackSpeedModel.h"
#include "MediaController/ThumbnailModel.h"
#include "MediaController/ThumbnailImage.h"

#include "./Cpp/Model/DeviceListModel.h"
#include "./Cpp/Model/SetupTreeNode.h"
#include "./Cpp/Model/ImageItem.h"
#include "./Cpp/ViewModel/Setup/SetupMainTreeViewModel.h"
#include "./Cpp/ViewModel/Setup/SetupMainTreeFilterProxyModel.h"
#include "./Cpp/ViewModel/DeviceSelectionTree/DeviceSelectionTreeSourceModel.h"
#include "./Cpp/ViewModel/DeviceSelectionTree/DevicePhysicalTreeSourceModel.h"
#include "./Cpp/ViewModel/DeviceSelectionTree/DeviceAlarmOutTreeSourceModel.h"
#include "./Cpp/ViewModel/DeviceSelectionTree/DeviceEmailTreeSourceModel.h"
#include "./Cpp/ViewModel/DeviceSelectionTree/DeviceEventTreeSourceModel.h"
#include "./Cpp/ViewModel/DeviceSelectionTree/DeviceAlarmInTreeSourceModel.h"
#include "./Cpp/ViewModel/DeviceFilterTree/DeviceFilterTreeSourceModel.h"
#include "./Cpp/ViewModel/EventFilterTree/EventFilterTreeSourceModel.h"
#include "./Cpp/ViewModel/SystemAdministration/UserFilterTreeSourceModel.h"
#include "./Cpp/ViewModel/EventLog/EventLogThread.h"
#include "./Cpp/ViewModel/EventLog/EventLogViewModel.h"
#include "./Cpp/ViewModel/SystemLog/SystemLogViewModel.h"
#include "./Cpp/ViewModel/SystemAdministration/AuditLogViewModel.h"
#include "./Cpp/ViewModel/SystemAdministration/AuditLogThread.h"
#include "./Cpp/ViewModel/ObjectSearch/GridListThread.h"
#include "./Cpp/ViewModel/TextSearch/TextSearchViewModel.h"

#include "./Cpp/ViewModel/Setup/User/UserLayoutTreeSourceModel.h"
#include "./Cpp/ViewModel/Setup/User/AddUserGroupViewModel.h"
#include "./Cpp/ViewModel/Setup/Device/DeviceListTreeSourceModel.h"
#include "./Cpp/ViewModel/Setup/DeviceMaintenance/DeviceConfigViewModel.h"
#include "./Cpp/ViewModel/Setup/Device/DeviceListViewModel.h"
#include "./Cpp/ViewModel/Setup/Device/ChannelListViewModel.h"
#include "./Cpp/ViewModel/Setup/User/AddUserGroupViewModel.h"
#include "./Cpp/ViewModel/Setup/User/AddUserViewModel.h"
#include "./Cpp/ViewModel/Setup/User/UserGroupViewModel.h"
#include "./Cpp/ViewModel/Setup/User/UserListViewModel.h"
#include "./Cpp/ViewModel/Setup/User/LdapUserViewModel.h"
#include "./Cpp/ViewModel/Setup/Event/EventRulesViewModel.h"
#include "./Cpp/ViewModel/Setup/Event/AddEventRuleViewModel.h"
#include "./Cpp/ViewModel/Setup/Event/EventNotificationsViewModel.h"
#include "./Cpp/ViewModel/Setup/Event/EventScheduleViewModel.h"
#include "./Cpp/ViewModel/Setup/Event/EventEmailViewModel.h"
#include "./Cpp/ViewModel/Setup/Device/AutoDiscoveryViewModel.h"
#include "./Cpp/ViewModel/Setup/Device/ManualDiscoveryViewModel.h"

#include "./Cpp/Model/ShortcutBoxListModel.h"

using namespace WisenetViewerDefine;

//#define WINDOWS_LEAK_TEST


void setupFontFamily(QGuiApplication& app, int language)
{
    SPDLOG_DEBUG("--------setupFontFamily START, language = {}", language);
    QStringList fonts;
    QString fontFamily;

    // 중국어는 폰트 설정 제외
    if (language == WisenetLanguage::Chinese ||
        language == WisenetLanguage::Taiwanese) {
        return; /* do nothing */
    }

    // 한국어/일본어는 NotoSans KR
    if (language == WisenetLanguage::Korean ) {
        QStringList krFonts = {
            ":/Assets/font/Noto_Sans_KR/NotoSansKR-Bold.otf",
            ":/Assets/font/Noto_Sans_KR/NotoSansKR-Regular.otf"

        };
        fonts = krFonts;
        fontFamily = "Noto Sans KR";
    }
    else if(language == WisenetLanguage::Japanese) {
        QStringList jpFonts = {
            ":/Assets/font/Noto_Sans_JP/NotoSansJP-Regular.otf",
            ":/Assets/font/Noto_Sans_JP/NotoSansJP-Bold.otf"
        };
        fonts = jpFonts;
        fontFamily = "Noto Sans JP";
    }
    // 나머지 언어는 NotoSans
    else {
        QStringList enFonts = {
            ":/Assets/font/Noto_Sans/NotoSans-Regular.ttf",
            ":/Assets/font/Noto_Sans/NotoSans-Italic.ttf",
            ":/Assets/font/Noto_Sans/NotoSans-Bold.ttf",
            ":/Assets/font/Noto_Sans/NotoSans-BoldItalic.ttf"
        };
        fonts = enFonts;
        fontFamily = "Noto Sans";
    }

    for (int i = 0 ; i < fonts.size() ; i++) {
        QString fontPath = fonts.at(i);
        auto id = QFontDatabase::addApplicationFont(fontPath);
        if (id < 0) {
            SPDLOG_INFO("Cannot add font, path={}", fontPath.toStdString());
            return;
        }
        else {
            QString family = QFontDatabase::applicationFontFamilies(id).at(0);
            SPDLOG_DEBUG("Add font into font database, path={}, family={}", fontPath.toStdString(), family.toStdString());
        }
    }

    SPDLOG_DEBUG(">> Current Font settings:{}", app.font().toString().toStdString());
    QFont font = QFont(fontFamily);
    app.setFont(font);
    SPDLOG_DEBUG(">> New Font settings:{}", app.font().toString().toStdString());

    SPDLOG_DEBUG("--------setupFontFamily END");
}

bool checkRunning(QSharedMemory& shared)
{
    /* https://doc.qt.io/qt-5/qsharedmemory.html#details
    When using this class, be aware of the following platform differences:
    Windows: QSharedMemory does not "own" the shared memory segment.
            When all threads or processes that have an instance of QSharedMemory
            attached to a particular shared memory segment have either destroyed
            their instance of QSharedMemory or exited,
            the Windows kernel releases the shared memory segment automatically.
    Unix: QSharedMemory "owns" the shared memory segment.
          When the last thread or process that has an instance of QSharedMemory
          attached to a particular shared memory segment detaches from the segment
          by destroying its instance of QSharedMemory,
          the Unix kernel release the shared memory segment.
          But if that last thread or process crashes without running the QSharedMemory destructor,
          the shared memory segment survives the crash.
    HP-UX: Only one attach to a shared memory segment is allowed per process.
          This means that QSharedMemory should not be used across multiple threads
          in the same process in HP-UX.
    */

    SPDLOG_DEBUG("try to create shared memory with {}", shared.key().toStdString());
    if (!shared.create(16, QSharedMemory::ReadWrite)) {
        SPDLOG_DEBUG("shared.create() fail (1)");
        // The failure may be caused by the shm already existing
        shared.attach();
        shared.detach();  // This should delete the shm if no process use it
        if (!shared.create(16, QSharedMemory::ReadWrite)) {
             // We really cannot create the share memory, report the error
            SPDLOG_DEBUG("shared.create() fail (2)");
            return true;
        }
    }

    SPDLOG_DEBUG("shared.create() return success");

    return false;
}

void checkAppDataPathChanged()
{
    QString oldDataLocation = GetUserAppDataLocation();    // 사용자별 설정 폴더 (v1.0.1 이하)
    QString newDataLocation = GetAppDataLocation();    // 공용 설정 폴더 (v1.1.0 이상)

    if (QFile::exists(oldDataLocation + "/management.sqlite") && !QFile::exists(newDataLocation + "/management.sqlite")) {
        // 구버전 폴더에 db 파일이 있고 신버전 폴더에 db 파일이 없으면, 기존 설정파일을 복사 후 삭제
        QDir oldDir;
        oldDir.setPath(oldDataLocation);

        QDir newDir;
        newDir.mkpath(newDataLocation);
        newDir.setPath(newDataLocation);

        // DB, config 파일 복사
        for(auto& fileName : oldDir.entryList(QDir::Files)) {
            QFile::copy(oldDataLocation + "/" + fileName, newDataLocation + "/" + fileName);
        }

        // logs 폴더 복사
        if(QDir(oldDataLocation + "/logs").exists()) {
            oldDir.setPath(oldDataLocation + "/logs");
            newDir.mkdir("logs");
            for(auto& fileName : oldDir.entryList(QDir::Files)) {
                QFile::copy(oldDataLocation + "/logs/" + fileName, newDataLocation + "/logs/" + fileName);
            }
        }

        // 기존 Data 폴더 삭제
        oldDir.setPath(oldDataLocation);
        oldDir.removeRecursively();
    }
}

void checkHanwhaVisionPathChanged()
{
    QString oldDataLocation = GetAppDataLocation();
    SPDLOG_INFO("checkHanwhaVisionPathChanged 1 oldDataLocation={}", oldDataLocation.toStdString());

    if(oldDataLocation.contains("Hanwha Vision"))
        oldDataLocation = oldDataLocation.replace("Hanwha Vision", "Hanwha Techwin");

    SPDLOG_INFO("checkHanwhaVisionPathChanged 2 oldDataLocation={}", oldDataLocation.toStdString());

    QString newDataLocation = GetAppDataLocation();

    if (QFile::exists(oldDataLocation + "/management.sqlite") && !QFile::exists(newDataLocation + "/management.sqlite")) {
        // 구버전 폴더에 db 파일이 있고 신버전 폴더에 db 파일이 없으면, 기존 설정파일을 복사 후 삭제
        QDir oldDir;
        oldDir.setPath(oldDataLocation);

        QDir newDir;
        newDir.mkpath(newDataLocation);
        newDir.setPath(newDataLocation);

        // DB, config 파일 복사
        for(auto& fileName : oldDir.entryList(QDir::Files)) {
            QFile::copy(oldDataLocation + "/" + fileName, newDataLocation + "/" + fileName);
        }

        // logs 폴더 복사
        if(QDir(oldDataLocation + "/logs").exists()) {
            oldDir.setPath(oldDataLocation + "/logs");
            newDir.mkdir("logs");
            for(auto& fileName : oldDir.entryList(QDir::Files)) {
                QFile::copy(oldDataLocation + "/logs/" + fileName, newDataLocation + "/logs/" + fileName);
            }
        }

        // 기존 Data 폴더 삭제
        oldDir.setPath(oldDataLocation);
        oldDir.removeRecursively();
    }
}


#ifdef Q_OS_WINDOWS
class WindowsTestEventFilter : public QAbstractNativeEventFilter
{
public :
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override
    {
        MSG* msg = (MSG*)message;
        if (msg->message == 0x003D) // ignore WM_GETOBJECT (Accessibility Event)
            return true;
        return false;
    }
};
#endif

int main(int argc, char* argv[])
{

    int result = 0;
#ifdef WINDOWS_LEAK_TEST
    int startBreakPoint = 0; // 여기에 시작 브레이크 포인트 후 메모리 캡쳐
    {
#endif

    QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.connections=false"));

    QCoreApplication::setOrganizationName("Hanwha Vision");
    QCoreApplication::setOrganizationDomain("HanwhaVision.com");
    QGuiApplication::setApplicationName("WisenetViewer");
    QString appVersion = "1.04.00";

#ifdef DEVELOP_VERSION
    appVersion += "dv";
#endif
#ifdef BETA_VERSION
    appVersion += "b";
#endif
#ifdef EC_VERSION
    appVersion += "ec";
#endif

    VersionManager::Instance()->setS1Support(false);
#ifdef WISENET_S1_VERSION
    VersionManager::Instance()->setS1Support(true);
    QProcess::startDetached("taskkill", QStringList() << "/im" << "S1SipDaemon.exe" << "/f");
#endif


    QGuiApplication::setApplicationVersion(appVersion);

    WisenetItemType::Register();
    WisenetItemStatus::Register();
    WisenetLanguage::Register();
    WisenetLoginErrorCode::Register();
    WisenetPasswordStrength::Register();
    WisenetRecordFilterType::Register();
    EventLogThread::registerQml();
    AuditLogThread::registerQml();
    GridListThread::registerQml();
    WisenetMediaPlayer::registerQml();
    WisenetSmartSearchType::Register();
    WisenetMaskType::Register();

    qRegisterMetaTypeStreamOperators<QList<QStringList>>("viewer_settings");
    qRegisterMetaTypeStreamOperators<QStringList>("qStringList");
    qmlRegisterType<ProcessModel>("Wisenet.Process", 1, 0, "ProcessModel");
    qmlRegisterType<CalendarTimelineModel>("Wisenet.MediaController", 1, 0, "CalendarTimelineModel");
    qmlRegisterType<MediaControlModel>("Wisenet.MediaController", 1, 0, "MediaControlModel");
    qmlRegisterType<TimeAreaModel>("Wisenet.MediaController", 1, 0, "TimeAreaModel");
    qmlRegisterType<DateAreaModel>("Wisenet.MediaController", 1, 0, "DateAreaModel");
    qmlRegisterType<GraphAreaModel>("Wisenet.MediaController", 1, 0, "GraphAreaModel");
    qmlRegisterType<MultiGraphAreaModel>("Wisenet.MediaController", 1, 0, "MultiGraphAreaModel");
    qmlRegisterType<BookmarkModel>("Wisenet.MediaController", 1, 0, "BookmarkModel");
    qmlRegisterType<ExportVideoModel>("Wisenet.MediaController", 1, 0, "ExportVideoModel");
    qmlRegisterType<RecordTypeFilterViewModel>("Wisenet.MediaController", 1, 0, "RecordTypeFilterViewModel");
    qmlRegisterType<PlaybackSpeedModel>("Wisenet.MediaController", 1, 0, "PlaybackSpeedModel");
    qmlRegisterType<ThumbnailModel>("Wisenet.MediaController", 1, 0, "ThumbnailModel");
    qmlRegisterType<ThumbnailImage>("Wisenet.MediaController", 1, 0, "ThumbnailImage");
    //qmlRegisterType<DeviceMaintenanceViewModel>("Wisenet.Device", 1, 0, "DeviceMaintenanceViewModel");


    qmlRegisterType<DeviceListModel>("Wisenet.DeviceListModel",1,0,"DeviceListModel");
    qmlRegisterType<DeviceSelectionTreeSourceModel>("Wisenet.DeviceSelectionTreeSourceModel",1,0,"DeviceSelectionTreeSourceModel");
    qmlRegisterType<TreeProxyModel>("Wisenet.ResourceTreeProxyModel", 1, 0, "ResourceTreeProxyModel");
    qmlRegisterType<EventFilterTreeSourceModel>("Wisenet.EventFilterTreeSourceModel", 1, 0, "EventFilterTreeSourceModel");
    qmlRegisterType<EventLogViewModel>("Wisenet.EventLogViewModel", 1, 0, "EventLogViewModel");
    qmlRegisterType<SystemLogViewModel>("Wisenet.SystemLogViewModel",1,0, "SystemLogViewModel");
    qmlRegisterType<UserFilterTreeSourceModel>("Wisenet.UserFilterTreeSourceModel", 1, 0, "UserFilterTreeSourceModel");
    qmlRegisterType<AuditLogViewModel>("Wisenet.AuditLogViewModel", 1, 0, "AuditLogViewModel");
    qmlRegisterType<ImageItem>("Wisenet.ImageItem", 1, 0, "ImageItem");
    qmlRegisterType<DeviceFilterTreeSourceModel>("Wisenet.DeviceFilterTreeSourceModel",1,0,"DeviceFilterTreeSourceModel");
    qmlRegisterType<DevicePhysicalTreeSourceModel>("Wisenet.Tree",1,0,"DevicePhysicalTreeSourceModel");
    qmlRegisterType<DeviceAlarmOutTreeSourceModel>("Wisenet.Tree",1,0,"DeviceAlarmOutTreeSourceModel");
    qmlRegisterType<DeviceEmailTreeSourceModel>("Wisenet.Tree",1,0,"DeviceEmailTreeSourceModel");
    qmlRegisterType<DeviceAlarmInTreeSourceModel>("Wisenet.Tree",1,0,"DeviceAlarmInTreeSourceModel");
    qmlRegisterType<DeviceEventTreeSourceModel>("Wisenet.Tree",1,0,"DeviceEventTreeSourceModel");
    qmlRegisterType<UserLayoutTreeSourceModel>("Wisenet.UserLayoutTreeSourceModel",1,0,"UserLayoutTreeSourceModel");

    qmlRegisterType<DeviceListTreeSourceModel>("Wisenet.DeviceListTreeSourceModel",1,0,"DeviceListTreeSourceModel");

    qmlRegisterType<SetupMainTreeFilterProxyModel>("Wisenet.Setup", 1, 0, "TreeProxyModel");
    qmlRegisterType<SetupMainTreeViewModel>("Wisenet.Setup", 1, 0, "TreeModel");
    qmlRegisterType<SetupTreeNode>("Wisenet.Setup", 1, 0, "TreeElement");

    qmlRegisterType<AutoDiscoveryViewModel>("Wisenet.Setup",1,0,"AutoDiscoveryViewModel");
    qmlRegisterType<ManualDiscoveryViewModel>("Wisenet.Setup",1,0,"ManualDiscoveryViewModel");
    qmlRegisterType<DeviceConfigViewModel>("Wisenet.Setup",1,0,"DeviceConfigViewModel");

    qmlRegisterType<DeviceListViewModel>("Wisenet.Setup",1,0,"DeviceListViewModel");
    qmlRegisterType<ChannelListViewModel>("Wisenet.Setup",1,0,"ChannelListViewModel");
    qmlRegisterType<AddUserGroupViewModel>("Wisenet.Setup",1,0,"AddUserGroupViewModel");
    qmlRegisterType<UserListViewModel>("Wisenet.Setup",1,0,"UserListViewModel");
    qmlRegisterType<UserGroupViewModel>("Wisenet.Setup",1,0,"UserGroupViewModel");
    qmlRegisterType<AddUserViewModel>("Wisenet.Setup",1,0,"AddUserViewModel");

    qmlRegisterType<EventRulesViewModel>("Wisenet.Setup",1,0,"EventRulesViewModel");
    qmlRegisterType<EventNotificationsViewModel>("Wisenet.Setup",1,0,"EventNotificationsViewModel");
    qmlRegisterType<AddEventRuleViewModel>("Wisenet.Setup",1,0,"AddEventRuleViewModel");

    qmlRegisterType<EventScheduleViewModel>("Wisenet.Setup", 1,0,"EventScheduleViewModel");

    qmlRegisterType<EventEmailViewModel>("Wisenet.Setup", 1, 0, "EventEmailViewModel");

    qmlRegisterType<TextSearchViewModel>("Wisenet.TextSearchViewModel", 1, 0,"TextSearchViewModel");
    qmlRegisterType<TextItemViewModel>("Wisenet.TextItemViewModel", 1, 0,"TextItemViewModel");

    qmlRegisterType<DragItemListModel>("WisenetMediaFramework",1,0,"DragItemListModel");
    qmlRegisterType<MainViewModel>("WisenetMediaFramework",1,0,"MainViewModel");

    qRegisterMetaType<MainViewModel::ViewerMode>("MainViewModel::ViewerMode");

    qmlRegisterType<LdapUserViewModel>("Wisenet.Setup", 1,0,"LdapUserViewModel");
    qmlRegisterType<ShortcutBoxListModel>("Wisenet.Shortcut",1,0,"ShortcutBoxListModel");

    // check AppDataPath changed
    checkAppDataPathChanged();
    checkHanwhaVisionPathChanged();

    // settings loading
    auto setting = QCoreServiceManager::Instance().Settings();
    auto decoderManager = DecoderManager::getInstance();

    if (setting != nullptr)
    {
        std::mutex mutex;
        mutex.lock();
        setting->loadSetting();
        mutex.unlock();

        if(decoderManager != nullptr) {
            decoderManager->SetUseHwDecoding(setting->useHwDecoding());
            decoderManager->SetQsvCount(setting->qsvChannelCount());
            decoderManager->SetCudaCount(setting->cudaChannelCount());
            decoderManager->SetToolboxCount(setting->toolboxChannelCount());
        }
    }

    QObject::connect(DecoderManager::getInstance(), &DecoderManager::cudaChannelCountChanged, QCoreServiceManager::Instance().Settings(), &QLocalSettings::saveCudaChannelCount);
    QObject::connect(DecoderManager::getInstance(), &DecoderManager::qsvChannelCountChanged, QCoreServiceManager::Instance().Settings(), &QLocalSettings::saveQsvChannelCount);
    QObject::connect(DecoderManager::getInstance(), &DecoderManager::toolboxChannelCountChanged, QCoreServiceManager::Instance().Settings(), &QLocalSettings::saveToolboxChannelCount);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#ifdef Q_OS_WINDOWS
        // windows인 경우 dpi 1.5일때 2.0으로 default 계산되는 것을 1.5 그대로 처리하기를 원할 때
        if (setting != nullptr) {
            if (setting->dpiPassThrough()) {
                QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
            }
        }
#endif
#endif

#ifdef Q_OS_WINDOWS // mac은 테스트 후에 넣자.
        QtWebEngine::initialize();
#endif

        Q_INIT_RESOURCE(WisenetStyle);
        Q_INIT_RESOURCE(WisenetMediaFramework);
        Q_INIT_RESOURCE(WisenetLanguage);

        QGuiApplication app(argc, argv);

#ifdef Q_OS_WINDOWS
        WindowsTestEventFilter winNativeEventFilter;
        app.installNativeEventFilter(&winNativeEventFilter);
#endif

    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    // disable file logging (--disable-flog)
    const QCommandLineOption disableFlogOption("disable-flog", "disable file logging");
    const QCommandLineOption restartOption("restart", "restart sw internally");
    parser.addOption(disableFlogOption);
    parser.addOption(restartOption);
    parser.process(app);
    bool forceDisablefLog = parser.isSet(disableFlogOption);
    bool isRestart = parser.isSet(restartOption);

    QString logPath = GetAppDataLocation();
    logPath += "/logs";
    QString nativePath = QDir::toNativeSeparators(logPath);
    Wisenet::Common::QtInitializeLogSettings(nativePath.toLocal8Bit().toStdString(), forceDisablefLog);

    // 언어설정 및 언어별 폰트 설정
    auto languageManager = LanguageManager::getInstance();
    if (languageManager != nullptr) {
        if(!QFile::exists(GetAppDataLocation()+"/viewer_settings.ini"))
            languageManager->setLanguageBySystemLocale();
        else
            languageManager->setLanguage(WisenetLanguage::Convert(LocalSettingViewModel::getInstance()->language()));

        QLocaleManager::Instance()->setLocale(languageManager->getCurrentLanguageString());
        SPDLOG_INFO("Set Language to {}", languageManager->getLanguageString(languageManager->language()).toStdString());
        setupFontFamily(app, languageManager->language());
    }

    // macOS 11버전 미만일 경우 강제종료
    QOperatingSystemVersion macOSVersion = QOperatingSystemVersion::current();
    if(macOSVersion.currentType()== QOperatingSystemVersion::MacOS && macOSVersion.majorVersion() < 11)
    {
        SPDLOG_INFO("macOS version is under 11. Show popup and exit the program.");
        QQmlApplicationEngine engine;
        engine.addImportPath("qrc:/");
        engine.load(QUrl(QStringLiteral("qrc:/Qml/View/MacOSExitDialog.qml")));

        if (engine.rootObjects().isEmpty()) {
            SPDLOG_INFO("engine load failed");
            return -1;
        }
        return app.exec();
    }

    // 프로그램 중복 실행 확인
    const QString kShm_key = "DEFD9B22-9CC5-4140-9A64-9386A20B3310";
    QSharedMemory shared(kShm_key);

    int checkCount = 0;
    while (checkRunning(shared)) {
        if (isRestart) {
            SPDLOG_INFO("Program is still running.. check restart option, PID={}, SHM_KEY={}", QGuiApplication::applicationPid(), kShm_key.toStdString());
            if (checkCount < 10) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                checkCount++;
                continue;
            }
        }
        SPDLOG_INFO("Program is already running.. exit process, PID={}, SHM_KEY={}", QGuiApplication::applicationPid(), kShm_key.toStdString());
        QQmlApplicationEngine engine;
        engine.addImportPath("qrc:/");
        engine.addImportPath("qrc:/WisenetStyle/");
        engine.addImportPath("qrc:/MediaController/");
        engine.addImportPath("qrc:/WisenetLanguage/");
        engine.load(QUrl(QStringLiteral("qrc:/Qml/View/AlreadyRunningDialog.qml")));
        if (engine.rootObjects().isEmpty()) {
            SPDLOG_INFO("engine load failed");
            return -1;
        }
        return app.exec();
    }

#ifdef Q_OS_LINUX
        QString srcFolder = QCoreApplication::applicationDirPath() + "/Manual/";
        QString dstFolder = GetDefaultMediaLocation() + "/Manual/";
        QDir docManual(dstFolder);

        // if there is a Manual directory in Document Directory, remove the Manual directory
        if(docManual.exists()) {
            SPDLOG_DEBUG("Remove the Manual directory in Documents");
            docManual.removeRecursively();
        }

        // copy new Manual directory to Document
        QStringList args = QStringList() << "-r" << srcFolder << dstFolder ;
        int execResult = QProcess::execute("cp", args);
        if(result == -1 || result == -2)
            SPDLOG_DEBUG("Failed to copy Manual directory to {} with fail code {}", dstFolder.toStdString(), execResult);
#endif

    // 여기에 버전정보, OS 정보 등 추가
    auto currentOS = QOperatingSystemVersion::current();
    SPDLOG_INFO("");
    SPDLOG_CRITICAL("=====================================================================");
    SPDLOG_CRITICAL(" START WISENET VIEWER BY HANWHA VISION");
    SPDLOG_CRITICAL(" APP VER={}, ABI={}, DATE={}, PID={}, RESTART={}",
                    QGuiApplication::applicationVersion().toStdString(),
                    QSysInfo::buildAbi().toStdString(),
                    __DATE__, QGuiApplication::applicationPid(), isRestart);
    SPDLOG_CRITICAL(" - OS = {} / {}.{}.{}, ProductName = {}",
                    currentOS.name().toStdString(),
                    currentOS.majorVersion(), currentOS.minorVersion(), currentOS.microVersion(),
                    QSysInfo::prettyProductName().toStdString());
    SPDLOG_CRITICAL(" - CPU = {}, Kernel Type = {}, Version = {}",
                    QSysInfo::currentCpuArchitecture().toStdString(),
                    QSysInfo::kernelType().toStdString(),
                    QSysInfo::kernelVersion().toStdString());
    SPDLOG_CRITICAL("=====================================================================");
    SPDLOG_INFO("");
    SPDLOG_INFO("Log initialize, flag={}", forceDisablefLog);

#ifdef WISENET_S1_VERSION
    QString daemonDir = QDir::currentPath() + "/S1SipDaemon/S1SipDaemon.exe";
    unsigned short daemonServerPort = QCoreServiceManager::Instance().Settings()->s1SipDaemonPort();

    QCoreServiceManager::Instance().SetS1DaemonPort(daemonServerPort);
    QStringList daemonArg;
    daemonArg.append(logPath);
    daemonArg.append(QString::number(daemonServerPort));
    SPDLOG_INFO("[S1 SIP] dir[{}] log dir[{}] server port[{}]", daemonDir.toStdString(), logPath.toStdString(), daemonServerPort);
    int executeResult = QProcess::startDetached(daemonDir, daemonArg);
    SPDLOG_INFO("[S1 SIP] Daemon execute result: {}", executeResult);
#endif

    QCoreServiceManager::Instance().Start();
    QCoreServiceManager::Instance().getUpdateFwInfoFile();

    WisenetViewer viewer;

    result = app.exec();

    SPDLOG_INFO("=========== Wisenet Viewer STOP ============");
    QCoreServiceManager::Instance().Stop();

    QCoreServiceManager::Release();
#ifdef WINDOWS_LEAK_TEST
    }
    int endBreakPoint = 0; // 여기에 끝 브레이크 포인트 후 메모리 캡쳐
#endif
    return result;
}
