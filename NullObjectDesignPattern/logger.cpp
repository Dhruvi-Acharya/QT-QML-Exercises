#include "logger.h"

Logger::Logger(QObject *parent)
    : QObject{parent}
{

}

void Logger::log(QString value)
{
    qInfo() << value;

    if(!file){
        qWarning("File not found");
        return;
    }
    if(!file->isWritable()) {
        qWarning("file is not writable");
        return;
    }

    QTextStream stream(file);
    stream << value;

}

QFile *Logger::getFile() const
{
    return file;
}

void Logger::setFile(QFile *newFile)
{
    file = newFile;
}
