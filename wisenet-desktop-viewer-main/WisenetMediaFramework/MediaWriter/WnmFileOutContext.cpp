#include "WnmFileOutContext.h"
#include "LogSettings.h"
#include <QDir>
#include <QDateTime>
#include <QtDebug>

WnmFileOutContext::WnmFileOutContext()
{

}

WnmFileOutContext::~WnmFileOutContext()
{
    Finalize();
}

void WnmFileOutContext::Finalize()
{
    CloseFile();
    ClearQueue();
}

void WnmFileOutContext::SetInitializeParameter(QString fileDir, QString fileName, WnmInitalizeParameter initParam)
{
    m_fileDir = fileDir;
    m_fileName = fileName;
    m_initParam = initParam;
}

bool WnmFileOutContext::InitializeFile()
{
    m_initialized = true;

    m_fileOffsetIndex.clear();
    m_startTime = 0;
    m_endTime = 0;
    m_prevSeconds = 0;

    if(m_file.isOpen())
        m_file.close();

    if(m_dataStream.device() != nullptr)
        m_dataStream.setDevice(nullptr);

    // 폴더 유무 체크 및 생성
    if(!QDir(m_fileDir).exists())
    {
        if(!QDir().mkpath(m_fileDir))
        {
            SPDLOG_ERROR("WnmFileOutContext::InitializeFile() Failed to create a directory. path={}", m_fileDir.toLocal8Bit().toStdString());
            return false;
        }
    }

    // 파일 확장자 체크
    int extensionIndex = m_fileName.lastIndexOf(".");
    if(extensionIndex == -1 || extensionIndex == m_fileName.length() - 1)
    {
        SPDLOG_ERROR("WnmFileOutContext::InitializeFile() File extension error fileName={}", m_fileName.toLocal8Bit().toStdString());
        return false;
    }

    QString name = m_fileName.left(extensionIndex);
    QString extension = m_fileName.right(m_fileName.length() - extensionIndex - 1);
    if(extension.toLower() != "wnm")
    {
        SPDLOG_ERROR("WnmFileOutContext::InitializeFile() File extension error fileName={}", m_fileName.toLocal8Bit().toStdString());
        return false;
    }

    // 파일명 중복 체크
    m_currentOutFilePath = m_fileNo == 1
            ? m_fileDir + "/" + m_fileName
            : m_fileDir + "/" + name + " (" + QString::number(m_fileNo) + ")." + extension;
    while (QFile(m_currentOutFilePath).exists())
        m_currentOutFilePath = m_fileDir + "/" + name + " (" + QString::number(++m_fileNo) + ")." + extension;

    // 파일 생성 및 Open
    m_file.setFileName(m_currentOutFilePath);
    if(m_file.open(QIODevice::ReadWrite) == false)
    {
        SPDLOG_ERROR("WnmFileOutContext::InitializeFile() File open error fileName={}", m_fileName.toLocal8Bit().toStdString());
        return false;
    }
    m_dataStream.setDevice(&m_file);

    // Write FileOffsetIndex space as 0
    char fileOffsetIndexBuffer[FILE_OFFSET_INDEX_LENGTH]={0};
    std::memset(&fileOffsetIndexBuffer, 0x00, FILE_OFFSET_INDEX_LENGTH);
    if(m_dataStream.writeRawData(fileOffsetIndexBuffer, FILE_OFFSET_INDEX_LENGTH) == -1)
    {
        SPDLOG_ERROR("WnmFileOutContext::InitializeFile() Write FileOffsetIndex error fileName={}", m_fileName.toLocal8Bit().toStdString());
        return false;
    }

    WriteInfos();       // Write Infos
    WriteEncryption();  // Write Encryption
    if(m_initParam.useDigitalSignature)
        WriteDigitalSigning(); //Write DigitalSigning

    return true;
}

void WnmFileOutContext::CloseFile()
{
    m_initialized = false;

    if(!m_file.isOpen() || m_dataStream.device() == nullptr)
    {
        m_file.close();
        m_dataStream.setDevice(nullptr);
        emit writeFailed("CloseFile error. already closed or dataStream losted");
        return;
    }

    // Write IFrame Index
    if(m_IFrameIndexList.count() > 0)
    {
        m_fileOffsetIndex[WnmElementType::IFrameIndex].startOffset = m_file.pos();

        for(auto& index : m_IFrameIndexList)
        {
            m_dataStream << index.pts;
            m_dataStream << index.pos;
        }

        m_fileOffsetIndex[WnmElementType::IFrameIndex].nextElementOffset = m_file.pos();
    }
    m_IFrameIndexList.clear();

    // Write Timelines
    if(m_TimelineList.count() > 0)
    {
        m_fileOffsetIndex[WnmElementType::Timelines].startOffset = m_file.pos();

        for(auto& index : m_TimelineList)
        {
            m_dataStream << index.startTime;
            m_dataStream << index.endTime;
        }

        m_fileOffsetIndex[WnmElementType::Timelines].nextElementOffset = m_file.pos();
    }
    m_TimelineList.clear();

    // Write File Offset Index
    m_file.seek(0);
    m_dataStream << m_fileOffsetIndex.count();  // Offset entry count

    int count = 0;
    QMapIterator<WnmElementType, WnmOffsetEntry> iter(m_fileOffsetIndex);
    while (iter.hasNext())
    {
        iter.next();
        m_dataStream << iter.key(); // element type
        m_dataStream << iter.value().startOffset;    // start offset
        m_dataStream << iter.value().nextElementOffset;    // next element offset

        if(++count == 16)
            break;
    }

    // Get signature from the m_dataStream and write it.
    if(m_initParam.useDigitalSignature && m_digitalSignature.isOpenPfx()) //파일 읽어서 해쉬한 다음 값을 signingPos에 써주기
    {
        size_t signatureLen = m_digitalSignature.getKeyLength();
        unsigned char* signature = new unsigned char[signatureLen];

        std::memset(signature, 0X00, signatureLen);

        // 파일 위치를 파일 첫 부분으로 변경
        m_file.seek(0);

        if(!m_digitalSignature.createSignature(&m_file, &m_dataStream, &signature, &signatureLen, m_signingFilePos))
        {
            SPDLOG_ERROR("WnmFileOutContext::CloseFile Failed to create the signature.");
        }
        else
        {
            SPDLOG_DEBUG("WnmFileOutContext::CloseFile Success to create the signature.");

            // 계산된 서명 값 확인 (나중에 주석처리)
            //for(int i = 0; i<signatureLen;i++)
            //   printf("\%hhx", signature[i]);
            //printf("\nWnmFileOutContext::CloseFile - signature **************************************\n");

            m_dataStream.resetStatus();

            // 파일 위치를 signing pos로 설정
            m_file.seek(m_signingFilePos);

            // 파일에 서명 값 써주기
            if (m_dataStream.writeRawData(reinterpret_cast<const char*>(signature), (int)signatureLen) == -1) // writeRawData에 파라미터로 넘길 수 있도록 타입 바꿔주기
                SPDLOG_ERROR("WnmFileOutContext::CloseFile m_DataStream.writeRawData error : Failed to write computed signature");
            else
                SPDLOG_DEBUG("WnmFileOutContext::CloseFile m_DataStream.writeRawData is done");

            // 파일 위치를 맨 마지막으로 변경
            m_file.seek(m_file.atEnd());
        }

        delete[] signature;
    }

    m_fileOffsetIndex.clear();

    // Close file
    m_file.close();
    m_dataStream.setDevice(nullptr);


    // Rename file
    int fileNo = 1;
    int extensionIndex = m_fileName.lastIndexOf(".");
    QString name = m_fileName.left(extensionIndex);
    QString extension = m_fileName.right(m_fileName.length() - extensionIndex - 1);

    QDateTime startDatetime = QDateTime::fromMSecsSinceEpoch(m_startTime);
    QDateTime endDatetime = QDateTime::fromMSecsSinceEpoch(m_endTime);
    QString dateTime = "_" + startDatetime.toString("yyyyMMdd") + "_" + startDatetime.toString("hhmmss") + "_" + endDatetime.toString("hhmmss");

    QString newOutFilePath = m_fileDir + "/" + name + dateTime + "." + extension;
    while (QFile(newOutFilePath).exists())
        newOutFilePath = m_fileDir + "/" + name + dateTime + " (" + QString::number(++fileNo) + ")." + extension;

    if(!QFile::rename(m_currentOutFilePath, newOutFilePath))
    {
        emit writeFailed("Rename error");
    }
}

bool WnmFileOutContext::AllocContext()
{
    return false;
}
void WnmFileOutContext::ReleaseContext()
{

}

void WnmFileOutContext::ClearQueue()
{

}

void WnmFileOutContext::WriteInfos()
{
    if(!m_file.isOpen() || m_dataStream.device() == nullptr)
        return;

    // Update WnmElement Offset
    m_fileOffsetIndex[WnmElementType::Infos].startOffset = m_file.pos();

    char buffer256[256]={0};

    // write device & channel Info
    std::memset(buffer256, 0x00, 256);
    std::memcpy(buffer256, m_initParam.deviceName.toUtf8().data(), m_initParam.deviceName.toUtf8().size()+1);
    m_dataStream.writeRawData(buffer256, 256);

    std::memset(buffer256, 0x00, 256);
    std::memcpy(buffer256, m_initParam.channelName.toUtf8().data(), m_initParam.channelName.toUtf8().size()+1);
    m_dataStream.writeRawData(buffer256, 256);

    std::memset(buffer256, 0x00, 256);
    std::memcpy(buffer256, m_initParam.deviceModel.toUtf8().data(), m_initParam.deviceModel.toUtf8().size()+1);
    m_dataStream.writeRawData(buffer256, 20);

    std::memset(buffer256, 0x00, 256);
    std::memcpy(buffer256, m_initParam.channelModel.toUtf8().data(), m_initParam.channelModel.toUtf8().size()+1);
    m_dataStream.writeRawData(buffer256, 20);

    // write Timezone Info
    m_dataStream << m_initParam.timezoneBias;
    m_dataStream << m_initParam.daylightBias;
    m_dataStream << m_initParam.dstStartTime;
    m_dataStream << m_initParam.dstEndTime;

    // write Version Info
    m_dataStream << m_initParam.majorVersion;
    m_dataStream << m_initParam.minorVersion;

    // Update WnmElement Offset
    m_fileOffsetIndex[WnmElementType::Infos].nextElementOffset = m_file.pos();
}

void WnmFileOutContext::WriteEncryption()
{
    if(!m_file.isOpen() || m_dataStream.device() == nullptr)
        return; // 파일 Open이 비정상인 경우 return

    if(!m_initParam.usePassword || m_initParam.password == "")
        return; // Password를 사용하지 않는 경우 return

    // Update WnmElement Offset
    m_fileOffsetIndex[WnmElementType::Encryption].startOffset = m_file.pos();

    // Write Encryption data
    char chMsg[255];
    std::strcpy(chMsg,"wisenet viewer media export file");
    int size = (int)std::strlen(chMsg);

    char chPassword[MAX_PASSWORD_LEN];
    std::memset(chPassword, 0x00, MAX_PASSWORD_LEN);
    std::strncpy(chPassword, m_initParam.password.toStdString().c_str(), m_initParam.password.toStdString().length());

    m_crypto.Init((unsigned char*)chPassword, (int)strlen(chPassword));
    char * chEncrypt = m_crypto.Encrypt(chMsg, &size);
    if(chEncrypt == nullptr)
    {
        // 암호화 실패 시 암호화 정보를 쓰지 않고 리턴
        m_fileOffsetIndex.remove(WnmElementType::Encryption);
        return;
    }

    m_dataStream << size;   // encryption data size
    m_dataStream.writeRawData(chEncrypt, size); // encryption data

    delete[] chEncrypt;

    // Update WnmElement Offset
    m_fileOffsetIndex[WnmElementType::Encryption].nextElementOffset = m_file.pos();
}

void WnmFileOutContext::WriteDigitalSigning()
{
    if(!m_initParam.useDigitalSignature && m_initParam.pfxPassword == NULL)
        return; // 디지털 서명을 사용하지 않거나 인증서 비밀번호가 없는 경우 return

    if(!m_file.isOpen() || m_dataStream.device() == nullptr)
        return; // 파일 Open이 비정상인 경우 return

    DigitalSignature ds;
    qint32 signingDataSize;
    qint32 derCertLength;
    unsigned char* derData = NULL;
    char buffer256[256];

    std::memset(buffer256, 0x00, 256);

    //Update WnmElement Offset
    m_fileOffsetIndex[WnmElementType::DigitalSigning].startOffset = m_file.pos();

    if (0 != m_digitalSignature.openPfxFile(m_initParam.pfxPassword, m_initParam.pfxFilePath))
        SPDLOG_ERROR("WnmFileOutContext::WriteDigitalSigning() Failed to open the pfx file.");
    else
    {
        // signing 키의 길이 (256bytes), (digital signing data의 size를 write)
        signingDataSize= m_digitalSignature.getKeyLength();
        m_dataStream << signingDataSize;
    }

    // 추후에 서명 값을 다시 써줄 수 있도록 signing 데이터 시작 위치를 저장해둬야함.
    m_signingFilePos = m_file.pos();

    // signing 데이터(256) write --> 임시적으로 이 영역을 0으로 채워줌
    m_dataStream.writeRawData(buffer256, 256);

    // der 포맷의 인증서 data 가져오기
    derCertLength = m_digitalSignature.getDerData(&derData);

    // 인증서의 길이 write
    //qDebug() << "dcertLength :" << (qint32)derCertLength;
    m_dataStream << derCertLength;

    // 인증서 데이터 write --> der 포맷의 인증서 데이터를 저장
    m_dataStream.writeRawData(reinterpret_cast<const char*>(derData), derCertLength);

    // Update WnmElement Offset
    m_fileOffsetIndex[WnmElementType::DigitalSigning].nextElementOffset = m_file.pos();
}


bool WnmFileOutContext::WriteFrame(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData)
{
    if(!m_initialized)
        return true;    // 파일 초기화 이전이면 리턴 (파일 분할하여 initialize 재수행 시 frame이 들어올 수 있음)

    if(!m_file.isOpen() || m_dataStream.device() == nullptr)
        return false;   // FileOpen이 비정상인 경우 실패 리턴 (쓰기 종료)

    if(mediaData->getDataSize() == 0)
        return true;    // dataSize가 0인 frame은 쓰지 않음.

    Wisenet::Media::MediaType mediaType = mediaData->getMediaType();

    // 첫 frame의 offset 저장
    if(m_fileOffsetIndex.find(WnmElementType::Frames) == m_fileOffsetIndex.end())
    {
        // 첫 I-Frame이 들어올 때 까지 쓰지 않고 리턴
        if(mediaType != Wisenet::Media::MediaType::VIDEO)
            return true;

        auto videoData = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaData);
        if(videoData->videoFrameType != Wisenet::Media::VideoFrameType::I_FRAME)
            return true;

        // 첫 frame의 offset 저장
        m_fileOffsetIndex[WnmElementType::Frames].startOffset = m_file.pos();
    }

    char* pEncrypt = nullptr;
    int pesLength = (int)mediaData->getDataSize();
    if(m_fileOffsetIndex.contains(WnmElementType::Encryption))
    {
        // 파일 암호화 된 경우 Frame 암호화
        pEncrypt = m_crypto.Encrypt((char*)mediaData->getDataPtr(), &pesLength);

        if(pEncrypt == nullptr)
            return true;    // 암호화 실패 시, frame을 쓰지 않고 리턴
    }

    // Base frame header
    qint64 frameHeaderOffset = m_file.pos();

    char startCode = 0x00;
    m_dataStream.writeRawData(&startCode, 1);   // start code

    char frameType = mediaType == Wisenet::Media::MediaType::VIDEO ? 0x01 :
                     mediaType == Wisenet::Media::MediaType::AUDIO ? 0x02 : 0x01;
    m_dataStream.writeRawData(&frameType, 1);   // frame type

    m_dataStream << (qint64)mediaData->frameTime.ptsTimestampMsec;  // pts

    m_dataStream << pesLength;  // pes length

    int originalDataLength = (int)mediaData->getDataSize();
    m_dataStream << originalDataLength;  // original data length

    char reserved[6] = {0};
    std::memset(reserved, 0x00, 6);
    m_dataStream.writeRawData(reserved, 6); // reserved

    if(mediaType == Wisenet::Media::MediaType::VIDEO)
    {
        // Video header
        auto videoData = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaData);

        char videoFrameType = videoData->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME ? 0x01 :
                              videoData->videoFrameType == Wisenet::Media::VideoFrameType::P_FRAME ? 0x02 : 0x00;
        m_dataStream.writeRawData(&videoFrameType, 1);  // video frame type

        char videoCodecType = videoData->videoCodecType == Wisenet::Media::VideoCodecType::H265 ? 0x01 :
                              videoData->videoCodecType == Wisenet::Media::VideoCodecType::H264 ? 0x02 :
                              videoData->videoCodecType == Wisenet::Media::VideoCodecType::MJPEG ? 0x03 :
                              videoData->videoCodecType == Wisenet::Media::VideoCodecType::MPEG4 ? 0x04 :
                              videoData->videoCodecType == Wisenet::Media::VideoCodecType::VP8 ? 0x05 :
                              videoData->videoCodecType == Wisenet::Media::VideoCodecType::VP9 ? 0x06 :
                              videoData->videoCodecType == Wisenet::Media::VideoCodecType::AV1 ? 0x07 : 0x00;
        m_dataStream.writeRawData(&videoCodecType, 1);  // video codec type

        m_dataStream << videoData->videoWidth;  // frame width
        m_dataStream << videoData->videoHeight; // frame height

        // update i-frame index list
        if(videoFrameType == 0x01)
        {
            IFrameIndex index;
            index.pts = mediaData->frameTime.ptsTimestampMsec;
            index.pos = frameHeaderOffset;
            m_IFrameIndexList.push_back(index);
        }

        // update timeline list
        int64_t currentSeconds = mediaData->frameTime.ptsTimestampMsec / 1000;
        if(m_prevSeconds == 0 || m_prevSeconds+4 <= currentSeconds)
        {
            // add new timeline block
            Timeline timeline;
            timeline.startTime = timeline.endTime = mediaData->frameTime.ptsTimestampMsec;
            m_TimelineList.push_back(timeline);
        }
        else if(m_TimelineList.count() != 0)
        {
            // update last timeline block's endTime
            m_TimelineList.last().endTime = mediaData->frameTime.ptsTimestampMsec;
        }
        m_prevSeconds = currentSeconds;

        // update start/end time
        if(m_startTime == 0)
            m_startTime = mediaData->frameTime.ptsTimestampMsec;
        m_endTime = mediaData->frameTime.ptsTimestampMsec;
    }
    else if(mediaType == Wisenet::Media::MediaType::AUDIO)
    {
        // Audio header
        auto audioData = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(mediaData);

        char audioCodecType = audioData->audioCodecType == Wisenet::Media::AudioCodecType::G711U ? 0x01 :
                              audioData->audioCodecType == Wisenet::Media::AudioCodecType::G711A ? 0x02 :
                              audioData->audioCodecType == Wisenet::Media::AudioCodecType::G723 ? 0x03 :
                              audioData->audioCodecType == Wisenet::Media::AudioCodecType::G726 ? 0x04 :
                              audioData->audioCodecType == Wisenet::Media::AudioCodecType::AAC ? 0x05 : 0x00;
        m_dataStream.writeRawData(&audioCodecType, 1);  // audio codec type

        char channels = audioData->audioChannels == 1 ? 0x01 :
                        audioData->audioChannels == 2 ? 0x02 : 0x00;
        m_dataStream.writeRawData(&channels, 1);    // channels

        m_dataStream << audioData->audioSampleRate;    // samplerate
        m_dataStream << audioData->audioBitPerSample; // bit per sample
        m_dataStream << audioData->audioBitrate;    // bitrate
    }

    // Frame data
    if(pEncrypt != nullptr)
    {
        m_dataStream.writeRawData(pEncrypt, pesLength);
        delete[] pEncrypt;
    }
    else
    {
        m_dataStream.writeRawData((char*)mediaData->getDataPtr(), (int)mediaData->getDataSize());
    }

    m_fileOffsetIndex[WnmElementType::Frames].nextElementOffset = m_file.pos();

    // file size check
    qint64 filesize = m_file.size();
    if (filesize >= kMaxFileSizeByte)
    {
        SPDLOG_DEBUG("WnmFileOutContext::WriteFrame() file size is too big...");
        CloseFile();
        m_fileNo++;
        emit fileSplitRequest();
        return true;
    }

    return true;
}
