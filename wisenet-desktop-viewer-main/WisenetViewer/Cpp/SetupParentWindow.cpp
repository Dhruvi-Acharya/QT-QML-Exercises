#include "SetupParentWindow.h"
#include "LanguageManager.h"
#include "DecoderManager.h"
#include "QLocaleManager.h"
#include "FilePathConveter.h"
#include "ViewModel/Setup/System/SystemMaintenanceViewModel.h"
#include "ViewModel/Setup/System/LocalSettingViewModel.h"
#include "Utility/PasswordStrengthChecker.h"
#include "LogoManager.h"
#include "ControllerManager.h"
#include "VersionManager.h"

SetupParentWindow::SetupParentWindow(QObject *parent,  PopupParentWindow* popupParent)
    : QObject(parent)
{
    engine.addImportPath("qrc:/");

    engine.rootContext()->setContextProperty("filePathConveter", FilePathConveter::getInstance());
    engine.rootContext()->setContextProperty("localSettingViewModel", LocalSettingViewModel::getInstance());
    engine.rootContext()->setContextProperty("systemMaintenanceViewModel", SystemMaintenanceViewModel::getInstance());

    engine.rootContext()->setContextProperty("languageManager", LanguageManager::getInstance());
    engine.rootContext()->setContextProperty("localeManager", QLocaleManager::Instance());
    engine.rootContext()->setContextProperty("versionManager", VersionManager::Instance());
    engine.rootContext()->setContextProperty("decoderManager", DecoderManager::getInstance());
    engine.rootContext()->setContextProperty("gControllerManager", ControllerManager::Instance());
    engine.rootContext()->setContextProperty("setupService", this);
    engine.rootContext()->setContextProperty("passwordStrengthChecker", PasswordStrengthChecker::getInstance());
    engine.rootContext()->setContextProperty("logoManager", LogoManager::getInstance());

    engine.load(QUrl(QStringLiteral("qrc:/Qml/View/SetupParent.qml")));

    connect(this, &SetupParentWindow::setupMainView_openP2p, popupParent, &PopupParentWindow::openP2pView);

    initializeTree();
}
