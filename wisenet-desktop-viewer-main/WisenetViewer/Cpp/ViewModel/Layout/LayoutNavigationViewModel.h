#pragma once
#include <QObject>
#include <QDebug>
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include "../../Model/LayoutTabListModel.h"
#include "../../WisenetMediaFramework/ViewModel/DragItemListModel.h"
#include "MediaLayoutViewModel.h"

class LayoutTabListModel;

class LayoutNavigationViewModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY (LayoutTabListModel* layoutTabModel READ layoutTabModel NOTIFY layoutChangedEvent)
    Q_PROPERTY (MediaLayoutViewModel* layoutViewModel READ layoutViewModel NOTIFY layoutViewModelChanged)

    explicit LayoutNavigationViewModel(QObject* parent = 0);
    ~LayoutNavigationViewModel();

    LayoutTabListModel* layoutTabModel(){
        return &m_layoutTabModel;
    }
    MediaLayoutViewModel* layoutViewModel()
    {
        return &m_MediaLayoutViewModel;
    }

    Q_INVOKABLE QList<QString> closeAllButThis(QString layoutId);
    Q_INVOKABLE QVariantList getLayoutListOfSequence(QString sequenceId);
    Q_INVOKABLE void loadFromCoreService(QString layoutId);

    QList<QString> getSavedLayoutList();

public slots:
    void makeNewTab();
    void addTab(int type, QString name, QString uuid);
    void saveTab(QString name);
    void closeTab(QString layoutId);
    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void closeAll();

signals:
    void layoutChangedEvent(LayoutTabListModel* arg);
    void mediaOpen(int type, QString itemId, QString channelNumber);
    void listMediaOpen(DragItemListModel* dragItemListModel);
    void gridItemsMediaOpen(QVariantList items);
    void layoutNameChanged(QString layoutId, QString layoutName);
    void layoutOpen(QString layoutId, QString layoutName, MediaLayoutViewModel* layoutViewModel);
    void sequenceOpen(QString sequenceId, QString sequenceName);
    void layoutViewModelChanged();

private:
    void Event_SaveLayout(QCoreServiceEventPtr event);
    void Event_RemoveLayout(QCoreServiceEventPtr event);
    void Event_RemoveSequenceLayout(QCoreServiceEventPtr event);

private:
    LayoutTabListModel m_layoutTabModel;
    MediaLayoutViewModel m_MediaLayoutViewModel;
};
