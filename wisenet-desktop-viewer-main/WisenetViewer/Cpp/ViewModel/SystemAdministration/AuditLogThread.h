/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
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
#pragma once
#include <QThread>
#include <QWaitCondition>
#include <QSharedPointer>
#include "QCoreServiceManager.h"

class AuditLogThread : public QThread
{
    Q_OBJECT
public:
    AuditLogThread(QObject *parent = nullptr);
    ~AuditLogThread();

    void process(std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> deletedDevices,
                 std::shared_ptr<std::vector<Wisenet::Core::AuditLog>> auditLog);

    static void registerQml();

signals:
    void processCompleted(std::shared_ptr<std::vector<Wisenet::Core::AuditLog>> auditLogs,
                          QSharedPointer<QVector<QVector<QVariant>>> data,
                          QSharedPointer<QVector<int>> dateTimeDescIndex,
                          QSharedPointer<QVector<int>> userDescIndex,
                          QSharedPointer<QVector<int>> cameraDescIndex,
                          QSharedPointer<QVector<int>> eventDescIndex,
                          QSet<QString> userList,
                          QVector<QStringList> cameraList,
                          QSet<QString> logTypeList);

protected:
    void run() override;

private:
    void convertLogIn(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertBackupSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertRestoreSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertInitializeSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertLogSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertUserSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertUserGroupSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertEventRuleSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertScheduleSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertDeviceSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertChannelSettings(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertDeviceStatus(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertPTZControl(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertWatchingLive(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertWatchingPlayback(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertExportingVideo(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);
    void convertUpdateSoftware(Wisenet::Core::AuditLog& auditLog,
                      QVector<QVector<QVariant>>& displays,
                      QSet<QString>& userList,
                      QSet<QString>& logTypeList,
                      QVector<QStringList>& cameraList,
                      QSet<QString>& cameraListFilter);

    QString GetDeletedDeviceName(std::string& deviceID);
    QString GetDeletedChannelName(std::string& deviceID, std::string& channelID);

    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_restart = false;
    bool m_abort = false;

    std::shared_ptr<std::vector<Wisenet::Core::AuditLog>> m_auditLogs;
    std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> m_deletedDevices;

    QSharedPointer<QVector<QVector<QVariant>>> m_data;
    QSharedPointer<QVector<int>> m_dateTimeDescIndex;
    QSharedPointer<QVector<int>> m_userDescIndex;
    QSharedPointer<QVector<int>> m_cameraDescIndex;
    QSharedPointer<QVector<int>> m_eventDescIndex;

};

Q_DECLARE_METATYPE(std::shared_ptr<std::vector<Wisenet::Core::AuditLog>>)

