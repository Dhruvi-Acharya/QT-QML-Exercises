#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);

    void log(QString value);

    QFile *getFile() const;
    void setFile(QFile *newFile);

signals:

private:
    QFile *file;
};

#endif // LOGGER_H
