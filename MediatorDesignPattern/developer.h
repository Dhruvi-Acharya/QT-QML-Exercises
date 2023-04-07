#ifndef DEVELOPER_H
#define DEVELOPER_H

#include <QObject>
#include <QDebug>

class Developer : public QObject
{
    Q_OBJECT
public:
    explicit Developer(QObject *parent = nullptr);

signals:
    void completed();

public slots:
    void createPrograms();

};

#endif // DEVELOPER_H
