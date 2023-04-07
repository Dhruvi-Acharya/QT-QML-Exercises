#include "filescanner.h"

FileScanner::FileScanner(QObject *parent)
    : QObject{parent}
{

}

void FileScanner::scan(QString path)
{
    qInfo() << "Scan start " << QThread::currentThread();
    count=0;

    QFuture<int> future = QtConcurrent::run(this,&FileScanner::performscan,path);

    qInfo() << "Main thread is free to do other things";
    qInfo() << "Result: " << future.result();

}

void FileScanner::performscan(QString path)
{
    int value = 0;

    QDir dir(path);
    if(!dir) return -1;
    qInfo() << "Performing scan on " << QThread::currentThread();

    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
    value = list.length();
    foreach (QFileInfo file, list) {
        value = value + performscan(file.filePath());
    }
    return value;
}
