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
#include "MaskItem.h"
#include "LogSettings.h"

MaskItem::MaskItem(int maskId, WisenetMaskType::MaskType maskType)
    : maskId(maskId)
    , maskType(maskType)    
{
    name = (QString("Mask %1").arg(maskId));
    if(maskType == WisenetMaskType::MaskType::Fixed)
        name += " (Fixed)";
    else if(maskType == WisenetMaskType::MaskType::Manual)
        name += " (Manual)";
    else if(maskType == WisenetMaskType::MaskType::Auto)
        name += " (Auto)";
}

MaskItem::~MaskItem()
{

}

void MaskItem::AddKeyFrameMaskIndicator(MaskIndicator newIndicator)
{
    if(m_maskIndicatorMap.contains(newIndicator.ptsTimestampMsec))
        m_maskIndicatorMap.remove(newIndicator.ptsTimestampMsec);

    m_maskIndicatorMap.insert(newIndicator.ptsTimestampMsec, newIndicator);
}

void MaskItem::DeleteKeyFrameMaskIndicator(qint64 timeStamp)
{
    if(m_maskIndicatorMap.contains(timeStamp))
        m_maskIndicatorMap.remove(timeStamp);
}

void MaskItem::UpdateFixedMaskIndicator(NormalMaskRect maskRect)
{
    qint64 startTime = m_maskIndicatorMap.firstKey();
    qint64 endTime = m_maskIndicatorMap.lastKey();

    SPDLOG_DEBUG("FIXED ITEM - MaskItem::UpdateFixedMaskIndicator m_maskIndicatorMap.size : {} maskRect x: {}, y: {}, width: {}, height: {}",
                 m_maskIndicatorMap.size(), maskRect.normalX, maskRect.normalY, maskRect.normalWidth, maskRect.normalHeight);

    if(m_maskIndicatorMap.contains(startTime)) {
        SPDLOG_DEBUG("FIXED ITEM - REMOVE START TIME");
        m_maskIndicatorMap.remove(startTime);
    }
    m_maskIndicatorMap.insert(startTime, MaskIndicator(maskRect, startTime));


    if(m_maskIndicatorMap.contains(endTime)) {
        SPDLOG_DEBUG("FIXED ITEM - REMOVE END TIME ");
        m_maskIndicatorMap.remove(endTime);
    }
    m_maskIndicatorMap.insert(endTime, MaskIndicator(maskRect, endTime));

    SPDLOG_DEBUG("FIXED ITEM - MaskItem::UpdateFixedMaskIndicator FINAL : m_maskIndicatorMap.size : {}", m_maskIndicatorMap.size());
}


void MaskItem::DeleteIndicatorsFromStartToParameter(qint64 timeStamp)
{
    auto maskIndicatorList = m_maskIndicatorMap.values();

    // Delete if timestamp is smaller than param
    for(int i=maskIndicatorList.size()-1; i>=0 ; i--)
    {
        qint64 key = maskIndicatorList[i].ptsTimestampMsec;

        if(key < timeStamp)
            m_maskIndicatorMap.remove(key);
    }

    this->AddKeyFrameMaskIndicator(MaskIndicator(m_maskIndicatorMap.first().maskRectangle, timeStamp));
}

void MaskItem::DeleteIndicatorsFromParamterToEnd(qint64 timeStamp)
{
    auto maskIndicatorList = m_maskIndicatorMap.values();

    // Delete if timestamp is bigger than param
    for(int i=maskIndicatorList.size()-1; i>=0 ; i--)
    {
        qint64 key = maskIndicatorList[i].ptsTimestampMsec;

        if(key > timeStamp)
            m_maskIndicatorMap.remove(key);
    }

    this->AddKeyFrameMaskIndicator(MaskIndicator(m_maskIndicatorMap.last().maskRectangle, timeStamp));
}

bool MaskItem::GetInterpolationMaskRect(qint64 timestamp, NormalMaskRect& maskRect)
{
    qint64 margin = 1000;

    if (m_maskIndicatorMap.size() == 0) {
        return false;   // mask가 없는 경우 실패
    }

    if (m_maskIndicatorMap.size() == 1) {
        // mask가 1개면 timestamp와 관계 없이 해당 위치를 마스킹
        maskRect = m_maskIndicatorMap.first().maskRectangle;
        return true;
    }

    if (timestamp < m_maskIndicatorMap.first().ptsTimestampMsec) {
        // mask 된 시간 범위보다 timestamp가 작은 경우
        if(timestamp + margin >= m_maskIndicatorMap.first().ptsTimestampMsec) {
            // 시간 Margin에 포함되면 첫 번째 마스크 리턴
            maskRect = m_maskIndicatorMap.first().maskRectangle;
            return true;
        }
        else {
            // 시간 Margin 초과하여 벗어나면 실패 리턴
            return false;
        }
    }

    if (timestamp > m_maskIndicatorMap.last().ptsTimestampMsec) {
        // mask 된 시간 범위보다 timestamp가 큰 경우
        if(timestamp - margin <= m_maskIndicatorMap.last().ptsTimestampMsec) {
            // 시간 Margin에 포함되면 마지막 마스크 리턴
            maskRect = m_maskIndicatorMap.last().maskRectangle;
            return true;
        }
        else {
            // 시간 Margin 초과하여 벗어나면 실패 리턴
            return false;
        }
    }

    auto maskIndicatorList = m_maskIndicatorMap.values();

    for(int i=0 ; i<maskIndicatorList.size() ; i++) {
        MaskIndicator& indicator = maskIndicatorList[i];
        if(indicator.ptsTimestampMsec == timestamp) {
            maskRect = indicator.maskRectangle;    // found same timestamp
            return true;
        }
        else if(indicator.ptsTimestampMsec > timestamp) {
            // 나머지 경우는 2개의 mask 좌표를 timeStamp 기준으로 보간하여 계산
            MaskIndicator& prevMask = maskIndicatorList[i-1];
            MaskIndicator& nextMask = maskIndicatorList[i];
            NormalMaskRect prevRect = prevMask.maskRectangle;
            NormalMaskRect nextRect = nextMask.maskRectangle;
            qint64 keyFrameTimeDiff = nextMask.ptsTimestampMsec - prevMask.ptsTimestampMsec;
            qint64 currentTimeDiff = timestamp - prevMask.ptsTimestampMsec;

            maskRect.normalX = prevRect.normalX != nextRect.normalX
                    ? (nextRect.normalX - prevRect.normalX) / keyFrameTimeDiff * currentTimeDiff + prevRect.normalX
                    : prevRect.normalX;

            maskRect.normalY = prevRect.normalY != nextRect.normalY
                    ? (nextRect.normalY - prevRect.normalY) / keyFrameTimeDiff * currentTimeDiff + prevRect.normalY
                    : prevRect.normalY;

            maskRect.normalWidth = prevRect.normalWidth != nextRect.normalWidth
                    ? (nextRect.normalWidth - prevRect.normalWidth) / keyFrameTimeDiff * currentTimeDiff + prevRect.normalWidth
                    : prevRect.normalWidth;

            maskRect.normalHeight = prevRect.normalHeight != nextRect.normalHeight
                    ? (nextRect.normalHeight - prevRect.normalHeight) / keyFrameTimeDiff * currentTimeDiff + prevRect.normalHeight
                    : prevRect.normalHeight;

            return true;
        }
    }

    return false;
}

void MaskItem::UpdateMaskIndicator(QMap<qint64, MaskIndicator> resultMap)
{
    qint64 startTimeStamp = resultMap.firstKey();
    qint64 endTimeStamp = resultMap.lastKey();

    for(auto& indicatorKey : m_maskIndicatorMap.keys()) {
        if ((startTimeStamp <= indicatorKey) && (indicatorKey <= endTimeStamp)) {
            m_maskIndicatorMap.remove(indicatorKey);
        }
        else if(endTimeStamp < indicatorKey){
            break;
        }
    }

    for(auto& newIndicator : resultMap) {
        AddKeyFrameMaskIndicator(newIndicator);
    }
}

// This function is only used when we wanna reset m_tempMaskItem in WisenetMediaPlayer
void MaskItem::Reset()
{
    maskId = -1;
    maskType =  WisenetMaskType::MaskType::Fixed;
    m_maskIndicatorMap.clear();
}
