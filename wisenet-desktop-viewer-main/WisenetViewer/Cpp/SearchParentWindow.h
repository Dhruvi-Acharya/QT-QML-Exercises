#pragma once
#include "PopupParentWindow.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class SearchParentWindow : public QObject
{
    Q_OBJECT
public:
    explicit SearchParentWindow(QObject *parent, PopupParentWindow* popupParent);

signals:
    void eventSearchView_SetVisible(bool visible);
    void eventSearchView_SetSearchMenu(int menu);
    void eventSearchView_ResetFilter();
    void eventSearchView_SetChannelFilter(QStringList channelGuids);
    void eventSearchView_SetDateFilter(QVariant start, QVariant end);
    void eventSearchView_SearchByFilter();
    void eventSearchPopupOpenShortcutClicked(int menu);

    void closeAll();
    void initializeTree();

    // Export progress view
    void exportProgressView_Open();
    void exportProgressView_ForceClose();

private:
    QQmlApplicationEngine engine;
};
