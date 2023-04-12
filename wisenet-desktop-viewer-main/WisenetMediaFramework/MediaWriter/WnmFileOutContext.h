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
#pragma once
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QMap>
#include "FFmpegStruct.h"
#include "WnmStruct.h"
#include "CryptoAdapter.h"
#include "DigitalSignature.h"

class WnmFileOutContext : public QObject
{
    Q_OBJECT

public:
    WnmFileOutContext();
    ~WnmFileOutContext();

    void SetInitializeParameter(QString fileDir, QString fileName, WnmInitalizeParameter initParam);
    bool InitializeFile();
    bool WriteFrame(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData);
    void CloseFile();

private:
    bool AllocContext();
    void ReleaseContext();
    void ClearQueue();
    void Finalize();

    void WriteInfos();
    void WriteEncryption();
    void WriteDigitalSigning();

signals:
    void writeFailed(const QString &message);
    void fileSplitRequest();

private:
    QString m_fileDir = "";
    QString m_fileName = "";
    WnmInitalizeParameter m_initParam;
    CryptoAdapter m_crypto;

    bool m_initialized = false;
    QFile m_file;
    QDataStream m_dataStream;

    unsigned int m_fileNo = 1;
    QString m_currentOutFilePath = "";
    int64_t m_prevSeconds = 0;
    int64_t m_startTime = 0;
    int64_t m_endTime = 0;

    QMap<WnmElementType, WnmOffsetEntry> m_fileOffsetIndex;
    QList<IFrameIndex> m_IFrameIndexList;
    QList<Timeline> m_TimelineList;

    DigitalSignature m_digitalSignature;
    qint64 m_signingFilePos = 0;

private:
    static const qint64 kMaxFileSizeByte = 4294967296;    // 4GB
};
