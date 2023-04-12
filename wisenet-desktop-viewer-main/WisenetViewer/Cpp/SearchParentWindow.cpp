#include "SearchParentWindow.h"
#include "LanguageManager.h"
#include "DecoderManager.h"
#include "QLocaleManager.h"
#include "VersionManager.h"
#include "PosMetaManager.h"
#include "FilePathConveter.h"
#include "Model/UserGroupModel.h"
#include "ViewModel/LoginViewModel.h"
#include "ViewModel/Event/EventListViewModel.h"
#include "ViewModel/ObjectSearch/ObjectSearchViewModel.h"
#include "ViewModel/SmartSearch/SmartSearchViewModel.h"
#include "ViewModel/TextSearch/TextSearchViewModel.h"
#include "ViewModel/Setup/System/LocalSettingViewModel.h"
#include "ControllerManager.h"
#include "LogoManager.h"
#include "MediaController/ExportVideoProgressModel.h"

#include <QDebug>

SearchParentWindow::SearchParentWindow(QObject *parent, PopupParentWindow* popupParent)
    : QObject(parent)
{
    engine.addImportPath("qrc:/");

    engine.rootContext()->setContextProperty("filePathConveter", FilePathConveter::getInstance());
    engine.rootContext()->setContextProperty("loginViewModel", LoginViewModel::getInstance());
    engine.rootContext()->setContextProperty("userGroupModel", UserGroupModel::getInstance());
    engine.rootContext()->setContextProperty("eventListViewModel", EventListViewModel::getInstance());
    engine.rootContext()->setContextProperty("objectSearchViewModel", ObjectSearchViewModel::getInstance());
    engine.rootContext()->setContextProperty("smartSearchViewModel", SmartSearchViewModel::getInstance());
    engine.rootContext()->setContextProperty("textSearchViewModel", TextSearchViewModel::getInstance());

    engine.rootContext()->setContextProperty("languageManager", LanguageManager::getInstance());
    engine.rootContext()->setContextProperty("localeManager", QLocaleManager::Instance());
    engine.rootContext()->setContextProperty("versionManager", VersionManager::Instance());
    engine.rootContext()->setContextProperty("decoderManager", DecoderManager::getInstance());
    engine.rootContext()->setContextProperty("gControllerManager", ControllerManager::Instance());
    engine.rootContext()->setContextProperty("gPosMetaManager", PosMetaManager::Instance());
    engine.rootContext()->setContextProperty("logoManager", LogoManager::getInstance());
    engine.rootContext()->setContextProperty("localSettingViewModel", LocalSettingViewModel::getInstance());
    engine.rootContext()->setContextProperty("exportVideoProgressModel", ExportVideoProgressModel::getInstance());
    engine.rootContext()->setContextProperty("searchService", this);

    engine.load(QUrl(QStringLiteral("qrc:/Qml/View/SearchParent.qml")));

    // connect Export progress view signals
    connect(this, &SearchParentWindow::exportProgressView_Open, popupParent, &PopupParentWindow::exportProgressView_Open);
    connect(this, &SearchParentWindow::exportProgressView_ForceClose, popupParent, &PopupParentWindow::exportProgressView_ForceClose);

    initializeTree();
}
