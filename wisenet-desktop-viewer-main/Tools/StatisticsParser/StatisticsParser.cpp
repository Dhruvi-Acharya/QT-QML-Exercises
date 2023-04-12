#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Cpp/MainViewModel.h"
#include "Cpp/FilePathConveter.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setOrganizationName("Hanwha Techwin");
    QCoreApplication::setOrganizationDomain("hanwha-security.com");
    QGuiApplication app(argc, argv);

    MainViewModel mainViewModel;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("mainViewModel", &mainViewModel);
    engine.rootContext()->setContextProperty("filePathConveter", FilePathConveter::getInstance());

    const QUrl url(QStringLiteral("qrc:/StatisticsParser.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
