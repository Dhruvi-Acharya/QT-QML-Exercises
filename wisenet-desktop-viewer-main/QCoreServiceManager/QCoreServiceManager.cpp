/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#include <functional>
#include <algorithm>
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include "AppLocalDataLocation.h"


using namespace std::placeholders;

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[QSVCMAN] "}, level, __VA_ARGS__)

std::shared_ptr<QCoreServiceManager> QCoreServiceManager::m_qsm_instance = nullptr;
std::atomic<bool> QCoreServiceManager::m_qsm_init(false);

QCoreServiceManager& QCoreServiceManager::Instance()
{
    if (!m_qsm_init) {
        m_qsm_init = true;
        m_qsm_instance = std::make_shared<QCoreServiceManager>(nullptr);
    }
    return *m_qsm_instance;
}

void QCoreServiceManager::Release()
{
    if (m_qsm_init) {
        m_qsm_instance.reset();
    }
}

QCoreServiceManager::QCoreServiceManager(QObject *parent)
    : QObject(parent)
    , m_db(new QCoreServiceDatabase(this))
    , m_localService(std::make_shared<Wisenet::Core::CoreService>())
{
    QString appPath = GetAppDataLocation();
    QString iniPath = appPath+"/viewer_settings.ini";
    m_settings = new QLocalSettings(iniPath,  QSettings::IniFormat, this);
    qRegisterMetaType<QCoreServiceReplyPtr>();
    m_updateManager = new QUpdateManager(this);
    m_signatureVerifier = new SignatureVerifier(this);

    connect(m_updateManager, &QUpdateManager::coreServiceEventTriggered, this, &QCoreServiceManager::coreServiceEventTriggered);
    connect(m_signatureVerifier, &SignatureVerifier::verifyFinished, this, &QCoreServiceManager::signatureVerifyFinished, Qt::QueuedConnection);
    //SPDLOG_INFO("Create a new QCoreServiceManager instance, iniPath={}", iniPath.toLocal8Bit().toStdString());
}

QCoreServiceManager::~QCoreServiceManager()
{
    //SPDLOG_INFO("Destroy a QCoreServiceManager instance");
}

void QCoreServiceManager::Start()
{
    SPDLOG_INFO("Start QCoreServiceManager");
    m_startedUtc =  QDateTime::currentDateTime().toSecsSinceEpoch();
    m_localService->Start();
}

void QCoreServiceManager::Stop()
{
    SPDLOG_INFO("Stop QCoreServiceManager instance");
    m_localService->Stop();
    if(m_signatureVerifier != nullptr) {
        m_signatureVerifier->Stop();
    }
}

void QCoreServiceManager::SetS1DaemonPort(unsigned short port)
{
    m_localService->SetS1DaemonPort(port);
}

QCoreServiceRequestorPtr QCoreServiceManager::makeServiceRequestor(QObject* sender, const QCoreServiceReplyHandler &replyHandler)
{
    QCoreServiceRequestorPtr requestor(new QCoreServiceRequestor);
    // handler가 nullptr인 경우 예외처리 (응답받지 않음)
    if (replyHandler != nullptr) {
        connect(requestor.data(), &QCoreServiceRequestor::Completed, sender, replyHandler, Qt::QueuedConnection);
    }
    return requestor;
}

void QCoreServiceManager::transferServiceResponse(const QCoreServiceRequestorPtr &requestor,
                                                  Wisenet::ResponseBaseSharedPtr response)
{
    QCoreServiceReplyPtr reply(new QCoreServiceReply);
    reply->requestorPtr = requestor;
    reply->responseDataPtr = response;
    emit requestor->Completed(reply);
}

bool QCoreServiceManager::isNotification(QCoreServiceEventPtr event)
{
    if( !event || this->Settings().isNull())
        return true;
    if( !(event->eventDataPtr) )
        return true;

    if(event->eventDataPtr->EventTypeId() != Wisenet::Core::ShowEventPanelEventType && event->eventDataPtr->EventTypeId() != Wisenet::Core::ShowSystemAlarmPanelEventType )
    {
        return true;
    }
    if( this->Settings()->eventNotificationShowAll() && this->Settings()->systemNotificationShowAll() )
    {
        return true;
    }

    QStringList savedList = this->Settings()->eventNotifications();
    std::string logType;
    if( event->eventDataPtr->EventTypeId() == Wisenet::Core::ShowEventPanelEventType)
    {
        logType = ((std::static_pointer_cast<Wisenet::Core::ShowEventPanelEvent>(event->eventDataPtr))->eventLog).type;
    }
    else// event->eventDataPtr->EventTypeId() == Wisenet::Core::ShowSystemAlarmPanelEventType
    {
        logType = ((std::static_pointer_cast<Wisenet::Core::ShowSystemAlarmPanelEvent>(event->eventDataPtr))->eventLog).type;
    }

    return !(savedList.contains( QString::fromStdString(Wisenet::GetTitleAlarmType(logType))));
}

void QCoreServiceManager::coreServiceEventHandler(const Wisenet::EventBaseSharedPtr &event)
{
    if(!m_db.isNull()) {
        m_db->Update(event);
    }


    QCoreServiceEventPtr qEvent(new QCoreServiceEvent);
    qEvent->eventDataPtr = event;
    if( !isNotification(qEvent)) // being filtered by event notification settings
        return;

    emit coreServiceEventTriggered(qEvent);

    if(event->EventTypeId() == Wisenet::Core::FullInfoEventType)
    {
        // Load setting
        QCoreServiceEventPtr qLoadLatestSettingsEvent(new QCoreServiceEvent);
        auto loadLatestSettingsEvent = std::make_shared<Wisenet::Core::LoadLatestSettingsEvent>();
        qLoadLatestSettingsEvent->eventDataPtr = loadLatestSettingsEvent;

        emit coreServiceEventTriggered(qLoadLatestSettingsEvent);
    }
}


void QCoreServiceManager::Login(const QString &id, const QString &password, bool hashPassword,
                                QObject* sender,
                                const QCoreServiceReplyHandler &replyHandler)
{
    m_userName = id.toStdString();
    m_password = password.toStdString();

    // 2022.12.29. coverity
    if (m_db == nullptr) {
        return;
    }

    if (m_db != nullptr) {
        m_db->SetLoginUserID(m_userName);
    }

    auto request = std::make_shared<Wisenet::Core::LoginRequest>();
    request->userName = m_userName;
    request->password = m_password;
    request->isHashed = hashPassword;

    auto requestor = makeServiceRequestor(sender, replyHandler);

    m_localService->Login(request, [requestor, this]
                          (const Wisenet::ResponseBaseSharedPtr &response)
    {
        SPDLOG_DEBUG("CoreService Login response isFailed={}", response->isFailed());
        transferServiceResponse(requestor, response);

        // start event subscription from CoreService
        if (!response->isFailed()) {
            auto eventSubscribeRequest = std::make_shared<Wisenet::Core::StartEventServiceRequest>();
            eventSubscribeRequest->eventHandler = std::bind(&QCoreServiceManager::coreServiceEventHandler, this, _1);
            m_localService->StartEventService(eventSubscribeRequest, nullptr);
         }
    });
}

void QCoreServiceManager::Logout()
{
    auto request = std::make_shared<Wisenet::Core::LogoutRequest>();
    request->userName = m_userName;
    m_localService->Logout(request, nullptr);
}

void QCoreServiceManager::RequestToCoreService(
        Wisenet::Core::memFuncType func,
        QObject *sender,
        const Wisenet::RequestBaseSharedPtr &request,
        const QCoreServiceReplyHandler &replyHandler)
{
    request->userName = m_userName;
    auto requestor = makeServiceRequestor(sender, replyHandler);

    std::function<void(const Wisenet::RequestBaseSharedPtr &, const Wisenet::ResponseBaseHandler &)> f
            = std::bind(func, m_localService, _1, _2);

    //SPDLOG_DEBUG("RequestToCoreService():: request start");

    f(request, [requestor]
      (const Wisenet::ResponseBaseSharedPtr &response)
    {
        SPDLOG_DEBUG("CoreService response isSuccess={}", response->isSuccess());
        transferServiceResponse(requestor, response);
    });
}


std::string QCoreServiceManager::CreateNewUUidString()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
}


QString QCoreServiceManager::CreateNewUUidQString()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QPointer<QCoreServiceDatabase> QCoreServiceManager::DB()
{
    return m_db;
}

QPointer<QLocalSettings> QCoreServiceManager::Settings()
{
    return m_settings;
}

QPointer<SignatureVerifier> QCoreServiceManager::Signature()
{
    return m_signatureVerifier;
}

QVariant QCoreServiceManager::getUpdateStatus(int nOsType, QMap<QString, QString> &mapUpdateFile)
{
    if(!m_updateManager.isNull())
    {
        return m_updateManager->getUpdateStatus(static_cast<FILE_OS_TYPE>(nOsType), mapUpdateFile);
    }
    return false;
}

void QCoreServiceManager::getUpdateFwInfoFile()
{    
    if(!m_updateManager.isNull())
        m_updateManager->getUpdateFwInfoFile();
}

QPair<QString,QString> QCoreServiceManager::getDeviceRecentVersion(QString deviceModelName)
{
    if (m_updateManager != nullptr)
    {
        return m_updateManager->getDeviceRecentVersion(deviceModelName);
    }

    return {"-", "-"};
}

Wisenet::Core::FirmwareUpdateStatus QCoreServiceManager::checkLatestVersion(const QString& modelName, const QString& firmwareVersion) const
{
    // 2023.01.02. coverity
    if (m_updateManager == nullptr)
    {
        return Wisenet::Core::FirmwareUpdateStatus::Unknown;
    }

    Wisenet::Core::FirmwareUpdateStatus status = Wisenet::Core::FirmwareUpdateStatus::Unknown;

    if (m_updateManager != nullptr) {
        status = m_updateManager->checkLatestVersion(modelName, firmwareVersion);
    }

    return status;
}

void QCoreServiceManager::updateStatistics()
{
    if (m_db == nullptr) {
        return;
    }

    Wisenet::Core::Statistics statistics = m_db->GetStatistics();

    int now = QDateTime::currentDateTime().toSecsSinceEpoch();
    int diff = (now - m_startedUtc) / 60;

    statistics.executionCount += 1;
    statistics.executionMinutes += diff;

    if((statistics.mediaRequestCount + statistics.currentProcessMediaRequestCount) != 0)
    {
        statistics.averageMediaResponseTime = (statistics.averageMediaResponseTime * statistics.mediaRequestCount + statistics.currentProcessTotalMediaResponseTime) /
                (statistics.mediaRequestCount + statistics.currentProcessMediaRequestCount);
        statistics.mediaRequestCount += statistics.currentProcessMediaRequestCount;
    }

    m_db->UpdateStatistics(statistics);

    auto updateStatisticsRequest = std::make_shared<Wisenet::Core::UpdateStatisticsRequest>();
    updateStatisticsRequest->statistics = statistics;

    SPDLOG_DEBUG("QCoreServiceManager::updateStatistics count:{}, minutes:{}, lc:{}, cc:{}, moc:{}, avg:{}",
                 statistics.executionCount, statistics.executionMinutes,
                 statistics.layoutOpenCount, statistics.channelOpenCount,
                 statistics.mediaRequestCount, statistics.averageMediaResponseTime);

    RequestToCoreService(
                &Wisenet::Core::ICoreService::UpdateStatistics,
                this, updateStatisticsRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("QCoreServiceManager::updateStatistics success");
        }
        else
            SPDLOG_DEBUG("QCoreServiceManager::updateStatistics fail");
    });
}

QByteArray QCoreServiceManager::generateStatistics(QString& key)
{
    auto db = QCoreServiceManager::Instance().DB();

    // A key should be matched when it will be decrypted. - /Tools/StatisticsParser
    CryptoAdapter cryptoAdapter;
    cryptoAdapter.Init("statistics");

    if (db == nullptr)
        return nullptr;

    auto devicesData = db->GetDevices();
    auto statisticsData = db->GetStatistics();

    key = QString(cryptoAdapter.Encrypt(QString::fromStdString(statisticsData.machineId)).toHex());

    QString country = QLocale::system().countryToString(QLocale::system().country());
    QString os = QSysInfo::prettyProductName();

    QVariantMap devices;

    for (auto& kv : devicesData) {
        auto& device = kv.second;

        QVariantMap deviceMap;

        deviceMap["method"] = getConnectedType(device.connectionInfo.connectedType);
        deviceMap["model"] = QString::fromUtf8(device.modelName.c_str());
        deviceMap["registration_time"] = device.connectionInfo.registrationMsec;
        deviceMap["channels"] = (int)device.channels.size();

        int used_count = 0;

        for (auto& channel : device.channels) {
            if(channel.second.use)
                used_count++;
        }

        deviceMap["channels_used"] = used_count;

        QString deviceMacAddress = QString::fromStdString(device.macAddress);

        devices[QString(cryptoAdapter.Encrypt(deviceMacAddress).toHex())] = deviceMap;
    }

    QVariantMap machineInfo;

    machineInfo["country"] = country;
    machineInfo["execution_count"] = statisticsData.executionCount;
    machineInfo["execution_minutes"] = statisticsData.executionMinutes;
    machineInfo["layout_open_count"] = statisticsData.layoutOpenCount;
    machineInfo["layout_channel_count"] = statisticsData.channelOpenCount;
    machineInfo["average_media_response_msec"] = statisticsData.averageMediaResponseTime;
    machineInfo["version"] = QCoreApplication::applicationVersion();
    machineInfo["installed_date"] = statisticsData.installedDatetime;
    machineInfo["last_updated_date"] = QDateTime::currentSecsSinceEpoch();
    machineInfo["devices"] = devices;
    machineInfo["os"] = os;
    machineInfo["license"] = m_db->GetLicenseInfo().activated;

    QJsonDocument jsonDoc = QJsonDocument::fromVariant(machineInfo);

    return jsonDoc.toJson();
}

QString QCoreServiceManager::getConnectedType(Wisenet::Device::ConnectedType connectedType)
{
    switch(connectedType){
    case Wisenet::Device::ConnectedType::StaticIPv4:
        return "StaticIPv4";
    case Wisenet::Device::ConnectedType::WisenetDDNS:
        return "WisenetDDNS";
    case Wisenet::Device::ConnectedType::HttpUrl:
        return "HttpUrl";
    case Wisenet::Device::ConnectedType::P2P:
        return "P2P";
    case Wisenet::Device::ConnectedType::P2P_RELAY:
        return "P2P_RELAY";
    case Wisenet::Device::ConnectedType::StaticIPv6:
        return "StaticIPv6";
    case Wisenet::Device::ConnectedType::S1_DDNS:
        return "S1_DDNS";
    case Wisenet::Device::ConnectedType::S1_SIP:
        return "S1_SIP";
    case Wisenet::Device::ConnectedType::S1_SIP_RELAY:
        return "S1_SIP_RELAY";
    default:
        return "IPv4";
    }
}

void QCoreServiceManager::signatureVerifyFinished(SignatureVerifyEventPtr verifyFinishEvent)
{
    // QCoreServiceEventPtr로 전환하여 이벤트 전파
    QCoreServiceEventPtr eventPtr(new QCoreServiceEvent);
    eventPtr->eventDataPtr = verifyFinishEvent->eventDataPtr;

    emit coreServiceEventTriggered(eventPtr);
}
