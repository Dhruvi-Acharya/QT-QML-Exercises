#ifndef AUCTIONEER_H
#define AUCTIONEER_H

#include <QObject>

class Auctioneer : public QObject
{
    Q_OBJECT
public:
    explicit Auctioneer(QObject *parent = nullptr);

signals:

};

#endif // AUCTIONEER_H
