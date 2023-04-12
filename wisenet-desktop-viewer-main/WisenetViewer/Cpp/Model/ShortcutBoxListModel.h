#ifndef SHORTCUTBOXLISTMODEL_H
#define SHORTCUTBOXLISTMODEL_H

#include "QCoreServiceManager.h"
#include <QAbstractListModel>
#include <QList>
#include <QString>

class ShortcutBoxListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(ShortcutMenu)

public:
    enum RoleNames
    {
        InUseRole = Qt::UserRole,
        MenuEnumRole,
    };

    enum ShortcutMenu
    {
        Search = 0,
        EventSearch,
        AISearch,
        SmartSearch,
        Setup,
        Register,
        PriorityAlarm,
        Volume,
        FullScreen,
        LayoutClear,
        ChannelName,
        Status,
        TextSearch,
        HealthCheck,
        Masking
    };

    explicit ShortcutBoxListModel(QObject *parent = nullptr);
    ~ShortcutBoxListModel();

    static ShortcutBoxListModel* getInstance(){
        static ShortcutBoxListModel instance;
        return &instance;
    }

    Q_INVOKABLE void loadShortcutSetting();
    Q_INVOKABLE void saveShortcutSetting();

    // Basic Functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Edit Data Functions
    void insert(int idx, QPair<int, bool> menu);
    void append(int menuEnum, bool inUse);

    Q_INVOKABLE QList<bool> getData( );
    Q_INVOKABLE bool getLicenseActivated() {return m_licenseActivated;}

    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void updateLicenseActivation(QCoreServiceEventPtr event);

signals:
    void settingChanged();

protected:
    QHash<int, QByteArray> roleNames() const override {return m_roleNames;}

private:
    QHash<int, QByteArray> m_roleNames;
    QList<QPair<int, bool>> m_data;
    QList<bool> m_shortcutUseList;
    int m_shortcutMenuCount = 15;  // 헬스체크, 텍스트 서치 포함된 메뉴 개수
    bool m_licenseActivated = false;

    // 숏컷 메뉴에 메뉴 추가/삭제되거나 순서 변경될 때 수정 필수
    QList<int> m_currentOrder = {ShortcutMenu::Search, ShortcutMenu::EventSearch, ShortcutMenu::AISearch, ShortcutMenu::SmartSearch,
                                 ShortcutMenu::Masking,
                                 ShortcutMenu::Setup, ShortcutMenu::Register, ShortcutMenu::PriorityAlarm, ShortcutMenu::Volume,
                                 ShortcutMenu::FullScreen, ShortcutMenu::LayoutClear, ShortcutMenu::ChannelName, ShortcutMenu::Status};

    QSet<int> m_defaultOffMenu = {ShortcutMenu::EventSearch, ShortcutMenu::AISearch, ShortcutMenu::SmartSearch, ShortcutMenu::TextSearch,
                                 ShortcutMenu::HealthCheck};

};
#endif // SHORTCUTBOXLISTMODEL_H
