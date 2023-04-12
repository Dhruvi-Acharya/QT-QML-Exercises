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

#include "ThumbnailModel.h"
#include "QCoreServiceManager.h"

ThumbnailModel::ThumbnailModel()
{
    m_requestTimer.setInterval(600);
    connect(&m_requestTimer, &QTimer::timeout, this, &ThumbnailModel::onRequestTimerTimeout, Qt::QueuedConnection);
}

ThumbnailModel::~ThumbnailModel()
{
}

/*
QString ThumbnailModel::key() const
{
    return m_key;
}
*/

QImage ThumbnailModel::frontThumbnail() const
{
    return m_frontThumbnail;
}

QImage ThumbnailModel::rearThumbnail() const
{
    return m_rearThumbnail;
}

QImage ThumbnailModel::hoveredThumbnail() const
{
    return m_hoveredThumbnail;
}

float ThumbnailModel::frontThumbnailPosition() const
{
    return m_frontThumbnailPosition;
}

float ThumbnailModel::rearThumbnailPosition() const
{
    return m_rearThumbnailPosition;
}

bool ThumbnailModel::hoveredThumbnailVisible() const
{
    return m_hoveredThumbnailVisible;
}

int ThumbnailModel::thumbnailWidth() const
{
    return m_thumbnailWidth;
}

int ThumbnailModel::thumbnailHeight() const
{
    return m_thumbnailHeight;
}

float ThumbnailModel::mouseX() const
{
    return m_mouseX;
}

float ThumbnailModel::selectRectanglePosition() const
{
    return m_selectRectanglePosition;
}

bool ThumbnailModel::selectRectangleVisible() const
{
    return m_selectRectangleVisible;
}

bool ThumbnailModel::isLoading() const
{
    return m_isLoading;
}

bool ThumbnailModel::thumbnailCap() const
{
    return m_thumbnailCap;
}

/*
void ThumbnailModel::setKey(const QString &key)
{
    if(m_key != key) {
        m_key = key;
        emit keyChanged();
    }
}
*/

void ThumbnailModel::setFrontThumbnail(const QImage &image)
{
    if(m_frontThumbnail != image) {
        m_frontThumbnail = image;
        emit frontThumbnailChanged();
    }
}

void ThumbnailModel::setRearThumbnail(const QImage &image)
{
    if(m_rearThumbnail != image) {
        m_rearThumbnail = image;
        emit rearThumbnailChanged();
    }
}

void ThumbnailModel::setHoveredThumbnail(const QImage &image)
{
    if(m_hoveredThumbnail != image) {
        m_hoveredThumbnail = image;
        emit hoveredThumbnailChanged();
    }
}

void ThumbnailModel::setFrontThumbnailPosition(const float position)
{
    if(m_frontThumbnailPosition != position) {
        m_frontThumbnailPosition = position;
        emit frontThumbnailPositionChanged();
    }
}

void ThumbnailModel::setRearThumbnailPosition(const float position)
{
    if(m_rearThumbnailPosition != position) {
        m_rearThumbnailPosition = position;
        emit rearThumbnailPositionChanged();
    }
}

void ThumbnailModel::setHoveredThumbnailVisible(const bool visible)
{
    if(m_hoveredThumbnailVisible != visible) {
        m_hoveredThumbnailVisible = visible;
        emit hoveredThumbnailVisibleChanged();
    }
}

void ThumbnailModel::setThumbnailWidth(const int width)
{
    if(m_thumbnailWidth != width) {
        m_thumbnailWidth = width;
        emit thumbnailWidthChanged();
    }
}

void ThumbnailModel::setThumbnailHeight(const int height)
{
    if(m_thumbnailHeight != height) {
        m_thumbnailHeight = height;
        emit thumbnailHeightChanged();
    }
}

void ThumbnailModel::setMouseX(const float mouseX)
{
    if(m_requestedTimeList.count() > 0)
        return; // 로딩 중에는 설정하지 않음

    if(m_mouseX != mouseX){
        m_mouseX = mouseX;

        float selectRectanglePosition = m_frontThumbnailPosition;
        while(true) {
            if(m_thumbnailWidth <= 0 || mouseX < selectRectanglePosition)
                return; // 무한루프 방지를 위한 예외처리

            if(selectRectanglePosition <= mouseX && mouseX < selectRectanglePosition + m_thumbnailWidth) {
                setSelectRectanglePosition(selectRectanglePosition);
                setSelectRectangleVisible(true);
                break;  // mouseX를 포함하는 섬네일 한장의 시작위치
            }

            selectRectanglePosition += m_thumbnailWidth;
        }
    }
}

void ThumbnailModel::setSelectRectanglePosition(const float position)
{
    if(m_selectRectanglePosition != position) {
        m_selectRectanglePosition = position;
        emit selectRectanglePositionChanged();
    }
}

void ThumbnailModel::setSelectRectangleVisible(const bool visible)
{
    if(m_selectRectangleVisible != visible) {
        m_selectRectangleVisible = visible;
        emit selectRectangleVisibleChanged();
    }
}

void ThumbnailModel::setIsLoading(const bool isLoading)
{
    if(m_isLoading != isLoading) {
        m_isLoading = isLoading;
        emit isLoadingChanged();
    }
}

void ThumbnailModel::setThumbnailCap(const bool cap)
{
    if(m_thumbnailCap != cap) {
        m_thumbnailCap = cap;
        emit thumbnailCapChanged();
    }
}

void ThumbnailModel::clearThumbnail(QString key)
{
    //qDebug() << "clearAllThumbnail()";

    m_sequenceNumber++;
    m_waitTimeList.clear();
    setFrontThumbnail(QImage());
    setRearThumbnail(QImage());
    setHoveredThumbnail(QImage());
    setHoveredThumbnailVisible(false);
    if(key == "") {
        m_thumbnailMap.clear();
        m_thumbnailParamMap.clear();
    }
    else {
        m_thumbnailMap.remove(key);
        m_thumbnailParamMap.remove(key);
    }

    m_requestTimer.start(); // clear된 경우 requestTimer를 통해 썸네일을 즉시 갱신하지 않도록 함

    m_requestedTimeList.clear();
    setIsLoading(true);

    //qDebug() << "clearAllThumbnail, sequenceNumber is " << m_sequenceNumber;
}

void ThumbnailModel::refreshTimelineThumbnail(const QString& deviceID, const QString& channelID, const int overlappedId, qint64 visibleStartTime, qint64 visibleEndTime,
                                              float timelineWidth, int thumbnailHeight)
{
    setSelectRectangleVisible(false);   // 썸네일을 다시 그리면 선택 Rect는 숨김 처리
    setHoveredThumbnailVisible(false);
    m_hoveredTime = 0;

    QString key = deviceID + "_" + channelID + "_" + QString::number(overlappedId);

    // 타임라인 표시 범위 변경 확인
    qulonglong visibleRange = visibleEndTime - visibleStartTime;
    if(m_visibleTimeRange != visibleRange) {
        m_visibleTimeRange = visibleRange;
        clearThumbnail();    // 타임라인 표시 범위가 변경되면 전체 채널의 섬네일 삭제
    }

    // 섬네일 한장의 크기 계산
    if(thumbnailHeight <= 0)
        return; // 섬네일 크기 예외처리

    int videoWidth = 0;
    int videoHeight = 0;
    Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
    if (QCoreServiceManager::Instance().DB()->FindChannel(deviceID, channelID, channel)) {
        videoWidth = channel.highResolution.width;
        videoHeight = channel.highResolution.height;
    }
#endif
    if(videoWidth == 0)
        videoWidth = 16;
    if(videoHeight == 0)
        videoHeight = 9;

    if(thumbnailHeight%2 != 0)
        thumbnailHeight += 1; // 2의 배수로 맞춤

    int thumbnailWidth = thumbnailHeight * videoWidth / videoHeight;
    if(thumbnailWidth%2 != 0)
        thumbnailWidth += 1; // 2의 배수로 맞춤

    setThumbnailWidth(thumbnailWidth);
    setThumbnailHeight(thumbnailHeight);

    // key 변경 설정
    if(m_key != key) {
        m_key = key;
        m_sequenceNumber++;
        m_waitTimeList.clear();
        setFrontThumbnail(QImage());
        setRearThumbnail(QImage());
        setHoveredThumbnail(QImage());
        setHoveredThumbnailVisible(false);

        // Thumbnail Cap 확인
        checkThumbnailCap(deviceID);
    }

    if(!m_thumbnailCap) {
        return; // 섬네일 미지원 장비면 리턴
    }

    // 기존 섬네일이 하나도 없거나, 섬네일 한장의 크기가 바뀐 경우 해당 key의 섬네일 재설정
    qint64 timeGap = m_visibleTimeRange * thumbnailWidth / timelineWidth; // 썸네일 한장의 시간 범위
    if(!m_thumbnailMap.contains(m_key) || !m_thumbnailParamMap.contains(m_key)
            || m_thumbnailParamMap[m_key].thumbnailWidth != thumbnailWidth
            || m_thumbnailParamMap[m_key].thumbnailHeight != thumbnailHeight
            || m_thumbnailParamMap[m_key].thumbnailTimeRange / m_thumbnailParamMap[m_key].column != timeGap) {

        clearThumbnail(m_key);

        ThumbnailParam param;
        param.thumbnailWidth = thumbnailWidth;
        param.thumbnailHeight = thumbnailHeight;

        if(timeGap > 0)
        {
            qint64 thumbnailEndTime = visibleStartTime;
            int column = 0;
            while(thumbnailEndTime < visibleEndTime)
            {
                thumbnailEndTime += timeGap;
                column++;
            }

            param.thumbnailTimeRange = thumbnailEndTime - visibleStartTime;
            param.column = column;
        }

        m_thumbnailParamMap[m_key] = param;
    }

    QList<qint64> requestTimeList;  // 섬네일 요청 할 시간
    qint64 firstThumbnailStartTime = 0; // Map의 첫 번 째 섬네일의 시작시간
    qint64 frontThumbnailStartTime = 0; // 화면에 표시 할 앞쪽 섬네일의 시작시간
    qint64 rearThumbnailStartTime = 0;  // 화면에 표시 할 뒤쪽 섬네일의 시작시간
    qint64 thumbnailTimeRange = m_thumbnailParamMap[m_key].thumbnailTimeRange;

    if(m_thumbnailMap.contains(m_key)) {
        QMap<qint64, QImage>::iterator itor = m_thumbnailMap[m_key].begin();
        while (itor != m_thumbnailMap[m_key].end()) {
            if(firstThumbnailStartTime == 0)
                firstThumbnailStartTime = itor.key();

            qint64 thumbStartTime = itor.key();
            qint64 thumbEndTime = thumbStartTime + thumbnailTimeRange;
            if(thumbStartTime <= visibleStartTime && thumbEndTime >= visibleStartTime) {
                // 타임라인 앞쪽에 걸치는 섬네일
                frontThumbnailStartTime = itor.key();
                setFrontThumbnail(itor.value());
                qint64 timeGap = thumbStartTime - visibleStartTime;
                float position = timeGap == 0 ? 0 : timelineWidth * (float)timeGap / (float)m_visibleTimeRange;
                setFrontThumbnailPosition(position);
                //qDebug() << "refreshTimelineThumbnail() New front thumbnail position : " << position;
                //qDebug() << "refreshTimelineThumbnail() front thumbnail time : " << frontThumbnailStartTime;
            }
            if(thumbStartTime <= visibleEndTime && thumbEndTime >= visibleEndTime) {
                // 타임라인 뒤쪽에 걸치는 섬네일
                rearThumbnailStartTime = itor.key();
                setRearThumbnail(itor.value());
                qint64 timeGap = thumbStartTime - visibleStartTime;
                float position = timeGap == 0 ? 0 : timelineWidth * (float)timeGap / (float)m_visibleTimeRange;
                setRearThumbnailPosition(position);
                //qDebug() << "refreshTimelineThumbnail() New rear thumbnail position : " << position;
                //qDebug() << "refreshTimelineThumbnail() rear thumbnail time : " << rearThumbnailStartTime;
            }

            if(frontThumbnailStartTime != 0 && rearThumbnailStartTime != 0)
                break;

            itor++;
        }
    }

    if(frontThumbnailStartTime == 0 && rearThumbnailStartTime == 0) {
        // 앞 뒤 섬네일이 하나도 없는 경우 현재 표시 범위 기준으로 재요청
        //qDebug() << "refreshTimelineThumbnail() no thumbnail";
        setFrontThumbnail(QImage());
        setRearThumbnail(QImage());
        if(firstThumbnailStartTime == 0) {
            requestTimeList.push_back(visibleStartTime);
        }
        else if(visibleStartTime >= firstThumbnailStartTime) {
            qint64 startTime = firstThumbnailStartTime;
            qint64 count = 0;
            while(startTime <= visibleStartTime) {
                startTime += thumbnailTimeRange;
                count++;
            }
            //qDebug() << "refreshTimelineThumbnail() count : " << count;
            requestTimeList.push_back(startTime);
            requestTimeList.push_back(startTime - thumbnailTimeRange);
        }
        else {
            qint64 startTime = firstThumbnailStartTime;
            qint64 count = 0;
            while(startTime >= visibleStartTime) {
                startTime -= thumbnailTimeRange;
                count++;
            }
            //qDebug() << "refreshTimelineThumbnail() count : " << count;
            requestTimeList.push_back(startTime);
            requestTimeList.push_back(startTime + thumbnailTimeRange);
        }
    }
    else if(frontThumbnailStartTime == 0) {
        // 앞쪽 섬네일이 없는 경우 추가 요청
        //qDebug() << "refreshTimelineThumbnail() no front thumbnail";
        setFrontThumbnail(QImage());
        requestTimeList.push_back(rearThumbnailStartTime - thumbnailTimeRange);
    }
    else if(rearThumbnailStartTime == 0) {
        // 뒤쪽 섬네일이 없는 경우 추가 요청
        //qDebug() << "refreshTimelineThumbnail() no rear thumbnail";
        setRearThumbnail(QImage());
        requestTimeList.push_back(frontThumbnailStartTime + thumbnailTimeRange);
    }

    // 추가 요청해야 할 섬네일이 있으면 요청
    getAdditionalTimelineThumbnail(m_key, requestTimeList);
}

void ThumbnailModel::getAdditionalTimelineThumbnail(QString key, QList<qint64> timeList)
{
    for(auto& time : timeList) {
        m_waitTimeList.push_back(time);
    }

    if(m_requestTimer.isActive()) {
        // requestTimer에 의해 요청 대기중인 상태
        return;
    }

    if(m_requestedTimeList.count() > 0) {
        // 응답 대기 중이면 추가 요청 하지 않고 리턴
        //SPDLOG_DEBUG("getAdditionalTimelineThumbnail() waiting response");
        return;
    }

    while(m_waitTimeList.count() > 2) {
        // 마지막 2개의 썸네일 요청만 처리
        m_waitTimeList.pop_front();
    }

    QStringList token = key.split("_");
    if(token.size() != 3) {
        SPDLOG_DEBUG("getAdditionalTimelineThumbnail() Invalid key : {}", key.toStdString());
        return;
    }
    QString deviceId = token[0];
    QString channelId = token[1];
    int overlappedId = token[2].toInt();

    if(!m_thumbnailParamMap.contains(key))
        return;

    int sequenceNumber = m_sequenceNumber;
    for(auto& startTime : m_waitTimeList) {
        // 같은 구간의 섬네일을 중복 요청하지 않도록 처리
        if(m_thumbnailMap.contains(key) && m_thumbnailMap[key].contains(startTime))
            continue;
        else
            m_thumbnailMap[key][startTime] = QImage();

        std::shared_ptr<Wisenet::Device::DeviceThumbnailRequest> deviceThumbnailRequest;
        deviceThumbnailRequest = std::make_shared<Wisenet::Device::DeviceThumbnailRequest>();
        deviceThumbnailRequest->deviceID = deviceId.toStdString();
        deviceThumbnailRequest->channelID = channelId.toStdString();
        deviceThumbnailRequest->fromDate = startTime;
        deviceThumbnailRequest->toDate = startTime + m_thumbnailParamMap[key].thumbnailTimeRange;
        deviceThumbnailRequest->width = m_thumbnailParamMap[key].thumbnailWidth;
        deviceThumbnailRequest->height = m_thumbnailParamMap[key].thumbnailHeight;
        deviceThumbnailRequest->column = m_thumbnailParamMap[key].column;
        deviceThumbnailRequest->row = 1;
        deviceThumbnailRequest->overlappedID = overlappedId;

        //qDebug() << "getAdditionalTimelineThumbnail, width={} height={}" << m_thumbnailWidth << m_thumbnailHeight;
        qDebug() << "getAdditionalTimelineThumbnail, key, fromDate ~ toDate" << key << deviceThumbnailRequest->fromDate << deviceThumbnailRequest->toDate;

        m_requestedTimeList.push_back(startTime);
        setIsLoading(true);

#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceThumbnail,
                    this, deviceThumbnailRequest,
                    [this, key, startTime, sequenceNumber](const QCoreServiceReplyPtr& reply)
        {
            qDebug() << "DeviceThumbnail, sequenceNumber is " << sequenceNumber << m_sequenceNumber;
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceThumbnailResponse>(reply->responseDataPtr);
            if(!response || response->isFailed()){
                qDebug() << "DeviceThumbnail, isFailed={}" << response->isFailed();
            }
            else if(response->thumbnailData.empty()){
                qDebug() << "DeviceThumbnail, snapShots is empty";
            }
            else if(sequenceNumber != m_sequenceNumber) {
                qDebug() << "DeviceThumbnail, sequenceNumber is not equal";
                if(m_thumbnailMap.contains(key) && m_thumbnailMap[key].contains(startTime)) {
                    m_thumbnailMap[key].remove(startTime);  // 이전 sequenceNumber로 요청시 넣어둔 비어있는 이미지 삭제
                }
            }
            else{
                qDebug() << "DeviceThumbnail, save new image";
                QImage image = QImage::fromData(response->thumbnailData.data(), response->thumbnailData.size(), "JPEG");
                m_thumbnailMap[key][startTime] = image; // 로드 한 image를 map에 저장
            }

            emit additionalThumbnailLoaded(key);   // 추가 섬네일 로드 signal -> qml에서 받아서 현재 표시범위 기준으로 redraw 요청

            m_requestedTimeList.removeOne(startTime);   // 응답 받은 요청을 request list에서 제거
            if(m_requestedTimeList.count() == 0) {
                setIsLoading(false);
                if(m_waitTimeList.count() != 0) {
                    getAdditionalTimelineThumbnail(m_key, QList<qint64>()); // 응답 대기가 없고 요청 대기가 있으면 추가 요청
                }
            }
        });
#endif
    }
}

void ThumbnailModel::refreshHoveredThumbnail(const QString& deviceID, const QString& channelID, const int overlappedId,
                                             qint64 time, bool useThumbnailParam)
{
    // 키 설정
    QString key = deviceID + "_" + channelID + "_" + QString::number(overlappedId);
    if(m_key != key || time == 0) {
        // 키가 변경되었거나 시간이 0이면 초기화
        m_hoveredTime = 0;
        setHoveredThumbnail(QImage());
        setHoveredThumbnailVisible(false);

        if(m_key != key) {
            m_key = key;
            // Thumbnail Cap 확인
            checkThumbnailCap(deviceID);
        }

        if(time == 0)
            return;
    }

    if(!m_thumbnailCap) {
        return; // 섬네일 미지원 장비면 리턴
    }


    // 시간 설정
    time = (qint64)(time / 1000) * 1000; // ms 단위를 버림
    if(m_hoveredTime == time) {
        //qDebug() << "refreshHoveredThumbnail, same hoveredTime";
        return;
    }
    m_hoveredTime = time;


    // 응답 대기 중이면 추가 요청 하지 않고 리턴
    if(m_hoveredRequested) {
        //SPDLOG_DEBUG("refreshHoveredThumbnail() waiting response");
        return;
    }


    // 기존  ThumbnailParam을 사용하지 않는 경우(Thumbnail 모드 off인 경우) 섬네일 한장의 크기 계산
    int thumbnailWidth = 0;
    int thumbnailHeight = 124;
    if(!useThumbnailParam) {
        int videoWidth = 0;
        int videoHeight = 0;
        Wisenet::Device::Device::Channel channel;
    #ifndef MEDIA_FILE_ONLY
        if (QCoreServiceManager::Instance().DB()->FindChannel(deviceID, channelID, channel)) {
            videoWidth = channel.highResolution.width;
            videoHeight = channel.highResolution.height;
        }
    #endif
        if(videoWidth == 0)
            videoWidth = 16;
        if(videoHeight == 0)
            videoHeight = 9;

        thumbnailWidth = thumbnailHeight * videoWidth / videoHeight;
        if(thumbnailWidth%2 != 0)
            thumbnailWidth += 1; // 2의 배수로 맞춤

        setThumbnailWidth(thumbnailWidth);
        setThumbnailHeight(thumbnailHeight);
    }

    /*
    // 기존 ThumbnailParam 유무 체크
    if(useThumbnailParam && !m_thumbnailParamMap.contains(key)) {
        SPDLOG_DEBUG("refreshHoveredThumbnail() Thumbnail param is not initialized");
        return;
    }
    */

    // 섬네일 1장 요청
    std::shared_ptr<Wisenet::Device::DeviceThumbnailRequest> deviceThumbnailRequest;
    deviceThumbnailRequest = std::make_shared<Wisenet::Device::DeviceThumbnailRequest>();
    deviceThumbnailRequest->deviceID = deviceID.toStdString();
    deviceThumbnailRequest->channelID = channelID.toStdString();
    deviceThumbnailRequest->fromDate = time;
    deviceThumbnailRequest->toDate = time;
    deviceThumbnailRequest->width = m_thumbnailWidth;
    deviceThumbnailRequest->height = m_thumbnailHeight;
    deviceThumbnailRequest->column = 1;
    deviceThumbnailRequest->row = 1;
    deviceThumbnailRequest->overlappedID = overlappedId;

    //qDebug() << "getAdditionalTimelineThumbnail, width={} height={}" << m_thumbnailWidth << m_thumbnailHeight;
    //qDebug() << "refreshHoveredThumbnail, key, fromDate" << key << deviceThumbnailRequest->fromDate;

    m_hoveredRequested = true;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceThumbnail,
                this, deviceThumbnailRequest,
                [this, deviceID, channelID, overlappedId, time, useThumbnailParam](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceThumbnailResponse>(reply->responseDataPtr);
        if(!response || response->isFailed()){
            qDebug() << "refreshHoveredThumbnail(), isFailed={}" << response->isFailed();
            setHoveredThumbnail(QImage());
            setHoveredThumbnailVisible(false);
        }
        else if(response->thumbnailData.empty()){
            qDebug() << "refreshHoveredThumbnail(), snapShots is empty";
            setHoveredThumbnail(QImage());
            setHoveredThumbnailVisible(false);
        }
        else{
            //qDebug() << "refreshHoveredThumbnail(), save new image";
            QImage image = QImage::fromData(response->thumbnailData.data(), response->thumbnailData.size(), "JPEG");
            setHoveredThumbnail(image); // 로드 한 image를 저장
            setHoveredThumbnailVisible(true);
        }

        m_hoveredRequested = false;
        if(m_hoveredTime != time) {
            refreshHoveredThumbnail(deviceID, channelID, overlappedId, m_hoveredTime, useThumbnailParam); // 요청 대기가 있으면 추가 요청
        }
    });
#endif
}

void ThumbnailModel::onRequestTimerTimeout()
{
    //qDebug() << "onRequestTimerTimeout()";
    m_requestTimer.stop();
    if(m_requestedTimeList.count() == 0) {
        setIsLoading(false);
        if(m_waitTimeList.count() != 0) {
            getAdditionalTimelineThumbnail(m_key, QList<qint64>()); // 응답 대기가 없고 요청 대기가 있으면 추가 요청
        }
    }
}

void ThumbnailModel::checkThumbnailCap(const QString& deviceID)
{
#ifndef MEDIA_FILE_ONLY
    Wisenet::Device::Device device;
    if (QCoreServiceManager::Instance().DB()->FindDevice(deviceID, device)) {
        setThumbnailCap(device.deviceCapabilities.thumbnail);
    }
#endif
}
