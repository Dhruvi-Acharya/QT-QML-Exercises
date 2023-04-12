#include "EventModel.h"

EventModel::EventModel(QObject *parent) : QObject(parent)
{
    //qDebug() << "EventModel()";
    m_utcTime = 0;
    m_deviceTime = 0;
}

EventModel::~EventModel()
{
    //qDebug() << "~EventModel()";
}
