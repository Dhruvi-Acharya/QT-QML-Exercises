#pragma once
#include "BaseTreeItemInfo.h"

class LayoutTreeItemInfo : public BaseTreeItemInfo
{
    Q_OBJECT
    Q_PROPERTY(bool saveStatus READ saveStatus NOTIFY saveStatusChanged)
    Q_PROPERTY(MediaLayoutViewModel* layoutViewModel READ layoutViewModel NOTIFY layoutViewModelChanged)
public:
    explicit LayoutTreeItemInfo(QObject *parent = 0);
    LayoutTreeItemInfo(const LayoutTreeItemInfo &other);
    virtual ~LayoutTreeItemInfo();

    // for layout
    void setLayout(Wisenet::Core::Layout &layout);
    bool saveStatus() const {return m_MediaLayoutViewModel.saveStatus();}
    MediaLayoutViewModel* layoutViewModel() {return &m_MediaLayoutViewModel;}
    void addCamera(const QString &deviceId, const QString &channelId);
    void addWebpage(const QString &webpage);
    void addLocalfile(const QString &fileUrl);
    void removeItem(const QString &itemId);

public slots:
    void saveLayout();

signals:
    void itemStatusChanged(int status);
    void saveStatusChanged();
    void layoutViewModelChanged();

private:
    MediaLayoutViewModel m_MediaLayoutViewModel;
};
