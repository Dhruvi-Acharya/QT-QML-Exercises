#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qtwebengineglobal.h>
#include "CameraSettingModel.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QtWebEngine::initialize();
    CameraSettingModel cameraSettingModel(&engine, &app);

    //app.setWindowIcon(QIcon());
    return app.exec();
}
