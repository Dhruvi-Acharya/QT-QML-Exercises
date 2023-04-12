#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>

#include "WisenetMediaPlayer.h"
#include "QLocaleManager.h"
#include "MainViewModel.h"
#include "WisenetViewerDefine.h"
#include "DummyUserGroupModel.h"
#include "DummyLoginViewModel.h"
#include "Resource/ResourceViewModel.h"
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
#include "MediaController/ThumbnailImage.h"
#include "MediaController/ThumbnailModel.h"
#include "AudioOutputManager.h"
#include "ControllerManager.h"
#include "LanguageManager.h"
#include "DecoderManager.h"
#include "AppLocalDataLocation.h"
#include "QLogSettings.h"
#include "LogoManager.h"
#include "VersionManager.h"

using namespace WisenetViewerDefine;

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

    // 한국어는 NotoSans KR
    if (language == WisenetLanguage::Korean) {
        QStringList krFonts = {
            ":/font/NotoSansKR-Bold.otf",
            ":/font/NotoSansKR-Regular.otf"
        };
        fonts = krFonts;
        fontFamily = "Noto Sans KR";
    }
    // 일본어는 NotoSans JP
    else if(language == WisenetLanguage::Japanese) {
        QStringList jpFonts = {
            ":/font/NotoSansJP-Regular.otf",
            ":/font/NotoSansJP-Bold.otf"
        };
        fonts = jpFonts;
        fontFamily = "Noto Sans JP";
    }
    // 나머지 언어는 NotoSans
    else {
        QStringList enFonts = {
            ":/font/NotoSans-Regular.ttf",
            ":/font/NotoSans-Italic.ttf",
            ":/font/NotoSans-Bold.ttf",
            ":/font/NotoSans-BoldItalic.ttf"
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


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QCoreApplication::setOrganizationName("Hanwha Techwin");
    QCoreApplication::setOrganizationDomain("hanwha-security.com");
    QGuiApplication::setApplicationName("Wisenet Player");
    QGuiApplication::setApplicationVersion("1.00.01");

    QGuiApplication app(argc, argv);

#ifdef Q_OS_WINDOWS
    WindowsTestEventFilter winNativeEventFilter;
    app.installNativeEventFilter(&winNativeEventFilter);
#endif

    // 실행 argunemt에서 파일 경로 Parsing
    QStringList arguments = app.arguments();
    QString filePathKey = "filepath=";
    QString filePath = "";
    for(auto& arg : arguments) {
        if(arg.startsWith(filePathKey)) {
            filePath = arg.right(arg.length() - filePathKey.length());
        }
    }

    QString logPath = GetAppDataLocation();
    logPath += "/logs";
    QString nativePath = QDir::toNativeSeparators(logPath);
    Wisenet::Common::QtInitializeLogSettings(nativePath.toLocal8Bit().toStdString(), false);

    SPDLOG_INFO("filePath:{}", filePath.toLocal8Bit().toStdString());

    // 초기 언어 로딩
    LanguageManager languageManager;
    languageManager.setLanguageBySystemLocale();
    QLocaleManager::Instance()->setLocale(languageManager.getLanguageString(languageManager.language()));
    setupFontFamily(app, languageManager.language());

    QQmlApplicationEngine engine;

    QObject::connect(&languageManager, &LanguageManager::languageChanged,
                     &app, [&app, &languageManager, &engine]() {
        // 언어 변경 실시간 반영
        QLocaleManager::Instance()->setLocale(languageManager.getLanguageString(languageManager.language()));
        setupFontFamily(app, languageManager.language());
        engine.retranslate();
    }, Qt::QueuedConnection);

    engine.addImportPath("qrc:/");
    engine.addImportPath("qrc:/WisenetStyle/");
    engine.addImportPath("qrc:/WisenetLanguage/");
    Q_INIT_RESOURCE(WisenetMediaFramework);
    Q_INIT_RESOURCE(WisenetStyle);
    Q_INIT_RESOURCE(WisenetLanguage);

    WisenetItemType::Register();
    WisenetItemStatus::Register();
    WisenetLanguage::Register();
    WisenetRecordFilterType::Register();
    WisenetMediaPlayer::registerQml();
    WisenetSmartSearchType::Register();
    WisenetMaskType::Register();
    qmlRegisterType<DragItemListModel>("WisenetMediaFramework",1,0,"DragItemListModel");
    qmlRegisterType<MainViewModel>("WisenetPlayer",1,0,"MainViewModel");
    qRegisterMetaType<MainViewModel::ViewerMode>("MainViewModel::ViewerMode");
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
    qmlRegisterType<ThumbnailImage>("Wisenet.MediaController", 1, 0, "ThumbnailImage");
    qmlRegisterType<ThumbnailModel>("Wisenet.MediaController", 1, 0, "ThumbnailModel");
    qmlRegisterSingletonType(QUrl("qrc:/MediaController/ExportVideoProgressView.qml"), "WisenetSingletonPopup", 1, 0, "ExportVideoProgressWindow");

    engine.rootContext()->setContextProperty("languageManager", &languageManager);
    engine.rootContext()->setContextProperty("localeManager", QLocaleManager::Instance());
    engine.rootContext()->setContextProperty("versionManager", VersionManager::Instance());

    DecoderManager::getInstance()->SetUseHwDecoding(false);
    DecoderManager::getInstance()->SetQsvCount(8);
    DecoderManager::getInstance()->SetCudaCount(16);
    DecoderManager::getInstance()->SetToolboxCount(0);  // Toolbox는 현재 미지원
    engine.rootContext()->setContextProperty("decoderManager", DecoderManager::getInstance());

    QString buildDate = __DATE__;
    MainViewModel::getInstance()->setBuildDate(buildDate);
    engine.rootContext()->setContextProperty("mainViewModel", MainViewModel::getInstance());

#ifdef Q_OS_MACOS
    filePath = QCoreApplication::applicationDirPath().toLocal8Bit() + "/../../../";
#endif
    ResourceViewModel resourceViewModel;
    resourceViewModel.initializeTree(filePath);
    engine.rootContext()->setContextProperty("resourceViewModel", &resourceViewModel);

    DummyUserGroupModel dummyUserGroupModel;
    engine.rootContext()->setContextProperty("userGroupModel", &dummyUserGroupModel);

    DummyLoginViewModel dummyLoginViewModel;
    engine.rootContext()->setContextProperty("loginViewModel", &dummyLoginViewModel);
    engine.rootContext()->setContextProperty("gAudioManager", AudioOutputManager::Instance());
    engine.rootContext()->setContextProperty("gControllerManager", ControllerManager::Instance());
    engine.rootContext()->setContextProperty("logoManager", LogoManager::getInstance());

    const QUrl url(QStringLiteral("qrc:/MainWindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
