#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QDebug>

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);
    void startProject();

signals:
    void getToWork();

public slots:
    void completed();

};

#endif // MANAGER_H
