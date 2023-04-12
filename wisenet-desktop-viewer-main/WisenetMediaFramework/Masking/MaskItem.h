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
#include <QImage>
#include <QMap>
#include <QRect>
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

struct NormalMaskRect
{
    // normalized rectangle from (0,0) to (1,1)
    float normalX = 0;
    float normalY = 0;
    float normalWidth = 0;
    float normalHeight = 0;

    NormalMaskRect() {};
    NormalMaskRect(float x, float y, float width, float height) {
        normalX = x;
        normalY = y;
        normalWidth = width;
        normalHeight = height;
    }

    QRect toRealSizeRect(int videoWith, int videoHeight) {
        return QRect(normalX*videoWith, normalY*videoHeight, normalWidth*videoWith, normalHeight*videoHeight);
    }

    void normalize(int x, int y, int width, int height, int videoWidth, int videoHeight) {
        normalX = (float)x / (float)videoWidth;
        normalY = (float)y / (float)videoHeight;
        normalWidth = (float)width / (float)videoWidth;
        normalHeight = (float)height / (float)videoHeight;
    }
};

// Mask data for each key frame
struct MaskIndicator
{
    MaskIndicator(NormalMaskRect rect, qint64 timestamp) {
        maskRectangle = rect;
        ptsTimestampMsec = timestamp;
    }

    NormalMaskRect maskRectangle;   // mask position and size
    qint64 ptsTimestampMsec;        // timestamp
};

// Mask data for each mask object
struct MaskItem
{    
    MaskItem(int maskId, WisenetMaskType::MaskType maskType);
    ~MaskItem();

    void AddKeyFrameMaskIndicator(MaskIndicator newIndicator);
    void DeleteKeyFrameMaskIndicator(qint64 timeStamp);

    void UpdateFixedMaskIndicator(NormalMaskRect maskRect);

    void DeleteIndicatorsFromStartToParameter(qint64 timeStamp);
    void DeleteIndicatorsFromParamterToEnd(qint64 timeStamp);

    bool GetInterpolationMaskRect(qint64 timestamp, NormalMaskRect& maskRect);
    void UpdateMaskIndicator(QMap<qint64, MaskIndicator> resultMap);
    void Reset();

    int maskId = 0;
    WisenetMaskType::MaskType maskType = WisenetMaskType::MaskType::Fixed;
    bool selected = false;
    bool visible = true;
    QMap<qint64, MaskIndicator> m_maskIndicatorMap;
    QImage cropImage;
    QImage fullImage;
    QString name = "";
};

