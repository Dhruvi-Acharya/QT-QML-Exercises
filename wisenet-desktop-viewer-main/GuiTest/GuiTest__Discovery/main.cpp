/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "QCoreServiceManager.h"
#include "LogSettings.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2>
#include <bpptablemodel.h>
#include <bppfontawesome.h>

#include "AutoDiscoveryModel.h"


int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("GuidTest__Discovery");
    QCoreApplication::setOrganizationDomain("hanwha-security.com");
    QCoreApplication::setOrganizationName("Hanwha Techwin");

    Wisenet::Common::InitializeLogSettings("logs");
    QCoreServiceManager::Instance().Start();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    bpp::TableModel::registerQml();
    bpp::FontAwesome::registerQml(engine);
    engine.addImportPath("qrc:/");

    AutoDiscoveryModel::registerQml();
    AutoDiscoveryModel autoDiscoveryModel;
    engine.rootContext()->setContextProperty("_autoDiscoveryModel", &autoDiscoveryModel);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
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
