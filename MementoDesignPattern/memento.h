#ifndef MEMENTO_H
#define MEMENTO_H

#include <QObject>

class Memento : public QObject
{
    Q_OBJECT
public:
    explicit Memento(QObject *parent = nullptr);
    void commit();
    void cancel();
    void rollback();
    void addTransaction(QString value);
    QString command();

signals:

private:
    QStringList commands;
    QString commmand;


};

#endif // MEMENTO_H
