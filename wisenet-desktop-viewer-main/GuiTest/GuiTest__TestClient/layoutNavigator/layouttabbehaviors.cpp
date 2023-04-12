#include "layouttabbehaviors.h"

LayoutTabBehaviors::LayoutTabBehaviors(QObject* parent) :
    QObject(parent)
{
}

LayoutTabBehaviors::~LayoutTabBehaviors()
{
    qDebug() << "qml: ~LayoutTabBehaviors()";
}

void LayoutTabBehaviors::setLayoutTabModel(LayoutTabModel* layoutTabModel)
{
    this->layoutTabModel = layoutTabModel;
}

void LayoutTabBehaviors::addTab(QString str)
{
    //qDebug() << "addTab model " << str;

    QObject* item1 = new QObject();
    item1->setProperty("tabText", "Layout " + str);
    this->layoutTabModel->append(item1);
}

void LayoutTabBehaviors::saveTab(QString str)
{
    qDebug() << "saveTab model " << str;
}

void LayoutTabBehaviors::closeTab(QString str)
{
    qDebug() << "closeTab model " << str;
    this->layoutTabModel->closeTab(str);
}

void LayoutTabBehaviors::closeAllButThis(QString str)
{
    qDebug() << "closeAllButThis model " << str;
    this->layoutTabModel->closeAllButThis(str);
}
