#include "QCoreServiceManager.h"
#include "DatabaseManager.h"
#include "LogManager.h"

#include "QLogSettings.h"
#include "bpptablemodel.h"
#include "bppfontawesome.h"
#include "alarm/AlarmModel.h"
#include "setup/AutoDiscoveryModel.h"
#include "tree/MainTreeModel.h"
#include "tree/MainTreeSortFilterProxyModel.h"
#include "login/LoginModel.h"
#include "layoutNavigator/layouttabbehaviors.h"
#include "layoutNavigator/layouttabmodel.h"

#include "windowHandler/multiwindowhandler.h"
//#include "video/QVideoQuickItem.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2>

#include <bpptablemodel.h>
#include <bppfontawesome.h>

#include <setup/AutoDiscoveryModel.h>
#include <setup/ManualDiscoveryModel.h>
#include <tree/MainTreeModel.h>
#include <tree/MainTreeSortFilterProxyModel.h>
#include <login/LoginModel.h>
#include <layoutNavigator/layouttabbehaviors.h>
#include <layoutNavigator/layouttabmodel.h>

#include <windowHandler/multiwindowhandler.h>
#include "MediaController/CalendarTimelineModel.h"
#include "MediaController/MediaControlModel.h"
#include "MediaController/TimeAreaModel.h"
#include "MediaController/DateAreaModel.h"
#include "MediaController/GraphAreaModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("GuidTest__TestClient");
    QCoreApplication::setOrganizationDomain("hanwha-security.com");
    QCoreApplication::setOrganizationName("Hanwha Techwin");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    Wisenet::Common::QtInitializeLogSettings("logs");

    // .exe reset to remove DB
    SPDLOG_INFO("argc={}", argc);
    for (int i = 0; i < argc ; i++)
        SPDLOG_INFO("argv{}={}", i, argv[i]);
    if (argc == 2 && strcmp(argv[1], "reset") == 0) {
        Wisenet::Core::DatabaseManager::Delete();
        Wisenet::Core::LogManager::Delete();
    }

    QCoreServiceManager::Instance().Start();



    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");

    qmlRegisterType<MainTreeModel>("Wisenet.Qmls", 0, 1, "MainTreeModel");
    MainTreeModel mainTreeModel;
    engine.rootContext()->setContextProperty("_mainTreeModel", &mainTreeModel);
    qmlRegisterType<MainTreeSortFilterProxyModel>("Wisenet.Qmls", 0, 1, "MainTreeSortFilterProxyModel");

    //qmlRegisterType<QVideoQuickItem>("Wisenet.Qmls", 0, 1, "QVideoQuickItem");

    bpp::TableModel::registerQml();
    bpp::FontAwesome::registerQml(engine);

    AutoDiscoveryModel::registerQml();
    AutoDiscoveryModel autoDiscoveryModel;
    engine.rootContext()->setContextProperty("_autoDiscoveryModel", &autoDiscoveryModel);

    ManualDiscoveryModel::registerQml();
    ManualDiscoveryModel manualDiscoveryModel;
    engine.rootContext()->setContextProperty("_manualDiscoveryModel", &manualDiscoveryModel);

    LoginModel::registerQml();
    LoginModel loginModel;
    engine.rootContext()->setContextProperty("loginModel", &loginModel);

    MultiWindowHandler multiWindowHanlder;
    engine.rootContext()->setContextProperty("windowHanlder", &multiWindowHanlder);

    LayoutTabBehaviors layoutTabBehaviors;
    LayoutTabModel layoutTabModel;
    engine.rootContext()->setContextProperty("layoutTabBehaviors", &layoutTabBehaviors);
    engine.rootContext()->setContextProperty("layoutTabModel", &layoutTabModel);
    layoutTabBehaviors.setLayoutTabModel(&layoutTabModel);

    AlarmModel alarmModel;
    engine.rootContext()->setContextProperty("alarmModel", &alarmModel);

    CalendarTimelineModel::registerQml();
    MediaControlModel::registerQml();

    QString key = QUuid::createUuid().toString();
    engine.rootContext()->setContextProperty("windowGuid", key);

    qmlRegisterType<TimeAreaModel>("Wisenet.Qmls", 0, 1, "TimeAreaModel");
    qmlRegisterType<DateAreaModel>("Wisenet.Qmls", 0, 1, "DateAreaModel");
    qmlRegisterType<GraphAreaModel>("Wisenet.Qmls", 0, 1, "GraphAreaModel");

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    int ret = app.exec();

    QCoreServiceManager::Instance().Stop();
    return ret;
}
