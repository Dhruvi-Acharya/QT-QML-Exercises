#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QtConcurrent>
#include <QThread>

class FileScanner : public QObject
{
    Q_OBJECT
public:
    explicit FileScanner(QObject *parent = nullptr);

    void scan(QString path);

signals:
    void updated(int count);

private:
    void performscan(QString path);
    int count;

};

#endif // FILESCANNER_H
