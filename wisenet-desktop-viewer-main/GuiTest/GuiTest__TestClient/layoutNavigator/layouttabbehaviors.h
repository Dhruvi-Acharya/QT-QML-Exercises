#ifndef LAYOUTTABBEHAVIORS_H
#define LAYOUTTABBEHAVIORS_H

#include <QObject>
#include <QDebug>
#include "layouttabmodel.h"

class LayoutTabModel;

class LayoutTabBehaviors : public QObject
{
    Q_OBJECT
public:
    explicit LayoutTabBehaviors(QObject* parent = 0);
    ~LayoutTabBehaviors();
    void setLayoutTabModel(LayoutTabModel * layoutTabModel);

public slots:
    void addTab(QString str);
    void saveTab(QString str);
    void closeTab(QString str);
    void closeAllButThis(QString str);

private:
    LayoutTabModel* layoutTabModel;
};

#endif // LAYOUTTABBEHAVIORS_H
