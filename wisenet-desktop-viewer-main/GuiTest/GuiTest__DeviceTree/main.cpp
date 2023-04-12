#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2>

#include "devicetreeitemtype.h"
#include "devicetreeitem.h"
#include "devicetreemodel.h"
#include <bppfontawesome.h>
#include "MainTreeModel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<DeviceTreeModel>("devicetreemodel.wisenet.com", 1, 0, "DeviceTreeModel");
    qmlRegisterType<DeviceTreeItemType>("devicetreeitemtype.wisenet.com", 1, 0, "DeviceTreeItemType");

    qmlRegisterType<MainTreeModel>("Wisenet.Qmls", 0, 1, "MainTreeModel");

    QQmlApplicationEngine engine;

    bpp::FontAwesome::registerQml(engine);
    engine.addImportPath("qrc:/");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
