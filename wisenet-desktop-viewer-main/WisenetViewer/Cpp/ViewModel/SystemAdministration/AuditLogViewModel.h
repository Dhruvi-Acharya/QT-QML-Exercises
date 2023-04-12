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
#include <QAbstractTableModel>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <set>
#include <QDateTime>
#include <QSharedPointer>
#include "QCoreServiceManager.h"
#include "AuditLogThread.h"

class AuditLogViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
public:
    AuditLogViewModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void search(const QDateTime &from, const QDateTime &to);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE bool exportCSV(QString path, QString fileName);
    Q_INVOKABLE int totalCount(){return m_totalCount;}
    Q_INVOKABLE void filter(const QStringList & users, const QStringList &channels, const QStringList &logTypes, const QString &description);

    enum Role{
        HoveredRole= Qt::UserRole,
    };
private slots:
    void processComplated(std::shared_ptr<std::vector<Wisenet::Core::AuditLog>> auditLogs,
                          QSharedPointer<QVector<QVector<QVariant>>> data,
                          QSharedPointer<QVector<int>> dateTimeDescIndex,
                          QSharedPointer<QVector<int>> userDescIndex,
                          QSharedPointer<QVector<int>> cameraDescIndex,
                          QSharedPointer<QVector<int>> eventDescIndex,
                          QSet<QString> userList,
                          QVector<QStringList> cameraList,
                          QSet<QString> logTypeList);

signals:
    void totalCountChanged();
    void searchStarted();
    void searchFinished(QSet<QString> userList, QVector<QStringList> cameraList, QSet<QString> logList);

private:
    QVariant getDataDisplayRole(const QModelIndex &index) const;

    void clearData();

    void sortFilter();

    bool containsUserFilter(int index);
    bool containsChannelFilter(int index);
    bool containsEventFilter(int index);
    bool containsDescriptionFilter(int index);

    std::map<int,QVector<int>> m_cameraName;
    std::map<int,QVector<int>> m_logType;

    int m_hoveredRow;

    AuditLogThread m_thread;

    std::shared_ptr<std::vector<Wisenet::Core::AuditLog>> m_auditLogs;
    int m_totalCount;

    QSharedPointer<QVector<QVector<QVariant>>> m_data;

    QVector<int> m_index;

    QSharedPointer<QVector<int>> m_dateTimeDescIndex;
    QSharedPointer<QVector<int>> m_userDescIndex;
    QSharedPointer<QVector<int>> m_cameraDescIndex;
    QSharedPointer<QVector<int>> m_eventDescIndex;

    int m_sortIndex;
    Qt::SortOrder m_sortOrder;
    QStringList m_filterUsers;
    QStringList m_filterChannels;
    QStringList m_filterLogTypes;
    QString m_filterDescription;

};

