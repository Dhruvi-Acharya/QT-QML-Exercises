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
#include <set>
#include "QCoreServiceManager.h"

class UserListViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int highlightRow READ highlightRow NOTIFY highlightRowChanged)
    Q_PROPERTY(int highlightRowCount READ highlightRowCount NOTIFY highlightRowCountChanged)
    Q_PROPERTY(int selectedRowCount READ selectedRowCount NOTIFY selectedRowCountChanged)

public:
    enum UserTitle {
        UserID = 0,
        UserCheck,
        UserLoginID,
        LdapUser,
        UserName,
        UserType,
        UserGroupID,
        UserGroupName,
        UserEmail,
        UserDescription
    };

    enum Role{
        HighlightRole = Qt::UserRole,
        HoveredRole
    };

    Q_INVOKABLE void reset();
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    Q_INVOKABLE void setHighlightRow(int rowNum, int modifiers);
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE bool setCheckState(int rowNum, bool checked);
    Q_INVOKABLE bool setAllCheckState(bool checked);
    Q_INVOKABLE QString getUserId(int rowNum);
    Q_INVOKABLE bool getIsLdapUser(int rowNum);
    Q_INVOKABLE void deleteUser();
    Q_INVOKABLE void editUser(int row, int col, QString item);

    UserListViewModel(QObject* parent = nullptr);

    int highlightRow() const;
    int highlightRowCount(){ return (int)m_highlightRows.size(); }
    int selectedRowCount();
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

signals:
    void highlightRowChanged();
    void highlightRowCountChanged();
    void selectedRowCountChanged();
    void resultMessage(bool success, QString msg);

private:
    void UpdateSortData(int column, Qt::SortOrder order);
    QVariant getDataDisplayRole(const QModelIndex &index) const;

    QVector<QVector<QVariant>> m_data;
    QVector<int> m_index;
    QVector<int> m_sorted;

    QStringList m_columnNames;

    int m_hoveredRow;
    int m_highlightRow;
    int m_lastHighlightRow;
    std::set<int> m_highlightRows;
};

