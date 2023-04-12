#pragma once

#include "memory"
#include <QDebug>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <layoutNavigator/layouttabbehaviors.h>
#include <layoutNavigator/layouttabmodel.h>
#include <setup/AutoDiscoveryModel.h>
#include <tree/MainTreeModel.h>
#include <tree/MainTreeSortFilterProxyModel.h>
#include <login/LoginModel.h>
#include <bpptablemodel.h>
#include <bppfontawesome.h>
#include "multiwindowhandler.h"

class MultiWindowHandler;
class MultiWindow : public QObject{
    Q_OBJECT
public:
    explicit MultiWindow(QObject *parent = nullptr);
    MultiWindow(QString key, MultiWindowHandler* handler, QObject *parent = nullptr);
    ~MultiWindow();

signals:
public slots:

private:
    QQmlApplicationEngine* engine;
    LayoutTabBehaviors* layoutTabBehaviors;
    LayoutTabModel* layoutTabModel;
    AutoDiscoveryModel* autoDiscoveryModel;
};
