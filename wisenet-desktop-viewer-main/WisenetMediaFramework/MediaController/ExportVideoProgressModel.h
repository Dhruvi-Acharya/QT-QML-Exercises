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

#include <QAbstractListModel>
#include "FileWriteManager.h"
#include "ExportProgressItem.h"

class ExportVideoProgressModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(ExportVideoProgressModel)
    Q_PROPERTY(bool exportFinished READ exportFinished NOTIFY exportFinishedChanged)

public:
    enum RoleNames
    {
        NameRole = Qt::UserRole,
        ProgressRole,
        FailedRole,
        CanceledRole,
    };

    explicit ExportVideoProgressModel(QObject *parent = nullptr);
    ~ExportVideoProgressModel();

    static ExportVideoProgressModel* getInstance(){
        static ExportVideoProgressModel instance;
        return &instance;
    }

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Data edit functionality:
    void insert(int index, ExportProgressItem* item);
    void append(ExportProgressItem* item);
    void remove(int index);
    void clear();

    bool exportFinished(){return m_exportFinished;}

    Q_INVOKABLE void clearFinishedItem();
    Q_INVOKABLE void stopExportItem(int index);
    Q_INVOKABLE void stopAllExportItem();
    Q_INVOKABLE void openFilePath(int index);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private slots:
    void onExportMediaItemAdded(ExportMediaItem* item);
    void onProgressChanged(ExportProgressItem* item);
    void onFailedChanged(ExportProgressItem* item);
    void onEndExportAll();

signals:
    void exportFinishedChanged();

private:
    QBasicMutex m_mutex;
    QHash<int, QByteArray> m_roleNames;
    QList<ExportProgressItem*> m_exportItemList;
    bool m_exportFinished = false;
};
