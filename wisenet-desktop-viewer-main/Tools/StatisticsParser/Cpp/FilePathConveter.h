/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once
#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

class FilePathConveter : public QObject
{
private:
    Q_OBJECT
    Q_DISABLE_COPY(FilePathConveter)

    explicit FilePathConveter(QObject* parent = nullptr){
        Q_UNUSED(parent);
    }
    virtual ~FilePathConveter(){}
public:
    static FilePathConveter* getInstance(){
        static FilePathConveter instance;
        return &instance;
    }

    Q_INVOKABLE QString getPathByUrl(QUrl qUrlPath){
        QString filePath = qUrlPath.toLocalFile();
        QFileInfo fileInfo(filePath);
        QString nativePath = QDir::toNativeSeparators(filePath);
        return nativePath;
    }

    Q_INVOKABLE QString getExtensionByUrl(QUrl url){
        QString filePath = url.toLocalFile();
        QFileInfo fileInfo(filePath);
        QString fileSuffix = fileInfo.suffix();
        return fileSuffix;
    }

    Q_INVOKABLE QString getFileNameByUrl(QUrl url){
        QString filePath = url.toLocalFile();
        QFileInfo fileInfo(filePath);
        QString name = fileInfo.fileName();
        return name;
    }

    Q_INVOKABLE QUrl getUrlNameByFileName(QString name){
        return QUrl::fromLocalFile(name);
    }
};
