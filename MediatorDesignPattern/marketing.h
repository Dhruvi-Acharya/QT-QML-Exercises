#ifndef MARKETING_H
#define MARKETING_H

#include <QObject>
#include <QDebug>

class Marketing : public QObject
{
    Q_OBJECT
public:
    explicit Marketing(QObject *parent = nullptr);

signals:
    void finished();

public slots:
    void createHype();

};

#endif // MARKETING_H
