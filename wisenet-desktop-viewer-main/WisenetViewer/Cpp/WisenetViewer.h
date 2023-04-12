#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "MultiWindowHandler.h"

class WisenetViewer
{
public:
    WisenetViewer();
    ~WisenetViewer();

private:
    MultiWindowHandler* m_MultiWindowHandler;
};
