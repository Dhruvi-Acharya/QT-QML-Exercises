#include "multiwindow.h"


MultiWindow::MultiWindow(QObject *parent) :
    QObject(parent)
{
}

MultiWindow::MultiWindow(QString key, MultiWindowHandler* handler, QObject *parent) :
    QObject(parent)
{    
    engine = new QQmlApplicationEngine();

    bpp::FontAwesome::registerQml(*engine);
    engine->addImportPath("qrc:/");

    layoutTabBehaviors = new LayoutTabBehaviors();
    layoutTabModel = new LayoutTabModel();
    autoDiscoveryModel = new AutoDiscoveryModel();

    engine->rootContext()->setContextProperty("layoutTabBehaviors", layoutTabBehaviors);
    engine->rootContext()->setContextProperty("layoutTabModel", layoutTabModel);
    layoutTabBehaviors->setLayoutTabModel(layoutTabModel);
    engine->rootContext()->setContextProperty("_autoDiscoveryModel", autoDiscoveryModel);
    engine->rootContext()->setContextProperty("windowHanlder", handler);
    engine->rootContext()->setContextProperty("windowGuid", key);
    engine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
}

MultiWindow::~MultiWindow()
{
    if(layoutTabBehaviors != nullptr)
        layoutTabBehaviors->deleteLater();

    if(layoutTabModel != nullptr)
        layoutTabModel->deleteLater();

    if(autoDiscoveryModel != nullptr)
        autoDiscoveryModel->deleteLater();

    engine->deleteLater();

    qDebug() << "qml: ~MultiWindow()";
}
