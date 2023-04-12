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

#include "Masking/MaskingTranscoder.h"
#include "FFmpegFileWriter.h"
#include "WnmFileWriter.h"

MaskingTranscoder::MaskingTranscoder(QObject *parent)
    : QObject(parent)
    , m_reader(new WnmReader())
    , m_videoDecoder(new VideoDecoder(nullptr, true, true))
    , m_videoEncoder(new VideoEncoder())
{
    SPDLOG_DEBUG("MaskingTranscoder::MaskingTranscoder()");

    connect(m_reader.get(), &WnmReader::openResult, this, &MaskingTranscoder::onReaderOpenResult);
    m_reader->start();
    m_mediaSourceFramesQueue = m_reader->getSourceFrameQueue();

    m_videoDecoder->setPixelFormat(VideoOutPixelFormat::YUV420P);   // fix decoder out pixel format to YUV420P
    m_videoDecoder->setVideoOutSize(0, 0, true);    // keep original size
}

MaskingTranscoder::~MaskingTranscoder()
{
    SPDLOG_DEBUG("MaskingTranscoder::~MaskingTranscoder()");
}

void MaskingTranscoder::loopTask()
{
    // check commands
    checkCommands();

    if(!m_mediaSourceFramesQueue || m_mediaSourceFramesQueue->size() == 0) {
        if(m_readerOpened && m_readerEmptyCount++ > 62) {
            // close if there's no more frame for 1000 ms after reader opened
            emit writeFinished(FileWriterBase::ErrorCode::NoError);
            close();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData(nullptr);
    if(!m_mediaSourceFramesQueue->pop_front(mediaData) || mediaData->commandSeq != m_commandSeq) {
        // pop failed or commandSeq not matched
        return;
    }

    if(mediaData->frameTime.ptsTimestampMsec >= m_endTime) {
        // close at the endTime
        emit writeFinished(FileWriterBase::ErrorCode::NoError);
        close();
        return;
    }

    if(mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO) {
        // decode & masking
        auto videoFramePtr = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaData);
        if(!decodeAndMaskFrame(videoFramePtr))
            return;

        // encode
        if(!m_videoEncoder->encode(m_VideoOutBuffer, m_encodedFrame))
            return;

        // push to writer queue
        if(m_writer)
            m_writer->PushMediaSourceFrame(m_encodedFrame);
    }
    else if(mediaData->getMediaType() == Wisenet::Media::MediaType::AUDIO) {
        // push to writer queue
        if(m_writer)
            m_writer->PushMediaSourceFrame(mediaData);
    }

    float progress = (float)(m_startTime - mediaData->frameTime.ptsTimestampMsec) / (float)(m_startTime - m_endTime) * 100;
    if(progress < 1)
        progress = 1;
    if(progress > 100)
        progress = 100;
    if(m_progress != (int)progress) {
        m_progress = (int)progress;
        emit progressChanged(m_progress);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

// slot for reader open result
void MaskingTranscoder::onReaderOpenResult(MediaControlBase::Error error)
{
    SPDLOG_DEBUG("MaskingTranscoder::onReaderOpenResult() error={}", error);
    if(error == MediaControlBase::Error::NoError) {
        // seek to export start time, after open successed
        m_reader->seek(m_startTime, m_commandSeq);
        m_readerOpened = true;
        m_readerEmptyCount = 0;
    }
    else if (error == MediaControlBase::Error::PasswordRequired) {
        // enter password
        m_reader->checkPassword(m_password);
    }
    else {
        // open failed
        emit writeFinished(FileWriterBase::ErrorCode::SetupFailed);
        close();
    }
}

// slot for writer finished
void MaskingTranscoder::onWriteFinished(FileWriterBase::ErrorCode errorCode)
{
    emit writeFinished(errorCode);
    close();
}

/**
 * @brief Set mask item list (must be setted before open)
 * @param maskItemList
 */
void MaskingTranscoder::setMaskItemList(const QList<MaskItem>& maskItemList)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_maskItemList = maskItemList;
}

/**
 * @brief Set WNM initialize parameter (must be setted before open, if the output format is wnm)
 * @param initParam (Encryption, Digital Signing, Etc.)
 */
void MaskingTranscoder::setWnmInitalizeParameter(const WnmInitalizeParameter initParam)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_wnmInitParam = initParam;
}

/**
 * @brief Set mask painter (must be setted before open)
 * @param painter (paintType, rgbColor, include/exclude option)
 */
void MaskingTranscoder::setMaskPainter(const CvMaskPainter painter)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_maskPainter = painter;
    m_maskPainter.setPixelRatio(1); // keep original size
}

void MaskingTranscoder::open(qint64 startTime, qint64 endTime, QUrl inputFileUrl, QString outputFilePath, QString outputFileName, QString password) {
    const std::lock_guard<std::mutex> lock(m_mutex);

    m_isOpenCommand = true;

    QString inputFilePath = inputFileUrl.toLocalFile();
    QString nativePath = QDir::toNativeSeparators(inputFilePath);

    m_startTime = startTime;
    m_endTime = endTime + 1000;
    m_inputFileNativePath = nativePath;
    m_outputFilePath = outputFilePath;
    m_outputFileName = outputFileName;
    m_password = password;
}

void MaskingTranscoder::close()
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    m_isCloseCommand = true;
}

void MaskingTranscoder::checkCommands()
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    if (m_isOpenCommand) {
        m_isOpenCommand = false;
        _open();
    }
    if (m_isCloseCommand) {
        m_isCloseCommand = false;
        _close();
    }
}

void MaskingTranscoder::_open()
{
    if(initializeFileWriter(m_outputFilePath, m_outputFileName)) {
        m_commandSeq++;
        m_reader->open(m_inputFileNativePath);
    }
    else {
        SPDLOG_DEBUG("MaskingTranscoder::_open() initialize file writer failed");
        emit writeFinished(FileWriterBase::ErrorCode::SetupFailed);
        close();
    }
}

void MaskingTranscoder::_close()
{
    if (m_reader) {
        m_reader->close();
    }

    if(m_writer) {
        m_writer->StopFileWrite();
    }

    if(m_mediaSourceFramesQueue)
        m_mediaSourceFramesQueue->clear();

    m_readerOpened = false;
    m_readerEmptyCount = 0;
}

bool MaskingTranscoder::initializeFileWriter(QString filePath, QString fileName)
{
    // 파일 확장자 체크 및 Writer 초기화
    int extensionIndex = fileName.lastIndexOf(".");
    if(extensionIndex == -1 || extensionIndex == fileName.length() - 1)
        return false;

    QString name = fileName.left(extensionIndex);
    QString extension = fileName.right(fileName.length() - extensionIndex - 1);
    if(extension.toLower() == "mkv")
        m_writer.reset(new FFmpegFileWriter(std::make_shared<MediaSourceFrameQueue>()));
    else if(extension.toLower() == "wnm")
        m_writer.reset(new WnmFileWriter(std::make_shared<MediaSourceFrameQueue>()));
    else
        return false;

    // 파일명에 (Masked) 추가
    fileName = name + "(Masked)." + extension;

    if(m_writer->GetWriterType() == FileWriterBase::WriterType::FFmpeg) {
        FFmpegFileWriter* ffmpegWriter = (FFmpegFileWriter*)m_writer.data();
        ffmpegWriter->SetOutFileInfo(filePath, fileName);
    }
    else if(m_writer->GetWriterType() == FileWriterBase::WriterType::Wnm) {
        WnmFileWriter* wnmWriter = (WnmFileWriter*)m_writer.data();
        wnmWriter->SetInitializeParameter(filePath, fileName, m_wnmInitParam);
    }

    connect(m_writer.data(), &FileWriterBase::writeFinished, this, &MaskingTranscoder::onWriteFinished, Qt::QueuedConnection);

    return true;
}

bool MaskingTranscoder::decodeAndMaskFrame(Wisenet::Media::VideoSourceFrameSharedPtr& videoFramePtr)
{
    m_VideoOutBuffer.reset();   // reset video out buffer

    // decode
    std::string errMsg;
    auto res = m_videoDecoder->decodeStart(videoFramePtr,
                                           m_VideoOutBuffer,
                                           true,
                                           videoFramePtr->videoWidth,
                                           videoFramePtr->videoHeight,
                                           true,
                                           errMsg);

    if (res != DecodeResult::Success) {
        SPDLOG_DEBUG("MaskingTranscoder::decodeVideoFrame() Decode failed, DecodeResult:{}", res);
        return false;
    }
    if (m_VideoOutBuffer) {
        //SPDLOG_DEBUG("MaskingTranscoder::decodeVideoFrame() Decode success");

        // paint mask
        QList<NormalMaskRect> maskRectList;
        for(auto& maskItem : m_maskItemList) {
            NormalMaskRect maskRect;
            if(maskItem.GetInterpolationMaskRect(m_VideoOutBuffer->ptsTimeMsec, maskRect)) {
                //SPDLOG_DEBUG("MaskingTranscoder maskRect x:{}, y:{}, width:{}, height:{}", maskRect.x(), maskRect.y(), maskRect.width(), maskRect.height());
                maskRectList.push_back(maskRect);
            }
        }
        m_maskPainter.paint(m_VideoOutBuffer, maskRectList);

        return true;
    }
    else {
        SPDLOG_DEBUG("MaskingTranscoder::decodeVideoFrame() Decode success, but frame may not be ready");
    }

    return false;
}
