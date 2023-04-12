#include "AuditLogThread.h"
#include "LogSettings.h"
#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include <JsonArchiver.h>
#include "BaseStructure.h"

AuditLogThread::AuditLogThread(QObject *parent)
    :QThread(parent)
{

}

AuditLogThread::~AuditLogThread()
{
    m_mutex.lock();
    m_abort = true;
    m_condition.wakeOne();
    m_mutex.unlock();

    wait();
}

void AuditLogThread::process(std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> deletedDevices,
                             std::shared_ptr<std::vector<Wisenet::Core::AuditLog> > auditLog)
{
    QMutexLocker locker(&m_mutex);

    this->m_auditLogs = auditLog;
    this->m_deletedDevices = deletedDevices;

    if(!isRunning()){
        start(LowPriority);
    } else {
        m_restart = true;
        m_condition.wakeOne();
    }
}

void AuditLogThread::registerQml()
{
    qRegisterMetaType<std::shared_ptr<std::vector<Wisenet::Core::AuditLog>>>();
}

void AuditLogThread::run()
{
    forever{

        m_mutex.lock();
        auto auditLogs = this->m_auditLogs;
        this->m_auditLogs.reset();
        m_mutex.unlock();

        QSharedPointer<QVector<QVector<QVariant>>> data(new QVector<QVector<QVariant>>);
        QSharedPointer<QVector<int>> dateTimeDescIndex(new QVector<int>);
        QSharedPointer<QVector<int>> userDescIndex(new QVector<int>);
        QSharedPointer<QVector<int>> cameraDescIndex(new QVector<int>);
        QSharedPointer<QVector<int>> eventDescIndex(new QVector<int>);

        QVector<QStringList> cameraList;

        QSet<QString> userList;
        QSet<QString> logTypeList;
        QSet<QString> cameraListFilter;


        if(m_abort){
            return;
        }

        if(auditLogs && !auditLogs->empty()){

            //1. build data for display
            for(auto& auditLog : *auditLogs){

                QVector<QVector<QVariant>> displays;

                if(0 == auditLog.actionType.compare("User Login")){
                    convertLogIn(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Backup Settings")){
                    convertBackupSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Restore Settings")){
                    convertRestoreSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Initialize Settings")){
                    convertInitializeSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Log Settings")){
                    convertLogSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("User Settings")){
                    convertUserSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("User Group Settings")){
                    convertUserGroupSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Event Rule Settings")){
                    convertEventRuleSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Schedule Settings")){
                    convertScheduleSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Device Settings")){
                    convertDeviceSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Channel Settings")){
                    convertChannelSettings(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Device Status")){
                    convertDeviceStatus(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("PTZ Control")){
                    convertPTZControl(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Watching Live")){
                    convertWatchingLive(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Watching Playback")){
                    convertWatchingPlayback(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Exporting Video")){
                    convertExportingVideo(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }else if(0 == auditLog.actionType.compare("Software upgrade")){
                    convertUpdateSoftware(auditLog, displays, userList, logTypeList, cameraList, cameraListFilter);
                }

                data->append(displays);
            }

            // sort of dateTime
            dateTimeDescIndex->fill(0,data->size());
            std::iota(dateTimeDescIndex->begin(), dateTimeDescIndex->end(), 0);

            // sort of user
            userDescIndex->fill(0,data->size());
            std::iota(userDescIndex->begin(), userDescIndex->end(), 0);
            std::sort(userDescIndex->begin(),userDescIndex->end(),[data](int indexA, int indexB){

                const QVariant& valueA = (*data)[indexA][1];
                const QVariant& valueB = (*data)[indexB][1];

                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;

            });

            // sort of cameraName
            cameraDescIndex->fill(0,data->size());
            std::iota(cameraDescIndex->begin(), cameraDescIndex->end(), 0);
            std::sort(cameraDescIndex->begin(),cameraDescIndex->end(),[data](int indexA, int indexB){

                const QVariant& valueA = (*data)[indexA][2];
                const QVariant& valueB = (*data)[indexB][2];

                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;

            });

            // sort of eventName
            eventDescIndex->fill(0,data->size());
            std::iota(eventDescIndex->begin(), eventDescIndex->end(), 0);
            std::sort(eventDescIndex->begin(),eventDescIndex->end(),[data](int indexA, int indexB){

                const QVariant& valueA = (*data)[indexA][3];
                const QVariant& valueB = (*data)[indexB][3];

                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;

            });

        }

        SPDLOG_DEBUG("Emit processCompleted");
        emit processCompleted(auditLogs,data,dateTimeDescIndex,userDescIndex,cameraDescIndex,eventDescIndex,userList,cameraList,logTypeList);

        m_mutex.lock();
        if(!m_restart){
            m_condition.wait(&m_mutex);
        }
        m_restart = false;
        m_mutex.unlock();
    }

}

void AuditLogThread::convertLogIn(Wisenet::Core::AuditLog &auditLog, QVector<QVector<QVariant>>& displays,
                                  QSet<QString>& userList, QSet<QString> &logTypeList, QVector<QStringList> &cameraList, QSet<QString> &cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::LogIn.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::LogIn.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::LogIn.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogLogInActionDetail logInActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & logInActionDetail;

    if(!logInActionDetail.isSuccess){

        display.push_back(QCoreApplication::translate("WisenetLinguist","User login failed."));

    } else{
        QString description = QCoreApplication::translate("WisenetLinguist","User login. StartTime: %1 , EndTime: %2");

        QString startTime("-"), endTime("-");
        if(logInActionDetail.startUtcTimeMsec > 0){
            startTime = QLocaleManager::Instance()->getDateTimeFromMsec(logInActionDetail.startUtcTimeMsec);
        }

        if(logInActionDetail.endUtcTimeMsec > 0){
            endTime = QLocaleManager::Instance()->getDateTimeFromMsec(logInActionDetail.endUtcTimeMsec);
        }
        display.push_back(description.arg(startTime,endTime));
    }

    displays.push_back(display);
}

void AuditLogThread::convertBackupSettings(Wisenet::Core::AuditLog &auditLog, QVector<QVector<QVariant>>& displays,
                                           QSet<QString>& userList, QSet<QString> &logTypeList, QVector<QStringList> &cameraList, QSet<QString> &cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::BackupSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::BackupSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::BackupSettings.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogBackupSettingsActionDetail backupSettinsActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & backupSettinsActionDetail;

    if(!backupSettinsActionDetail.isSuccess){

        display.push_back(QCoreApplication::translate("WisenetLinguist","System configuration failed to back up."));

    } else{

        display.push_back(QCoreApplication::translate("WisenetLinguist","System configuration has been backed up."));
    }

    displays.push_back(display);
}

void AuditLogThread::convertRestoreSettings(Wisenet::Core::AuditLog &auditLog, QVector<QVector<QVariant>>& displays,
                                            QSet<QString>& userList, QSet<QString> &logTypeList, QVector<QStringList> &cameraList, QSet<QString> &cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::RestoreSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::RestoreSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::RestoreSettings.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogRestoreSettingsActionDetail restoreSettinsActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & restoreSettinsActionDetail;

    if(!restoreSettinsActionDetail.isSuccess){

        display.push_back(QCoreApplication::translate("WisenetLinguist","System configuration failed to restore."));

    } else{

        display.push_back(QCoreApplication::translate("WisenetLinguist","System configuration has been restored."));
    }

    displays.push_back(display);
}

void AuditLogThread::convertInitializeSettings(Wisenet::Core::AuditLog &auditLog, QVector<QVector<QVariant>>& displays,
                                               QSet<QString>& userList,  QSet<QString> &logTypeList, QVector<QStringList> &cameraList, QSet<QString> &cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::InitializeSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::InitializeSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::InitializeSettings.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogInitializeSettingsActionDetail initializeSettinsActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & initializeSettinsActionDetail;

    if(!initializeSettinsActionDetail.isSuccess){

        display.push_back(QCoreApplication::translate("WisenetLinguist","System configuration failed to reset."));

    } else{

        display.push_back(QCoreApplication::translate("WisenetLinguist","System configuration has been reset."));
    }

    displays.push_back(display);
}

void AuditLogThread::convertLogSettings(Wisenet::Core::AuditLog &auditLog, QVector<QVector<QVariant>>& displays,
                                        QSet<QString>& userList, QSet<QString> &logTypeList, QVector<QStringList> &cameraList, QSet<QString> &cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::LogSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::LogSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::LogSettings.c_str()));
    }

    display.push_back("");
    displays.push_back(display);
}

void AuditLogThread::convertUserSettings(Wisenet::Core::AuditLog& auditLog,
                                         QVector<QVector<QVariant>>& displays,
                                         QSet<QString>& userList,
                                         QSet<QString>& logTypeList,
                                         QVector<QStringList>& cameraList,
                                         QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::UserSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::UserSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::UserSettings.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogUserActionDetail userActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & userActionDetail;

    if(Wisenet::Core::AuditLogOperationType::Added == userActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","User(%1) is added.");
        display.push_back(description.arg(QString(userActionDetail.userName.c_str())));
    }else if(Wisenet::Core::AuditLogOperationType::Removed == userActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","User(%1) is deleted.");
        display.push_back(description.arg(QString(userActionDetail.userName.c_str())));
    }else {
        QString description = QCoreApplication::translate("WisenetLinguist", "User(%1) is updated.");
        display.push_back(description.arg(QString(userActionDetail.userName.c_str())));

    }

    displays.push_back(display);

}

void AuditLogThread::convertUserGroupSettings(Wisenet::Core::AuditLog& auditLog,
                                              QVector<QVector<QVariant>>& displays,
                                              QSet<QString>& userList,
                                              QSet<QString>& logTypeList,
                                              QVector<QStringList>& cameraList,
                                              QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::UserGroupSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::UserGroupSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::UserGroupSettings.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogUserGroupActionDetail userGroupActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & userGroupActionDetail;

    if(Wisenet::Core::AuditLogOperationType::Added == userGroupActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","User group(%1) is added.");
        display.push_back(description.arg(QString(userGroupActionDetail.userGroupName.c_str())));
    }else if(Wisenet::Core::AuditLogOperationType::Removed == userGroupActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","User group(%1) is deleted.");
        display.push_back(description.arg(QString(userGroupActionDetail.userGroupName.c_str())));
    }else {
        QString description = QCoreApplication::translate("WisenetLinguist", "User group(%1) is updated.");
        display.push_back(description.arg(QString(userGroupActionDetail.userGroupName.c_str())));
    }

    displays.push_back(display);
}

void AuditLogThread::convertEventRuleSettings(Wisenet::Core::AuditLog& auditLog,
                                              QVector<QVector<QVariant>>& displays,
                                              QSet<QString>& userList,
                                              QSet<QString>& logTypeList,
                                              QVector<QStringList>& cameraList,
                                              QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::EventRuleSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::EventRuleSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::EventRuleSettings.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogEventRuleActionDetail eventRuleActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & eventRuleActionDetail;

    if(Wisenet::Core::AuditLogOperationType::Added == eventRuleActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","EventRule(%1) is added.");
        display.push_back(description.arg(QString(eventRuleActionDetail.eventRuleName.c_str())));
    }else if(Wisenet::Core::AuditLogOperationType::Removed == eventRuleActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","EventRule(%1) is deleted.");
        display.push_back(description.arg(QString(eventRuleActionDetail.eventRuleName.c_str())));
    }else {
        QString description = QCoreApplication::translate("WisenetLinguist", "EventRule(%1) is updated.");
        display.push_back(description.arg(QString(eventRuleActionDetail.eventRuleName.c_str())));
    }
    
    displays.push_back(display);
}

void AuditLogThread::convertScheduleSettings(Wisenet::Core::AuditLog& auditLog,
                                             QVector<QVector<QVariant>>& displays,
                                             QSet<QString>& userList,
                                             QSet<QString>& logTypeList,
                                             QVector<QStringList>& cameraList,
                                             QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::ScheduleSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::ScheduleSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::ScheduleSettings.c_str()));
    }

    //5. description
    Wisenet::Core::AuditLogScheduleActionDetail scheduleActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & scheduleActionDetail;

    if(Wisenet::Core::AuditLogOperationType::Added == scheduleActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","Schedule(%1) is added.");
        display.push_back(description.arg(QString(scheduleActionDetail.scheduleName.c_str())));
    }else if(Wisenet::Core::AuditLogOperationType::Removed == scheduleActionDetail.operationType){
        QString description = QCoreApplication::translate("WisenetLinguist","Schedule(%1) is deleted.");
        display.push_back(description.arg(QString(scheduleActionDetail.scheduleName.c_str())));
    }else {
        QString description = QCoreApplication::translate("WisenetLinguist", "Schedule(%1) is updated.");
        display.push_back(description.arg(QString(scheduleActionDetail.scheduleName.c_str())));
    }

    displays.push_back(display);
}

void AuditLogThread::convertDeviceSettings(Wisenet::Core::AuditLog& auditLog,
                                           QVector<QVector<QVariant>>& displays,
                                           QSet<QString>& userList,
                                           QSet<QString>& logTypeList,
                                           QVector<QStringList>& cameraList,
                                           QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    Wisenet::Device::Device findedDevice;
    QString deletedDeviceName("");

    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    if (db->FindDevice(auditLog.itemID,findedDevice)){
        display.push_back(QString::fromUtf8(findedDevice.name.c_str()));
    }else{
        deletedDeviceName = GetDeletedDeviceName(auditLog.itemID);
        display.push_back(deletedDeviceName);
    }

    if(cameraListFilter.end() == cameraListFilter.find(QString::fromUtf8(auditLog.itemID.c_str()))){
        cameraListFilter.insert(QString::fromUtf8(auditLog.itemID.c_str()));
        QStringList cameraName;
        cameraName << QString::fromUtf8(auditLog.itemID.c_str()) << "" << deletedDeviceName << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::DeviceSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::DeviceSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::DeviceSettings.c_str()));
    }

    Wisenet::Core::AuditLogDeviceActionDetail deviceActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & deviceActionDetail;

    //5. description
    if(Wisenet::Core::AuditLogOperationType::Added == deviceActionDetail.operationType){
        display.push_back(QCoreApplication::translate("WisenetLinguist","Device is added."));
    }else if(Wisenet::Core::AuditLogOperationType::Removed == deviceActionDetail.operationType){
        display.push_back(QCoreApplication::translate("WisenetLinguist","Device is deleted."));
    }else {
        display.push_back(QCoreApplication::translate("WisenetLinguist", "Device is updated."));
    }

    displays.push_back(display);
}

void AuditLogThread::convertChannelSettings(Wisenet::Core::AuditLog& auditLog,
                                            QVector<QVector<QVariant>>& displays,
                                            QSet<QString>& userList,
                                            QSet<QString>& logTypeList,
                                            QVector<QStringList>& cameraList,
                                            QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    Wisenet::Device::Device::Channel findedChannel;
    QString deletedDeviceName("");
    QString deletedChannelName("");

    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    bool found = false;
    if (db != nullptr) {
        found = db->FindChannel(auditLog.parentID,auditLog.itemID,findedChannel);
    }

    if (found) {
        display.push_back(QString::fromUtf8(findedChannel.name.c_str()));
    } else {
         deletedDeviceName = GetDeletedDeviceName(auditLog.parentID);
         deletedChannelName = GetDeletedChannelName(auditLog.parentID,auditLog.itemID);
         display.push_back(deletedChannelName);
    }

    QString id = QString::fromUtf8(auditLog.parentID.c_str()) + "_" + QString::fromUtf8(auditLog.itemID.c_str());
    if(cameraListFilter.end() == cameraListFilter.find(id)){
        cameraListFilter.insert(id);
        QStringList cameraName;
        cameraName << QString::fromUtf8(auditLog.parentID.c_str()) << QString::fromUtf8(auditLog.itemID.c_str()) << deletedDeviceName << deletedChannelName;
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::ChannelSettings.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::ChannelSettings.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::ChannelSettings.c_str()));
    }

    Wisenet::Core::AuditLogChannelActionDetail channelActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & channelActionDetail;

    //5. description
    if(Wisenet::Core::AuditLogOperationType::Added == channelActionDetail.operationType){
        display.push_back(QCoreApplication::translate("WisenetLinguist","Channel is added."));
    }else if(Wisenet::Core::AuditLogOperationType::Removed == channelActionDetail.operationType){
        display.push_back(QCoreApplication::translate("WisenetLinguist","Channel is deleted."));
    }else {
        display.push_back(QCoreApplication::translate("WisenetLinguist", "Channel is updated."));
    }

    displays.push_back(display);
}

void AuditLogThread::convertDeviceStatus(Wisenet::Core::AuditLog& auditLog,
                                         QVector<QVector<QVariant>>& displays,
                                         QSet<QString>& userList,
                                         QSet<QString>& logTypeList,
                                         QVector<QStringList>& cameraList,
                                         QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    Wisenet::Device::Device findedDevice;
    QString deletedDeviceName("");

    // 2023.01.02. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    bool found = false;
    if (db != nullptr) {
        found = db->FindDevice(auditLog.itemID,findedDevice);
    }

    if (found){
        display.push_back(QString::fromUtf8(findedDevice.name.c_str()));
    }else{
        deletedDeviceName = GetDeletedDeviceName(auditLog.itemID);
        display.push_back(deletedDeviceName);
    }

    if(cameraListFilter.end() == cameraListFilter.find(QString::fromUtf8(auditLog.itemID.c_str()))){
        cameraListFilter.insert(QString::fromUtf8(auditLog.itemID.c_str()));
        QStringList cameraName;
        cameraName << QString::fromUtf8(auditLog.itemID.c_str()) << "" << deletedDeviceName << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::DeviceStatus.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::DeviceStatus.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::DeviceStatus.c_str()));
    }

    Wisenet::Core::AuditLogDeviceStatusActionDetail deviceStatusActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & deviceStatusActionDetail;

    //5. description
    if(Wisenet::Core::AuditLogOperationType::Connect == deviceStatusActionDetail.operationType){
        display.push_back(QCoreApplication::translate("WisenetLinguist","Connect Device."));
    }else {
        display.push_back(QCoreApplication::translate("WisenetLinguist","Disconnect Device."));
    }

    displays.push_back(display);
}

void AuditLogThread::convertPTZControl(Wisenet::Core::AuditLog& auditLog,
                                       QVector<QVector<QVariant>>& displays,
                                       QSet<QString>& userList,
                                       QSet<QString>& logTypeList,
                                       QVector<QStringList>& cameraList,
                                       QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    Wisenet::Device::Device::Channel findedChannel;
    QString deletedDeviceName("");
    QString deletedChannelName("");

    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    bool found = false;
    if (db != nullptr) {
        found = db->FindChannel(auditLog.parentID,auditLog.itemID,findedChannel);
    }

    if (found) {
        display.push_back(QString::fromUtf8(findedChannel.name.c_str()));
    }else{
        deletedDeviceName = GetDeletedDeviceName(auditLog.parentID);
        deletedChannelName = GetDeletedChannelName(auditLog.parentID,auditLog.itemID);
        display.push_back(deletedChannelName);
    }

    QString id = QString::fromUtf8(auditLog.parentID.c_str()) + "_" + QString::fromUtf8(auditLog.itemID.c_str());
    if(cameraListFilter.end() == cameraListFilter.find(id)){
        cameraListFilter.insert(id);
        QStringList cameraName;
        cameraName << QString::fromUtf8(auditLog.parentID.c_str()) << QString::fromUtf8(auditLog.itemID.c_str()) << deletedDeviceName << deletedChannelName;
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::PTZControl.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::PTZControl.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::PTZControl.c_str()));
    }

    Wisenet::Core::AuditLogPTZControlActionDetail ptzControlActionDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & ptzControlActionDetail;

    //5. description
    QString description = QCoreApplication::translate("WisenetLinguist","PTZcontrol occurred from %1 to %2.");

    QString startTime("-"), endTime("-");
    if(ptzControlActionDetail.startUtcTimeMsec > 0){
        startTime = QLocaleManager::Instance()->getDateTimeFromMsec(ptzControlActionDetail.startUtcTimeMsec);
    }

    if(ptzControlActionDetail.endUtcTimeMsec > 0){
        endTime = QLocaleManager::Instance()->getDateTimeFromMsec(ptzControlActionDetail.endUtcTimeMsec);
    }

    display.push_back(description.arg(startTime,endTime));

    displays.push_back(display);
}

void AuditLogThread::convertWatchingLive(Wisenet::Core::AuditLog& auditLog,
                                         QVector<QVector<QVariant>>& displays,
                                         QSet<QString>& userList,
                                         QSet<QString>& logTypeList,
                                         QVector<QStringList>& cameraList,
                                         QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    Wisenet::Device::Device::Channel findedChannel;
    QString deletedDeviceName("");
    QString deletedChannelName("");

    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    bool found = false;
    if (db != nullptr) {
        found = db->FindChannel(auditLog.parentID,auditLog.itemID,findedChannel);
    }

    if (found) {
        display.push_back(QString::fromUtf8(findedChannel.name.c_str()));
    }else{
        deletedDeviceName = GetDeletedDeviceName(auditLog.parentID);
        deletedChannelName = GetDeletedChannelName(auditLog.parentID,auditLog.itemID);
        display.push_back(deletedChannelName);
    }

    QString id = QString::fromUtf8(auditLog.parentID.c_str()) + "_" + QString::fromUtf8(auditLog.itemID.c_str());
    if(cameraListFilter.end() == cameraListFilter.find(id)){
        cameraListFilter.insert(id);
        QStringList cameraName;
        cameraName << QString::fromUtf8(auditLog.parentID.c_str()) << QString::fromUtf8(auditLog.itemID.c_str()) << deletedDeviceName << deletedChannelName;
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::WatchingLive.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::WatchingLive.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::WatchingLive.c_str()));
    }

    Wisenet::Core::AuditLogWatchingLiveDetail watchingLiveDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & watchingLiveDetail;

    //5. description
    QString description = QCoreApplication::translate("WisenetLinguist","User watched live video from %1 to %2.");

    QString startTime("-"), endTime("-");
    if(watchingLiveDetail.startUtcTimeMsec > 0){
        startTime = QLocaleManager::Instance()->getDateTimeFromMsec(watchingLiveDetail.startUtcTimeMsec);
    }

    if(watchingLiveDetail.endUtcTimeMsec > 0){
        endTime = QLocaleManager::Instance()->getDateTimeFromMsec(watchingLiveDetail.endUtcTimeMsec);
    }

    display.push_back(description.arg(startTime,endTime));

    displays.push_back(display);
}

void AuditLogThread::convertWatchingPlayback(Wisenet::Core::AuditLog& auditLog,
                                             QVector<QVector<QVariant>>& displays,
                                             QSet<QString>& userList,
                                             QSet<QString>& logTypeList,
                                             QVector<QStringList>& cameraList,
                                             QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    Wisenet::Device::Device::Channel findedChannel;
    QString deletedDeviceName("");
    QString deletedChannelName("");

    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    bool found = false;
    if (db != nullptr) {
        found = db->FindChannel(auditLog.parentID,auditLog.itemID,findedChannel);
    }

    if (found) {
        display.push_back(QString::fromUtf8(findedChannel.name.c_str()));
    }else{
        deletedDeviceName = GetDeletedDeviceName(auditLog.parentID);
        deletedChannelName = GetDeletedChannelName(auditLog.parentID, auditLog.itemID);
        display.push_back(deletedChannelName);
    }

    QString id = QString::fromUtf8(auditLog.parentID.c_str()) + "_" + QString::fromUtf8(auditLog.itemID.c_str());
    if(cameraListFilter.end() == cameraListFilter.find(id)){
        cameraListFilter.insert(id);
        QStringList cameraName;
        cameraName << QString::fromUtf8(auditLog.parentID.c_str()) << QString::fromUtf8(auditLog.itemID.c_str()) << deletedDeviceName << deletedChannelName;
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::WatchingPlayback.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::WatchingPlayback.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::WatchingPlayback.c_str()));
    }

    Wisenet::Core::AuditLogWatchingPlayBackDetail watchingPlayBackDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & watchingPlayBackDetail;

    //5. description
    QString description = QCoreApplication::translate("WisenetLinguist","User played video from %1 to %2.");

    QString startTime("-"), endTime("-");
    if(watchingPlayBackDetail.startUtcTimeMsec > 0){
        startTime = QLocaleManager::Instance()->getDateTimeFromMsec(watchingPlayBackDetail.startUtcTimeMsec);
    }

    if(watchingPlayBackDetail.endUtcTimeMsec > 0){
        endTime = QLocaleManager::Instance()->getDateTimeFromMsec(watchingPlayBackDetail.endUtcTimeMsec);
    }

    display.push_back(description.arg(startTime,endTime));

    displays.push_back(display);
}

void AuditLogThread::convertExportingVideo(Wisenet::Core::AuditLog& auditLog,
                                           QVector<QVector<QVariant>>& displays,
                                           QSet<QString>& userList,
                                           QSet<QString>& logTypeList,
                                           QVector<QStringList>& cameraList,
                                           QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    Wisenet::Device::Device::Channel findedChannel;
    QString deletedDeviceName("");
    QString deletedChannelName("");

    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    bool found = false;
    if (db != nullptr) {
        found = db->FindChannel(auditLog.parentID,auditLog.itemID,findedChannel);
    }

    if (found) {
        display.push_back(QString::fromUtf8(findedChannel.name.c_str()));
    }else{
        deletedDeviceName = GetDeletedDeviceName(auditLog.parentID);
        deletedChannelName = GetDeletedChannelName(auditLog.parentID, auditLog.itemID);
        display.push_back(deletedChannelName);
    }

    QString id = QString::fromUtf8(auditLog.parentID.c_str()) + "_" + QString::fromUtf8(auditLog.itemID.c_str());
    if(cameraListFilter.end() == cameraListFilter.find(id)){
        cameraListFilter.insert(id);
        QStringList cameraName;
        cameraName << QString::fromUtf8(auditLog.parentID.c_str()) << QString::fromUtf8(auditLog.itemID.c_str()) << deletedDeviceName << deletedChannelName;
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::ExportingVideo.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::ExportingVideo.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::ExportingVideo.c_str()));
    }

    Wisenet::Core::AuditLogExportingVideoDetail exportingVideoDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & exportingVideoDetail;

    //5. description
    QString description = QCoreApplication::translate("WisenetLinguist","Exported from %1 to %2.");

    QString startTime("-"), endTime("-");
    if(exportingVideoDetail.startUtcTimeMsec > 0){
        startTime = QLocaleManager::Instance()->getDateTimeFromMsec(exportingVideoDetail.startUtcTimeMsec);
    }

    if(exportingVideoDetail.endUtcTimeMsec > 0){
        endTime = QLocaleManager::Instance()->getDateTimeFromMsec(exportingVideoDetail.endUtcTimeMsec);
    }

    display.push_back(description.arg(startTime,endTime));

    displays.push_back(display);
}

void AuditLogThread::convertUpdateSoftware(Wisenet::Core::AuditLog& auditLog,
                                           QVector<QVector<QVariant>>& displays,
                                           QSet<QString>& userList,
                                           QSet<QString>& logTypeList,
                                           QVector<QStringList>& cameraList,
                                           QSet<QString>& cameraListFilter)
{
    QVector<QVariant> display;

    //1.dateTime
    display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(auditLog.serviceUtcTimeMsec));

    //2. User ID
    QString userID = QString::fromUtf8(auditLog.userName.c_str());
    QString displayUserID;
    if(0 == userID.compare("Service")){
        displayUserID = QCoreApplication::translate("WisenetLinguist","Service");
    }else{
        displayUserID = userID;
    }
    display.push_back(displayUserID);

    if(userList.end() == userList.find(displayUserID)){
        userList.insert(displayUserID);
    }

    //3. camera name
    display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));

    if(cameraListFilter.end() == cameraListFilter.find("Service")){
        cameraListFilter.insert("Service");
        QStringList cameraName;
        cameraName << "Service" << "" << QCoreApplication::translate("WisenetLinguist","Service") << "";
        cameraList.push_back(cameraName);
    }

    //4. log type
    display.push_back(QCoreApplication::translate("WisenetLinguist",Wisenet::AuditLogType::UpdateSoftware.c_str()));

    if(logTypeList.end() == logTypeList.find(QString::fromUtf8(Wisenet::AuditLogType::UpdateSoftware.c_str()))){
        logTypeList.insert(QString::fromUtf8(Wisenet::AuditLogType::UpdateSoftware.c_str()));
    }

    Wisenet::Core::AuditLogUpdateSoftwareActionDetail updateSoftwareDetail;
    JsonReader reader(auditLog.actionDetail.c_str());
    reader & updateSoftwareDetail;

    //5. description
    display.push_back(QCoreApplication::translate("WisenetLinguist","Software upgrade is tried"));

    displays.push_back(display);
}

QString AuditLogThread::GetDeletedDeviceName(std::string &deviceID)
{
    QString name;
    QString deletedItem = QCoreApplication::translate("WisenetLinguist","Deleted");

    if(!m_deletedDevices || m_deletedDevices->empty()){
        name = deletedItem;
        return name;
    }

    auto itor = m_deletedDevices->find(deviceID);
    if(itor == m_deletedDevices->end()){
        name = deletedItem;
        return name;
    }

    name = itor->second.name.c_str();
    name += " [" + deletedItem + "]";
    return name;
}

QString AuditLogThread::GetDeletedChannelName(std::string &deviceID, std::string &channelID)
{
    QString name;
    QString deletedItem = QCoreApplication::translate("WisenetLinguist","Deleted");

    if(!m_deletedDevices || m_deletedDevices->empty()){
        name = deletedItem;
        return name;
    }

    auto itor = m_deletedDevices->find(deviceID);
    if(itor == m_deletedDevices->end()){
        name = deletedItem;
        return name;
    }

    auto itorChannel = itor->second.channels.find(channelID);
    if(itorChannel == itor->second.channels.end()){
        name = deletedItem;
        return name;
    }

    name = itorChannel->second.name.c_str();
    name += " [" + deletedItem + "]";

    return name;
}
