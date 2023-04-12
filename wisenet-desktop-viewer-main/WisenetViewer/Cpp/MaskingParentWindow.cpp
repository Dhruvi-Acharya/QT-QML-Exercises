#include "MaskingParentWindow.h"
#include "LanguageManager.h"
#include "DecoderManager.h"
#include "QLocaleManager.h"
#include "VersionManager.h"
#include "PosMetaManager.h"
#include "FilePathConveter.h"
#include "Model/UserGroupModel.h"
#include "ViewModel/LoginViewModel.h"
#include "ViewModel/Setup/System/LocalSettingViewModel.h"
#include "ControllerManager.h"
#include "LogoManager.h"
#include "MediaController/ExportVideoProgressModel.h"
#include "Utility/PasswordStrengthChecker.h"

#include <QDebug>

MaskingParentWindow::MaskingParentWindow(QObject *parent)
    : QObject(parent)
{
    engine.addImportPath("qrc:/");

    engine.rootContext()->setContextProperty("filePathConveter", FilePathConveter::getInstance());
    engine.rootContext()->setContextProperty("loginViewModel", LoginViewModel::getInstance());
    engine.rootContext()->setContextProperty("userGroupModel", UserGroupModel::getInstance());
    engine.rootContext()->setContextProperty("languageManager", LanguageManager::getInstance());
    engine.rootContext()->setContextProperty("localeManager", QLocaleManager::Instance());
    engine.rootContext()->setContextProperty("versionManager", VersionManager::Instance());
    engine.rootContext()->setContextProperty("decoderManager", DecoderManager::getInstance());
    engine.rootContext()->setContextProperty("gControllerManager", ControllerManager::Instance());
    engine.rootContext()->setContextProperty("gPosMetaManager", PosMetaManager::Instance());
    engine.rootContext()->setContextProperty("logoManager", LogoManager::getInstance());
    engine.rootContext()->setContextProperty("localSettingViewModel", LocalSettingViewModel::getInstance());
    engine.rootContext()->setContextProperty("exportVideoProgressModel", ExportVideoProgressModel::getInstance());
    engine.rootContext()->setContextProperty("passwordStrengthChecker", PasswordStrengthChecker::getInstance());
    engine.rootContext()->setContextProperty("maskingService", this);

    engine.load(QUrl(QStringLiteral("qrc:/Qml/View/MaskingParent.qml")));
}
