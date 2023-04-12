#ifndef LDAPUSERVIEWMODEL_H
#define LDAPUSERVIEWMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <set>
#include "QCoreServiceManager.h"

class LdapUserViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int highlightRow READ highlightRow NOTIFY highlightRowChanged)
    Q_PROPERTY(int highlightRowCount READ highlightRowCount NOTIFY highlightRowCountChanged)
    Q_PROPERTY(int selectedRowCount READ selectedRowCount NOTIFY selectedRowCountChanged)

    Q_PROPERTY(QUrl serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QString adminDN READ adminDN WRITE setAdminDN NOTIFY adminDNChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString searchBase READ searchBase WRITE setSearchBase NOTIFY searchBaseChanged)
    Q_PROPERTY(QString searchFilter READ searchFilter WRITE setSearchFilter NOTIFY searchFilterChanged)

public:
    enum ldapUserTitle {
        LdapUserCheck = 0,
        LdapUserID,
        LdapUserName,
        LdapUserEmail,
        LdapUserDN,
        LdapDuplicateID,
    };

    enum Role {
        HighlightRole = Qt::UserRole,
        HoveredRole
    };

    LdapUserViewModel(QObject* parent = nullptr);

    Q_INVOKABLE void setHighlightRow(int rowNum, int modifiers);
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE bool setCheckState(int rowNum, bool checked);
    Q_INVOKABLE bool setAllCheckState(bool checked);

    Q_INVOKABLE void clear();
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override; // need to override

    Q_INVOKABLE void checkLdapServerUrl();
    Q_INVOKABLE QStringList getUserGroupList();
    Q_INVOKABLE void registerLdapUser(QString userGroupName);
    Q_INVOKABLE void updateLdapSetting();
    Q_INVOKABLE void loadLdapSetting();
    Q_INVOKABLE bool isDuplicateID(int rowNum) {return m_data[rowNum][LdapDuplicateID].toBool();};
    Q_INVOKABLE bool checkDuplicateUser();
    Q_INVOKABLE void showLdapUsers();

    // need to override
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int highlightRow() const;
    int highlightRowCount(){ return (int)m_highlightRows.size(); }
    int selectedRowCount();

    void setServerUrl(QUrl serverUrl);
    void setAdminDN(QString adminDN);
    void setPassword(QString password);
    void setSearchBase(QString searchBase);
    void setSearchFilter(QString searchFilter);

    QUrl serverUrl() {return m_serverUrl;}
    QString adminDN() {return m_adminDN;}
    QString password() {return m_password;}
    QString searchBase() {return m_searchBase;}
    QString searchFilter() {return m_searchFilter;}

    void setLdapConnection();
    void makeConnection(bool isSsl);

signals:
    void highlightRowChanged();
    void highlightRowCountChanged();
    void selectedRowCountChanged();
    void resultMessage(bool success, QString msg);

    void serverUrlChanged();
    void adminDNChanged();
    void passwordChanged();
    void searchBaseChanged();
    void searchFilterChanged();

    void connCompleted(bool connSuccess, bool searchSuccess, bool isServerChanged, int fetchedUserCnt, QString error);

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

    QUrl m_serverUrl;
    QString m_adminDN;
    QString m_password;
    QString m_searchBase;
    QString m_searchFilter;
    //std::string m_userFilter = "(|(objectclass=user)(objectclass=person)(objectclass=inetOrgPerson)(objectclass=posixAccount)(objectclass=organizationalPerson))";

    bool m_isServerChanged = false;

    std::vector<Wisenet::Core::LdapUserInfo> m_searchResults;
};

#endif // LDAPUSERVIEWMODEL_H
