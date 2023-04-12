#include "PopupParentWindow.h"
#include "LanguageManager.h"
#include "DecoderManager.h"
#include "QLocaleManager.h"
#include "VersionManager.h"

#include "ViewModel/Setup/System/LocalSettingViewModel.h"

#include "ViewModel/Sequence/SequenceAddViewModel.h"

#include "ViewModel/Setup/Device/AutoDiscoveryViewModel.h"
#include "ViewModel/Setup/Device/ManualDiscoveryViewModel.h"

#include "ViewModel/Webpage/WebpageAddViewModel.h"
#include "MediaController/ExportVideoProgressModel.h"

#include <QDebug>

PopupParentWindow::PopupParentWindow(QObject *parent)
    : QObject(parent)
{
    engine.addImportPath("qrc:/");

    engine.rootContext()->setContextProperty("localSettingViewModel", LocalSettingViewModel::getInstance());

    engine.rootContext()->setContextProperty("sequenceAddViewModel", SequenceAddViewModel::getInstance());

    engine.rootContext()->setContextProperty("manualDiscoveryViewModel", ManualDiscoveryViewModel::getInstance());
    engine.rootContext()->setContextProperty("autoDiscoveryViewModel", AutoDiscoveryViewModel::getInstance());

    engine.rootContext()->setContextProperty("webpageAddViewModel", WebpageAddViewModel::getInstance());

    engine.rootContext()->setContextProperty("languageManager", LanguageManager::getInstance());
    engine.rootContext()->setContextProperty("localeManager", QLocaleManager::Instance());
    engine.rootContext()->setContextProperty("versionManager", VersionManager::Instance());
    engine.rootContext()->setContextProperty("decoderManager", DecoderManager::getInstance());
    engine.rootContext()->setContextProperty("exportVideoProgressModel", ExportVideoProgressModel::getInstance());
    engine.rootContext()->setContextProperty("popupService", this);

    connect(LanguageManager::getInstance(), &LanguageManager::languageChanged, this, &PopupParentWindow::ApplyLanguage);

    engine.load(QUrl(QStringLiteral("qrc:/Qml/View/PopupParent.qml")));
}

void PopupParentWindow::ApplyLanguage()
{
    engine.retranslate();
}
