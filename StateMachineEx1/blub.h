#ifndef BLUB_H
#define BLUB_H

#include <QObject>
#include <QDebug>

class Blub : public QObject // application state
{
    Q_OBJECT
public:
    explicit Blub(QObject *parent = nullptr);

    bool getPowered() const;
    void setPowered(bool newPowered);

signals:
private:
    bool powered; // class state (object state)

};

#endif // BLUB_H
