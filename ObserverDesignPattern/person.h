#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QDebug>
#include <QRandomGenerator>

class Person : public QObject
{
    Q_OBJECT
public:
    explicit Person(QObject *parent = nullptr);

signals:
    void bid(int value);

public slots:
    void placeBid(QString item);

};

#endif // PERSON_H
