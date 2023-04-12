#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include <QtWebEngine>

#include "MainModel.h"
#include "QLogSettings.h"
#include "WisenetMediaPlayer.h"


void InitService()
{
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    logPath += "/logs";
    QString nativePath = QDir::toNativeSeparators(logPath);
    Wisenet::Common::QtInitializeLogSettings(nativePath.toLocal8Bit().toStdString());
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setOrganizationName("Hanwha Techwin");
    QCoreApplication::setOrganizationDomain("hanwha-security.com");
    QGuiApplication::setApplicationName("ViewingGridTest");

    // https://doc.qt.io/qt-5/qtwebengine-debugging.html
    // https://myprogrammingnotes.com/pass-parameters-chromium-qtwebengine.html

    //--ignore-certificate-errors --ignore-ssl-errors --log-level=3
    //qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--ignore-gpu-blacklist" );
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-accelerated-video-decode --disable-accelerated-video-encode --disable-software-video-decoders --disable-accelerated-2d-canvas" );
    //qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu");
    QtWebEngine::initialize();
#ifdef Q_OS_MACOS // TESTING
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setMajorVersion(4);
    surfaceFormat.setMinorVersion(1);
    surfaceFormat.setProfile(QSurfaceFormat::NoProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);
#endif

    int result = 0;
    {
        QGuiApplication app(argc, argv);

        Q_INIT_RESOURCE(WisenetStyle);
        Q_INIT_RESOURCE(WisenetMediaFramework);
        Q_INIT_RESOURCE(WisenetLanguage);

        InitService();
        //QLoggingCategory::setFilterRules(QStringLiteral("qt.multimedia.video.debug=true"));
    
        QQmlApplicationEngine engine;
        engine.addImportPath("qrc:/");
        engine.addImportPath("qrc:/WisenetStyle/");
        engine.addImportPath("qrc:/MediaController/");
        engine.addImportPath("qrc:/WisenetLanguage/");

        WisenetMediaPlayer::registerQml();
        WisenetSmartSearchType::Register();

        MainModel service;
        engine.rootContext()->setContextProperty("serviceModel", &service);

        const QUrl url(QStringLiteral("qrc:/main.qml"));
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
        engine.load(url);

        service.serviceStart();

        result = app.exec();
    }
    QCoreServiceManager::Instance().Stop();
    QCoreServiceManager::Release();
    return result;
}
