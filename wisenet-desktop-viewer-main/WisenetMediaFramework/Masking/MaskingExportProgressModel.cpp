/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#include "MaskingExportProgressModel.h"
#include <QUrl>
#include <QDesktopServices>

MaskingExportProgressModel::MaskingExportProgressModel()
{

}

MaskingExportProgressModel::~MaskingExportProgressModel()
{

}

void MaskingExportProgressModel::setProgress(const int progress)
{
    if(m_progress != progress) {
        m_progress = progress;
        emit progressChanged();
    }
}

void MaskingExportProgressModel::setFailed(const bool failed)
{
    if(m_failed != failed) {
        m_failed = failed;
        emit failedChanged();
    }
}

void MaskingExportProgressModel::setCanceled(const bool canceled)
{
    if(m_canceled != canceled) {
        m_canceled = canceled;
        emit canceledChanged();
    }
}

void MaskingExportProgressModel::setFilePath(const QString filePath)
{
    if(m_filePath != filePath) {
        m_filePath = filePath;
        emit filePathChanged();
    }
}

void MaskingExportProgressModel::openFilePath()
{
    QUrl url = QUrl::fromLocalFile(m_filePath);
    QDesktopServices::openUrl(url);
}

void MaskingExportProgressModel::onProgressChanged(int progress)
{
    setProgress(progress);
}

void MaskingExportProgressModel::onWriteFinished(FileWriterBase::ErrorCode errorCode)
{
    if(errorCode == FileWriterBase::ErrorCode::NoError) {
        setProgress(100);
    }
    else {
        setFailed(true);
    }
}
