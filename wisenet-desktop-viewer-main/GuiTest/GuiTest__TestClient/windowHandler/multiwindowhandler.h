#pragma once

#include "memory"
#include <QDebug>
#include <QObject>
#include <QMap>

#include <windowHandler/multiwindow.h>

class MultiWindow;

class MultiWindowHandler : public QObject {
    Q_OBJECT
public:
    explicit MultiWindowHandler(QObject* parent = nullptr);

signals:

public slots:
    void instatiateNewWindow();
    void destroyWindow(QString key);

private:
    //QMap<QString, QQmlApplicationEngine*> m_windows;
    QMap<QString, MultiWindow*> m_windows;
};
